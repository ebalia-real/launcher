#include "CreatorDialog.hpp"
#include "InstanceIcons.hpp"
#include "Language.hpp"
#include "Ui.hpp"
#include <QtWidgets>

namespace {
QString t(const char *text){return Language::key(QString::fromUtf8(text));}
QLabel *copy(const QString &text,QBoxLayout *layout,const char *name){auto label=new QLabel(text);label->setWordWrap(true);label->setTextFormat(Qt::PlainText);label->setObjectName(name);layout->addWidget(label);return label;}
QPushButton *link(const QString &text,const QString &url,QBoxLayout *layout,const char *name,const QString &icon="external-link"){
    auto button=new QPushButton(" "+text);button->setObjectName(name);button->setIcon(Ui::icon(icon,Qt::white));button->setIconSize(QSize(16,16));button->setCursor(Qt::PointingHandCursor);button->setProperty("destination",url);layout->addWidget(button);
    QObject::connect(button,&QPushButton::clicked,button,[url]{QDesktopServices::openUrl(QUrl(url));});return button;
}
QList<CreatorMod> bundled(){
    // Dark artwork that suits horror mods.
    QList<CreatorMod> mods{{"ooo.jar","ooo-jar","An unsettling adventure hidden in your Minecraft world.",false,":/art/backgrounds/nether.png"},
                           {"In Your World","in-your-world","Something in your world is not quite right.",false,":/art/backgrounds/warden.png"},
                           {"Secret 01","secret-01-proyect","A new mystery by EBALIA. Coming soon.",true,":/art/backgrounds/mineshaft.png"}};
    mods[0].text="modOoo";mods[1].text="modInYourWorld";mods[2].text="modSecret";
    for(auto &mod:mods)if(!mod.soon)mod.curseforge="https://www.curseforge.com/minecraft/mc-mods/"+mod.slug;
    return mods;
}
QList<CreatorMod> &current(){static QList<CreatorMod> mods=bundled();return mods;}
QString onHost(const QString &address,const QStringList &hosts){const QUrl url(address);return url.scheme()=="https"&&hosts.contains(url.host())?url.toString():QString();}
QString key(const CreatorMod &m){return QStringList{m.name,m.slug,m.description,m.soon?"soon":"",m.page,m.icon,QString::number(m.downloads),m.text,m.curseforge,m.modrinth,m.collab?"collab":"",m.credit,m.creditNames}.join('\n');}

// The mod's picture over its art, as wide as the card and 16:9, rounded at the top like the card.
class ModCover:public QWidget {
public:
    ModCover(){QSizePolicy policy(QSizePolicy::Expanding,QSizePolicy::Preferred);policy.setHeightForWidth(true);setSizePolicy(policy);}
    void setPixmap(const QPixmap &pixmap){m_pixmap=pixmap;m_scaled={};update();}
    bool hasHeightForWidth() const override {return true;}
    int heightForWidth(int width) const override {return width*9/16;}
    QSize sizeHint() const override {return {320,180};}
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);QPainterPath clip;clip.addRoundedRect(QRectF(rect()).adjusted(0,0,0,14),13,13);p.setClipPath(clip);
        p.fillRect(rect(),QColor(24,24,29));if(m_pixmap.isNull())return;
        // Scaled once per size, not at every repaint.
        if(const auto target=size()*devicePixelRatioF();m_scaled.size()!=target){m_scaled=Ui::cover(m_pixmap,target);m_scaled.setDevicePixelRatio(devicePixelRatioF());}
        p.drawPixmap(0,0,m_scaled);
    }
private:
    QPixmap m_pixmap,m_scaled;
};

// A small button for each store the mod is on, in the store's colour.
void stores(const CreatorMod &mod,QBoxLayout *layout){
    for(const auto &[name,url]:{std::pair<QString,QString>{"CurseForge",mod.curseforge},{"Modrinth",mod.modrinth}}){
        if(url.isEmpty())continue;
        auto button=link(name+" ↗",url,layout,"creatorModLink");button->setIcon({});button->setText(name+"  ↗");button->setProperty("store",name.toLower());button->setProperty("project",mod.slug);
    }
}
QFrame *modCard(const CreatorMod &mod,const CreatorDialog::PictureLoader &picture,const QStringList &art){
    auto card=new QFrame;card->setObjectName("creatorModCard");card->setProperty("project",mod.slug);card->setProperty("collab",mod.collab);card->setMinimumWidth(240);
    auto layout=new QVBoxLayout(card);layout->setContentsMargins(1,1,1,14);layout->setSpacing(0);
    auto cover=new ModCover;layout->addWidget(cover);
    auto body=new QVBoxLayout;body->setContentsMargins(16,12,16,0);body->setSpacing(5);layout->addLayout(body,1);
    const auto downloads=QLocale(Language::current).toString(mod.downloads)+" "+Language::key("downloads").toUpper();
    copy(mod.soon?"EBALIA  ·  "+t("Coming soon").toUpper():mod.collab?downloads:"EBALIA  ·  "+downloads,body,"newsMeta");
    copy(mod.name,body,"creatorModTitle");
    if(mod.collab)copy(mod.by(),body,"creatorModCredit");
    copy(mod.about(),body,"creatorModAbout");
    body->addStretch();body->addSpacing(8);
    auto links=new QHBoxLayout;links->setSpacing(8);body->addLayout(links);stores(mod,links);links->addStretch();
    // The store's picture once it loads, the bundled one until then.
    const auto background=!mod.background.isEmpty()?mod.background:art.isEmpty()?QString(":/art/f1_2.jpg"):art[int(qHash(mod.slug)%uint(art.size()))];
    auto compose=[cover=QPointer<ModCover>(cover),background](const QPixmap &image){
        if(cover)Ui::loadArt(background,{640,360},cover,[cover,image](const QPixmap &p){if(cover)cover->setPixmap(creatorCover(p,image));});};
    const auto own=":/creations/"+mod.slug+".png";compose(QFile::exists(own)?QPixmap(own):QPixmap());
    if(!mod.icon.isEmpty()&&picture)picture(mod.icon,compose);
    return card;
}
void section(QBoxLayout *body,const QString &title,const QString &about,const QList<CreatorMod> &mods,const CreatorDialog::PictureLoader &picture,const char *name){
    if(mods.isEmpty())return;
    body->addSpacing(14);copy(title,body,"sectionTitle");copy(about,body,"muted");body->addSpacing(4);
    const auto art=InstanceIcons::backgrounds();auto grid=new Ui::CardGrid(260);grid->setObjectName(name);
    for(const auto &mod:mods)grid->add(modCard(mod,picture,art));
    body->addWidget(grid);
}
// What Patreon brings, and the way there.
void patreon(QBoxLayout *body){
    auto card=new QFrame;card->setObjectName("creatorPatreonCard");body->addWidget(card);
    auto row=new QHBoxLayout(card);row->setContentsMargins(22,20,22,20);row->setSpacing(18);
    auto heart=new QLabel;heart->setObjectName("creatorPatreonHeart");heart->setFixedSize(52,52);heart->setAlignment(Qt::AlignCenter);heart->setPixmap(Ui::pixmap("heart",24,Qt::white));row->addWidget(heart,0,Qt::AlignTop);
    auto text=new QVBoxLayout;text->setSpacing(8);row->addLayout(text,1);
    copy("EBALIA · PATREON",text,"archiveEyebrow");copy(t("Be part of the next creation"),text,"sectionTitle");
    // In a row, or one under another in a narrow window.
    auto perks=new Ui::ResponsiveRow(430);perks->box()->setSpacing(10);text->addWidget(perks);
    for(const auto &[icon,perk]:{std::pair<const char*,const char*>{"sparkles","Exclusive creations"},{"image","Mod previews"},{"clock","Early access"}}){
        auto item=new QWidget;auto line=new QHBoxLayout(item);line->setContentsMargins(0,0,8,0);line->setSpacing(6);perks->box()->addWidget(item,0,Qt::AlignLeft);
        auto mark=new QLabel;mark->setPixmap(Ui::pixmap(icon,16,QColor(255,163,150)));line->addWidget(mark);copy(t(perk),line,"creatorPerk")->setWordWrap(false);
    }
    perks->box()->addStretch();
    copy(t("According to your Patreon membership."),text,"muted");
    auto actions=new Ui::ResponsiveRow(560);actions->box()->setSpacing(8);text->addSpacing(4);text->addWidget(actions);
    auto join=link(t("Join EBALIA on Patreon"),"https://www.patreon.com/EBALIA",actions->box(),"joinPatreon","heart");join->setProperty("patreon",true);actions->box()->setAlignment(join,Qt::AlignLeft);
    auto member=link(t("Already a member? View exclusive posts"),"https://www.patreon.com/c/ebalia/posts",actions->box(),"patreonContent");member->setProperty("link",true);member->setIcon(Ui::icon("external-link",Ui::accent()));actions->box()->setAlignment(member,Qt::AlignLeft);
    actions->box()->addStretch();
}
}

QString CreatorMod::url() const { return !page.isEmpty() ? page : !curseforge.isEmpty() ? curseforge : "https://www.curseforge.com/minecraft/mc-mods/" + slug; }
QString CreatorMod::about() const {
    if(!text.isEmpty())if(const auto translated=Language::key(text);translated!=text)return translated; // the site's description, in every language
    return Language::key(description);
}
QString CreatorMod::by() const {
    if(!collab)return {};
    return Language::key(credit=="remakeBy"?"Remake by %1":"With %1").arg(creditNames);
}
QList<CreatorMod> creatorMods(){return current();}
void resetCreatorMods(){current()=bundled();}
bool setCreatorMods(const QJsonObject &live){
    if(live["format"].toInt()!=1)return false;
    static const QRegularExpression id("^[a-z0-9][a-z0-9._-]{0,80}$"),text("^mod[A-Za-z0-9]{1,40}$");
    QList<CreatorMod> own,collab;const auto known=bundled();
    for(const auto &value:live["mods"].toArray()){
        const auto o=value.toObject();const auto slug=o["id"].toString();const auto group=o["group"].toString();
        if((group!="own"&&group!="collab")||!id.match(slug).hasMatch()||o["name"].toString().trimmed().isEmpty())continue;
        CreatorMod mod{{},slug,o["summary"].toString(),false,{}};
        for(const auto &base:known)if(base.slug==slug)mod=base; // its art and translated description stay
        mod.name=o["name"].toString().trimmed().left(80);mod.soon=o["soon"].toBool();mod.collab=group=="collab";
        if(const auto key=o["text"].toString();text.match(key).hasMatch())mod.text=key;
        mod.curseforge=onHost(o["curseforge"].toObject()["url"].toString(),{"www.curseforge.com","curseforge.com"});
        mod.modrinth=onHost(o["modrinth"].toObject()["url"].toString(),{"modrinth.com"});
        mod.page=!mod.curseforge.isEmpty()?mod.curseforge:mod.modrinth;
        mod.icon=onHost(o["icon"].toString(),{"media.forgecdn.net","cdn.modrinth.com"});mod.downloads=qMax<qint64>(0,qint64(o["downloads"].toDouble()));
        if(const auto credit=o["credit"].toArray();mod.collab&&(credit.at(0).toString()=="collabWith"||credit.at(0).toString()=="remakeBy")){mod.credit=credit.at(0).toString();mod.creditNames=credit.at(1).toString().left(120);}
        (mod.collab?collab:own)<<mod;
    }
    if(own.isEmpty()&&collab.isEmpty())return false;
    QStringList before,after;for(const auto &m:current())before<<key(m);for(const auto &m:own+collab)after<<key(m);
    current()=own+collab;return before!=after;
}
QPixmap creatorCover(const QPixmap &art,const QPixmap &picture){
    QPixmap canvas(640,360);canvas.fill(QColor(20,20,24));QPainter painter(&canvas);painter.setRenderHint(QPainter::SmoothPixmapTransform);painter.setRenderHint(QPainter::Antialiasing);
    if(!art.isNull())painter.drawPixmap(0,0,Ui::cover(art,{640,360}));painter.fillRect(canvas.rect(),QColor(8,8,12,170));
    if(!picture.isNull()){QRect box(0,0,240,240);box.moveCenter(canvas.rect().center());QPainterPath frame;frame.addRoundedRect(QRectF(box.adjusted(-6,-6,6,6)),24,24);painter.fillPath(frame,QColor(0,0,0,120));
        QPainterPath clip;clip.addRoundedRect(QRectF(box),18,18);painter.setClipPath(clip);painter.drawPixmap(box,picture);}
    painter.end();return canvas;
}
CreatorDialog::CreatorDialog(bool patreonOnly,QWidget *parent,PictureLoader picture):QDialog(parent){
    setObjectName(patreonOnly?"patreonInvite":"myModsDialog");setWindowTitle(patreonOnly?"EBALIA · Patreon":QString("EBALIA Mods"));setMinimumSize(480,400);
    auto layout=new QVBoxLayout(this);layout->setContentsMargins(0,0,0,0);layout->setSpacing(0);
    auto scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);layout->addWidget(scroll,1);
    auto content=new QWidget;content->setObjectName("creatorContent");auto body=new QVBoxLayout(content);body->setContentsMargins(26,22,26,16);body->setSpacing(6);scroll->setWidget(content);
    copy("EBALIA / CREATOR COLLECTION",body,"archiveEyebrow");copy(patreonOnly?t("Enter the world of EBALIA"):QString("EBALIA Mods"),body,"pageTitle");
    copy(t(patreonOnly?"Support new creations and discover exclusive content on Patreon.":"Stories, strange worlds and creations by EBALIA."),body,"pageSubtitle");
    if(!patreonOnly){
        // Its own and the collaborations apart, as on the site; every published mod counts in the total.
        QList<CreatorMod> own,collab;qint64 total=0;int published=0;
        for(const auto &mod:creatorMods()){(mod.collab?collab:own)<<mod;if(!mod.soon){total+=mod.downloads;++published;}}
        if(total>0)copy(t("%1 downloads · %2 mods").arg(QLocale(Language::current).toString(total)).arg(published),body,"creatorStats");
        section(body,t("Solo"),t("Made by EBALIA alone."),own,picture,"creatorOwnMods");
        section(body,t("Collaborations"),t("Made together with other creators."),collab,picture,"creatorCollabMods");
        body->addSpacing(18);
    }else body->addSpacing(12);
    patreon(body);body->addStretch();
    auto footer=new QHBoxLayout;footer->setContentsMargins(26,12,26,16);layout->addLayout(footer);
    if(patreonOnly){auto never=new QCheckBox(t("Do not show this invitation again"));never->setObjectName("hidePatreonInvite");never->setChecked(QSettings().value("ui/hidePatreonInvite",false).toBool());footer->addWidget(never);connect(never,&QCheckBox::toggled,this,[](bool checked){QSettings().setValue("ui/hidePatreonInvite",checked);});}
    footer->addStretch();auto close=new QPushButton(t("Close"));close->setObjectName("closeCreatorDialog");footer->addWidget(close);connect(close,&QPushButton::clicked,this,&QDialog::accept);
    Ui::fitToScreen(this,patreonOnly?QSize(680,470):QSize(980,780));
}
