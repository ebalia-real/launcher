#include "SkinsPage.hpp"
#include <QtConcurrent>
#include "ModRepository.hpp"
#include "Archive.hpp"
#include "SkinManager.hpp"
#include "SkinViewer.hpp"
#include "AccountManager.hpp"
#include "McInstanceManager.hpp"
#include "MsAuth.hpp"
#include "Language.hpp"
#include "Ui.hpp"
#include <QtWidgets>
using Language::key;
SkinsPage::SkinsPage(QString root,AccountManager *accounts,McInstanceManager *instances,Work work,QWidget *parent)
    :QWidget(parent),m_root(std::move(root)),m_accounts(accounts),m_instances(instances),m_work(std::move(work)){
    auto outer=new QVBoxLayout(this);outer->setContentsMargins(0,0,0,0);auto row=new Ui::ResponsiveRow(660);outer->addWidget(row);auto layout=row->box();layout->setSpacing(22);
    auto visual=new QVBoxLayout;layout->addLayout(visual,1);m_preview=new SkinViewer;visual->addWidget(m_preview,1);
    // Under the model: half a turn (or front and back in 2D), and the choice between the 3D model and the flat view.
    auto viewRow=new QHBoxLayout;viewRow->setSpacing(8);visual->addLayout(viewRow);
    auto turn=new QPushButton(key("Front / back"));turn->setObjectName("skinTurn");viewRow->addWidget(turn,1);connect(turn,&QPushButton::clicked,this,[this]{m_preview->turnAround();});
    auto mode=new QPushButton;mode->setObjectName("skinViewMode");viewRow->addWidget(mode,1);
    auto modeText=[this,mode]{mode->setText(m_preview->threeD()?key("2D view"):key("3D view"));};
    connect(mode,&QPushButton::clicked,this,[this,modeText]{m_preview->setThreeD(!m_preview->threeD());QSettings().setValue("ui/skin3d",m_preview->threeD());modeText();});
    m_preview->setThreeD(QSettings().value("ui/skin3d",true).toBool());modeText();
    auto controls=new QVBoxLayout;layout->addLayout(controls,2);m_list=new QListWidget;m_list->setObjectName("skinList");controls->addWidget(m_list,1);connect(m_list,&QListWidget::currentRowChanged,this,[this]{showSkin();});
    m_details=new QLabel;m_details->setWordWrap(true);m_details->setTextFormat(Qt::PlainText);controls->addWidget(m_details);
    auto add=[&](QString title,std::function<void()> fn){auto b=new QPushButton(title);controls->addWidget(b);connect(b,&QPushButton::clicked,this,fn);return b;};
    add(key("Import PNG skin"),[this]{importSkin();});
    add(key("Apply to Microsoft account"),[this]{apply(false);})->setProperty("play",true);
    // Locally: the skin of every instance (also the ones created later, when they start), or of one; each instance keeps
    // the one it was given alone until the skin of all of them changes.
    add(key("Apply locally to all instances"),[this]{applyAll();})->setObjectName("applyLocalAll");
    add(key("Apply locally to an instance"),[this]{apply(true);})->setObjectName("applyLocalOne");
    add(key("Restore local appearance"),[this]{
        auto busy=[this](const McInstance &i){return m_instances->isRunning(i.dir)||m_instances->isInstalling(i.dir);};
        QStringList names{key("All instances")};QList<McInstance> targets;for(const auto &i:m_instances->instances())if(!busy(i)){names<<i.name+" · "+i.mcVersion;targets<<i;}
        bool ok;auto value=QInputDialog::getItem(this,key("Restore local appearance"),key("Instance"),names,0,false,&ok);const int choice=names.indexOf(value);if(!ok||choice<0)return;
        try{
            if(choice==0){
                SkinManager::clearDefaultLocal(m_root);
                for(const auto &i:m_instances->instances()){if(SkinManager::localMode(i.dir).isEmpty())continue;
                    if(busy(i))SkinManager::markLocal(i.dir,"restore"); // open: when it starts again
                    else{SkinManager::removeLocal(i.dir);SkinManager::markLocal(i.dir,{});}}
                m_details->setText(key("Local appearance restored in every instance."));
            }else{const auto &target=targets[choice-1];SkinManager::removeLocal(target.dir);SkinManager::markLocal(target.dir,"none");m_details->setText(key("Local appearance restored."));}
        }catch(const std::exception &e){QMessageBox::warning(this,"EBALIA",Language::message(QString::fromUtf8(e.what())));}
        showDefaultLocal();
    });
    m_defaultLocal=new QLabel;m_defaultLocal->setObjectName("defaultLocalSkin");m_defaultLocal->setWordWrap(true);m_defaultLocal->setTextFormat(Qt::PlainText);controls->addWidget(m_defaultLocal);showDefaultLocal();
    // Skins of the library only: the one an account wears comes from Microsoft or the game.
    m_remove=add(key("Delete skin"),[this]{removeSkin();});m_remove->setObjectName("deleteSkin");m_remove->setProperty("danger",true);m_remove->setEnabled(false);
    auto shortcut=new QShortcut(QKeySequence::Delete,m_list,nullptr,nullptr,Qt::WidgetShortcut);connect(shortcut,&QShortcut::activated,this,[this]{if(m_remove->isEnabled())removeSkin();});
    connect(m_accounts,&AccountManager::accountsChanged,this,[this]{refresh();});
    auto note=new QLabel(key("Microsoft skins are visible on servers. Local skins only change default textures on this client; other default players may also look different."));note->setWordWrap(true);note->setObjectName("muted");controls->addWidget(note);refresh();
}
namespace {
// The player's head next to each skin in the list, enlarged pixel by pixel.
QIcon headIcon(const QImage &skin){const auto head=SkinManager::head(skin);if(head.isNull())return {};const auto face=QPixmap::fromImage(head.scaled(32,32,Qt::IgnoreAspectRatio,Qt::FastTransformation));
    QIcon icon(face);icon.addPixmap(face,QIcon::Selected);return icon;} // the same face when selected, not a dimmed copy
}
void SkinsPage::refresh(){m_list->clear();m_list->setIconSize(QSize(32,32));
    try{m_skins=SkinManager(m_root).skins();for(const auto &v:m_skins){auto o=v.toObject();new QListWidgetItem(headIcon(QImage(o["file"].toString())),o["name"].toString()+" · "+(o["variant"].toString()=="slim"?key("Slim"):key("Classic")),m_list);}
        if(m_list->count())m_list->setCurrentRow(0);else{m_details->setText(key("Import a 64 × 64 or 64 × 32 PNG to start your skin library."));m_preview->setMessage(key("Skins"));}}catch(const std::exception &e){m_details->setText(Language::message(QString::fromUtf8(e.what())));}
    // The skin the Microsoft account wears now, from Mojang's profile service (cached), first in the list.
    const auto account=m_accounts->active();if(account.uuid.isEmpty())return;
    if(account.type!="msa"){showDefaultSkin(account.name,account.uuid);return;}
    const auto uuid=QString(account.uuid).remove('-');const auto file=m_root+"/mc/cache/skins/"+uuid+".png";const auto name=account.name,id=account.uuid;
    auto watcher=new QFutureWatcher<QString>(this);
    connect(watcher,&QFutureWatcher<QString>::finished,this,[this,watcher,file,name,id]{
        const auto variant=watcher->result();watcher->deleteLater();const QImage skin(file);if(skin.isNull())return;
        m_accounts->setSkinHead(id,SkinManager::head(skin)); // the account's picture follows the skin it wears
        for(int n=0;n<m_skins.size();++n)if(m_skins[n].toObject()["current"].toBool())return;
        m_skins.prepend(QJsonObject{{"name",name+" · "+key("Current skin")},{"file",file},{"variant",variant},{"current",true}});
        m_list->insertItem(0,new QListWidgetItem(headIcon(skin),name+" · "+key("Current skin")+" (Microsoft)"));m_list->setCurrentRow(0);});
    watcher->setFuture(QtConcurrent::run(Ui::backgroundPool(),[uuid,file]{return SkinManager::fetchProfileSkin(uuid,file);}));
}
void SkinsPage::showSkin(){int index=m_list->currentRow();const bool valid=index>=0&&index<m_skins.size();if(m_remove)m_remove->setEnabled(valid&&m_skins[index].toObject().contains("id")&&!m_skins[index].toObject()["current"].toBool());if(!valid)return;auto o=m_skins[index].toObject();QImage image(o["file"].toString());m_preview->setSkin(image,o["variant"].toString()=="slim");m_details->setText(o["name"].toString());}
void SkinsPage::removeSkin(){
    const int index=m_list->currentRow();if(index<0||index>=m_skins.size())return;const auto o=m_skins[index].toObject();if(!o.contains("id")||o["current"].toBool())return;
    if(QMessageBox::question(this,"EBALIA",key("Delete the skin “%1” from the library? The PNG you imported stays where it was.").arg(o["name"].toString()))!=QMessageBox::Yes)return;
    try{SkinManager(m_root).removeSkin(QFileInfo(o["file"].toString()).completeBaseName());}catch(const std::exception &e){QMessageBox::warning(this,"EBALIA",Language::message(QString::fromUtf8(e.what())));return;}
    // Out of the list without reading the library and the account's skin again; the next one is shown.
    m_skins.removeAt(index);delete m_list->takeItem(index);
    if(m_list->count())m_list->setCurrentRow(std::min(index,m_list->count()-1));
    else{m_details->setText(key("Import a 64 × 64 or 64 × 32 PNG to start your skin library."));m_preview->setMessage(key("Skins"));m_remove->setEnabled(false);}
}
void SkinsPage::importSkin(){
    auto file=QFileDialog::getOpenFileName(this,key("Import PNG skin"),{},"PNG (*.png)");if(file.isEmpty())return;QDialog dialog(this);dialog.setWindowTitle(key("Import PNG skin"));QFormLayout form(&dialog);QLineEdit name(QFileInfo(file).completeBaseName());QComboBox variant;variant.addItem(key("Classic"),"classic");variant.addItem(key("Slim"),"slim");form.addRow(key("Name"),&name);form.addRow(key("Model"),&variant);QDialogButtonBox buttons(QDialogButtonBox::Save|QDialogButtonBox::Cancel);form.addRow(&buttons);connect(&buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);connect(&buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);if(Ui::openWindow(dialog)!=QDialog::Accepted)return;
    try{SkinManager(m_root).importSkin(file,name.text(),variant.currentData().toString());refresh();}catch(const std::exception &e){QMessageBox::warning(this,"EBALIA",Language::message(QString::fromUtf8(e.what())));}
}
void SkinsPage::apply(bool local){
    int index=m_list->currentRow();if(index<0||index>=m_skins.size())return;auto skin=m_skins[index].toObject();auto file=skin["file"].toString(),variant=skin["variant"].toString();
    if(local){
        QStringList names;QList<McInstance> targets;for(const auto &i:m_instances->instances())if(i.ready&&!m_instances->isRunning(i.dir)&&!m_instances->isInstalling(i.dir)){names<<i.name+" · "+i.mcVersion;targets<<i;}
        if(names.isEmpty()){QMessageBox::information(this,"EBALIA",key("Install an instance and close the game before applying a local skin."));return;}
        bool ok;auto value=QInputDialog::getItem(this,key("Apply locally to an instance"),key("Instance"),names,0,false,&ok);if(!ok)return;auto target=targets[names.indexOf(value)];auto jar=m_instances->mcDir()+"/versions/"+target.mcVersion+"/"+target.mcVersion+".jar";
        m_work(key("Applying skin…"),[=]{SkinManager::applyLocal(file,variant,target.dir,jar);SkinManager::markLocal(target.dir,"own");return QJsonObject{};},[this](QJsonObject){m_details->setText(key("Skin applied. Start the game to see it."));});return;
    }
    auto account=m_accounts->active();if(account.type!="msa"){QMessageBox::information(this,"EBALIA",key("Choose a Microsoft account to apply an online skin."));return;}
    QProgressDialog progress(key("Refreshing session…"),key("Cancel"),0,0,this);progress.setWindowModality(Qt::ApplicationModal);MsAuth auth;QString token,refresh,uuid,name;
    connect(&auth,&MsAuth::loginDone,&progress,[&](const QString &t,const QString &r,const QString &u,const QString &n){token=t;refresh=r;uuid=u;name=n;progress.accept();});
    connect(&auth,&MsAuth::loginFailed,&progress,[&](const QString &error){progress.reject();QMessageBox::warning(this,"EBALIA",Language::message(error));});
    QTimer::singleShot(0,&auth,[&]{auth.refresh(account.refreshToken);});progress.exec();auth.cancel();if(token.isEmpty())return;
    m_accounts->addAccount({"msa",name,uuid,token,refresh});
    m_work(key("Applying skin…"),[=]{SkinManager::upload(file,variant,token);return QJsonObject{};},[this,file,uuid](QJsonObject){m_accounts->setSkinHead(uuid,SkinManager::head(QImage(file)));m_details->setText(key("Skin applied. Start the game to see it."));});
}
void SkinsPage::applyAll(){
    const int index=m_list->currentRow();if(index<0||index>=m_skins.size())return;const auto skin=m_skins[index].toObject();
    const auto file=skin["file"].toString(),variant=skin["variant"].toString(),name=skin["name"].toString().section(" — ",0,0);
    // Installed and closed: now. Open, being installed or not installed yet: when they start (McInstanceManager::launch).
    struct Target{QString name,dir,jar;bool now;};QList<Target> targets;
    for(const auto &i:m_instances->instances())targets<<Target{i.name,i.dir,m_instances->mcDir()+"/versions/"+i.mcVersion+"/"+i.mcVersion+".jar",i.ready&&!m_instances->isRunning(i.dir)&&!m_instances->isInstalling(i.dir)};
    const auto root=m_root;
    m_work(key("Applying skin…"),[=]{
        SkinManager::setDefaultLocal(root,file,variant,name);const auto chosen=SkinManager::defaultLocal(root);
        int done=0,later=0;QJsonArray failed;
        for(const auto &t:targets){
            if(!t.now){SkinManager::markLocal(t.dir,"default");++later;continue;}
            try{SkinManager::applyLocal(chosen["file"].toString(),variant,t.dir,t.jar);SkinManager::markLocal(t.dir,"default",chosen["stamp"].toString());++done;}
            catch(const std::exception &e){SkinManager::markLocal(t.dir,"default");failed.append(QJsonObject{{"name",t.name},{"error",QString::fromUtf8(e.what())}});}
        }
        return QJsonObject{{"done",done},{"later",later},{"failed",failed}};
    },[this](QJsonObject result){
        QStringList lines{result["done"].toInt()+result["later"].toInt()>0?key("Skin applied to %1 instances. New instances get it as soon as they are installed; the ones that are open or still installing, when they start.").arg(result["done"].toInt())
                                                                           :key("Saved as the local skin of all instances: new instances get it as soon as they are installed.")};
        for(const auto &f:result["failed"].toArray())lines<<key("Could not apply it to %1: %2").arg(f.toObject()["name"].toString(),Language::message(f.toObject()["error"].toString()));
        m_details->setText(lines.join('\n'));showDefaultLocal();
    });
}
void SkinsPage::showDefaultLocal(){
    const auto chosen=SkinManager::defaultLocal(m_root);
    m_defaultLocal->setText(chosen.isEmpty()?key("Local skin of all instances: none"):key("Local skin of all instances: %1").arg(chosen["name"].toString()));
}

// Profiles without Minecraft (no premium) wear one of the game's default skins (see SkinManager::defaultSkin).
void SkinsPage::showDefaultSkin(const QString &name,const QString &uuid){
    bool slim=false;QString skin;const auto image=SkinManager::defaultSkin(m_root,uuid,&slim,&skin);if(image.isNull())return;
    const auto file=m_root+"/mc/cache/skins/default-"+QString(uuid).remove('-')+".png";QDir().mkpath(QFileInfo(file).absolutePath());image.save(file,"PNG");
    m_accounts->setSkinHead(uuid,SkinManager::head(image));
    const auto title=name+" · "+key("Current skin")+" ("+QString(skin).replace(0,1,skin.left(1).toUpper())+")";
    m_skins.prepend(QJsonObject{{"name",title+" — "+key("No premium: in-game default skin. Apply a skin locally to change it.")},{"file",file},{"variant",slim?"slim":"classic"},{"current",true}});
    m_list->insertItem(0,new QListWidgetItem(headIcon(image),title));m_list->setCurrentRow(0);
}
