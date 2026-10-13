#include "TitleBar.hpp"
#include "Ui.hpp"
#include "Language.hpp"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QToolButton>
#include <QWindow>
namespace {
constexpr int barHeight=34,edge=5,corner=12;
// Minimize, maximize, restore and close drawn as thin lines, sharp at any scale.
QIcon glyph(const QString &kind){
    QIcon icon;
    for(const auto &[mode,color]:{std::pair{QIcon::Normal,Ui::themed(QColor("#cfcfd4"))},std::pair{QIcon::Active,kind=="close"?QColor(Qt::white):Ui::themed(QColor("#ffffff"))}})
        for(const qreal ratio:{1.0,2.0}){
            QPixmap pixmap(QSize(20,20)*ratio);pixmap.setDevicePixelRatio(ratio);pixmap.fill(Qt::transparent);
            QPainter p(&pixmap);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(color,1.1));p.setBrush(Qt::NoBrush);
            if(kind=="minimize")p.drawLine(QPointF(5,10.5),QPointF(15,10.5));
            else if(kind=="maximize")p.drawRect(QRectF(5.5,5.5,9,9));
            else if(kind=="restore"){p.drawRect(QRectF(5.5,7.5,7,7));p.drawPolyline(QPolygonF{QPointF(7.5,7.5),QPointF(7.5,5.5),QPointF(14.5,5.5),QPointF(14.5,12.5),QPointF(12.5,12.5)});}
            else{p.drawLine(QPointF(5.5,5.5),QPointF(14.5,14.5));p.drawLine(QPointF(14.5,5.5),QPointF(5.5,14.5));}
            p.end();icon.addPixmap(pixmap,mode);
        }
    return icon;
}
// An invisible strip along one side of the window that resizes it, with the window's outline drawn on its outer edge.
class Grip:public QWidget {
public:
    Grip(QWidget *parent,Qt::Edges edges):QWidget(parent),m_edges(edges){
        const bool horizontal=edges==Qt::LeftEdge||edges==Qt::RightEdge,vertical=edges==Qt::TopEdge||edges==Qt::BottomEdge;
        setCursor(horizontal?Qt::SizeHorCursor:vertical?Qt::SizeVerCursor:edges==(Qt::TopEdge|Qt::LeftEdge)||edges==(Qt::BottomEdge|Qt::RightEdge)?Qt::SizeFDiagCursor:Qt::SizeBDiagCursor);
    }
    Qt::Edges edges() const {return m_edges;}
protected:
    void mousePressEvent(QMouseEvent *event) override {if(event->button()==Qt::LeftButton&&window()->windowHandle())window()->windowHandle()->startSystemResize(m_edges);}
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);p.setPen(Ui::themed(QColor("#34343a")));const auto r=rect();
        if(m_edges&Qt::TopEdge)p.drawLine(r.topLeft(),r.topRight());if(m_edges&Qt::BottomEdge)p.drawLine(r.bottomLeft(),r.bottomRight());
        if(m_edges&Qt::LeftEdge)p.drawLine(r.topLeft(),r.bottomLeft());if(m_edges&Qt::RightEdge)p.drawLine(r.topRight(),r.bottomRight());
    }
private:
    Qt::Edges m_edges;
};
}
bool TitleBar::install(QMainWindow *window){
    const auto choice=qEnvironmentVariable("EBALIA_TITLE_BAR").toLower();bool wanted=choice=="custom";
    if(choice.isEmpty()&&QGuiApplication::platformName()=="wayland"&&!qEnvironmentVariableIsSet("QT_WAYLAND_DISABLE_WINDOWDECORATION")){
        // Mutter (GNOME, Pantheon, Phosh) never draws title bars for Wayland windows; KDE, Sway or Hyprland do, or leave
        // them out on purpose.
        const auto desktop=qEnvironmentVariable("XDG_CURRENT_DESKTOP").toLower();
        for(const auto name:{"gnome","pantheon","unity","phosh"})if(desktop.contains(QLatin1String(name)))wanted=true;
    }
    if(!wanted)return false;
    window->setWindowFlag(Qt::FramelessWindowHint);window->setMenuWidget(new TitleBar(window));return true;
}
TitleBar::TitleBar(QMainWindow *window):QWidget(window),m_window(window){
    setObjectName("titleBar");setAttribute(Qt::WA_StyledBackground);setFixedHeight(barHeight);
    auto row=new QHBoxLayout(this);row->setContentsMargins(12,0,0,0);row->setSpacing(8);
    auto icon=new QLabel;icon->setPixmap(qApp->windowIcon().pixmap(16,16));row->addWidget(icon);
    m_title=new QLabel(window->windowTitle());m_title->setObjectName("titleBarText");row->addWidget(m_title,1);
    auto control=[this,row](const QString &kind,const QString &tip,const char *name){
        auto b=new QToolButton;b->setObjectName(name);b->setIcon(glyph(kind));b->setIconSize({20,20});b->setFixedSize(46,barHeight);b->setAutoRaise(true);b->setFocusPolicy(Qt::NoFocus);b->setToolTip(tip);b->setAccessibleName(tip);row->addWidget(b);return b;};
    connect(control("minimize",Language::key("Minimize"),"titleMinimize"),&QToolButton::clicked,this,[this]{m_window->showMinimized();});
    m_maximize=control("maximize",Language::key("Maximize"),"titleMaximize");connect(m_maximize,&QToolButton::clicked,this,&TitleBar::toggleMaximized);
    connect(control("close",Language::key("Close"),"titleClose"),&QToolButton::clicked,m_window,&QWidget::close);
    for(const Qt::Edges edges:{Qt::Edges(Qt::TopEdge),Qt::Edges(Qt::BottomEdge),Qt::Edges(Qt::LeftEdge),Qt::Edges(Qt::RightEdge),Qt::TopEdge|Qt::LeftEdge,Qt::TopEdge|Qt::RightEdge,Qt::BottomEdge|Qt::LeftEdge,Qt::BottomEdge|Qt::RightEdge})
        m_grips<<new Grip(window,edges);
    window->installEventFilter(this);sync();
}
void TitleBar::toggleMaximized(){if(m_window->isMaximized())m_window->showNormal();else m_window->showMaximized();}
// Dragging starts only past the drag distance, so a double-click still reaches the bar instead of the compositor.
void TitleBar::mousePressEvent(QMouseEvent *event){if(event->button()==Qt::LeftButton){m_press=event->position().toPoint();m_pressed=true;}QWidget::mousePressEvent(event);}
void TitleBar::mouseMoveEvent(QMouseEvent *event){
    if(m_pressed&&(event->buttons()&Qt::LeftButton)&&(event->position().toPoint()-m_press).manhattanLength()>=QApplication::startDragDistance()){
        m_pressed=false;if(auto handle=m_window->windowHandle())handle->startSystemMove();}
}
void TitleBar::mouseReleaseEvent(QMouseEvent *event){m_pressed=false;QWidget::mouseReleaseEvent(event);}
void TitleBar::mouseDoubleClickEvent(QMouseEvent *event){if(event->button()==Qt::LeftButton){m_pressed=false;toggleMaximized();}}
bool TitleBar::eventFilter(QObject *watched,QEvent *event){
    if(watched==m_window){
        if(event->type()==QEvent::WindowTitleChange)m_title->setText(m_window->windowTitle());
        if(event->type()==QEvent::WindowStateChange||event->type()==QEvent::Resize||event->type()==QEvent::Show)sync();
    }
    return QWidget::eventFilter(watched,event);
}
void TitleBar::sync(){
    const bool full=m_window->isMaximized()||m_window->isFullScreen();setVisible(!m_window->isFullScreen()); // full screen: no bar at all
    m_maximize->setIcon(glyph(full?"restore":"maximize"));m_maximize->setToolTip(full?Language::key("Restore"):Language::key("Maximize"));m_maximize->setAccessibleName(m_maximize->toolTip());
    const int w=m_window->width(),h=m_window->height();
    for(auto widget:m_grips){auto grip=static_cast<Grip*>(widget);const auto e=grip->edges();
        QRect r;if(e==Qt::TopEdge)r={corner,0,w-2*corner,edge};else if(e==Qt::BottomEdge)r={corner,h-edge,w-2*corner,edge};
        else if(e==Qt::LeftEdge)r={0,corner,edge,h-2*corner};else if(e==Qt::RightEdge)r={w-edge,corner,edge,h-2*corner};
        else{r={e&Qt::LeftEdge?0:w-corner,e&Qt::TopEdge?0:h-corner,corner,corner};
            // Corners are L-shaped, so the close button under the top right one still gets its clicks.
            grip->setMask(QRegion(0,e&Qt::TopEdge?0:corner-edge,corner,edge)+QRegion(e&Qt::LeftEdge?0:corner-edge,0,edge,corner));}
        grip->setGeometry(r);grip->setVisible(!full);grip->raise();}
}
