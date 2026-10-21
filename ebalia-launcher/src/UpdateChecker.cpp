#include "UpdateChecker.hpp"
#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVersionNumber>
#include <QRegularExpression>
#include <QTimer>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDesktopServices>
#include "Language.hpp"
#include "ModPage.hpp"
#include "Ui.hpp"
UpdateChecker::UpdateChecker(QObject *parent):QObject(parent){m_nam.setTransferTimeout(15000);}
bool UpdateChecker::newerStable(QString tag,const QString &current){
 if(tag.startsWith('v'))tag.remove(0,1);
 static const QRegularExpression semver("^[0-9]+\\.[0-9]+\\.[0-9]+$");
 return semver.match(tag).hasMatch() && QVersionNumber::compare(QVersionNumber::fromString(tag),QVersionNumber::fromString(current.startsWith('v')?current.mid(1):current))>0;
}
QString UpdateChecker::releasePage(const QJsonObject &release,bool gitgud){
 const QUrl url(gitgud?release["_links"].toObject()["self"].toString():release["html_url"].toString());
 const bool pathAllowed=gitgud ? url.path().startsWith("/castigarse/launcher/-/releases/")
     : url.path().startsWith("/ebalia-real/launcher/releases/tag/");
 if(url.scheme()!="https"||url.host()!=(gitgud?"gitgud.io":"github.com")||!pathAllowed||!url.userInfo().isEmpty())return {};
 return url.toString();
}
void UpdateChecker::check(){if(m_busy)return;if(qEnvironmentVariableIsSet("EBALIA_NO_NETWORK")){emit checkFailed("Network disabled.");return;}m_busy=true;fetch(false);}
void UpdateChecker::fetch(bool gitgud){
 QNetworkRequest request{QUrl(gitgud?"https://gitgud.io/api/v4/projects/castigarse%2Flauncher/releases?per_page=20":"https://api.github.com/repos/ebalia-real/launcher/releases?per_page=20")};
 request.setHeader(QNetworkRequest::UserAgentHeader,"EBALIA-Launcher/"+QCoreApplication::applicationVersion());
 auto reply=m_nam.get(request);
 connect(reply,&QNetworkReply::downloadProgress,reply,[reply](qint64 n,qint64){if(n>2*1024*1024)reply->abort();});
 connect(reply,&QNetworkReply::finished,this,[this,reply,gitgud]{
  reply->deleteLater();const auto doc=QJsonDocument::fromJson(reply->readAll());
  if(reply->error()!=QNetworkReply::NoError||!doc.isArray()){if(!gitgud){fetch(true);return;}m_busy=false;emit checkFailed("No se pudo consultar GitHub ni GitGud.");return;}
  QString best=QCoreApplication::applicationVersion(),page;QJsonObject selected;
  for(auto v:doc.array()){auto release=v.toObject();auto tag=release["tag_name"].toString();if(release["draft"].toBool()||release["prerelease"].toBool()||release["upcoming_release"].toBool())continue;auto url=releasePage(release,gitgud);if(url.isEmpty()||!newerStable(tag,best))continue;best=tag;page=url;selected=release;}
  if(page.isEmpty()&&!gitgud){fetch(true);return;}
  if(page.isEmpty()){m_busy=false;emit upToDate();return;}m_releaseUrl=page;m_release=selected;fetchTranslation(gitgud,best);
 });
}

// The changelog in the launcher's language as that version has it (resources/changelog/<language>.md), from the same
// host as the release. Whatever happens, the update is announced: without it the English notes are shown.
void UpdateChecker::fetchTranslation(bool gitgud,const QString &tag){
 m_translated.clear();
 static const QRegularExpression version(R"(^v\d+\.\d+\.\d+$)"),language(R"(^[a-z]{2}$)");
 if(Language::current=="en"||!language.match(Language::current).hasMatch()||!version.match(tag).hasMatch()){m_busy=false;emit updateAvailable(tag);return;}
 const auto path="ebalia-launcher/resources/changelog/"+Language::current+".md";
 QNetworkRequest request{QUrl(gitgud?"https://gitgud.io/castigarse/launcher/-/raw/"+tag+"/"+path:"https://raw.githubusercontent.com/ebalia-real/launcher/"+tag+"/"+path)};
 request.setHeader(QNetworkRequest::UserAgentHeader,"EBALIA-Launcher/"+QCoreApplication::applicationVersion());
 auto reply=m_nam.get(request);
 connect(reply,&QNetworkReply::downloadProgress,reply,[reply](qint64 n,qint64){if(n>1024*1024)reply->abort();});
 connect(reply,&QNetworkReply::finished,this,[this,reply,tag]{
  reply->deleteLater();if(reply->error()==QNetworkReply::NoError)m_translated=QString::fromUtf8(reply->readAll());
  m_busy=false;emit updateAvailable(tag);
 });
}
QString UpdateChecker::notesFor(const QString &current,const QString &target,bool *translated) const{
 const auto english=releaseNotes(m_release,current,target);
 const auto versions=[](const QString &notes){QStringList out;static const QRegularExpression heading(R"((?m)^#\s+v?(\d+\.\d+\.\d+))");for(auto it=heading.globalMatch(notes);it.hasNext();)out<<it.next().captured(1);return out;};
 if(const auto local=releaseNotes(m_translated,current,target);!m_translated.isEmpty()&&!local.isEmpty()&&versions(local)==versions(english)){if(translated)*translated=true;return local;}
 if(translated)*translated=false;return english;
}
QString UpdateChecker::releaseNotes(const QJsonObject &release,const QString &current,const QString &target){
 // GitHub gives the notes as "body", GitGud as "description".
 return releaseNotes(release.contains("body")?release["body"].toString():release["description"].toString(),current,target);
}
QString UpdateChecker::releaseNotes(const QString &markdown,const QString &current,const QString &target){
 auto body=markdown;body.replace("\r\n","\n");
 static const QRegularExpression heading(R"(^#\s+v?(\d+\.\d+\.\d+)(?:\s+\(\d{4}-\d{2}-\d{2}\))?\s*$)");
 QStringList kept;bool keep=false,versions=false;
 for(const auto &line:body.split('\n')){
  if(const auto m=heading.match(line);m.hasMatch()){versions=true;keep=newerStable(m.captured(1),current)&&!newerStable(m.captured(1),target);}
  if(keep)kept<<line;
 }
 return (versions?kept.join('\n'):body).trimmed(); // notes without version headings are shown whole
}

QDialog *UpdateChecker::showUpdatePrompt(QWidget *parent,const QString &version,std::function<void()> install,const QString &notes,bool translated) {
 auto box=new QDialog(parent);box->setObjectName("launcherUpdatePrompt");box->setAttribute(Qt::WA_DeleteOnClose);box->setModal(false);
 box->setWindowTitle(Language::text("Actualización disponible","Update available","Atualização disponível"));
 auto layout=new QVBoxLayout(box);layout->setContentsMargins(22,20,22,18);layout->setSpacing(14);
 auto head=new QHBoxLayout;head->setSpacing(14);layout->addLayout(head);
 auto badge=new QLabel;badge->setObjectName("statIcon");badge->setFixedSize(42,42);badge->setAlignment(Qt::AlignCenter);badge->setPixmap(Ui::pixmap("download",22,Ui::accent()));head->addWidget(badge,0,Qt::AlignTop);
 auto words=new QVBoxLayout;words->setSpacing(6);head->addLayout(words,1);
 auto title=new QLabel(Language::text("EBALIA Launcher %1 está disponible.","EBALIA Launcher %1 is available.","O EBALIA Launcher %1 está disponível.").arg(version));
 title->setObjectName("updateTitle");title->setTextFormat(Qt::PlainText);title->setWordWrap(true);title->setStyleSheet("font-size:17px;font-weight:800;");words->addWidget(title);
 auto info=new QLabel(Language::text("El launcher descargará y verificará la nueva versión, se reiniciará y eliminará los archivos de la versión anterior. Tus mundos, mods y ajustes se conservan.","The launcher will download and verify the new version, restart, and remove the previous launcher files. Your worlds, mods and settings are preserved.","O launcher vai baixar e verificar a nova versão, reiniciar e remover os arquivos da versão anterior. Seus mundos, mods e configurações são mantidos."));
 info->setObjectName("muted");info->setWordWrap(true);words->addWidget(info);
 // What the new version brings, as its release notes say.
 if(!notes.isEmpty()){
  auto heading=new QLabel(Language::key("What's new"));heading->setObjectName("updateNotesTitle");heading->setStyleSheet("font-size:14px;font-weight:800;");layout->addWidget(heading);
  auto view=new QTextBrowser;view->setObjectName("updateNotes");view->setOpenExternalLinks(true);view->setMinimumHeight(220);
  view->document()->setIndentWidth(16);
  view->document()->setDefaultStyleSheet("h3{font-size:15px;font-weight:800;margin-top:6px;margin-bottom:6px;}li{margin-bottom:6px;}a{color:#7bd96a;}");
  // Each version's "# x.y.z" becomes a small heading: rich text gives level one headings a size of their own.
  auto markdown=notes;static const QRegularExpression version(R"(^#\s+)",QRegularExpression::MultilineOption);markdown.replace(version,"### ");
  view->setHtml(ModPage::html(markdown,true));layout->addWidget(view,1);
  if(Language::current!="en"&&!translated){auto english=new QLabel(Language::key("These notes are in English."));english->setObjectName("muted");english->setStyleSheet("font-size:12px;");layout->addWidget(english);}
 }
 auto buttons=new QHBoxLayout;buttons->setSpacing(10);buttons->addStretch();layout->addLayout(buttons);
 auto later=new QPushButton(Language::text("Más tarde","Later","Mais tarde"));later->setObjectName("updateLater");later->setDefault(true);buttons->addWidget(later);
 auto download=new QPushButton(Ui::icon("download",Qt::white)," "+Language::text("Actualizar y reiniciar","Update and restart","Atualizar e reiniciar"));download->setObjectName("updateInstall");download->setProperty("play",true);buttons->addWidget(download);
 QObject::connect(later,&QPushButton::clicked,box,&QDialog::reject);
 QObject::connect(download,&QPushButton::clicked,box,[box,install]{box->accept();if(install)QTimer::singleShot(0,qApp,install);});
 Ui::fitToScreen(box,notes.isEmpty()?QSize(520,220):QSize(640,600));
 box->show();return box;
}
