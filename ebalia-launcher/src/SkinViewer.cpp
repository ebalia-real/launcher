#include "SkinViewer.hpp"
#include "SkinManager.hpp"
#include "Language.hpp"
#include "Ui.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QVariantAnimation>
#include <QVector3D>
#include <algorithm>
#include <cmath>
namespace {
// Model units are skin pixels: feet at y 0, top of the head at 32, the player faces +z (the viewer) and their right
// side is at -x. A box carries the corner of its texture and its size there, as in Minecraft's model files.
struct Box {QVector3D min,max;QPoint uv;float w,h,d;bool mirror=false;};
struct Part {Box base,overlay;bool hasOverlay;};
struct Face {QVector3D origin,across,down,normal;QRectF source;};
double radians(double degrees){return degrees*M_PI/180;}
QVector3D turn(const QVector3D &p,double yaw,double pitch){
    const double x=p.x()*std::cos(yaw)+p.z()*std::sin(yaw),z=-p.x()*std::sin(yaw)+p.z()*std::cos(yaw);
    return QVector3D(float(x),float(p.y()*std::cos(pitch)-z*std::sin(pitch)),float(p.y()*std::sin(pitch)+z*std::cos(pitch)));
}
QList<Part> model(bool slim,bool legacy){
    // Slim arms are 3 pixels wide and sit half a pixel lower, as in the game.
    const float arm=slim?3:4,armTop=slim?23.5f:24;
    auto box=[](QVector3D min,QVector3D max,QPoint uv,float w,float h,float d,bool mirror=false){return Box{min,max,uv,w,h,d,mirror};};
    auto grow=[](Box b,float by,QPoint uv){b.min-=QVector3D(by,by,by);b.max+=QVector3D(by,by,by);b.uv=uv;return b;};
    const auto head=box({-4,24,-4},{4,32,4},{0,0},8,8,8),body=box({-4,12,-2},{4,24,2},{16,16},8,12,4);
    const auto rightArm=box({-4-arm,armTop-12,-2},{-4,armTop,2},{40,16},arm,12,4),rightLeg=box({-4,0,-2},{0,12,2},{0,16},4,12,4);
    // Old 64 × 32 skins have no left limbs: the right ones, mirrored.
    const auto leftArm=legacy?box({4,armTop-12,-2},{4+arm,armTop,2},{40,16},arm,12,4,true):box({4,armTop-12,-2},{4+arm,armTop,2},{32,48},arm,12,4);
    const auto leftLeg=legacy?box({0,0,-2},{4,12,2},{0,16},4,12,4,true):box({0,0,-2},{4,12,2},{16,48},4,12,4);
    return {{head,grow(head,0.5f,{32,0}),true},{body,grow(body,0.25f,{16,32}),!legacy},{rightArm,grow(rightArm,0.25f,{40,32}),!legacy},
            {leftArm,grow(leftArm,0.25f,{48,48}),!legacy},{rightLeg,grow(rightLeg,0.25f,{0,32}),!legacy},{leftLeg,grow(leftLeg,0.25f,{0,48}),!legacy}};
}
// The six faces of a box with the part of the texture each one shows, laid out as Minecraft unwraps a box.
QList<Face> faces(const Box &b){
    const auto &n=b.min,&x=b.max;const float u=b.uv.x(),v=b.uv.y(),w=b.w,h=b.h,d=b.d;
    QRectF top(u+d,v,w,d),bottom(u+d+w,v,w,d),right(u,v+d,d,h),front(u+d,v+d,w,h),left(u+d+w,v+d,d,h),back(u+2*d+w,v+d,w,h);
    if(b.mirror)std::swap(right,left);
    const QVector3D down(0,n.y()-x.y(),0);
    QList<Face> out{
        {{n.x(),x.y(),x.z()},{x.x()-n.x(),0,0},down,{0,0,1},front},
        {{x.x(),x.y(),n.z()},{n.x()-x.x(),0,0},down,{0,0,-1},back},
        {{n.x(),x.y(),n.z()},{0,0,x.z()-n.z()},down,{-1,0,0},right},
        {{x.x(),x.y(),x.z()},{0,0,n.z()-x.z()},down,{1,0,0},left},
        {{n.x(),x.y(),n.z()},{x.x()-n.x(),0,0},{0,0,x.z()-n.z()},{0,1,0},top},
        {{n.x(),n.y(),n.z()},{x.x()-n.x(),0,0},{0,0,x.z()-n.z()},{0,-1,0},bottom}};
    if(b.mirror)for(auto &f:out){f.origin+=f.across;f.across=-f.across;}
    return out;
}
}
SkinViewer::SkinViewer(QWidget *parent):QWidget(parent),m_turn(new QVariantAnimation(this)){
    setObjectName("skinPreview");setMinimumSize(240,320);setCursor(Qt::OpenHandCursor);
    m_turn->setDuration(450);m_turn->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_turn,&QVariantAnimation::valueChanged,this,[this](const QVariant &value){m_yaw=value.toDouble();update();});
}
void SkinViewer::setSkin(const QImage &skin,bool slim){
    m_message.clear();m_slim=slim;m_skin=skin.convertToFormat(QImage::Format_ARGB32);
    if(!m_skin.isNull()&&m_skin.width()>=64&&(m_skin.height()==m_skin.width()||m_skin.height()*2==m_skin.width())){
        const int k=m_skin.width()/64;
        // As the game does: an old skin whose hat area has no transparent pixel wears no hat.
        if(m_skin.height()*2==m_skin.width()){bool clear=false;for(int y=0;y<16*k&&!clear;++y)for(int x=32*k;x<64*k;++x)if(qAlpha(m_skin.pixel(x,y))<128){clear=true;break;}
            if(!clear)for(int y=0;y<16*k;++y)for(int x=32*k;x<64*k;++x)m_skin.setPixel(x,y,0);}
        // The inner layer is always opaque in the game.
        m_base=m_skin;for(int y=0;y<m_base.height();++y){auto line=reinterpret_cast<QRgb*>(m_base.scanLine(y));for(int x=0;x<m_base.width();++x)line[x]|=0xff000000;}
    }else{m_skin={};m_base={};}
    update();
}
void SkinViewer::setMessage(const QString &text){m_message=text;m_skin={};m_base={};update();}
void SkinViewer::setThreeD(bool on){m_threeD=on;setCursor(on?Qt::OpenHandCursor:Qt::ArrowCursor);update();}
void SkinViewer::setView(double yaw,double pitch){m_turn->stop();m_yaw=yaw;m_pitch=pitch;update();}
void SkinViewer::turnAround(){
    if(!m_threeD){m_back=!m_back;update();return;}
    const double from=m_turn->state()==QAbstractAnimation::Running?m_turn->endValue().toDouble():m_yaw;m_turn->stop();
    m_turn->setStartValue(m_yaw);m_turn->setEndValue(from+180);m_turn->start();
}
void SkinViewer::paintEvent(QPaintEvent *){
    QPainter p(this);QStyleOption option;option.initFrom(this);style()->drawPrimitive(QStyle::PE_Widget,&option,&p,this);
    if(m_skin.isNull()){p.setPen(Ui::themed(QColor("#9a9aa3")));p.drawText(rect(),Qt::AlignCenter|Qt::TextWordWrap,m_message);return;}
    if(!m_threeD){
        const auto flat=SkinManager::preview(m_skin,m_slim?"slim":"classic",m_back);const auto area=rect().adjusted(12,12,-12,-12);
        const double fit=std::min({1.0,double(area.width())/flat.width(),double(area.height())/flat.height()});const QSizeF size=QSizeF(flat.size())*fit;
        p.drawImage(QRectF(QPointF(area.center().x()-size.width()/2,area.center().y()-size.height()/2),size),flat);return;
    }
    paintModel(p);
    auto font=p.font();font.setPixelSize(11);p.setFont(font);p.setPen(Ui::themed(QColor("#7d7d86")));
    p.drawText(rect().adjusted(8,0,-8,-10),Qt::AlignHCenter|Qt::AlignBottom,Language::key("Drag to turn"));
}
void SkinViewer::paintModel(QPainter &p){
    const double yaw=radians(m_yaw),pitch=radians(m_pitch);
    const QRectF area=QRectF(rect()).adjusted(16,18,-16,-36);const double scale=std::min(area.width()/22,area.height()/38);const QPointF center=area.center();
    auto project=[&](const QVector3D &q){const auto r=turn(q-QVector3D(0,16,0),yaw,pitch);return QPointF(center.x()+r.x()*scale,center.y()-r.y()*scale);};
    auto vector=[&](const QVector3D &q){const auto r=turn(q,yaw,pitch);return QPointF(r.x()*scale,-r.y()*scale);};
    // A soft shadow on the floor under the feet.
    {const auto feet=project({0,0,0});const double rx=9*scale,ry=std::max(2.0,9*scale*std::abs(std::sin(pitch)));
     QRadialGradient shade(feet,rx);shade.setColorAt(0,QColor(0,0,0,90));shade.setColorAt(1,QColor(0,0,0,0));
     p.save();p.translate(feet);p.scale(1,ry/rx);p.translate(-feet);p.setPen(Qt::NoPen);p.setBrush(shade);p.drawEllipse(feet,rx,rx);p.restore();}
    // Parts back to front, each with its outer layer over it: the painter's algorithm, enough for a figure of boxes.
    auto parts=model(m_slim,m_skin.height()*2==m_skin.width());
    std::stable_sort(parts.begin(),parts.end(),[&](const Part &a,const Part &b){return turn((a.base.min+a.base.max)/2,yaw,pitch).z()<turn((b.base.min+b.base.max)/2,yaw,pitch).z();});
    const double k=m_skin.width()/64.0,seam=0.6/scale; // faces overlap by about half a screen pixel, so no gaps show between them
    p.setRenderHint(QPainter::SmoothPixmapTransform,false);
    for(const auto &part:parts)for(int layer=0;layer<(part.hasOverlay?2:1);++layer)
        for(const auto &f:faces(layer?part.overlay:part.base)){
            if(turn(f.normal,yaw,pitch).z()<=0.001f)continue; // facing away
            const auto o=project(f.origin),a=vector(f.across),b=vector(f.down);const double w=f.source.width(),h=f.source.height();
            p.setTransform(QTransform(a.x()/w,a.y()/w,b.x()/h,b.y()/h,o.x(),o.y()));
            p.drawImage(QRectF(-seam,-seam,w+2*seam,h+2*seam),layer?m_skin:m_base,QRectF(f.source.x()*k,f.source.y()*k,w*k,h*k));
        }
    p.resetTransform();
}
void SkinViewer::mousePressEvent(QMouseEvent *event){
    if(!m_threeD||event->button()!=Qt::LeftButton)return QWidget::mousePressEvent(event);
    m_turn->stop();m_dragging=true;m_last=event->position().toPoint();setCursor(Qt::ClosedHandCursor);
}
void SkinViewer::mouseMoveEvent(QMouseEvent *event){
    if(!m_dragging)return QWidget::mouseMoveEvent(event);
    const auto delta=event->position().toPoint()-m_last;m_last=event->position().toPoint();
    m_yaw+=delta.x()*0.6;m_pitch=std::clamp(m_pitch+delta.y()*0.4,-60.0,80.0);update();
}
void SkinViewer::mouseReleaseEvent(QMouseEvent *event){
    if(!m_dragging)return QWidget::mouseReleaseEvent(event);
    m_dragging=false;setCursor(Qt::OpenHandCursor);
}
void SkinViewer::mouseDoubleClickEvent(QMouseEvent *event){
    if(!m_threeD||event->button()!=Qt::LeftButton)return QWidget::mouseDoubleClickEvent(event);
    m_turn->stop();m_turn->setStartValue(m_yaw);m_turn->setEndValue(std::round((m_yaw+28)/360)*360-28);m_pitch=12;m_turn->start();
}
