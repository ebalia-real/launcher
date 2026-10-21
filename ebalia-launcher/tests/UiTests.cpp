#include <QtTest>
#include <QtWidgets>
#include <QtConcurrent>
#include "Uninstaller.hpp"
#include "MainWindow.hpp"
#include "VersionManager.hpp"
#include "Language.hpp"
#include "McInstanceManager.hpp"
#include "CreateInstanceDialog.hpp"
#include "ModRepository.hpp"
#include "SetupDialog.hpp"
#include "AccountManager.hpp"
#include "HomeBanner.hpp"
#include "CreatorDialog.hpp"
#include "PatreonAuth.hpp"
#include "UpdateChecker.hpp"
#include "PatreonNewsPage.hpp"
#include "Ui.hpp"
#include "BedrockPage.hpp"
#include "BedrockInstaller.hpp"
#include "LauncherUpdater.hpp"
#include "MsAuth.hpp"
#include "InstanceIcons.hpp"
#include "InstanceViews.hpp"
#include "ServerList.hpp"
#include "SkinsPage.hpp"
#include "SkinViewer.hpp"
#include "SkinManager.hpp"
#include "TitleBar.hpp"
#include "InstanceContent.hpp"
#include "LogWindow.hpp"
#include "Archive.hpp"
#include <QTcpSocket>
#include <QUrlQuery>
#ifdef __GLIBC__
#include <malloc.h>
#endif
class UiTests:public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void captureUrl(const QUrl &url){lastUrl=url;}
private:
    QUrl lastUrl;
private slots:
    void microsoftBrowserCallback() {
        const auto previous=qgetenv("EBALIA_MS_CLIENT_ID");
        const bool wasSet=qEnvironmentVariableIsSet("EBALIA_MS_CLIENT_ID");
        auto restore=qScopeGuard([&]{QDesktopServices::unsetUrlHandler("https");if(wasSet)qputenv("EBALIA_MS_CLIENT_ID",previous);else qunsetenv("EBALIA_MS_CLIENT_ID");});
        qputenv("EBALIA_MS_CLIENT_ID","00000000-0000-0000-0000-000000000001");
        QDesktopServices::setUrlHandler("https",this,"captureUrl");lastUrl=QUrl();
        MsAuth auth;QSignalSpy failed(&auth,&MsAuth::loginFailed);QSignalSpy done(&auth,&MsAuth::loginDone);
        auth.startLogin();
        QCOMPARE(lastUrl.host(),QString("login.microsoftonline.com"));
        QUrlQuery query(lastUrl);QCOMPARE(query.queryItemValue("code_challenge_method"),QString("S256"));
        QCOMPARE(query.queryItemValue("response_type"),QString("code"));
        QVERIFY(query.queryItemValue("state").size()>=32);QVERIFY(query.queryItemValue("code_challenge").size()>=43);
        const QUrl redirect(query.queryItemValue("redirect_uri"));QCOMPARE(redirect.host(),QString("localhost"));
        auto callback=[&](const QString &state){
            QTcpSocket socket;socket.connectToHost(QHostAddress::LocalHost,redirect.port());
            if(!socket.waitForConnected(2000))return QByteArray();
            socket.write("GET /?error=access_denied&state="+state.toUtf8()+" HTTP/1.1\r\nHost: localhost\r\n\r\n");socket.flush();
            QElapsedTimer timer;timer.start();QByteArray response;
            while(timer.elapsed()<2000&&!response.contains("\r\n\r\n")){QCoreApplication::processEvents();response+=socket.readAll();QTest::qWait(5);}
            return response;
        };
        QVERIFY(callback("wrong-state").startsWith("HTTP/1.1 400"));QCOMPARE(failed.count(),0);QCOMPARE(done.count(),0);
        QVERIFY(callback(query.queryItemValue("state")).startsWith("HTTP/1.1 200"));QCOMPARE(failed.count(),1);QCOMPARE(done.count(),0);
        auth.cancel();
    }
    // Like closing the launcher: what is still downloading stops, so the tests end with their last result.
    void cleanupTestCase(){ModRepository::cancelAll();QThreadPool::globalInstance()->waitForDone(5000);}
    void initTestCase(){
#ifdef __GLIBC__
        mallopt(M_MMAP_THRESHOLD,256*1024);mallopt(M_ARENA_MAX,4); // as the launcher does in main()
#endif
        QSettings().setValue("ui/tutorialSeen",true);QApplication::setStyle("Fusion");QCoreApplication::setOrganizationName("EBALIA-test");QCoreApplication::setApplicationName("UI-test");QSettings().setValue("ui/tutorialSeen",true);qputenv("EBALIA_NO_NETWORK","1");}

    void bedrockVersionsAndProfile() {
#ifdef Q_OS_WIN
        QSKIP("Windows plays Minecraft for Windows from the Store; no Minecraft Linux files.");
#else
        // Minecraft Linux's files: a downloaded version, one without the game library, and the player's profile with
        // the compatibility module. EBALIA lists the playable version and writes its own profile with that module.
        QTemporaryDir data;qputenv("EBALIA_BEDROCK_GAMEDATA",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_BEDROCK_GAMEDATA");});
        QVERIFY(QDir().mkpath(data.path()+"/versions/1.26.51.1/lib/x86_64"));QFile lib(data.path()+"/versions/1.26.51.1/lib/x86_64/libminecraftpe.so");QVERIFY(lib.open(QIODevice::WriteOnly));lib.close();
        QVERIFY(QDir().mkpath(data.path()+"/versions/broken/lib/x86_64"));
        {QSettings names(data.path()+"/versions/versions.ini",QSettings::IniFormat);names.setValue("1.26.51.1/versionName","1.26.51.1");}
        {QSettings profiles(data.path()+"/profiles/profiles.ini",QSettings::IniFormat);profiles.setValue("selected","Mine");
         profiles.beginGroup("Mine");profiles.setValue("version","googleplay");profiles.beginWriteArray("mods");profiles.setArrayIndex(0);profiles.setValue("path","/mods/mcpelauncher-updates/1.26.45.1/x86_64/");profiles.endArray();profiles.endGroup();}
        const auto versions=BedrockInstaller::versions();QCOMPARE(versions.size(),1);QCOMPARE(versions.first().dir,QString("1.26.51.1"));QCOMPARE(versions.first().abi,QString("x86_64"));
        BedrockInstaller::writeProfile("1.26.51.1");
        QSettings profiles(data.path()+"/profiles/profiles.ini",QSettings::IniFormat);
        QCOMPARE(profiles.value("EBALIA/version").toString(),QString("dir 1.26.51.1"));QCOMPARE(profiles.value("EBALIA/arch").toString(),QString("x86_64"));
        profiles.beginGroup("EBALIA");QCOMPARE(profiles.beginReadArray("mods"),1);profiles.setArrayIndex(0);QCOMPARE(profiles.value("path").toString(),QString("/mods/mcpelauncher-updates/1.26.45.1/x86_64/"));profiles.endArray();profiles.endGroup();
        QCOMPARE(profiles.value("selected").toString(),QString("Mine"));QCOMPARE(profiles.value("Mine/version").toString(),QString("googleplay"));
        QVERIFY_EXCEPTION_THROWN(BedrockInstaller::writeProfile("broken"),std::runtime_error);
        // What the end of a game log means.
        QCOMPARE(BedrockInstaller::problem("...\nSomething went wrong\n"),QString("licence"));QCOMPARE(BedrockInstaller::problem("Failed to load Minecraft library"),QString("incompatible"));QVERIFY(BedrockInstaller::problem("Bye").isEmpty());
#endif
    }
    void bedrockPageStates() {
#ifdef Q_OS_WIN
        QSKIP("Detection on Windows asks the Store packages of this computer.");
#elif defined(Q_OS_MACOS)
        if(!BedrockInstaller::unsupportedReason().isEmpty())QSKIP("This Mac cannot run Minecraft Linux.");
#endif
        QTemporaryDir data,game;qputenv("EBALIA_BEDROCK_GAMEDATA",game.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_BEDROCK_GAMEDATA");qunsetenv("EBALIA_BEDROCK_RUNTIME");});
        // Not installed: Install, no Play.
        qputenv("EBALIA_BEDROCK_RUNTIME","");
        {BedrockPage page(data.path());page.resize(700,700);page.show();
         auto launch=page.findChild<QPushButton*>("bedrockPlay");QVERIFY(launch);
         QTRY_VERIFY(!page.findChild<QLabel*>("bedrockProviderStatus")->text().contains("…"));
         QVERIFY(!launch->isEnabled());QVERIFY(page.findChild<QPushButton*>("bedrockManager")->isHidden());
         if(!QStandardPaths::findExecutable("flatpak").isEmpty()||QSysInfo::productType()=="macos")QVERIFY(page.findChild<QPushButton*>("bedrockInstall")->isVisible());
         // Opening the page again does not look again; "Check again" does.
         auto status=page.findChild<QLabel*>("bedrockProviderStatus");const auto found=status->text();page.hide();page.show();QCOMPARE(status->text(),found);
         page.findChild<QPushButton*>("bedrockDetect")->click();QVERIFY(status->text().contains("…"));QTRY_VERIFY(!status->text().contains("…"));}
        // Installed with two downloaded versions: both listed, Play enabled, no Install.
        qputenv("EBALIA_BEDROCK_RUNTIME","--user");
        for(const auto version:{"1.21.100.1","1.26.51.1"}){QVERIFY(QDir().mkpath(game.path()+"/versions/"+version+"/lib/x86_64"));QFile lib(game.path()+"/versions/"+version+"/lib/x86_64/libminecraftpe.so");QVERIFY(lib.open(QIODevice::WriteOnly));}
        BedrockPage page(data.path());page.resize(700,700);page.show();
        auto launch=page.findChild<QPushButton*>("bedrockPlay");auto version=page.findChild<QComboBox*>("bedrockVersion");
        QTRY_VERIFY(launch->isEnabled());QCOMPARE(version->count(),2);QCOMPARE(version->itemData(0).toString(),QString("1.26.51.1"));
        QVERIFY(version->isVisible());QVERIFY(page.findChild<QPushButton*>("bedrockInstall")->isHidden());QVERIFY(page.findChild<QPushButton*>("bedrockManager")->isVisible());QVERIFY(page.findChild<QPushButton*>("bedrockUninstall")->isVisible());
    }
    void lostUninstallCancelPreservesFiles() {
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());
        VersionManager versions;versions.loadVersions();QVERIFY(!versions.getVersions().isEmpty());
        for(const auto &v:versions.getVersions()) {
            auto dir=versions.getInstallPath(v);QDir().mkpath(dir);
            QFile marker(dir+"/.installed.json");QVERIFY(marker.open(QIODevice::WriteOnly));marker.write("{}");marker.close();
        }
        MainWindow window;window.show();window.showPage(MainWindow::Lost);
        auto uninstall=window.findChild<QPushButton*>("lostUninstall");QVERIFY(uninstall);QVERIFY(uninstall->isVisible());QVERIFY(uninstall->isEnabled());
        QTimer::singleShot(0,[] {if(auto box=qobject_cast<QMessageBox*>(QApplication::activeModalWidget()))box->done(QMessageBox::No);});
        uninstall->click();
        for(const auto &v:versions.getVersions())QVERIFY(versions.isVersionInstalled(v));
        QVERIFY(!window.findChild<QWidget*>("backgroundTaskProgress"));
    }
    void asynchronousArtwork(){
        const QString path=":/art/backgrounds/village.png";const QSize bounds(137,137);QObject receiver;auto removed=new QObject;
        int delivered=0;QPixmap first,second;bool stale=false;
        Ui::loadArt(path,bounds,removed,[&](const QPixmap &){stale=true;});delete removed;
        Ui::loadArt(path,bounds,&receiver,[&](const QPixmap &p){first=p;++delivered;});Ui::loadArt(path,bounds,&receiver,[&](const QPixmap &p){second=p;++delivered;});
        QCOMPARE(delivered,0);QTRY_COMPARE_WITH_TIMEOUT(delivered,2,5000);QVERIFY(!stale);QVERIFY(!first.isNull());QCOMPARE(first.cacheKey(),second.cacheKey());QVERIFY(first.width()<=137&&first.height()<=137);
        Ui::loadArt(path,bounds,&receiver,[&](const QPixmap &p){QCOMPARE(p.cacheKey(),first.cacheKey());++delivered;});QCOMPARE(delivered,3);
    }
    void creatorPopupsAndPatreonSections(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");MainWindow window;window.resize(900,600);window.show();window.showPage(MainWindow::Home);
        auto mods=window.findChild<QPushButton*>("myModsButton");QVERIFY(mods);QTest::mouseClick(mods,Qt::LeftButton);auto dialog=window.findChild<QDialog*>("myModsDialog");QVERIFY(dialog&&dialog->isVisible());
        auto links=dialog->findChildren<QPushButton*>("creatorModLink");QCOMPARE(links.size(),2);QDesktopServices::setUrlHandler("https",this,"captureUrl");
        const QStringList slugs{"ooo-jar","in-your-world"};for(int i=0;i<2;++i){QTest::mouseClick(links[i],Qt::LeftButton);QCOMPARE(lastUrl,QUrl("https://www.curseforge.com/minecraft/mc-mods/"+slugs[i]));QCOMPARE(links[i]->property("store").toString(),QString("curseforge"));}
        QCOMPARE(dialog->findChildren<QFrame*>("creatorModCard").size(),3);QVERIFY(!dialog->findChild<QWidget*>("creatorCollabMods")); // no collaborations known yet
        dialog->close();QTest::mouseClick(window.findChild<QPushButton*>("patreonInviteButton"),Qt::LeftButton);auto invite=window.findChild<QDialog*>("patreonInvite");QVERIFY(invite&&invite->isVisible());QTest::mouseClick(invite->findChild<QPushButton*>("joinPatreon"),Qt::LeftButton);QCOMPARE(lastUrl,QUrl("https://www.patreon.com/EBALIA"));
        auto hide=invite->findChild<QCheckBox*>("hidePatreonInvite");hide->setChecked(true);QVERIFY(QSettings().value("ui/hidePatreonInvite").toBool());QSettings().remove("ui/hidePatreonInvite");invite->close();QDesktopServices::unsetUrlHandler("https");
        window.showPage(MainWindow::News);auto news=window.findChild<QTabWidget*>("newsTabs");QVERIFY(news);QCOMPARE(news->count(),2);news->setCurrentIndex(1);auto sections=window.findChild<QTabWidget*>("patreonNewsTabs");QVERIFY(sections);QCOMPARE(sections->count(),2);
        auto paid=window.findChild<QScrollArea*>("patreonPaidPosts");QVERIFY(paid);QVERIFY(paid->findChildren<QPushButton*>("patreonReadPost").isEmpty());QVERIFY(!window.findChild<PatreonAuth*>()->paidMember());
    }
    void simulatedPatreonMembershipAndCards(){
        PatreonAuth auth;auth.m_verified=true;auth.m_paid=true;auth.m_name="SIMULACIÓN · Miembro de prueba";auth.m_title="Gold";
        auth.m_status="SIMULACIÓN LOCAL · No es una suscripción real";
        QJsonObject post{{"title","Actualización exclusiva · Nuevo mundo"},{"date","2026-10-03"},{"is_public",false},{"url","https://www.patreon.com/EBALIA"},{"content","<p>Una aventura para los miembros de EBALIA.</p><p><b>Novedades:</b> nuevos biomas, criaturas y mejoras.</p><p><a href='https://www.curseforge.com/minecraft/mc-mods/in-your-world'>Ver el mod en CurseForge ↗</a></p><img src='https://c10.patreonusercontent.com/test-preview.png'><p>Gracias por acompañar el desarrollo.</p>"}};
        auth.m_posts=QJsonArray{post};PatreonNewsPage page(&auth);page.setStyleSheet(Ui::styleSheet());page.resize(800,700);page.show();page.findChild<QTabWidget*>("patreonNewsTabs")->setCurrentIndex(1);
        auto paid=page.findChild<QScrollArea*>("patreonPaidPosts");QVERIFY(auth.paidMember());QCOMPARE(paid->findChildren<QFrame*>("patreonPostCard").size(),1);
        auto body=paid->findChild<QTextBrowser*>("patreonPostBody");QVERIFY(body);QImage art(800,360,QImage::Format_RGB32);art.fill(QColor("#427659"));QPainter painter(&art);painter.setPen(Qt::white);painter.setFont(QFont("Sans",28,QFont::Bold));painter.drawText(art.rect(),Qt::AlignCenter,"EBALIA · VISTA PREVIA");painter.end();body->document()->addResource(QTextDocument::ImageResource,QUrl("https://c10.patreonusercontent.com/test-preview.png"),art);
        for(int width:{640,1280}){page.resize(width,720);QTest::qWait(60);QCOMPARE(paid->horizontalScrollBar()->maximum(),0);QCOMPARE(body->horizontalScrollBar()->maximum(),0);auto dir=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!dir.isEmpty()){QDir().mkpath(dir);QVERIFY(page.grab().save(dir+"/patreon-subscriber-"+QString::number(width)+".png"));}}
        QDesktopServices::setUrlHandler("https",this,"captureUrl");QTest::mouseClick(paid->findChild<QPushButton*>("patreonReadPost"),Qt::LeftButton);QCOMPARE(lastUrl,QUrl("https://www.patreon.com/EBALIA"));QDesktopServices::unsetUrlHandler("https");
        auth.m_paid=false;emit auth.changed();QVERIFY(!auth.paidMember());QVERIFY(paid->findChildren<QFrame*>("patreonPostCard").isEmpty());
        auth.m_paid=true;auth.m_verified=false;emit auth.changed();QVERIFY(!auth.paidMember());QVERIFY(paid->findChildren<QFrame*>("patreonPostCard").isEmpty());
    }
    void patreonTiersRefreshWithoutRestart(){
        PatreonAuth auth;PatreonNewsPage page(&auth);page.show();
        auto label=page.findChild<QLabel*>("patreonAvailableTiers");QVERIFY(label);
        for(const QString &name:{QString("Exclusive Chad"),QString("Explicit Chad"),QString("SENIOR CHAD"),QString("Nuevo nivel creado después")}){
            auth.m_availableTiers.append(QJsonObject{{"id",QString::number(auth.m_availableTiers.size()+1)},{"title",name},{"amount_cents",250}});
            auth.m_verified=true;auth.m_paid=true;auth.m_title=name;auth.m_tierIds={QString::number(auth.m_availableTiers.size())};emit auth.changed();
            QVERIFY(label->text().contains(name));QVERIFY(page.findChild<QLabel*>("patreonMembership")->text().contains(name));QCOMPARE(auth.tierIds().size(),1);
        }
        auth.m_paid=false;emit auth.changed();QVERIFY(auth.tierIds().isEmpty());
    }
    void firstRunTourAndReplay(){
        QSettings().setValue("ui/tutorialSeen",false);QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());
        MainWindow window;window.resize(640,480);window.show();
        QTRY_VERIFY(window.findChild<QWidget*>("interactiveTour"));
        auto tour=window.findChild<QWidget*>("interactiveTour");auto next=tour->findChild<QPushButton*>("tourNext");
        QVERIFY(next);QVERIFY(!tour->findChild<QPushButton*>("tourBack")->isEnabled());
        // The step's text is written letter by letter, laid out whole from the start; the speaker turns Silence's sounds off.
        auto body=tour->findChild<QLabel*>("tourBody");QVERIFY(body->text().contains("color:transparent"));const auto cardSize=tour->findChild<QFrame*>("tourCard")->size();
        QTRY_VERIFY_WITH_TIMEOUT(!body->text().contains("color:transparent\">B")&&body->text().endsWith("<span style=\"color:transparent\"></span>"),8000);QCOMPARE(tour->findChild<QFrame*>("tourCard")->size(),cardSize);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/tour-step.png");
        auto sound=tour->findChild<QToolButton*>("tourSound");QVERIFY(sound);const bool was=sound->isChecked();sound->click();QCOMPARE(QSettings().value("ui/sounds",true).toBool(),!was);sound->click();
        for(int i=0;i<13;++i){next->click();QTest::qWait(10);QVERIFY(tour->rect().contains(tour->findChild<QFrame*>("tourCard")->geometry()));}
        next->click();QTRY_VERIFY(!window.findChild<QWidget*>("interactiveTour"));QVERIFY(QSettings().value("ui/tutorialSeen").toBool());
        window.showPage(MainWindow::Guide);window.findChild<QPushButton*>("startInteractiveTour")->click();
        QTRY_VERIFY(window.findChild<QWidget*>("interactiveTour"));window.findChild<QPushButton*>("tourSkip")->click();
        QTRY_VERIFY(!window.findChild<QWidget*>("interactiveTour"));window.close();
        MainWindow second;second.show();QTest::qWait(450);QVERIFY(!second.findChild<QWidget*>("interactiveTour"));
    }
    void updatePackageValidation(){
        QVERIFY(LauncherUpdater::trustedAsset("https://github.com/ebalia-real/launcher/releases/download/v1.2.0/ebalia-windows-x64.zip","v1.2.0"));
        for(const QString &url:{QString("https://evil.test/ebalia-real/launcher/releases/download/v1.2.0/x.zip"),QString("https://github.com/other/launcher/releases/download/v1.2.0/x.zip"),QString("https://github.com/ebalia-real/launcher/releases/download/v1.1.0/x.zip"),QString("https://github.com/ebalia-real/launcher/releases/download/v1.2.0/../x.zip")})QVERIFY(!LauncherUpdater::trustedAsset(url,"v1.2.0"));
        const auto hash=QByteArray(64,'a');
        QCOMPARE(LauncherUpdater::checksum(hash+"  file.zip\n","file.zip"),hash);
        QVERIFY_EXCEPTION_THROWN(LauncherUpdater::checksum(hash+"  wrong.zip\n","file.zip"),std::runtime_error);
        QVERIFY_EXCEPTION_THROWN(LauncherUpdater::checksum(hash+"  file.zip\n"+hash+"  file.zip\n","file.zip"),std::runtime_error);
        QCOMPARE(LauncherUpdater::assetName("linux","x86_64"),QString("ebalia-linux-x64-update.zip"));
        QVERIFY(LauncherUpdater::assetName("windows","arm64").isEmpty());
        QCOMPARE(LauncherUpdater::assetName("appimage","x86_64"),QString("ebalia-linux-x86_64.AppImage"));
        QCOMPARE(LauncherUpdater::assetName("macos","x86_64"),QString("ebalia-macos-x64.dmg"));
    }
    void updateReplacementAndCleanup(){
#ifdef Q_OS_MACOS
        QSKIP("The macOS updater replaces complete app bundles; this fixture covers file-manifest packages.");
#else
        QTemporaryDir temp;const auto root=temp.path()+"/data",destination=temp.path()+"/installed";
        const auto stage=root+"/updates/1.2.0-11111111-1111-1111-1111-111111111111";
#ifdef Q_OS_WIN
        const QString relative="ebalia-launcher.exe";
#else
        const QString relative="bin/ebalia-launcher";
#endif
        auto write=[](const QString &path,const QByteArray &bytes){QDir().mkpath(QFileInfo(path).absolutePath());QFile file(path);if(!file.open(QIODevice::WriteOnly))return false;return file.write(bytes)==bytes.size();};
        auto manifest=[](const QStringList &files){return QJsonDocument(QJsonObject{{"format",1},{"files",QJsonArray::fromStringList(files)}}).toJson();};
        QVERIFY(write(destination+"/"+relative,"old binary"));
        QVERIFY(write(destination+"/old-library.dll","old library"));
        QVERIFY(write(destination+"/worlds/keep.txt","my world"));
        QVERIFY(write(destination+"/update-files.json",manifest({relative,"old-library.dll","update-files.json"})));
        const auto source=stage+"/app";QDir().mkpath(QFileInfo(source+"/"+relative).absolutePath());
        QVERIFY(QFile::copy(QString::fromUtf8(EBALIA_UPDATE_HELPER),source+"/"+relative));
        QVERIFY(write(source+"/update-files.json",manifest({relative,"update-files.json"})));
        QJsonObject prepared{{"version","1.2.0"},{"stage",stage},{"destination",destination},{"executable",source+"/"+relative},{"nix",false}};
        QVERIFY(LauncherUpdater::applyPrepared(prepared,root));
        QTRY_VERIFY_WITH_TIMEOUT(QFileInfo::exists(root+"/update-child-started"),5000);
        QVERIFY(!QFileInfo::exists(destination+"/old-library.dll"));
        QFile world(destination+"/worlds/keep.txt");QVERIFY(world.open(QIODevice::ReadOnly));QCOMPARE(world.readAll(),QByteArray("my world"));
        QVERIFY(QFileInfo::exists(stage+"/previous/old-library.dll"));
        QVERIFY(write(root+"/updates/cleanup.json",QJsonDocument(QJsonObject{{"version",QCoreApplication::applicationVersion()},{"folders",QJsonArray{QFileInfo(stage).fileName()}}}).toJson()));
        LauncherUpdater::cleanupInstalled(root);
        QVERIFY(!QFileInfo::exists(stage));QVERIFY(QFileInfo::exists(destination+"/"+relative));QVERIFY(QFileInfo::exists(destination+"/worlds/keep.txt"));
#endif
    }
    void updateFailureRestoresPreviousFiles(){
#ifdef Q_OS_MACOS
        QSKIP("Manifest replacement fixture applies to Linux and Windows.");
#else
        QTemporaryDir temp;const auto root=temp.path()+"/data",destination=temp.path()+"/installed";
        const auto stage=root+"/updates/1.2.0-22222222-2222-2222-2222-222222222222";
#ifdef Q_OS_WIN
        const QString relative="ebalia-launcher.exe";
#else
        const QString relative="bin/ebalia-launcher";
#endif
        auto write=[](const QString &path,const QByteArray &bytes){QDir().mkpath(QFileInfo(path).absolutePath());QFile file(path);if(!file.open(QIODevice::WriteOnly))return false;return file.write(bytes)==bytes.size();};
        auto manifest=[](const QStringList &files){return QJsonDocument(QJsonObject{{"format",1},{"files",QJsonArray::fromStringList(files)}}).toJson();};
        QVERIFY(write(destination+"/"+relative,"old binary"));
        QVERIFY(write(destination+"/update-files.json",manifest({relative,"update-files.json"})));
        QVERIFY(write(stage+"/app/update-files.json",manifest({relative,"update-files.json"})));
        QJsonObject prepared{{"version","1.2.0"},{"stage",stage},{"destination",destination},{"executable",stage+"/app/"+relative},{"nix",false}};
        QVERIFY_EXCEPTION_THROWN(LauncherUpdater::applyPrepared(prepared,root),std::runtime_error);
        QFile old(destination+"/"+relative);QVERIFY(old.open(QIODevice::ReadOnly));QCOMPARE(old.readAll(),QByteArray("old binary"));
        QVERIFY(QFileInfo::exists(destination+"/update-files.json"));old.close();
        QDir().mkpath(QFileInfo(stage+"/app/"+relative).absolutePath());
        QVERIFY(QFile::copy(QString::fromUtf8(EBALIA_UPDATE_HELPER),stage+"/app/"+relative));
        prepared["version"]="1.3.0"; // helper reports 1.2.0: startup verification must roll back.
        QVERIFY_EXCEPTION_THROWN(LauncherUpdater::applyPrepared(prepared,root),std::runtime_error);
        QVERIFY(old.open(QIODevice::ReadOnly));QCOMPARE(old.readAll(),QByteArray("old binary"));
#endif
    }
    void updatePromptCanBeDeferred(){
        // The prompt shows what changed since this version: the sections of the notes newer than it, up to the one offered.
        const QJsonObject release{{"body","# 1.3.0\r\n\r\n- Third\r\n\r\n# 1.2.0\r\n\r\n- Second **change**, see [the page](https://example.org)\r\n\r\n# 1.1.9\r\n\r\n- Already here\r\n"}};
        const auto notes=UpdateChecker::releaseNotes(release,"1.1.9","1.2.0");
        QVERIFY(notes.startsWith("# 1.2.0"));QVERIFY(notes.contains("Second"));QVERIFY(!notes.contains("Third"));QVERIFY(!notes.contains("Already"));
        const auto skipped=UpdateChecker::releaseNotes(release,"1.1.8","1.3.0");QVERIFY(skipped.contains("Third")&&skipped.contains("Second")&&skipped.contains("Already"));
        QCOMPARE(UpdateChecker::releaseNotes(QJsonObject{{"description","Plain notes"}},"1.0.0","1.2.0"),QString("Plain notes")); // GitGud, without headings
        qApp->setStyleSheet(Ui::styleSheet());auto unstyle=qScopeGuard([]{qApp->setStyleSheet({});});
        QWidget parent;
        QPointer<QDialog> prompt=UpdateChecker::showUpdatePrompt(&parent,"1.2.0",{},notes);
        QVERIFY(prompt->isVisible());QVERIFY(!prompt->isModal());QVERIFY(prompt->findChild<QLabel*>("updateTitle")->text().contains("1.2.0"));
        auto view=prompt->findChild<QTextBrowser*>("updateNotes");QVERIFY(view);QVERIFY(view->toPlainText().contains("Second change"));QVERIFY(!view->toPlainText().contains("**"));
        QVERIFY(view->height()>=200);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);QVERIFY(prompt->grab().save(out+"/update-prompt.png"));}
        QTest::mouseClick(prompt->findChild<QPushButton*>("updateLater"),Qt::LeftButton);QTRY_VERIFY(!prompt||!prompt->isVisible());
        // Without notes the prompt is the short one; Escape leaves it for later too.
        QPointer<QDialog> plain=UpdateChecker::showUpdatePrompt(&parent,"1.2.0");QVERIFY(!plain->findChild<QTextBrowser*>("updateNotes"));
        QTest::keyClick(plain,Qt::Key_Escape);QTRY_VERIFY(!plain||!plain->isVisible());
        // Search fields keep their magnifier as far from the border as from the text.
        QWidget host;host.setStyleSheet("background:#121214;");auto search=new QLineEdit(&host);search->setFocusPolicy(Qt::NoFocus);search->setPlaceholderText("Filtrar líneas…");
        Ui::addSearchIcon(search,Qt::white);QCOMPARE(search->property("searchField").toBool(),true);QCOMPARE(search->actions().size(),1);
        search->setGeometry(0,0,280,search->sizeHint().height());host.resize(search->size());host.show();QVERIFY(QTest::qWaitForWindowExposed(&host));QTest::qWait(50);
        const auto shot=search->grab().toImage();const int middle=shot.height()/2,border=qRound(shot.devicePixelRatio());
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);QVERIFY(shot.save(out+"/search-field.png"));}
        QList<int> lit;for(int x=border;x<shot.width()-border;++x)for(int y=middle-8*border;y<=middle+8*border;++y)if(qGray(shot.pixel(x,y))>90){lit<<x;break;}
        QVERIFY(lit.size()>10);int iconEnd=lit.first();while(lit.contains(iconEnd+1))++iconEnd;int text=-1;for(int x:lit)if(x>iconEnd){text=x;break;}QVERIFY(text>0);
        const int before=lit.first()-border,after=text-iconEnd-1;QVERIFY2(qAbs(before-after)<=border&&before>=6*border,qPrintable(QString("border %1 px, text %2 px").arg(before).arg(after)));
    }
    void launcherUpdateButton(){
        // The sidebar offers the update only while one is available.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,820);window.show();
        auto sidebar=window.findChild<QPushButton*>("launcherUpdateAvailable");QVERIFY(sidebar);QSignalSpy install(&window,&MainWindow::installLauncherUpdateRequested);
        QVERIFY(!sidebar->isVisible());
        for(auto state:{MainWindow::LauncherUpdate::Checking,MainWindow::LauncherUpdate::Current,MainWindow::LauncherUpdate::Failed}){window.setLauncherUpdate(state);QVERIFY(!sidebar->isVisible());}
        window.setLauncherUpdate(MainWindow::LauncherUpdate::Available,"9.9.9");QVERIFY(sidebar->isVisible());QVERIFY(sidebar->text().contains("9.9.9"));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);window.grab().save(out+"/update-available.png");}
        QTest::mouseClick(sidebar,Qt::LeftButton);QCOMPARE(install.size(),1);
        window.setLauncherUpdate(MainWindow::LauncherUpdate::Installing);QVERIFY(sidebar->isVisible());QVERIFY(!sidebar->isEnabled());
        window.setLauncherUpdate(MainWindow::LauncherUpdate::Available);QVERIFY(sidebar->isEnabled()); // a failed update can be retried
        window.resize(900,700);QTest::qWait(30);QVERIFY(sidebar->isVisible());QVERIFY(sidebar->text().isEmpty()); // compact sidebar: icon only
    }
    void instanceBackgrounds(){
        // A launcher picture, a picture from the computer, or the automatic one; anything else falls back to automatic.
        QTemporaryDir t;McInstanceManager manager(t.path());auto dir=manager.createInstance("Art","1.21.1","vanilla");
        const auto automatic=Ui::artFor(dir);QCOMPARE(InstanceIcons::background(dir),automatic);
        QVERIFY(InstanceIcons::backgrounds().size()>=20);for(const auto &art:InstanceIcons::backgrounds())QVERIFY2(!QImage(art).isNull(),qPrintable(art));
        auto set=[&](const QString &key){auto info=ModRepository::read(dir+"/instance.json");info["background"]=key;ModRepository::write(dir+"/instance.json",info);};
        set(":/art/backgrounds/nether.png");QCOMPARE(InstanceIcons::background(dir),QString(":/art/backgrounds/nether.png"));
        set(":/art/../icon.png");QCOMPARE(InstanceIcons::background(dir),automatic);
        set("custom");QCOMPARE(InstanceIcons::background(dir),automatic); // no picture saved yet
        QImage picture(320,180,QImage::Format_RGB32);picture.fill(Qt::red);QVERIFY(InstanceIcons::saveBackground(dir,picture));
        QCOMPARE(InstanceIcons::background(dir),dir+"/instance-background.jpg");QCOMPARE(QImage(dir+"/instance-background.jpg").size(),QSize(320,180));
        // The picker lists every picture and returns the one chosen.
        QString key;QImage custom;
        QTimer::singleShot(1500,[&]{QDialog *d=nullptr;for(auto w:QApplication::topLevelWidgets())if(w->objectName()=="instanceBackgroundWindow"&&w->isVisible())d=qobject_cast<QDialog*>(w);if(!d){QFAIL("picker not shown");return;}auto list=d->findChild<QListWidget*>("backgroundList");QCOMPARE(list->count(),InstanceIcons::backgrounds().size()+2);
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);d->grab().save(out+"/background-picker.png");}
            for(int n=0;n<list->count();++n)if(list->item(n)->data(Qt::UserRole).toString()==":/art/backgrounds/warden.png")list->setCurrentRow(n);d->accept();});
        QVERIFY(InstanceIcons::chooseBackground(nullptr,dir,"custom",key,custom));QCOMPARE(key,QString(":/art/backgrounds/warden.png"));QVERIFY(custom.isNull());
    }
    void crashSummaryWindow(){
        // A friend's real Forge 26.3 crash: a missing dependency and an AMD driver crash, explained with a button each.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,820);window.show();auto manager=window.findChild<McInstanceManager*>();
        auto dir=manager->createInstance("Biomes","26.3","forge","66.0.9");
        {QFile f(dir+"/launcher.log");QVERIFY(f.open(QIODevice::WriteOnly));f.write("Missing or unsupported mandatory dependencies:\n\tMod ID: 'terrablender', Requested by: 'biomesoplenty', Expected range: '[26.3.0.0.6,)', Actual version: '[MISSING]'\n#\n# Problematic frame:\n# C  [atio6axx.dll+0x192b60]\n");}
        bool shown=false;
        QTimer::singleShot(800,&window,[&]{QDialog *d=nullptr;for(auto w:QApplication::topLevelWidgets())if(w->objectName()=="crashSummaryWindow"&&w->isVisible())d=qobject_cast<QDialog*>(w);
            if(!d)return;shown=true;QCOMPARE(d->findChildren<QFrame*>("crashFinding").size(),2);QVERIFY(d->findChild<QPushButton*>("installMissingMod"));
            QStringList texts;for(auto l:d->findChildren<QLabel*>())texts<<l->text();const auto all=texts.join("\n");QVERIFY(all.contains("terrablender"));QVERIFY(all.contains("biomesoplenty"));QVERIFY(all.contains("AMD"));
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);d->grab().save(out+"/crash-summary.png");}
            d->reject();});
        emit manager->gameEnded(dir,1);QTRY_VERIFY_WITH_TIMEOUT(shown,5000);
    }
    void pastedInstanceAppearsWhileOpen(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,820);window.show();auto manager=window.findChild<McInstanceManager*>();QTest::qWait(200);
        const auto prism=manager->instancesRoot()+"/Copied From Prism";QDir().mkpath(prism+"/minecraft/mods");
        {QFile f(prism+"/mmc-pack.json");QVERIFY(f.open(QIODevice::WriteOnly));f.write(QJsonDocument(QJsonObject{{"formatVersion",1},{"components",QJsonArray{QJsonObject{{"uid","net.minecraft"},{"version","1.21.1"}},QJsonObject{{"uid","net.fabricmc.fabric-loader"},{"version","0.16.7"}}}}}).toJson());}
        {QFile f(prism+"/instance.cfg");QVERIFY(f.open(QIODevice::WriteOnly));f.write("[General]\nname=Copied From Prism\n");}
        {QFile f(prism+"/minecraft/mods/a.jar");QVERIFY(f.open(QIODevice::WriteOnly));f.write("mod");}
        QTRY_VERIFY_WITH_TIMEOUT([&]{for(const auto &i:manager->instances())if(i.name=="Copied From Prism"&&i.loader=="fabric"&&i.mcVersion=="1.21.1")return true;return false;}(),20000);
        QVERIFY(QFile::exists(manager->instancesRoot()+"/Copied From Prism/mods/a.jar"));
    }
    void instanceIconsFromGameTextures(){
        // Opt-in: EBALIA_TEST_JARS=<client jars separated by ;> (Minecraft is not part of the repository).
        const auto jars=qEnvironmentVariable("EBALIA_TEST_JARS").split(';',Qt::SkipEmptyParts);if(jars.isEmpty())QSKIP("Set EBALIA_TEST_JARS to Minecraft client jars");
        QImage sheet(16*13*4,int(jars.size())*16*4+8,QImage::Format_ARGB32);sheet.fill(QColor(24,24,28));QPainter p(&sheet);
        for(int row=0;row<jars.size();++row){QTemporaryDir t;QVERIFY2(InstanceIcons::extractGameTextures(jars[row],t.path()),qPrintable(jars[row]));int col=0;
            for(const auto &key:InstanceIcons::keys())if(key!="ebalia"){QImage icon(t.path()+"/"+key+".png");QVERIFY2(!icon.isNull()&&icon.width()==icon.height(),qPrintable(key));p.drawImage(QRect(col++*72+4,row*68+4,64,64),icon);}}
        p.end();if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);sheet.save(out+"/instance-icons.png");}
    }
    void patreonPostsLookTidy(){
        // A public post as Patreon's editor writes it: small caps, font sizes, empty paragraphs and long text.
        PatreonAuth auth;auth.m_status="Updated · 12:16:28";
        QString body="<p><span style='font-size:18px'>English:</span></p><p>If you are a paying member, tell me and you will have an exclusive role.</p><p><br></p><p><br></p>"
            "<p><span style='font-variant:small-caps;font-family:Georgia'>Unete a mi server de Discord para descargar el mod y se parte de la comunidad!</span></p>"
            "<p><font size='5' color='red'>HERE</font> / <a href='https://discord.gg/x'>AQUI</a> (PERMANENT LINK)</p>";
        for(int n=0;n<6;++n)body+="<p>Línea "+QString::number(n+1)+" de una publicación larga para comprobar que se recorta con Ver más.</p>";
        auth.m_posts=QJsonArray{QJsonObject{{"title","NEW EBALIA LAUNCHER - COMING SOON"},{"date","2024-08-10"},{"is_public",true},{"url","https://www.patreon.com/EBALIA"},{"content",body}},
                                QJsonObject{{"title","Devlog #3"},{"date","2024-07-01"},{"is_public",true},{"url","https://www.patreon.com/EBALIA"},{"content","<p>Short update.</p>"}}};
        PatreonNewsPage page(&auth);page.setStyleSheet(Ui::styleSheet());page.resize(900,760);page.show();QTest::qWait(100);
        auto cards=page.findChild<QScrollArea*>("patreonPublicPosts")->findChildren<QFrame*>("patreonPostCard");QCOMPARE(cards.size(),2);
        auto text=cards[0]->findChild<QTextBrowser*>("patreonPostBody");QVERIFY(!text->toHtml().contains("small-caps"));QVERIFY(!text->toHtml().contains("Georgia"));
        auto more=cards[0]->findChild<QPushButton*>("patreonShowMore");QVERIFY(more->isVisible());const int closed=text->height();QTest::mouseClick(more,Qt::LeftButton);QVERIFY(text->height()>closed);
        QVERIFY(!cards[1]->findChild<QPushButton*>("patreonShowMore")->isVisible());
        QTest::mouseClick(more,Qt::LeftButton);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);page.grab().save(out+"/patreon-page.png");}
    }
    void guideQuestionsAndAnswers(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,900);window.show();window.showPage(MainWindow::Guide);QTest::qWait(100);
        // The guide's steps are all in view at 1280 × 900, without scrolling: three columns, two rows.
        auto steps=window.findChildren<QFrame*>("guideStep");QCOMPARE(steps.size(),6);
        auto stepsArea=qobject_cast<QScrollArea*>(window.findChild<QWidget*>("guide")->parentWidget()->parentWidget());QVERIFY(stepsArea);QVERIFY(!stepsArea->verticalScrollBar()->isVisible());
        QCOMPARE(steps[0]->y(),steps[2]->y());QVERIFY(steps[3]->y()>steps[0]->y());
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/guide-steps.png");
        auto tabs=window.findChild<QTabWidget*>("guideTabs");QVERIFY(tabs);tabs->setCurrentIndex(1);QTest::qWait(50);
        auto questions=window.findChildren<QPushButton*>("faqQuestion");QVERIFY(questions.size()>=10);auto answers=window.findChildren<QLabel*>("faqAnswer");
        for(auto a:answers)QVERIFY(!a->isVisible());
        QTest::mouseClick(questions[0],Qt::LeftButton);QVERIFY(answers[0]->isVisible());QVERIFY(answers[0]->text().contains(".mrpack"));
        QTest::mouseClick(questions[1],Qt::LeftButton);QTest::qWait(50);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);window.grab().save(out+"/guide-faq.png");
            for(auto card:window.findChildren<QFrame*>("modCard")){}window.showPage(MainWindow::Home);QTest::qWait(400);window.grab().save(out+"/home-mods.png");}
        QTest::mouseClick(questions[0],Qt::LeftButton);QVERIFY(!answers[0]->isVisible());
    }
    void colourThemes(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");Ui::setTheme("ebalia");qApp->setStyleSheet(Ui::styleSheet());});
        QCOMPARE(Ui::themed(QColor("#3c8527")),QColor("#3c8527"));QCOMPARE(Ui::styleSheet().contains("#3c8527"),true);
        for(const auto &t:Ui::themes()){
            Ui::setTheme(t.id);QCOMPARE(Ui::theme(),t.id);const auto sheet=Ui::styleSheet();
            if(t.id!="ebalia"){QVERIFY2(!sheet.contains("#3c8527")&&!sheet.contains("#4a9e31"),qPrintable(t.id));QCOMPARE(Ui::themed(QColor("#ed6957")),QColor("#ed6957"));QCOMPARE(Ui::themed(Qt::white),QColor(Qt::white));}
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){qApp->setStyleSheet(sheet);MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);QTest::qWait(150);window.grab().save(out+"/theme-"+t.id+".png");}
        }
        // Custom colours: a blue accent and a dark red background.
        Ui::setCustomTheme(QColor(40,120,255),QColor(40,10,12));QCOMPARE(Ui::theme(),QString("custom"));
        {auto accent=Ui::themed(QColor("#3c8527"));QVERIFY2(qAbs(accent.hslHue()-215)<8,qPrintable(accent.name()));auto back=Ui::themed(QColor("#141416"));QVERIFY2(back.hslHue()>350||back.hslHue()<8,qPrintable(back.name()));QCOMPARE(Ui::themed(Qt::white),QColor(Qt::white));}
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){qApp->setStyleSheet(Ui::styleSheet());MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);QTest::qWait(150);window.grab().save(out+"/theme-custom.png");}
        Ui::setTheme("unknown");QCOMPARE(Ui::theme(),QString("ebalia"));
    }
    void serverListEditing(){
        QTemporaryDir t;const auto file=t.path()+"/servers.dat";
        ServerList::add(file,"§6Hypixel §cNetwork","mc.hypixel.net");ServerList::add(file,"Local","localhost");
        ServerList::edit(file,0,"Hypixel","hypixel.net");auto list=ServerList::read(file);QCOMPARE(list.size(),2);QCOMPARE(list[0].name,QString("Hypixel"));QCOMPARE(list[0].address,QString("hypixel.net"));QCOMPARE(list[1].address,QString("localhost"));
        QVERIFY_THROWS_EXCEPTION(std::runtime_error,ServerList::edit(file,5,"x","y"));
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        McInstanceManager manager(data.path());auto dir=manager.createInstance("Servers","1.21.1","vanilla");ServerList::add(dir+"/servers.dat","§aGreen §lBold","play.example.org");
        InstanceDetail detail;detail.resize(1280,900);detail.show();InstanceInfo info;info.base=McInstance{"Servers",dir,"1.21.1","vanilla",true,4096,0,0};detail.showInstance(info);
        auto servers=detail.findChild<QListWidget*>("instanceServers");QCOMPARE(servers->count(),1);QVERIFY(servers->item(0)->text().startsWith("Green Bold\n"));
        QVERIFY(detail.findChild<QPushButton*>("editServer"));
    }
    void modIconsInManageMods(){
        const auto folder=qEnvironmentVariable("EBALIA_MOD_JARS");if(folder.isEmpty())QSKIP("Set EBALIA_MOD_JARS to a folder of mod jars");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,820);window.show();auto manager=window.findChild<McInstanceManager*>();auto dir=manager->createInstance("Mods","1.21.1","fabric");
        for(const auto &f:QDir(folder).entryList({"*.jar"}))QFile::copy(folder+"/"+f,dir+"/mods/"+f);
        QTimer::singleShot(6000,&window,[&]{QDialog *d=nullptr;for(auto w:QApplication::topLevelWidgets())if(w->objectName()=="modsWindow"&&w->isVisible())d=qobject_cast<QDialog*>(w);if(!d)return;
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())d->grab().save(out+"/mod-icons.png");d->reject();});
        window.openMods(dir);
    }
    void microsoftSkinOnSkinsPage(){
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});qunsetenv("EBALIA_NO_NETWORK");auto net=qScopeGuard([]{qputenv("EBALIA_NO_NETWORK","1");});
        MainWindow window;window.resize(1280,820);window.show();auto accounts=window.findChild<AccountManager*>();
        accounts->addAccount({"msa","Notch","069a79f444e94726a5befca90e38aaf5","token","refresh"});accounts->setActive("069a79f444e94726a5befca90e38aaf5");
        window.showPage(MainWindow::Skins);
        QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(data.path()+"/mc/cache/skins/069a79f444e94726a5befca90e38aaf5.png"),20000);QTest::qWait(300);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/skins-microsoft.png");
    }
    void offlineDefaultSkin(){
        const auto jar=qEnvironmentVariable("EBALIA_TEST_JARS").section(';',0,0);if(jar.isEmpty())QSKIP("Set EBALIA_TEST_JARS to a Minecraft client jar");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QDir().mkpath(data.path()+"/mc/versions/1.21.1");QFile::copy(jar,data.path()+"/mc/versions/1.21.1/1.21.1.jar");
        MainWindow window;window.resize(1280,820);window.show();auto accounts=window.findChild<AccountManager*>();
        // Offline UUID of "Steve" (OfflinePlayer:Steve).
        accounts->addAccount({"offline","Steve","b876ec32-e396-376b-a3a2-17d0f3d6e4e5","",""});accounts->setActive("b876ec32-e396-376b-a3a2-17d0f3d6e4e5");
        window.showPage(MainWindow::Skins);QTest::qWait(300);
        auto list=window.findChild<QListWidget*>("skinList");QVERIFY(list->count()>=1);QVERIFY(list->item(0)->text().startsWith("Steve · "));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/skins-offline.png");
    }
    void wheelDoesNotChangeLists(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);QTest::qWait(100);
        auto language=window.findChild<QComboBox*>("languageChoice");QVERIFY(language);const int before=language->currentIndex();
        auto wheel=[&](QWidget *w){QWheelEvent e(QPointF(10,10),w->mapToGlobal(QPointF(10,10)),QPoint(),QPoint(0,-120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);QApplication::sendEvent(w,&e);};
        window.setFocus();wheel(language);QCOMPARE(language->currentIndex(),before);
        // Not even after clicking it: lists never change with the wheel.
        language->setFocus();QTest::qWait(20);wheel(language);QCOMPARE(language->currentIndex(),before);
    }
    void exploreModIcons(){
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        {McInstanceManager setup(data.path());setup.createInstance("Mods","1.20.1","forge");}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);QTest::qWait(8000);
        int real=0;for(int n=0;n<results->count();++n)if(results->item(n)->icon().availableSizes().value(0).width()>=80)++real;
        qInfo()<<"items"<<results->count()<<"with icon"<<real;
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/explore-icons.png");
        QVERIFY(real>results->count()/2);
    }
    void exploreModPreview(){
        // The page of a mod shows its pictures; a double-click starts its installation.
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        {McInstanceManager setup(data.path());setup.createInstance("Mods","1.21.1","fabric");}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);
        const auto wanted=qEnvironmentVariable("EBALIA_TEST_MOD","Entity Culling");int row=0;
        for(int n=0;n<results->count();++n)if(results->item(n)->text().startsWith(wanted)){row=n;break;}
        results->setCurrentRow(row);auto body=window.findChild<QTextBrowser*>("modPreviewBody");QVERIFY(body);
        QTest::qWait(10000);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/explore-preview.png");
        int pictures=0;for(auto block=body->document()->begin();block.isValid();block=block.next())for(auto it=block.begin();!it.atEnd();++it)if(it.fragment().charFormat().isImageFormat())++pictures;
        qInfo()<<"pictures in description"<<pictures;
        auto item=results->item(row);int clicks=0;connect(results,&QListWidget::itemDoubleClicked,&window,[&]{++clicks;});
        results->scrollToItem(item);QTest::qWait(100);const auto point=results->visualItemRect(item).center();
        QTest::mousePress(results->viewport(),Qt::LeftButton,{},point);QTest::mouseRelease(results->viewport(),Qt::LeftButton,{},point);QTest::mouseDClick(results->viewport(),Qt::LeftButton,{},point);
        QCOMPARE(clicks,1);
        QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QWidget*>("backgroundTaskProgress"),5000);
    }
    void customThemePreview(){
        // The custom theme window shows a piece of the launcher in the chosen colours, without applying them until Save.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        const auto before=QSettings().value("ui/theme");
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);QTest::qWait(50);
        QPushButton *custom=nullptr;for(auto b:window.findChildren<QPushButton*>("themeChoice"))if(b->property("theme")=="custom")custom=b;QVERIFY(custom);
        bool seen=false;QTimer::singleShot(200,&window,[&]{
            for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->objectName()=="customThemeWindow"&&d->isVisible()){
                QFrame *preview=nullptr;for(auto f:d->findChildren<QFrame*>())if(f->property("customThemePreview").toBool())preview=f;
                seen=preview&&!preview->styleSheet().isEmpty();
                if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())d->grab().save(out+"/custom-theme.png");
                d->reject();}
        });
        custom->click();QVERIFY(seen);QCOMPARE(QSettings().value("ui/theme"),before);
    }
    void homeBannerKeepsItsHeight(){
        // Full screen: the covers below grow, the banner must not shrink to a strip.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1920,1080);window.show();window.showPage(MainWindow::Home);QTest::qWait(100);
        auto hero=window.findChild<QWidget*>("hero");QVERIFY(hero);QVERIFY2(hero->height()>=hero->width()*0.25,qPrintable(QString("%1x%2").arg(hero->width()).arg(hero->height())));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/home-full.png");
    }
    void discoverSearchesWhileTyping(){
        // Discover searches while typing, once typing pauses, with no Enter; what stops a search (offline here, or no instance)
        // shows in the list, never in a window that would interrupt the typing.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        {
            MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Explore);
            auto results=window.findChild<QListWidget*>("modResults");auto query=window.findChild<QLineEdit*>("modQuery");QVERIFY(results&&query);
            QTest::keyClicks(query,"sodium");
            QTRY_COMPARE(results->count(),1);QCOMPARE(results->item(0)->text(),Language::key("Create an instance with a mod loader first."));QVERIFY(!QApplication::activeModalWidget());
        }
        {McInstanceManager setup(data.path());setup.createInstance("Mods","1.20.1","forge");}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");auto query=window.findChild<QLineEdit*>("modQuery");auto delay=window.findChild<QTimer*>("modSearchDelay");QVERIFY(results&&query&&delay);
        auto note=[&]{return results->count()==1&&!(results->item(0)->flags()&Qt::ItemIsSelectable)&&results->item(0)->text()!="placeholder";};
        QTRY_VERIFY(note()); // opening the page searched by itself; offline, the list says so
        QVERIFY(!QApplication::activeModalWidget());
        results->clear();results->addItem("placeholder");
        QTest::keyClicks(query,"clo");QVERIFY(delay->isActive());QTest::qWait(150);QTest::keyClicks(query,"ud");QVERIFY(delay->isActive());
        QCOMPARE(results->item(0)->text(),QString("placeholder")); // not while typing
        QTRY_VERIFY(note());QVERIFY(!QApplication::activeModalWidget());QCOMPARE(query->text(),QString("cloud"));
        // Enter searches at once, and so does clearing the field.
        results->clear();results->addItem("placeholder");QTest::keyClick(query,Qt::Key_Return);QVERIFY(!delay->isActive());QTRY_VERIFY(note());
        results->clear();results->addItem("placeholder");query->selectAll();QTest::keyClick(query,Qt::Key_Backspace);QVERIFY(delay->isActive());QTRY_VERIFY(note());
        QVERIFY(!QApplication::activeModalWidget());
    }
    void creatorScreens(){
        // Pictures of EBALIA's mod window, the Patreon invitation and the pages with cards (EBALIA_TEST_ARTIFACTS).
        const auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(out.isEmpty())QSKIP("Opt-in pictures (EBALIA_TEST_ARTIFACTS)");QDir().mkpath(out);
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");auto restore=qScopeGuard([]{resetCreatorMods();qunsetenv("EBALIA_DATA_DIR");});
        QFile live(qEnvironmentVariable("EBALIA_TEST_LIVE_MODS"));if(live.open(QIODevice::ReadOnly)){QDir().mkpath(data.path()+"/cache");QFile copy(data.path()+"/cache/ebalia-mods.json");copy.open(QIODevice::WriteOnly);copy.write(live.readAll());} // site-src/mods/live.json
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Home);QTest::qWait(300);
        for(const auto &[name,patreon]:QList<std::pair<QString,bool>>{{"creator-mods",false},{"creator-patreon",true}}){
            QTimer::singleShot(400,&window,[&,name]{for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->isVisible()&&(d->objectName()=="myModsDialog"||d->objectName()=="patreonInvite")){d->grab().save(out+"/"+name+".png");if(auto all=d->findChild<QWidget*>("creatorContent"))all->grab().save(out+"/"+name+"-all.png");d->close();}});
            window.findChild<QPushButton*>(patreon?"patreonInviteButton":"myModsButton")->click();QTest::qWait(800);
        }
        for(const auto &[name,page]:QList<std::pair<QString,int>>{{"page-community",MainWindow::Community},{"page-guide",MainWindow::Guide},{"page-bedrock",MainWindow::Bedrock},{"page-news",MainWindow::News}}){window.showPage(page);QTest::qWait(300);window.grab().save(out+"/"+name+".png");}
    }
    void creatorModsFollowTheLiveData(){
        // EBALIA's own mods take their name, downloads, page and picture from the live data (site-src/live.py), a mod that
        // comes out stops being "coming soon", a new one joins; collaborations and addresses off the stores stay out.
        auto restore=qScopeGuard([]{resetCreatorMods();qunsetenv("EBALIA_DATA_DIR");});
        auto mod=[](const QString &id,const QString &name,qint64 downloads,QJsonObject extra={}){QJsonObject o{{"id",id},{"group","own"},{"name",name},{"soon",false},{"downloads",double(downloads)}};for(auto it=extra.begin();it!=extra.end();++it)o[it.key()]=it.value();return o;};
        const QJsonObject live{{"format",1},{"downloads",7000000.0},{"published",5},{"mods",QJsonArray{
            mod("in-your-world","In Your World: Renamed",1947203,{{"icon","https://media.forgecdn.net/avatars/thumbnails/1/2/256/256/new.png"},{"curseforge",QJsonObject{{"url","https://www.curseforge.com/minecraft/mc-mods/in-your-world"}}}}),
            mod("ooo-jar","ooo.jar · 000.jar",1659641,{{"curseforge",QJsonObject{{"url","https://www.curseforge.com/minecraft/mc-mods/ooo-jar"}}}}),
            mod("brand-new","Brand New",782,{{"summary","A new mod."},{"icon","javascript:alert(1)"},{"curseforge",QJsonObject{{"url","https://evil.example/x"}}},{"modrinth",QJsonObject{{"url","https://modrinth.com/mod/brand-new"}}}}),
            mod("secret-01-proyect","SECRET 01",12345,{{"icon","https://cdn.modrinth.com/data/x/icon.webp"},{"curseforge",QJsonObject{{"url","https://www.curseforge.com/minecraft/mc-mods/secret-01"}}}}),
            QJsonObject{{"id","disturbed"},{"group","collab"},{"name","DISTURBED"},{"soon",false},{"downloads",3273326.0},{"text","modDisturbed"},{"credit",QJsonArray{"collabWith","LUNAR_CORE · VantablackDEV"}},{"curseforge",QJsonObject{{"url","https://www.curseforge.com/minecraft/mc-mods/disturbed"}}}},
            QJsonObject{{"id","neighbor-jar"},{"group","collab"},{"name","Neighbor.jar"},{"soon",false},{"downloads",207.0},{"credit",QJsonArray{"collabWith","PKFL · TingKH"}},{"summary","Adds Gerald."}},
            QJsonObject{{"id","../escape"},{"group","own"},{"name","Bad id"},{"soon",false}}}}};
        QVERIFY(setCreatorMods(live));QVERIFY(!setCreatorMods(live)); // the same again changes nothing
        const auto mods=creatorMods();QCOMPARE(mods.size(),6);
        QVERIFY(!mods[3].collab&&mods[4].collab&&mods[5].collab); // its own first, then the collaborations
        QCOMPARE(mods[4].by(),Language::key("With %1").arg("LUNAR_CORE · VantablackDEV"));QCOMPARE(mods[4].about(),Language::key("modDisturbed"));QVERIFY(mods[4].about()!="modDisturbed");
        QCOMPARE(mods[5].about(),QString("Adds Gerald.")); // new to the catalog: the store's summary
        QCOMPARE(mods[0].name,QString("In Your World: Renamed"));QCOMPARE(mods[0].downloads,qint64(1947203));QCOMPARE(mods[0].background,QString(":/art/backgrounds/warden.png")); // its art stays
        QCOMPARE(mods[0].icon,QString("https://media.forgecdn.net/avatars/thumbnails/1/2/256/256/new.png"));QCOMPARE(mods[0].url(),QString("https://www.curseforge.com/minecraft/mc-mods/in-your-world"));
        QCOMPARE(mods[2].slug,QString("brand-new"));QVERIFY(mods[2].icon.isEmpty());QCOMPARE(mods[2].url(),QString("https://modrinth.com/mod/brand-new"));QCOMPARE(mods[2].description,QString("A new mod."));
        QCOMPARE(mods[3].name,QString("SECRET 01"));QVERIFY(!mods[3].soon);QCOMPARE(mods[3].url(),QString("https://www.curseforge.com/minecraft/mc-mods/secret-01"));
        QVERIFY(!setCreatorMods(QJsonObject{{"format",2},{"mods",QJsonArray{}}}));QCOMPARE(creatorMods().size(),6); // unknown data leaves the list alone
        // Home shows them from the copy kept last time, at once, with their downloads.
        resetCreatorMods();QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        QDir().mkpath(data.path()+"/cache");{QFile f(data.path()+"/cache/ebalia-mods.json");QVERIFY(f.open(QIODevice::WriteOnly));f.write(QJsonDocument(live).toJson());}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Home);
        QTRY_COMPARE(window.findChildren<QPushButton*>("modCard").size(),3);QStringList names,metas;
        for(auto card:window.findChildren<QPushButton*>("modCard"))if(card->isVisible()){names<<card->accessibleName();metas<<card->findChild<QLabel*>("newsMeta")->text();}
        QVERIFY(names.contains("In Your World: Renamed"));QVERIFY(names.contains("Brand New"));QVERIFY(!names.contains("DISTURBED"));QVERIFY(!names.contains("Neighbor.jar")); // never a collaboration on Home
        // The mods window has them apart, as the site: its own, then the collaborations with who made them too.
        window.findChild<QPushButton*>("myModsButton")->click();auto dialog=window.findChild<QDialog*>("myModsDialog");QVERIFY(dialog);
        auto own=dialog->findChild<QWidget*>("creatorOwnMods"),collab=dialog->findChild<QWidget*>("creatorCollabMods");QVERIFY(own&&collab);
        QStringList ownCards,collabCards;for(auto card:own->findChildren<QFrame*>("creatorModCard"))ownCards<<card->property("project").toString();for(auto card:collab->findChildren<QFrame*>("creatorModCard"))collabCards<<card->property("project").toString();
        QCOMPARE(ownCards,(QStringList{"in-your-world","ooo-jar","brand-new","secret-01-proyect"}));QCOMPARE(collabCards,(QStringList{"disturbed","neighbor-jar"}));
        QStringList credits;for(auto label:collab->findChildren<QLabel*>("creatorModCredit"))credits<<label->text();QVERIFY(credits.contains(Language::key("With %1").arg("PKFL · TingKH")));
        QVERIFY(dialog->findChild<QLabel*>("creatorStats")->text().contains(QLocale(Language::current).toString(qint64(1947203+1659641+782+12345+3273326+207))));
        dialog->close();
        QVERIFY(metas.join("|").contains(QLocale(Language::current).toString(1947203)+" "+Language::key("downloads").toUpper()));
    }
    void launcherLocationInSettings(){
        // Settings open the folder the launcher is in, and say whether it is installed or a portable copy.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qunsetenv("EBALIA_PORTABLE");qunsetenv("APPIMAGE");QDesktopServices::unsetUrlHandler("file");});
        QDesktopServices::setUrlHandler("file",this,"captureUrl");
        auto check=[&](const QString &folder,const char *says){
            MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);
            auto open=window.findChild<QPushButton*>("openLauncherLocation");auto where=window.findChild<QLabel*>("launcherLocation");QVERIFY(open&&where);QTRY_VERIFY(open->isVisible());
            QCOMPARE(where->text(),Language::key(says).arg(QDir::toNativeSeparators(folder)));
            lastUrl=QUrl();QTest::mouseClick(open,Qt::LeftButton);QCOMPARE(lastUrl,QUrl::fromLocalFile(folder));
        };
        check(QCoreApplication::applicationDirPath(),"Installed in %1");
        qputenv("EBALIA_PORTABLE","1");check(QCoreApplication::applicationDirPath(),"Portable version in %1");qunsetenv("EBALIA_PORTABLE");
#ifdef Q_OS_LINUX
        QTemporaryDir usb;qputenv("APPIMAGE",(usb.path()+"/EBALIA_Launcher.AppImage").toUtf8());check(usb.path(),"Installed in %1"); // the folder of the AppImage file
#endif
    }
    void uninstallFromSettings(){
        // Settings offer to uninstall: what goes is said first, the data only when ticked, and Cancel leaves everything.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QDir().mkpath(data.path()+"/mc/instances/World");
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Settings);
        auto open=window.findChild<QPushButton*>("uninstallLauncher");QVERIFY(open);QTRY_VERIFY(open->isVisible());QVERIFY(open->property("danger").toBool());
        bool checked=false;
        QTimer::singleShot(200,&window,[&]{
            auto dialog=qobject_cast<QDialog*>(QApplication::activeModalWidget());QVERIFY(dialog);QCOMPARE(dialog->objectName(),QString("uninstallDialog"));
            auto close=qScopeGuard([dialog=QPointer<QDialog>(dialog)]{if(dialog)dialog->reject();}); // a failed check below must not leave it open
            // The tests run from a build folder that does not list its files: it cannot remove itself, only the data.
            const auto plan=Uninstaller::plan(data.path(),false);QCOMPARE(plan.kind,Uninstaller::Kind::Unknown);
            QCOMPARE(dialog->findChild<QLabel*>("uninstallWhat")->text(),Language::key("This copy does not list its files, so it cannot remove itself safely: close it and delete its folder, %1. Here you can delete your data.").arg(QDir::toNativeSeparators(plan.location)));
            auto box=dialog->findChild<QCheckBox*>("uninstallData");auto note=dialog->findChild<QLabel*>("uninstallNote");auto confirm=dialog->findChild<QPushButton*>("uninstallConfirm");
            QVERIFY(!box->isChecked()&&box->isEnabled());QVERIFY(!confirm->isEnabled());QCOMPARE(confirm->text().trimmed(),Language::key("Delete my data"));
            QCOMPARE(note->text(),Language::key("Your data stays in %1.").arg(QDir::toNativeSeparators(data.path())));
            box->setChecked(true);QVERIFY(confirm->isEnabled());QVERIFY(note->text().contains(QDir::toNativeSeparators(data.path())));QVERIFY(note->text()!=Language::key("Your data stays in %1.").arg(QDir::toNativeSeparators(data.path())));
            QVERIFY(dialog->findChild<QPushButton*>("uninstallCancel")->isDefault());checked=true;
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);QVERIFY(dialog->grab().save(out+"/uninstall-dialog.png"));}
            QTest::mouseClick(dialog->findChild<QPushButton*>("uninstallCancel"),Qt::LeftButton);
        });
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){for(auto w=open->parentWidget();w;w=w->parentWidget())if(auto area=qobject_cast<QScrollArea*>(w)){area->ensureWidgetVisible(open,0,40);break;}QTest::qWait(100);QVERIFY(window.grab().save(out+"/settings-uninstall.png"));}
        QTest::mouseClick(open,Qt::LeftButton);QVERIFY(checked);
        QVERIFY(window.isVisible());QVERIFY(QFileInfo::exists(data.path()+"/mc/instances/World")); // nothing removed
    }
    void homeBannerChangesCleanly(){
        // Changing the background with the arrows: the picture shown stays where it is while the next one loads (it used to jump
        // to where the next slide sits, a strip across the banner), then fades into it, and nothing of it is left after.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        HomeBanner banner;banner.resize(1560,440);banner.show();QVERIFY(QTest::qWaitForWindowExposed(&banner));QTest::qWait(500);
        auto next=banner.findChild<QToolButton*>("heroNext"),previous=banner.findChild<QToolButton*>("heroPrevious");
        auto scene=[&]{return banner.grab().toImage().copy(QRect(300,60,900,150));}; // away from the title, the edition and the controls
        auto picture=[&]{auto image=banner.grab().toImage();QPainter p(&image);p.fillRect(banner.findChild<QWidget*>("heroControls")->geometry(),Qt::black);return image;}; // the arrows light up under the mouse
        for(auto arrow:{next,next,previous,previous,previous}){
            Ui::releaseMemory(); // the next picture is not ready yet
            const auto before=scene();arrow->click();QVERIFY2(scene()==before,qPrintable(QString("slide %1 moved before loading").arg(banner.property("currentSlide").toInt())));
            QTRY_VERIFY_WITH_TIMEOUT(scene()!=before,5000);QTest::qWait(900);
            const auto settled=picture();banner.hide();banner.show();QVERIFY(QTest::qWaitForWindowExposed(&banner));QTest::qWait(30);
            const auto again=picture();
            if(again!=settled)if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);settled.save(out+"/banner-settled.png");again.save(out+"/banner-again.png");}
            QVERIFY2(again==settled,qPrintable(QString("slide %1 still shows the one before").arg(banner.property("currentSlide").toInt())));
        }
    }
    void exploreModPages(){
        // Pictures of real mod pages (EBALIA_TEST_ARTIFACTS), searched by name: EBALIA_TEST_MODS="Entity Culling,Chat Heads".
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS")||qEnvironmentVariable("EBALIA_TEST_MODS").isEmpty())QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        {McInstanceManager setup(data.path());setup.createInstance("Mods","1.21.1","fabric");}
        MainWindow window;window.resize(1400,900);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");auto query=window.findChild<QLineEdit*>("modQuery");QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);
        int n=0;
        for(const auto &name:qEnvironmentVariable("EBALIA_TEST_MODS").split(',')) {
            query->setText(name.trimmed());QTest::keyClick(query,Qt::Key_Return);QTest::qWait(300);
            QTRY_VERIFY_WITH_TIMEOUT(results->count()>0&&results->item(0)->text().contains(name.trimmed(),Qt::CaseInsensitive),30000);
            results->setCurrentRow(0);QTest::qWait(9000);
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){
                auto panel=window.findChild<QWidget*>("modPreview");panel->grab().save(out+QString("/mod-page-%1.png").arg(++n));
                auto body=window.findChild<QTextBrowser*>("modPreviewBody");for(int part:{1,2}){body->verticalScrollBar()->setValue(body->verticalScrollBar()->maximum()*part/3);QTest::qWait(300);panel->grab().save(out+QString("/mod-page-%1-%2.png").arg(n).arg(part));}
            }
        }
    }
    void exploreInstallButton(){
        // The Install button of the mod page installs the mod with its dependencies and the launcher keeps running.
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        QString dir;{McInstanceManager setup(data.path());dir=setup.createInstance("Mods","1.21.1","fabric");}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);
        const auto wanted=qEnvironmentVariable("EBALIA_TEST_MOD","Sodium");int row=0;
        for(int n=0;n<results->count();++n)if(results->item(n)->text().startsWith(wanted)){row=n;break;}
        results->setCurrentRow(row);QTest::qWait(1500);
        auto closer=new QTimer(&window);connect(closer,&QTimer::timeout,&window,[]{for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->isVisible()){qInfo()<<"dialog"<<d->windowTitle()<<(qobject_cast<QMessageBox*>(d)?qobject_cast<QMessageBox*>(d)->text():QString());d->reject();}});closer->start(100);
        window.findChild<QPushButton*>("modPreviewInstall")->click();
        QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QWidget*>("backgroundTaskProgress"),5000);
        QTRY_VERIFY_WITH_TIMEOUT(!window.findChild<QWidget*>("backgroundTaskProgress"),120000);
        qInfo()<<"status"<<window.findChild<QLabel*>("statusText")->text()<<"mods"<<QDir(dir+"/mods").entryList(QDir::Files);
        QVERIFY(!QDir(dir+"/mods").entryList({"*.jar"},QDir::Files).isEmpty());
    }
    void exploreKindsAndGallery(){
        // Resource packs and shaders from the stores into their folders; the gallery viewer of a mod.
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        QString dir;{McInstanceManager setup(data.path());dir=setup.createInstance("Packs","1.21.1","fabric");}
        MainWindow window;window.resize(1400,900);window.show();window.showPage(MainWindow::Explore);
        const auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");
        auto results=window.findChild<QListWidget*>("modResults");auto kind=window.findChild<QComboBox*>("modKind");auto provider=window.findChild<QComboBox*>("modProvider")?window.findChild<QComboBox*>("modProvider"):[&]{for(auto c:window.findChildren<QComboBox*>())if(c->findData("curseforge")>=0)return c;return (QComboBox*)nullptr;}();QVERIFY(kind);
        QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);
        // A mod without a banner picture (Bug #1) and a mod with a gallery.
        for(int n=0;n<results->count();++n)if(results->item(n)->text().startsWith("Fabric API")){results->setCurrentRow(n);break;}
        QTest::qWait(4000);if(!out.isEmpty())window.findChild<QWidget*>("modPreview")->grab().save(out+"/no-banner.png");
        auto gallery=window.findChild<QListWidget*>("modGallery");int withGallery=-1;
        for(int n=0;n<results->count()&&withGallery<0;++n){results->setCurrentRow(n);QTest::qWait(2500);if(gallery->count()>1)withGallery=n;}
        QVERIFY2(withGallery>=0,"no mod with a gallery among the results");qInfo()<<"gallery of"<<results->item(withGallery)->text().section('\n',0,0)<<gallery->count();
        if(!out.isEmpty())window.findChild<QWidget*>("modPreview")->grab().save(out+"/gallery-strip.png");
        bool viewer=false;QTimer::singleShot(2500,&window,[&]{for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->objectName()=="modGalleryViewer"){viewer=true;
            QTest::keyClick(d,Qt::Key_Right);QTest::qWait(2500);if(!out.isEmpty())d->grab().save(out+"/gallery-viewer.png");d->reject();}});
        emit gallery->itemClicked(gallery->item(0));QVERIFY(viewer);
        // Packs: Modrinth resource pack and shader, then CurseForge resource packs.
        auto closer=new QTimer(&window);connect(closer,&QTimer::timeout,&window,[]{for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QMessageBox*>(w);d&&d->isVisible()){qInfo()<<"asked:"<<d->text();d->reject();}});closer->start(100);
        for(const auto &[type,folder]:QList<std::pair<QString,QString>>{{"resourcepack","resourcepacks"},{"shader","shaderpacks"}}){
            kind->setCurrentIndex(kind->findData(type));emit kind->activated(kind->currentIndex());
            QTRY_VERIFY_WITH_TIMEOUT(results->count()>0&&!window.findChild<QWidget*>("backgroundTaskProgress"),30000);
            qInfo()<<type<<"first:"<<results->item(0)->text().section('\n',0,0);
            results->setCurrentRow(0);QTest::qWait(500);window.findChild<QPushButton*>("modPreviewInstall")->click();
            QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QWidget*>("backgroundTaskProgress"),5000);
            QTRY_VERIFY_WITH_TIMEOUT(!window.findChild<QWidget*>("backgroundTaskProgress"),180000);
            qInfo()<<folder<<QDir(dir+"/"+folder).entryList(QDir::Files)<<window.findChild<QLabel*>("statusText")->text();
            QVERIFY(!QDir(dir+"/"+folder).entryList(QDir::Files).isEmpty());
            // Installed: the list and the button say so.
            QTRY_VERIFY(results->item(0)->data(Qt::UserRole+1).toBool());
            QVERIFY(!window.findChild<QPushButton*>("modPreviewInstall")->isEnabled());QVERIFY(window.findChild<QPushButton*>("modPreviewInstall")->text().contains(Language::key("Already installed")));
            // A shader pack has no picture of its own: its window shows the Modrinth project's icon.
            if(folder=="shaderpacks"){const auto file=dir+"/shaderpacks/"+QDir(dir+"/shaderpacks").entryList({"*.zip"},QDir::Files).value(0);
                QVERIFY(!InstanceContent::modrinthIcon(file,data.path()+"/icons").isNull());QVERIFY(!InstanceContent::modrinthIcon(file,data.path()+"/icons").isNull());} // the second from cache
        }
        if(!ModRepository::curseForgeKey().isEmpty()){
            provider->setCurrentIndex(provider->findData("curseforge"));kind->setCurrentIndex(kind->findData("resourcepack"));emit kind->activated(kind->currentIndex());
            QTRY_VERIFY_WITH_TIMEOUT(results->count()>0&&!window.findChild<QWidget*>("backgroundTaskProgress"),30000);
            qInfo()<<"curseforge resource pack:"<<results->item(0)->text().section('\n',0,0);
        }
    }
    void contentWindows(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        // Clicking anywhere on a card of the instance page opens its window (before the main window and its first-run tour).
        InstanceDetail detail;detail.resize(1280,900);detail.show();InstanceInfo info;info.base=McInstance{"Packs",data.path(),"1.21.1","fabric",true,4096,0,0};detail.showInstance(info);
        QString opened;detail.manage=[&](const QString &folder){opened=folder;};QFrame *card=nullptr;
        for(auto c:detail.findChildren<QFrame*>("statCard"))if(auto title=c->findChild<QLabel*>("statTitle");title&&title->text()==Language::key("Shader packs"))card=c;
        QVERIFY(card);QTest::mouseClick(card,Qt::LeftButton,{},QPoint(card->width()/2,30));QCOMPARE(opened,QString("shaderpacks"));
        MainWindow window;window.resize(1280,820);window.show();auto manager=window.findChild<McInstanceManager*>();auto dir=manager->createInstance("Packs","1.21.1","fabric");
        auto write=[](const QString &path,const QByteArray &bytes){QDir().mkpath(QFileInfo(path).absolutePath());QFile f(path);if(f.open(QIODevice::WriteOnly))f.write(bytes);};
        write(dir+"/resourcepacks/Faithful/pack.mcmeta",R"({"pack":{"pack_format":34,"description":"§eThe classic look, twice as sharp"}})");
        {QImage icon(16,16,QImage::Format_ARGB32);icon.fill(QColor(80,160,60));icon.save(dir+"/resourcepacks/Faithful/pack.png");}
        write(dir+"/shaderpacks/Complementary.zip","zip");write(dir+"/saves/Survival/level.dat","x");
        {QImage icon(64,64,QImage::Format_ARGB32);icon.fill(QColor(90,120,200));icon.save(dir+"/saves/Survival/icon.png");}
        // Each window opens with its entries; the first button turns the selected one on.
        auto check=[&](const QString &folder,std::function<void(QDialog*,QListWidget*)> inside){
            bool seen=false;
            std::function<void()> poll=[&]{QDialog *d=nullptr;for(auto w:QApplication::topLevelWidgets())if(w->objectName()=="contentWindow"&&w->isVisible())d=qobject_cast<QDialog*>(w);
                if(!d){QTimer::singleShot(100,&window,poll);return;}
                QTimer::singleShot(600,d,[&,d]{seen=true;auto list=d->findChild<QListWidget*>("contentList");inside(d,list);
                    if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())d->grab().save(out+"/content-"+folder+".png");d->reject();});};
            QTimer::singleShot(100,&window,poll);window.openContent(dir,folder);QVERIFY(seen);};
        check("resourcepacks",[&](QDialog *d,QListWidget *list){QCOMPARE(list->count(),1);QVERIFY(list->item(0)->text().contains("The classic look"));
            d->findChild<QPushButton*>("toggleContent")->click();QVERIFY(list->item(0)->data(Qt::UserRole+1).toBool());});
        QFile options(dir+"/options.txt");QVERIFY(options.open(QIODevice::ReadOnly));QVERIFY(options.readAll().contains("\"file/Faithful\""));
        check("shaderpacks",[&](QDialog *d,QListWidget *list){QCOMPARE(list->count(),1);d->findChild<QPushButton*>("toggleContent")->click();});
        QCOMPARE(InstanceContent::shader(dir),QString("Complementary.zip"));
        check("saves",[&](QDialog *,QListWidget *list){QCOMPARE(list->count(),1);QVERIFY(list->item(0)->text().startsWith("Survival\n"));});
    }
    void skinViewer3D(){
        // A skin whose head shows a different colour on each side; the right side is half back (yellow), half front (green).
        QImage skin(64,64,QImage::Format_ARGB32);skin.fill(Qt::transparent);
        auto fill=[&](QRect r,QColor c){for(int y=r.top();y<=r.bottom();++y)for(int x=r.left();x<=r.right();++x)skin.setPixelColor(x,y,c);};
        fill({8,8,8,8},Qt::red);fill({24,8,8,8},Qt::blue);fill({0,8,4,8},Qt::yellow);fill({4,8,4,8},Qt::green);fill({8,0,8,8},Qt::magenta);
        SkinViewer viewer;viewer.resize(240,320);viewer.setSkin(skin,false);viewer.show();
        // 240 × 320: seven pixels per skin pixel, the middle of the head's front at (120, 67).
        auto at=[&](double yaw,double pitch,QPoint point){viewer.setView(yaw,pitch);return viewer.grab().toImage().pixelColor(point);};
        QCOMPARE(at(0,0,{120,67}),QColor(Qt::red));QCOMPARE(at(180,0,{120,67}),QColor(Qt::blue));
        QCOMPARE(at(90,0,{132,67}),QColor(Qt::green));QCOMPARE(at(90,0,{108,67}),QColor(Qt::yellow));
        QCOMPARE(at(0,60,{120,95}),QColor(Qt::magenta)); // from above: the top of the head
        // The flat view is still there.
        viewer.setThreeD(false);QCOMPARE(viewer.grab().toImage().pixelColor(120,56),QColor(Qt::red));
        viewer.setThreeD(true);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){
            viewer.setView(-28,12);viewer.grab().save(out+"/skin-3d.png");
            // A real skin (EBALIA_TEST_SKIN, "slim:" in front for the slim model) from a few sides.
            if(auto real=qEnvironmentVariable("EBALIA_TEST_SKIN");!real.isEmpty()){const bool slim=real.startsWith("slim:");viewer.setSkin(QImage(slim?real.mid(5):real),slim);viewer.resize(320,420);
                for(const auto &[yaw,pitch]:{std::pair{-28.0,12.0},std::pair{150.0,12.0},std::pair{70.0,-20.0},std::pair{-20.0,55.0}})
                    {viewer.setView(yaw,pitch);viewer.grab().save(out+QString("/skin-real-%1-%2.png").arg(int(yaw)).arg(int(pitch)));}}}
    }
    void titleBarDoubleClick(){
        qputenv("EBALIA_TITLE_BAR","custom");auto unset=qScopeGuard([]{qunsetenv("EBALIA_TITLE_BAR");});
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        MainWindow window;window.resize(1000,700);window.show();QVERIFY(QTest::qWaitForWindowExposed(&window));
        auto bar=window.findChild<QWidget*>("titleBar");QVERIFY(bar);QVERIFY(window.windowFlags()&Qt::FramelessWindowHint);QVERIFY(window.findChild<QToolButton*>("titleClose"));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/title-bar.png");
        QTest::mouseDClick(bar,Qt::LeftButton,{},QPoint(300,15));QTRY_VERIFY(window.isMaximized());
        QTest::mouseDClick(bar,Qt::LeftButton,{},QPoint(300,15));QTRY_VERIFY(!window.isMaximized());
        window.findChild<QToolButton*>("titleMaximize")->click();QTRY_VERIFY(window.isMaximized());
    }
    void profileDiscoverTyping(){
        // How long the window freezes while typing in Discover, with the real stores, split into while typing and after it.
        // Opt-in: EBALIA_PROFILE=1 (EBALIA_PROFILE_CACHE: a launcher's cache/images folder, to start as full as a real one).
        if(!qEnvironmentVariableIsSet("EBALIA_PROFILE"))QSKIP("Opt-in profiling (EBALIA_PROFILE=1)");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        {McInstanceManager setup(data.path());setup.createInstance("Mods","1.20.1","forge");}
        // A picture cache as full as a real one (EBALIA_PROFILE_CACHE: a launcher's cache/images folder).
        if(const auto cache=qEnvironmentVariable("EBALIA_PROFILE_CACHE");!cache.isEmpty()){QDirIterator it(cache,QDir::Files,QDirIterator::Subdirectories);int n=0;
            while(it.hasNext()){const auto file=it.next();const auto to=data.path()+"/cache/images/"+QDir(cache).relativeFilePath(file);QDir().mkpath(QFileInfo(to).path());if(QFile::copy(file,to))++n;}qInfo()<<"cache files:"<<n;}
        qunsetenv("EBALIA_NO_NETWORK");auto offline=qScopeGuard([]{qputenv("EBALIA_NO_NETWORK","1");});
        QElapsedTimer clock;clock.start();qint64 last=0,worst=0;QTimer beat;beat.setInterval(5);QObject::connect(&beat,&QTimer::timeout,[&]{const auto now=clock.elapsed();worst=std::max(worst,now-last);last=now;});beat.start();
        MainWindow window;window.resize(1920,1080);window.show();window.showPage(MainWindow::Explore);
        auto results=window.findChild<QListWidget*>("modResults");auto query=window.findChild<QLineEdit*>("modQuery");auto provider=window.findChild<QComboBox*>("modProvider");
        QTRY_VERIFY_WITH_TIMEOUT(results->count()>1,30000);QTest::qWait(3000);
        // Typing with pauses, as people do: the results of the first part arrive while the rest is typed.
        auto type=[&](const QString &first,const QString &rest){
            query->clear();QTest::qWait(1500);worst=0;last=clock.elapsed();
            for(const auto c:first){QTest::keyClicks(query,QString(c));QTest::qWait(130);}QTest::qWait(900);
            for(const auto c:rest){QTest::keyClicks(query,QString(c));QTest::qWait(130);}
            const auto typed=worst;worst=0;last=clock.elapsed();QTest::qWait(5000);
            qInfo().noquote()<<QString("%1 (%2): worst freeze while typing %3 ms, after %4 ms, %5 results").arg(first+rest,-16).arg(provider->currentText()).arg(typed,3).arg(worst,3).arg(results->count());};
        for(auto [a,b]:QList<std::pair<QString,QString>>{{"sod","ium"},{"cre","ate"},{"dyn","amic lights"},{"jei",""},{"clo","ud"}})type(a,b);
        provider->setCurrentIndex(1);emit provider->activated(1);QTest::qWait(4000);
        for(auto [a,b]:QList<std::pair<QString,QString>>{{"cre","ate"},{"je","i"},{"clo","ud"}})type(a,b);
    }
    void profileLag(){
        // How long the window freezes for common actions on a full instance: real mod jars (EBALIA_PROFILE_MODS), resource
        // packs, large shader packs, worlds and screenshots. Opt-in: EBALIA_PROFILE=1.
        if(!qEnvironmentVariableIsSet("EBALIA_PROFILE"))QSKIP("Opt-in profiling (EBALIA_PROFILE=1)");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QString dir;{McInstanceManager setup(data.path());dir=setup.createInstance("Heavy","1.20.1","forge");}
        const auto source=qEnvironmentVariable("EBALIA_PROFILE_MODS");int mods=0;
        for(int round=0;round<4&&mods<40;++round)for(const auto &jar:QDir(source).entryList({"*.jar"},QDir::Files))if(mods<40&&QFile::copy(source+"/"+jar,dir+"/mods/"+QString::number(round)+"-"+jar))++mods;
        QTemporaryDir stage;
        for(int n=0;n<10;++n){QDir().mkpath(stage.path()+"/p"+QString::number(n));QFile meta(stage.path()+"/p"+QString::number(n)+"/pack.mcmeta");meta.open(QIODevice::WriteOnly);meta.write(R"({"pack":{"pack_format":15,"description":"Pack"}})");meta.close();
            QImage icon(128,128,QImage::Format_ARGB32);icon.fill(QColor(20*n,90,160));icon.save(stage.path()+"/p"+QString::number(n)+"/pack.png");QDir().mkpath(dir+"/resourcepacks");Archive::compress(stage.path()+"/p"+QString::number(n),dir+"/resourcepacks/Pack"+QString::number(n)+".zip");}
        QDir().mkpath(dir+"/shaderpacks");for(int n=0;n<3;++n){QFile zip(dir+"/shaderpacks/Shader"+QString::number(n)+".zip");zip.open(QIODevice::WriteOnly);QByteArray chunk(1<<20,char(n));for(int m=0;m<25;++m)zip.write(chunk);}
        for(int n=0;n<3;++n){QDir().mkpath(dir+"/saves/World"+QString::number(n));QFile level(dir+"/saves/World"+QString::number(n)+"/level.dat");level.open(QIODevice::WriteOnly);level.write("x");QImage(64,64,QImage::Format_ARGB32).save(dir+"/saves/World"+QString::number(n)+"/icon.png");}
        QDir().mkpath(dir+"/screenshots");for(int n=0;n<20;++n){QImage shot(1920,1080,QImage::Format_RGB32);shot.fill(QColor(10*n,120,200));shot.save(dir+"/screenshots/2026-10-1"+QString::number(n%10)+"_"+QString::number(n)+".png");}
        qInfo()<<"instance:"<<mods<<"mods";qunsetenv("EBALIA_NO_NETWORK");auto offline=qScopeGuard([]{qputenv("EBALIA_NO_NETWORK","1");});
        // How long a task the person starts waits behind background work (icons, lookups) before it runs.
        auto queueWait=[]{QElapsedTimer t;t.start();auto f=QtConcurrent::run([t]{return t.elapsed();});while(!f.isFinished())QCoreApplication::processEvents(QEventLoop::AllEvents,5);return f.result();};
        // A timer that should fire every 5 ms: the longest gap is the longest freeze.
        QElapsedTimer clock;clock.start();qint64 last=0,worst=0;QTimer beat;beat.setInterval(5);QObject::connect(&beat,&QTimer::timeout,[&]{const auto now=clock.elapsed();worst=std::max(worst,now-last);last=now;});beat.start();
        // Private memory of the launcher, the figure task managers show (Linux only here: the anonymous pages).
        auto memory=[]{QFile f("/proc/self/smaps_rollup");if(!f.open(QIODevice::ReadOnly))return QString();for(const auto &line:f.readAll().split('\n'))if(line.startsWith("Anonymous:"))return QString(" · memory %1 MB").arg(line.mid(10).trimmed().split(' ').first().toLongLong()/1024);return QString();};
        auto measure=[&](const char *name,std::function<void()> action,int settle){QTest::qWait(50);worst=0;last=clock.elapsed();QElapsedTimer t;t.start();action();const auto call=t.elapsed();QTest::qWait(settle);
            qInfo().noquote()<<QString("%1: call %2 ms, worst freeze %3 ms").arg(name,-28).arg(call,5).arg(worst,5)+memory();}; // the beat cannot fire while the window is held, so the gap includes any freeze
        std::unique_ptr<MainWindow> window;
        measure("start (window)",[&]{window=std::make_unique<MainWindow>();window->resize(1920,1080);window->show();},1500);
        measure("full repaint",[&]{window->repaint();},100);
        InstanceGrid *grid=nullptr;InstanceDetail *detail=nullptr;
        for(auto w:window->findChildren<QWidget*>()){if(auto g=dynamic_cast<InstanceGrid*>(w))grid=g;if(auto d=dynamic_cast<InstanceDetail*>(w))detail=d;}QVERIFY(grid&&detail);
        measure("open Instances page",[&]{window->showPage(MainWindow::Instances);},300);
        measure("open instance page",[&]{grid->open(dir);},800);
        measure("back to all instances",[&]{detail->back();},300);
        measure("open instance page again",[&]{grid->open(dir);},800);
        // Inside a window: the action runs once the window is open; the window closes after it settles.
        auto inWindow=[&](const char *name,std::function<void()> open,std::function<void(QDialog*)> inside){
            auto poll=std::make_shared<std::function<void()>>();
            *poll=[&,name,inside,poll]{
                QDialog *d=nullptr;for(auto w:QApplication::topLevelWidgets())if((w->objectName()=="modsWindow"||w->objectName()=="contentWindow")&&w->isVisible())d=qobject_cast<QDialog*>(w);
                if(!d){QTimer::singleShot(5,window.get(),[poll]{(*poll)();});return;}
                QTimer::singleShot(400,d,[&,name,inside,d,poll]{measure(name,[&]{inside(d);},600);d->reject();*poll=nullptr;});};
            QTimer::singleShot(0,window.get(),[poll]{(*poll)();});
            QElapsedTimer opened;opened.start();bool seen=false;
            QTimer::singleShot(0,window.get(),[&]{std::function<void()> wait;auto check=std::make_shared<std::function<void()>>();*check=[&,check]{for(auto w:QApplication::topLevelWidgets())if((w->objectName()=="modsWindow"||w->objectName()=="contentWindow")&&w->isVisible()){
                if(!seen){seen=true;qInfo().noquote()<<QString("%1: open to visible %2 ms, a new task waits %3 ms, pool busy %4/%5").arg(name,-28).arg(opened.elapsed()).arg(queueWait()).arg(QThreadPool::globalInstance()->activeThreadCount()).arg(QThreadPool::globalInstance()->maxThreadCount());}*check=nullptr;return;}
                QTimer::singleShot(1,window.get(),[check]{if(*check)(*check)();});};(*check)();});
            open();};
        auto buttonNamed=[](QDialog *d,const QString &text){for(auto b:d->findChildren<QPushButton*>())if(b->text().trimmed()==text)return b;return (QPushButton*)nullptr;};
        inWindow("mods window: remove a mod",[&]{window->openMods(dir);},[&](QDialog *d){auto list=d->findChild<QListWidget*>("modList");list->setCurrentRow(0);buttonNamed(d,Language::text("Quitar mod","Remove mod","Remover mod"))->click();});
        inWindow("mods window: turn one off",[&]{window->openMods(dir);},[&](QDialog *d){auto list=d->findChild<QListWidget*>("modList");list->setCurrentRow(1);buttonNamed(d,Language::text("Activar / desactivar","Enable / disable","Ativar / desativar"))->click();});
        for(const auto folder:{"resourcepacks","shaderpacks","saves"})
            inWindow((QString(folder)+" window: toggle").toUtf8().constData(),[&,folder]{window->openContent(dir,folder);},[&](QDialog *d){auto list=d->findChild<QListWidget*>("contentList");list->setCurrentRow(0);if(auto t=d->findChild<QPushButton*>("toggleContent"))t->click();});
        measure("Skins page",[&]{window->showPage(MainWindow::Skins);},500);
        measure("Guide page",[&]{window->showPage(MainWindow::Guide);},300);
        measure("Settings page",[&]{window->showPage(MainWindow::Settings);},300);
        measure("Lost versions page",[&]{window->showPage(MainWindow::Lost);},300);
        measure("Home page",[&]{window->showPage(MainWindow::Home);},300);
        // Discover: the list, going down it (each mod's page, pictures and originals), the picture viewer.
        auto results=window->findChild<QListWidget*>("modResults");
        measure("Discover page (search)",[&]{window->showPage(MainWindow::Explore);},100);QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,30000);
        measure("going down 10 mods",[&]{for(int n=0;n<10&&n<results->count();++n){results->setCurrentRow(n);QTest::qWait(250);}},2500);
        measure("staying on a mod (originals)",[&]{results->setCurrentRow(0);},3000);
        qInfo()<<"a new task waits"<<queueWait()<<"ms at the end";
    }
    void discoverMarksInstalled(){
        // A project the instance has says so in the list, and Install neither counts nor installs it again.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QString dir;{McInstanceManager setup(data.path());dir=setup.createInstance("Packs","1.21.1","fabric");}
        QDir().mkpath(dir+"/shaderpacks");{QFile zip(dir+"/shaderpacks/Vivid.zip");QVERIFY(zip.open(QIODevice::WriteOnly));zip.write("zip");}
        ModRepository::write(dir+"/packs.json",QJsonObject{{"packs",QJsonArray{QJsonObject{{"project_id","vivid"},{"kind","shader"},{"filename","Vivid.zip"},{"title","Vivid"}}}}});
        MainWindow window;window.resize(1280,820);window.show();auto results=window.findChild<QListWidget*>("modResults");
        for(const auto &[id,name]:QList<std::pair<QString,QString>>{{"vivid","Vivid Horizon"},{"other","Other Shader"}}){
            auto item=new QListWidgetItem(name,results);item->setData(Qt::UserRole,QJsonObject{{"project_id",id},{"title",name},{"author","Someone"},{"description","Shader"}});}
        window.showPage(MainWindow::Explore);
        QVERIFY(results->item(0)->text().startsWith("✓"));QVERIFY(results->item(0)->text().contains(Language::key("Already installed")));QVERIFY(!results->item(1)->text().startsWith("✓"));
        auto install=window.findChild<QPushButton*>("modPreviewInstall");
        results->setCurrentRow(0);QVERIFY(!install->isEnabled());QVERIFY(install->text().contains(Language::key("Already installed")));
        results->setCurrentRow(1);QVERIFY(install->isEnabled());
        results->item(0)->setSelected(true);results->item(1)->setSelected(true);QVERIFY(install->isEnabled());QVERIFY(!install->text().contains("(2)"));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())window.grab().save(out+"/discover-installed.png");
        // The pack removed from its folder: installable again.
        QFile::remove(dir+"/shaderpacks/Vivid.zip");window.showPage(MainWindow::Home);window.showPage(MainWindow::Explore);QVERIFY(!results->item(0)->text().startsWith("✓"));
    }
    void liveLog(){
        QTemporaryDir dir;auto write=[&](const QByteArray &bytes,bool fresh){QFile f(dir.path()+"/launcher.log");if(f.open(fresh?QIODevice::WriteOnly:QIODevice::Append))f.write(bytes);};
        write("[12:00:00] [main/INFO]: Loading Minecraft\n",true);
        bool running=true;LogWindow log(dir.path(),"Test",[&]{return running;},nullptr);log.show();
        auto view=log.findChild<QPlainTextEdit*>("logView");auto state=log.findChild<QLabel*>("logState");QVERIFY(view&&state);
        QVERIFY(view->toPlainText().contains("Loading Minecraft"));QVERIFY(state->text().contains(Language::key("Live")));
        // Lines appear while the game writes them; a line still being written waits for its end.
        write("[12:00:01] [Render thread/ERROR]: Missing texture\n[12:00:02] [main/WARN]: Slow",false);
        QTRY_VERIFY(view->toPlainText().contains("Missing texture"));QVERIFY(!view->toPlainText().contains("Slow"));
        write(" chunk\n",false);QTRY_VERIFY(view->toPlainText().contains("Slow chunk"));
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())log.grab().save(out+"/live-log.png");
        auto filter=log.findChild<QLineEdit*>("logFilter");filter->setText("error");QCOMPARE(view->toPlainText(),QString("[12:00:01] [Render thread/ERROR]: Missing texture"));filter->clear();
        // A new game writes the file from the beginning again; then the game closes.
        write("[13:00:00] [main/INFO]: Second run\n",true);running=false;
        QTRY_VERIFY(view->toPlainText().startsWith("[13:00:00]"));QVERIFY(!view->toPlainText().contains("Loading Minecraft"));QTRY_VERIFY(!state->text().contains(Language::key("Live")));
    }
    void comboPopupUnderTheBox(){
        // The list opens under the box with one border, not over the box with empty strips above and below it.
        qApp->setStyleSheet(Ui::styleSheet());QWidget host;auto box=new QComboBox(&host);box->addItems({"Mods","Resource packs","Shader packs"});box->setCurrentIndex(1);box->setGeometry(20,20,240,40);
        host.resize(300,300);host.show();QVERIFY(QTest::qWaitForWindowExposed(&host));
        box->showPopup();auto popup=box->view()->window();QTRY_VERIFY(popup->isVisible());
        QVERIFY(popup->mapToGlobal(QPoint(0,0)).y()>=box->mapToGlobal(QPoint(0,box->height())).y()-2);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty())popup->grab().save(out+"/combo-popup.png");
        box->hidePopup();
    }
    void deleteSkin(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QImage image(64,64,QImage::Format_ARGB32);image.fill(QColor(30,140,200));const auto png=data.path()+"/blue.png";QVERIFY(image.save(png));
        SkinManager(data.path()).importSkin(png,"Blue","classic");SkinManager(data.path()).importSkin(png,"Blue slim","slim");
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Skins);
        auto list=window.findChild<QListWidget*>("skinList");auto remove=window.findChild<QPushButton*>("deleteSkin");QVERIFY(list&&remove);QCOMPARE(list->count(),2);
        list->setCurrentRow(1);QVERIFY(remove->isEnabled());
        QTimer::singleShot(300,&window,[]{for(auto w:QApplication::topLevelWidgets())if(auto box=qobject_cast<QMessageBox*>(w);box&&box->isVisible())box->button(QMessageBox::Yes)->click();});
        remove->click();QCOMPARE(list->count(),1);QCOMPARE(list->currentRow(),0);QCOMPARE(SkinManager(data.path()).skins().size(),1);QVERIFY(QFile::exists(png));
    }
    void skinHeads(){
        // Account pictures become skin heads: Microsoft (a public profile) and a profile without premium (default skin
        // from a downloaded game, EBALIA_TEST_VERSIONS); the Skins list shows the head next to each skin.
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qunsetenv("EBALIA_NO_NETWORK");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");qputenv("EBALIA_NO_NETWORK","1");});
        QDir().mkpath(data.path()+"/mc");if(const auto versions=qEnvironmentVariable("EBALIA_TEST_VERSIONS");!versions.isEmpty())QFile::link(versions,data.path()+"/mc/versions");
        {AccountManager accounts(data.path()+"/mc");accounts.addAccount({"msa","Notch","069a79f4-44e9-4726-a5be-fca90e38aaf5","x","y"});accounts.addAccount({"offline","Glitchy","3f9c1d2e-8a7b-4c6d-9e0f-1a2b3c4d5e6f",{},{}});}
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Skins);
        QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(data.path()+"/mc/avatars/069a79f4-44e9-4726-a5be-fca90e38aaf5-skin.png"),20000);
        if(!qEnvironmentVariable("EBALIA_TEST_VERSIONS").isEmpty())QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(data.path()+"/mc/avatars/3f9c1d2e-8a7b-4c6d-9e0f-1a2b3c4d5e6f-skin.png"),20000);
        QTest::qWait(1500);
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){window.grab().save(out+"/skins-heads.png");
            auto list=window.findChild<QListWidget*>("accountList");if(list)list->grab().save(out+"/account-list.png");}
        QVERIFY(!window.findChild<QListWidget*>("skinList")->item(0)->icon().isNull());
    }
    void releaseDetection(){
        QVERIFY(!UpdateChecker::releasePage(QJsonObject{{"html_url","https://github.com/ebalia-real/launcher/releases/tag/v1.1.0"}},false).isEmpty());
        QVERIFY(UpdateChecker::releasePage(QJsonObject{{"html_url","https://github.com/other/launcher/releases/tag/v1.1.0"}},false).isEmpty());
        QVERIFY(UpdateChecker::newerStable("v4.0.1","4.0.0"));QVERIFY(!UpdateChecker::newerStable("v4.0.1","v4.1.0"));QVERIFY(UpdateChecker::newerStable("4.10.0","4.9.0"));
        for(const QString &tag:{QString("v4.0.0"),QString("3.9.0"),QString("v5.0.0-beta"),QString("latest")})QVERIFY(!UpdateChecker::newerStable(tag,"4.0.0"));
        QVERIFY(UpdateChecker::releasePage(QJsonObject{{"html_url","https://evil.test/ebalia-real/launcher/releases/tag/v5.0.0"}},false).isEmpty());
    }
    void backgroundCarousel(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");QSettings().setValue("ui/rotateBackgrounds",true);
        MainWindow window;window.resize(1280,800);window.show();window.showPage(MainWindow::Home);
        auto banner=window.findChild<HomeBanner*>();QVERIFY(banner);auto timer=banner->findChild<QTimer*>("heroRotation");QVERIFY(timer);
        auto pause=banner->findChild<QToolButton*>("heroPause");auto next=banner->findChild<QToolButton*>("heroNext");auto previous=banner->findChild<QToolButton*>("heroPrevious");QVERIFY(pause&&next&&previous);
        QVERIFY(timer->isActive());QCOMPARE(banner->slideCount(),4);
        QTest::mouseClick(pause,Qt::LeftButton);QVERIFY(!timer->isActive());QVERIFY(!QSettings().value("ui/rotateBackgrounds").toBool());
        for(int i=1;i<=banner->slideCount();++i){QTest::mouseClick(next,Qt::LeftButton);QCOMPARE(banner->currentSlide(),i%banner->slideCount());QVERIFY(!timer->isActive());}
        QTest::mouseClick(previous,Qt::LeftButton);QCOMPARE(banner->currentSlide(),banner->slideCount()-1);
        auto dots=banner->findChildren<QToolButton*>("heroDot");QCOMPARE(dots.size(),banner->slideCount());QTest::mouseClick(dots[1],Qt::LeftButton);QCOMPARE(banner->currentSlide(),1);QVERIFY(dots[1]->isChecked());
        {HomeBanner another;another.show();QVERIFY(another.findChild<QToolButton*>("heroPause")->isChecked());QVERIFY(!another.findChild<QTimer*>("heroRotation")->isActive());}
        timer->setInterval(80);QTest::mouseClick(pause,Qt::LeftButton);QTRY_VERIFY_WITH_TIMEOUT(banner->currentSlide()!=1,500);
        window.showPage(MainWindow::Instances);QVERIFY(!timer->isActive());const int hidden=banner->currentSlide();QTest::qWait(180);QCOMPARE(banner->currentSlide(),hidden);
        window.showPage(MainWindow::Home);QVERIFY(timer->isActive());window.close();QSettings().remove("ui/rotateBackgrounds");
    }
    void responsiveHomeAndArchive(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        MainWindow window;window.show();auto pages=window.findChild<QStackedWidget*>();QVERIFY(pages);
        // The first lost version counts as installed, so its Uninstall and Log buttons are in the row too.
        {auto lost=window.findChild<QListWidget*>("lostList");QVERIFY(lost&&lost->count()>1);const auto id=lost->item(0)->data(Qt::UserRole).toString();
         QDir().mkpath(data.path()+"/versions/"+id);QFile mark(data.path()+"/versions/"+id+"/.installed.json");QVERIFY(mark.open(QIODevice::WriteOnly));mark.write("{}");mark.close();
         lost->setCurrentRow(1);lost->setCurrentRow(0);}
        for(const QSize size:{QSize(640,480),QSize(800,600),QSize(1280,800),QSize(1920,1080)}){
            window.resize(size);window.showPage(MainWindow::Home);QTest::qWait(80);QCOMPARE(window.size(),size);
            auto home=qobject_cast<QScrollArea*>(pages->currentWidget());QVERIFY(home);QCOMPARE(home->horizontalScrollBar()->maximum(),0);
            auto play=window.findChild<QPushButton*>("homePlay");auto picker=window.findChild<QPushButton*>("instancePicker");auto bar=window.findChild<QFrame*>("playBar");
            QRect playRect(play->mapTo(bar,QPoint()),play->size()),pickerRect(picker->mapTo(bar,QPoint()),picker->size());
            QVERIFY(bar->rect().contains(playRect));QVERIFY(bar->rect().contains(pickerRect));QVERIFY(!playRect.intersects(pickerRect));
            window.showPage(MainWindow::Lost);QTest::qWait(80);auto archive=qobject_cast<QScrollArea*>(pages->currentWidget());QVERIFY(archive);QVERIFY2(archive->horizontalScrollBar()->maximum()==0,qPrintable(QString("Lost archive overflows by %1 px at %2 px").arg(archive->horizontalScrollBar()->maximum()).arg(size.width())));
            auto detail=window.findChild<QFrame*>("lostDetail");auto install=window.findChild<QPushButton*>("lostInstall");auto launch=window.findChild<QPushButton*>("lostPlay");
            QRect installRect(install->mapTo(detail,QPoint()),install->size()),launchRect(launch->mapTo(detail,QPoint()),launch->size());
            QVERIFY(detail->rect().contains(installRect));QVERIFY(detail->rect().contains(launchRect));QVERIFY(!installRect.intersects(launchRect));
            QVERIFY(installRect.left()-launchRect.right()>=10);
            auto uninstall=window.findChild<QPushButton*>("lostUninstall");QVERIFY(uninstall->isVisible());QVERIFY(window.findChild<QToolButton*>("lostLog")->isVisible());
            QVERIFY(detail->rect().contains(QRect(uninstall->mapTo(detail,QPoint()),uninstall->size())));
            if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()&&size==QSize(1280,800))window.grab().save(out+"/lost-installed.png");
            for(int page=0;page<=MainWindow::Settings;++page){
                window.showPage(page);QTest::qWait(30);auto area=qobject_cast<QScrollArea*>(pages->currentWidget());
                QVERIFY2(area->horizontalScrollBar()->maximum()==0,qPrintable(QString("Page %1 overflows at %2 px").arg(page).arg(size.width())));
            }
        }
        auto list=window.findChild<QListWidget*>("lostList");QVERIFY(list);QSet<QByteArray> images;
        for(int i=0;i<list->count();++i){auto path=list->item(i)->data(Qt::UserRole+1).toJsonObject()["art"].toString();QFile image(path);QVERIFY2(image.open(QIODevice::ReadOnly),qPrintable(path));images.insert(QCryptographicHash::hash(image.readAll(),QCryptographicHash::Sha256));}
        QCOMPARE(images.size(),list->count());window.close();
    }
    void compactInstanceDetail(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        {McInstanceManager manager(data.path());manager.createInstance("Mi mundo de aventuras con amigos","1.20.1","vanilla");}
        MainWindow window;window.resize(640,480);window.show();window.showPage(MainWindow::Instances);QTest::qWait(60);
        auto card=window.findChild<QFrame*>("instanceCard");QVERIFY(card);QTest::mouseClick(card,Qt::LeftButton,{},QPoint(30,30));QTest::qWait(60);
        auto content=window.findChild<QWidget*>("instanceDetailContent");auto play=window.findChild<QPushButton*>("detailPlay");auto name=content->findChild<QLabel*>("detailTitle");
        QVERIFY(content&&play&&name);QRect playRect(play->mapTo(content,QPoint()),play->size()),nameRect(name->mapTo(content,QPoint()),name->size());
        QVERIFY(content->rect().contains(playRect));QVERIFY(content->rect().contains(nameRect));QVERIFY(!playRect.intersects(nameRect));
        QCOMPARE(content->width(),content->parentWidget()->width());
        for(auto stat:content->findChildren<QFrame*>("statCard"))QVERIFY(content->rect().contains(QRect(stat->mapTo(content,QPoint()),stat->size())));
    }
    void instanceGalleryServersAndIcon(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        QString dir;{McInstanceManager manager(data.path());dir=manager.createInstance("Galería","1.20.1","fabric");}
        QDir().mkpath(dir+"/screenshots");
        for(int n=0;n<3;++n){QImage shot(320,180,QImage::Format_RGB32);shot.fill(QColor::fromHsv(n*100,180,200));QVERIFY(shot.save(dir+QString("/screenshots/2026-10-0%1_12.00.00.png").arg(n+1)));}
        ServerList::add(dir+"/servers.dat","Hypixel","mc.hypixel.net");ServerList::add(dir+"/servers.dat","","play.example.org");
        MainWindow window;window.resize(1280,900);window.show();window.showPage(MainWindow::Instances);QTest::qWait(60);
        auto card=window.findChild<QFrame*>("instanceCard");QVERIFY(card);QTest::mouseClick(card,Qt::LeftButton,{},QPoint(30,30));QTest::qWait(60);
        auto gallery=window.findChild<QListWidget*>("instanceGallery");auto servers=window.findChild<QListWidget*>("instanceServers");QVERIFY(gallery&&servers);
        QTRY_COMPARE(gallery->count(),3);QCOMPARE(servers->count(),2);QVERIFY(servers->item(1)->text().contains("play.example.org"));
        QTRY_VERIFY(!gallery->item(0)->icon().isNull());
        // Built-in icon from the instance page menu.
        auto icon=window.findChild<QToolButton*>("detailIcon");QVERIFY(icon&&icon->menu());
        QAction *tnt=nullptr;for(auto a:icon->menu()->actions())if(a->text()==InstanceIcons::name("tnt"))tnt=a;
        QVERIFY(tnt);QVERIFY(icon->menu()->findChild<QAction*>("customInstanceIcon"));tnt->trigger();
        QCOMPARE(ModRepository::read(dir+"/instance.json")["icon"].toString(),QString("tnt"));
        // A picture from the computer, after the instance exists.
        auto detail=static_cast<InstanceDetail*>(window.findChild<QWidget*>("instanceDetail"));QVERIFY(detail&&detail->changeIcon);
        QImage picture(400,300,QImage::Format_ARGB32);picture.fill(QColor(200,40,180));detail->changeIcon("custom",picture.scaled(128,128));
        QCOMPARE(ModRepository::read(dir+"/instance.json")["icon"].toString(),QString("custom"));QVERIFY(QFile::exists(dir+"/instance-icon.png"));
        QCOMPARE(QImage(dir+"/instance-icon.png").size(),QSize(128,128));
        const auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!out.isEmpty()){QDir().mkpath(out);QTest::qWait(300);QVERIFY(window.grab().save(out+"/instance-detail-extras.png"));}
    }
    void instanceLogUnderTheGallery(){
        // The game's log lives in the instance page, under the screenshots and servers; full screen gives it the whole page.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        QString dir;{McInstanceManager manager(data.path());dir=manager.createInstance("Registro","1.20.1","fabric");}
        {QFile log(dir+"/launcher.log");QVERIFY(log.open(QIODevice::WriteOnly));log.write("[12:00:00] [Render thread/INFO]: Setting user: Steve\n[12:00:01] [main/ERROR]: Missing texture\n");}
        MainWindow window;window.resize(1280,900);window.show();window.showPage(MainWindow::Instances);QTest::qWait(60);
        auto card=window.findChild<QFrame*>("instanceCard");QVERIFY(card);QTest::mouseClick(card,Qt::LeftButton,{},QPoint(30,30));QTest::qWait(60);
        auto detail=static_cast<InstanceDetail*>(window.findChild<QWidget*>("instanceDetail"));auto content=window.findChild<QWidget*>("instanceDetailContent");
        auto panel=dynamic_cast<LogView*>(detail->findChild<QWidget*>("logPanel"));auto view=detail->findChild<QPlainTextEdit*>("logView");auto servers=window.findChild<QListWidget*>("instanceServers");
        QVERIFY(panel&&view&&servers&&content);QCOMPARE(panel->folder(),dir);
        QTRY_VERIFY(view->toPlainText().contains("Missing texture"));
        QVERIFY(panel->mapTo(content,QPoint()).y()>servers->mapTo(content,QPoint()).y()+servers->height()); // below the screenshots and servers
        // Lines the game writes appear while the page is open.
        {QFile log(dir+"/launcher.log");QVERIFY(log.open(QIODevice::Append));log.write("[12:00:02] [Server thread/INFO]: Done\n");}
        QTRY_VERIFY(view->toPlainText().contains("Server thread/INFO]: Done"));
        // Full screen: the same view, filling the page; Escape or the back link return it.
        auto expand=detail->findChild<QToolButton*>("logFullScreen");auto page=detail->findChild<QWidget*>("logPage");QVERIFY(expand&&page);
        auto filter=panel->findChild<QLineEdit*>("logFilter");filter->setText("error");QCOMPARE(view->toPlainText(),QString("[12:00:01] [main/ERROR]: Missing texture"));
        expand->click();QVERIFY(page->isVisible());QVERIFY(page->isAncestorOf(panel));QVERIFY(!content->isVisible());
        QVERIFY(panel->height()>detail->height()/2);QCOMPARE(filter->text(),QString("error")); // what the view had stays
        QCOMPARE(expand->toolTip(),Language::key("Exit full screen"));
        const auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!out.isEmpty()){QDir().mkpath(out);QVERIFY(window.grab().save(out+"/instance-log-full.png"));}
        QTest::keyClick(view,Qt::Key_Escape);QVERIFY(!page->isVisible());QVERIFY(content->isVisible());QVERIFY(!page->isAncestorOf(panel));QVERIFY(panel->isVisible());
        filter->clear();window.findChild<QToolButton*>("showLog")->click();QVERIFY(page->isVisible()); // the log button at the top opens it full screen
        detail->findChild<QPushButton*>("logBack")->click();QVERIFY(content->isVisible());
        if(!out.isEmpty()){auto scroll=qobject_cast<QScrollArea*>(content->parentWidget()->parentWidget());if(scroll)scroll->ensureWidgetVisible(panel);QTest::qWait(100);QVERIFY(window.grab().save(out+"/instance-log.png"));}
    }
    void changelogPage(){
        // Every version of the bundled CHANGELOG, newest first, opened from the sidebar or from the version under it.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");});
        QFile file(":/CHANGELOG.md");QVERIFY(file.open(QIODevice::ReadOnly));const auto changelog=QString::fromUtf8(file.readAll());
        const QRegularExpression heading(R"(^# (\d+\.\d+\.\d+))",QRegularExpression::MultilineOption);
        int versions=0;for(auto it=heading.globalMatch(changelog);it.hasNext();it.next())++versions;QVERIFY(versions>=12);
        MainWindow window;window.resize(1280,820);window.show();
        auto nav=window.findChild<QListWidget*>("navigation");QCOMPARE(nav->count(),int(MainWindow::Settings)); // every page but Settings
        nav->setCurrentRow(MainWindow::Changelog);
        auto list=window.findChild<QListWidget*>("changelogList");auto notes=window.findChild<QTextBrowser*>("changelogNotes");auto title=window.findChild<QLabel*>("changelogTitle");
        QVERIFY(list&&notes&&title);QTRY_VERIFY(list->isVisible());QCOMPARE(list->count(),versions);
        QCOMPARE(list->item(0)->data(Qt::UserRole).toString(),heading.match(changelog).captured(1));
        int row=-1;for(int n=0;n<list->count();++n)if(list->item(n)->data(Qt::UserRole).toString()=="1.2.0")row=n;QVERIFY(row>=0);
        list->setCurrentRow(row);QCOMPARE(title->text(),QString("EBALIA Launcher 1.2.0"));QVERIFY(notes->toPlainText().contains("Las versiones perdidas instalan solo su juego")); // in the launcher's language
        QLabel *english=nullptr;for(auto label:window.findChildren<QLabel*>())if(label->text()==Language::key("These notes are in English."))english=label;QVERIFY(english&&!english->isVisible()); // translated, so no notice
        QVERIFY(!notes->toPlainText().contains("# 1.1.9")); // one version at a time
        QVERIFY(list->item(row)->text().contains("15/10/26")||list->item(row)->text().contains("2026")); // its date
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);QTest::qWait(100);QVERIFY(window.grab().save(out+"/changelog.png"));}
        // The search keeps the versions that mention it and marks it in the notes.
        auto filter=window.findChild<QLineEdit*>("changelogFilter");filter->setText("1.Li02");
        int shown=0;for(int n=0;n<list->count();++n)if(!list->item(n)->isHidden())++shown;
        QVERIFY(shown>=1&&shown<versions);QVERIFY(list->currentItem()&&!list->currentItem()->isHidden());QVERIFY(!notes->extraSelections().isEmpty());
        filter->setText("no version says this");QCOMPARE(title->text(),Language::key("No version matches this search."));
        filter->clear();for(int n=0;n<list->count();++n)QVERIFY(!list->item(n)->isHidden());
        // The version under the sidebar leads here too.
        window.showPage(MainWindow::Home);QCOMPARE(nav->currentRow(),int(MainWindow::Home));
        auto version=window.findChild<QPushButton*>("sidebarVersion");QVERIFY(version);version->click();QCOMPARE(nav->currentRow(),int(MainWindow::Changelog));
    }
    void sidebarIconsAndWebsite(){
        // The sidebar uses pixel art in the game's style, each icon its own, enlarged so every pixel stays sharp. Home shows EBALIA's website.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");auto restore=qScopeGuard([]{qunsetenv("EBALIA_DATA_DIR");QDesktopServices::unsetUrlHandler("https");});
        QVERIFY(!QFile::exists(":/icons/navigation/house.png")); // the earlier artwork is gone
        MainWindow window;window.resize(1280,820);window.show();window.showPage(MainWindow::Home);
        auto nav=window.findChild<QListWidget*>("navigation");QVERIFY(nav);QCOMPARE(nav->iconSize(),QSize(32,32));
        auto sharp=[](const QIcon &icon){const auto image=icon.pixmap(QSize(32,32)).toImage().convertToFormat(QImage::Format_ARGB32);if(image.size()!=QSize(32,32))return false;
            int opaque=0;for(int y=0;y<32;y+=2)for(int x=0;x<32;x+=2){const auto p=image.pixel(x,y);if(image.pixel(x+1,y)!=p||image.pixel(x,y+1)!=p||image.pixel(x+1,y+1)!=p)return false;if(qAlpha(p)==255)++opaque;}
            return opaque>60;}; // whole pixels of a 16-pixel drawing, and a real drawing
        QList<QImage> seen;
        for(int n=0;n<nav->count();++n){
            const auto icon=nav->item(n)->icon();const auto name=nav->item(n)->text().trimmed();QVERIFY2(sharp(icon),qPrintable(name));
            const auto image=icon.pixmap(QSize(32,32)).toImage();QVERIFY2(!seen.contains(image),qPrintable(name+" repeats another icon"));seen<<image;
            QCOMPARE(icon.pixmap(QSize(32,32),QIcon::Selected).toImage(),image); // the page shown keeps its colours
        }
        auto settings=window.findChild<QPushButton*>("navSettings");QCOMPARE(settings->iconSize(),QSize(32,32));QVERIFY(sharp(settings->icon()));QVERIFY(!seen.contains(settings->icon().pixmap(QSize(32,32)).toImage()));
        // The website: the site's own preview card in the launcher's language, what it has, and a way to open it, between the mods and the news.
        QVERIFY(!window.findChild<QPushButton*>("heroWebsite")); // not a link on the banner
        auto card=window.findChild<QPushButton*>("websiteCard");QVERIFY(card);QTRY_VERIFY(card->isVisible());QCOMPARE(card->accessibleName(),Language::key("Official website"));
        auto preview=card->findChild<QWidget*>("websitePreview");QVERIFY(preview);
        for(const auto &language:Language::available())QVERIFY2(QImageReader(":/website/ebalia-"+language+".jpg").size()==QSize(1200,630),qPrintable(language)); // in every language
        for(const auto &language:Language::available()){if(language=="en")continue;QFile f(":/locales/"+language+".json");QVERIFY(f.open(QIODevice::ReadOnly));const auto data=QJsonDocument::fromJson(f.readAll()).object();
            for(auto key:{"Official website","Minecraft horror mods, its own launcher and lost versions.","Everything EBALIA makes, in one place.","Visit the website"})QVERIFY2(data[key].toString()!=QString(key),qPrintable(language+": "+key));}
        QTRY_VERIFY(!preview->grab().toImage().pixelColor(preview->width()/4,preview->height()/3).name().startsWith("#181818")); // the picture, not the empty cover
        QStringList texts;for(auto label:card->findChildren<QLabel*>())texts<<label->text();
        QVERIFY(texts.contains(Language::key("Minecraft horror mods, its own launcher and lost versions.")));QVERIFY(texts.contains("EBALIA.GITGUD.SITE"));
        auto mods=window.findChild<QWidget*>("homeMods"),news=window.findChild<QWidget*>("homeNews"),section=window.findChild<QWidget*>("homeWebsite");
        QVERIFY(mods->geometry().bottom()<=section->geometry().top()&&section->geometry().bottom()<=news->geometry().top());
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QDir().mkpath(out);QTest::qWait(300);QVERIFY(window.grab().save(out+"/home-sidebar.png"));QVERIFY(section->grab().save(out+"/home-website.png"));}
        QVERIFY2(qAbs(preview->width()/double(preview->height())-1200.0/630.0)<0.05,qPrintable(QString("%1x%2").arg(preview->width()).arg(preview->height()))); // whole, not cropped
        QDesktopServices::setUrlHandler("https",this,"captureUrl");
        for(auto open:{card,card->findChild<QPushButton*>("websiteVisit"),window.findChild<QPushButton*>("websiteLink")}){QVERIFY(open);lastUrl=QUrl();QTest::mouseClick(open,Qt::LeftButton);QCOMPARE(lastUrl,QUrl("https://ebalia.gitgud.site/"));}
        // The picture opens it too: a click on the window where it is, found the way a real one is.
        for(auto w=preview->parentWidget();w;w=w->parentWidget())if(auto area=qobject_cast<QScrollArea*>(w)){area->ensureWidgetVisible(preview,0,0);break;}
        QTest::qWait(30);lastUrl=QUrl();QTest::mouseClick(window.windowHandle(),Qt::LeftButton,{},preview->mapTo(&window,preview->rect().center()));QCOMPARE(lastUrl,QUrl("https://ebalia.gitgud.site/"));
        // In a narrow window the text goes under the picture, which stays whole.
        window.resize(700,700);QTest::qWait(50);window.showPage(MainWindow::Home);QTRY_VERIFY(preview->isVisible());
        auto visit=card->findChild<QPushButton*>("websiteVisit");QTRY_VERIFY(visit->mapTo(card,QPoint()).y()>preview->mapTo(card,QPoint()).y()+preview->height());
        QVERIFY(card->rect().contains(QRect(preview->mapTo(card,QPoint()),preview->size())));QVERIFY(card->rect().contains(QRect(visit->mapTo(card,QPoint()),visit->size())));
    }
    void lostScreenshotsAndFacts(){
        // Each lost version shows screenshots taken in it and what it is, in the player's language.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        VersionManager versions;versions.loadVersions();QVERIFY(!versions.getVersions().isEmpty());
        for(const auto &v:versions.getVersions()){
            QVERIFY2(v.screenshots>=3,qPrintable(v.id));QVERIFY2(!v.about.isEmpty()&&!v.game.isEmpty()&&v.installedSize>0,qPrintable(v.id));
            for(int n=1;n<=v.screenshots;++n){QImageReader shot(QString(":/lost/%1/%2.jpg").arg(v.id).arg(n));QVERIFY2(shot.canRead(),qPrintable(v.id));QCOMPARE(shot.size(),QSize(1280,720));}
            QVERIFY2(Language::key(v.about)!=v.about,qPrintable(v.id)); // translated
        }
        MainWindow window;window.resize(1400,900);window.show();window.showPage(MainWindow::Lost);QTest::qWait(60);
        auto list=window.findChild<QListWidget*>("lostList");auto shots=window.findChild<QListWidget*>("lostShots");auto about=window.findChild<QLabel*>("lostAbout");QVERIFY(list&&shots&&about);
        int ghost=-1;for(int n=0;n<list->count();++n)if(list->item(n)->data(Qt::UserRole).toString()=="ghost")ghost=n;QVERIFY(ghost>=0);
        list->setCurrentRow(ghost);QTest::qWait(50);VersionInfo v;for(const auto &entry:versions.getVersions())if(entry.id=="ghost")v=entry;
        QCOMPARE(shots->count(),v.screenshots);QCOMPARE(about->text(),Language::key(v.about));QVERIFY(about->text().contains("Locod"));
        QStringList facts;for(auto label:window.findChildren<QLabel*>("lostFactValue"))if(label->isVisible())facts<<label->text();
        QVERIFY(facts.contains("Minecraft Ghost"));QVERIFY(facts.contains(Language::key("Rescued by EBALIA")));QVERIFY(facts.contains("Locod"));
        QVERIFY(facts.join("|").contains(Language::key("Java 8, on Windows, macOS and Linux")));
        QTRY_VERIFY(!shots->item(2)->icon().isNull());
        if(auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");!out.isEmpty()){QTest::qWait(300);QVERIFY(window.grab().save(out+"/lost-screenshots.png"));}
        // The full-size viewer opens at the picture chosen, and leaves it chosen.
        QTimer::singleShot(200,&window,[&]{auto viewer=qobject_cast<QDialog*>(QApplication::activeModalWidget());if(!viewer)for(auto w:QApplication::topLevelWidgets())if(w->objectName()=="lostScreenshotsViewer"&&w->isVisible())viewer=qobject_cast<QDialog*>(w);
            QVERIFY(viewer);QCOMPARE(viewer->objectName(),QString("lostScreenshotsViewer"));QTest::keyClick(viewer,Qt::Key_Right);viewer->reject();});
        emit shots->itemActivated(shots->item(1));QCOMPARE(shots->currentRow(),2);
    }
    void dialogOpenedFromAPageStaysUsable(){
        // Regression: the skin import dialog (a child of a page) opened with Save and Cancel disabled.
        QMainWindow window;auto central=new QWidget;auto layout=new QVBoxLayout(central);auto page=new QWidget;layout->addWidget(page);auto behind=new QPushButton("behind");layout->addWidget(behind);window.setCentralWidget(central);window.show();
        QDialog dialog(page);auto form=new QFormLayout(&dialog);auto buttons=new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);form->addRow(buttons);
        connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
        QTimer::singleShot(200,&dialog,[&]{
            QVERIFY(dialog.isEnabled());auto save=buttons->button(QDialogButtonBox::Save);QVERIFY(save->isEnabled());QVERIFY(central->isEnabled()); // not greyed out
            bool clicked=false;QObject::connect(behind,&QPushButton::clicked,behind,[&]{clicked=true;});QTest::mouseClick(behind,Qt::LeftButton);QVERIFY(!clicked); // but blocked
            QTest::mouseClick(save,Qt::LeftButton);
        });
        QCOMPARE(Ui::openWindow(dialog),int(QDialog::Accepted));QVERIFY(central->isEnabled());
    }
    void compactInstanceWizard(){
        QTemporaryDir data;McInstanceManager manager(data.path());Language::current="es";
        CreateInstanceDialog dialog(&manager,{{"1.20.1","release","","2023-06-12"}},{},{},false);dialog.resize(640,480);dialog.show();
        for(int page=0;page<=CreateInstanceDialog::Technic;++page){
            dialog.showPage(page);QTest::qWait(30);QCOMPARE(dialog.size(),QSize(640,480));
            auto area=dialog.findChild<QScrollArea*>("instanceFormScroll");QVERIFY(area);QVERIFY2(area->horizontalScrollBar()->maximum()==0,qPrintable(QString("Instance page %1 overflows by %2 px").arg(page).arg(area->horizontalScrollBar()->maximum())));
            auto create=dialog.findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok);
            QVERIFY(dialog.rect().contains(QRect(create->mapTo(&dialog,QPoint()),create->size())));
        }
        auto sources=dialog.findChild<QListWidget*>("instanceSources");QVERIFY(sources->width()<100);for(int i=0;i<sources->count();++i){QVERIFY(!sources->item(i)->toolTip().isEmpty());QVERIFY(!sources->item(i)->icon().isNull());}
    }
    void everyLanguage(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());QSettings().setValue("ui/tutorialSeen",true);
        for(const auto &lang:Language::available()){
            qputenv("EBALIA_LANGUAGE",lang.toUtf8());MainWindow window;window.show();window.showPage(0);QTest::qWait(25);
            QCOMPARE(Language::current,lang);
            auto nav=window.findChild<QListWidget*>("navigation");QVERIFY(nav);QCOMPARE(nav->count(),int(MainWindow::Settings));
            auto pages=window.findChild<QStackedWidget*>();QVERIFY(pages);QCOMPARE(pages->count(),int(MainWindow::Settings)+1);
            for(int page=0;page<=MainWindow::Settings;++page){
                window.showPage(page);QTest::qWait(5);QVERIFY(window.isVisible());QCOMPARE(pages->currentIndex(),page);
                QCOMPARE(nav->currentRow(),page==MainWindow::Settings?-1:page);QCOMPARE(window.findChild<QPushButton*>("navSettings")->isChecked(),page==MainWindow::Settings);
                for(auto b:window.findChildren<QPushButton*>())if(b->isVisible())QVERIFY2(!b->text().trimmed().isEmpty()||!b->icon().isNull()||!b->accessibleName().isEmpty(),"Unnamed visible button");
            }
            nav->setCurrentRow(MainWindow::Lost);QCOMPARE(pages->currentIndex(),int(MainWindow::Lost));
            window.showPage(MainWindow::Community);auto tabs=window.findChild<QTabWidget*>("communityTabs");QVERIFY(tabs);QCOMPARE(tabs->count(),5);for(int tab=0;tab<5;++tab){tabs->setCurrentIndex(tab);QTest::qWait(5);}tabs->setCurrentIndex(0);
            QString dir=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!dir.isEmpty()){QDir().mkpath(dir);window.showPage(MainWindow::Home);QVERIFY(window.grab().save(dir+"/"+lang+"-home.png"));window.showPage(MainWindow::Guide);QVERIFY(window.grab().save(dir+"/"+lang+"-guide.png"));}
            window.showPage(MainWindow::Community);QCOMPARE(window.findChildren<QToolButton*>("fanartCard").size(),20);
            if(!dir.isEmpty()){QVERIFY(window.grab().save(dir+"/"+lang+"-fanarts.png"));tabs->setCurrentIndex(4);QVERIFY(window.grab().save(dir+"/"+lang+"-patreon.png"));tabs->setCurrentIndex(0);}
            window.close();
        }
    }
    void languageSwitchRebuilds(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","en");MainWindow window;window.show();window.showPage(MainWindow::Settings);
        auto combo=window.findChild<QComboBox*>("languageChoice");QVERIFY(combo);combo->setCurrentIndex(combo->findData("es"));emit combo->activated(combo->currentIndex());
        auto first=[&]{auto nav=window.centralWidget()->findChild<QListWidget*>("navigation");return nav?nav->item(0)->data(Qt::AccessibleTextRole).toString():QString();};
        QTRY_COMPARE(first(),QString("Inicio"));QCOMPARE(window.centralWidget()->findChild<QStackedWidget*>()->currentIndex(),int(MainWindow::Settings));
        QVERIFY(window.findChild<QPushButton*>("navSettings")->isChecked());QSettings().remove("ui/language");
    }
    void instanceWizardFilters(){
        QTemporaryDir data;McInstanceManager manager(data.path());QList<McVersion> catalog{{"1.20.1","release","","2023-06-12"},{"24w01a","snapshot","","2024-01-01"},{"b1.7.3","old_beta","","2011-07-08"}};
        for(auto language:Language::available()){Language::current=language;CreateInstanceDialog dialog(&manager,catalog,{}, {},false);dialog.show();auto tree=dialog.findChild<QTreeWidget*>("minecraftVersions");QVERIFY(tree);QCOMPARE(tree->topLevelItemCount(),1);auto sources=dialog.findChild<QListWidget*>("instanceSources");QCOMPARE(sources->count(),9);auto name=dialog.findChild<QLineEdit*>("instanceName");name->setText("Test");auto snapshot=dialog.findChild<QCheckBox*>("snapshotFilter");snapshot->setChecked(true);QCOMPARE(tree->topLevelItemCount(),2);dialog.findChild<QLineEdit*>("versionSearch")->setText("24w");QCOMPARE(tree->topLevelItemCount(),1);QCOMPARE(dialog.configuration()["mcVersion"].toString(),QString("24w01a"));QString dir=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!dir.isEmpty()){dialog.findChild<QLineEdit*>("versionSearch")->clear();QVERIFY(dialog.grab().save(dir+"/"+language+"-create.png"));}dialog.close();}
    }
    void deleteInstanceThroughUi(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","en");
        McInstanceManager setup(data.path());auto first=setup.createInstance("Keep","1.20.1","vanilla");auto second=setup.createInstance("Remove","1.20.1","vanilla");QFile world(second+"/world.txt");QVERIFY(world.open(QIODevice::WriteOnly));world.write("world data");world.close();
        MainWindow window;window.resize(1200,800);window.show();window.showPage(MainWindow::Instances);QTest::qWait(20);
        auto cards=[&]{return window.findChildren<QFrame*>("instanceCard");};QCOMPARE(cards().size(),2);
        QFrame *card=nullptr;for(auto c:cards())if(c->property("dir").toString()==second)card=c;QVERIFY(card);QTest::mouseClick(card,Qt::LeftButton,{},QPoint(30,30));QTest::qWait(20);
        auto remove=window.findChild<QPushButton*>("deleteInstance");QVERIFY(remove);QVERIFY(remove->isVisible());
        QTimer::singleShot(10,&window,[]{auto box=qobject_cast<QMessageBox*>(QApplication::activeModalWidget());QVERIFY(box);QTest::mouseClick(box->button(QMessageBox::No),Qt::LeftButton);});QTest::mouseClick(remove,Qt::LeftButton);QTest::qWait(20);QCOMPARE(cards().size(),2);QVERIFY(QFile::exists(second));
        QTimer::singleShot(10,&window,[]{auto box=qobject_cast<QMessageBox*>(QApplication::activeModalWidget());QVERIFY(box);QTest::mouseClick(box->button(QMessageBox::Yes),Qt::LeftButton);});QTest::mouseClick(remove,Qt::LeftButton);QTest::qWait(20);
        QCOMPARE(cards().size(),1);QVERIFY(window.findChild<QWidget*>("instanceGrid")->isVisible());QVERIFY(QFile::exists(first+"/instance.json"));QVERIFY(!QFile::exists(second));
        QDir trash(data.path()+"/mc/trash");auto entries=trash.entryList(QDir::Dirs|QDir::NoDotAndDotDot);QCOMPARE(entries.size(),1);QVERIFY(QFile::exists(trash.filePath(entries.first()+"/world.txt")));
    }
    void playAsksOnlyForAName(){
        // First game: no account and nothing installed. One press asks for a player name and starts the installation.
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","en");
        {McInstanceManager setup(data.path());setup.createInstance("First","0.0-test","vanilla");}
        MainWindow window;window.resize(1200,800);window.show();window.showPage(MainWindow::Instances);QTest::qWait(20);
        auto play=window.findChild<QPushButton*>("cardPlay");QVERIFY(play);QVERIFY(play->text().contains("Play"));
        bool asked=false;auto answer=new QTimer(&window);answer->setInterval(20);
        connect(answer,&QTimer::timeout,&window,[&,answer]{QInputDialog *dialog=nullptr;for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QInputDialog*>(w);d&&d->isVisible())dialog=d;if(!dialog)return;
            answer->stop();asked=!dialog->textValue().isEmpty();dialog->setTextValue("Tester");dialog->accept();});answer->start();
        QTest::mouseClick(play,Qt::LeftButton);QVERIFY(asked);
        auto accounts=window.findChild<AccountManager*>();QVERIFY(accounts);QCOMPARE(accounts->active().name,QString("Tester"));
        auto manager=window.findChild<McInstanceManager*>();QVERIFY(manager->isInstalling(manager->instances().first().dir));
        // The unknown version fails; close the error so the test does not wait on it.
        auto closer=new QTimer(&window);connect(closer,&QTimer::timeout,&window,[]{for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->isVisible()){qInfo()<<"closing"<<d->metaObject()->className()<<d->objectName()<<d->windowTitle();d->reject();}});closer->start(50);
        QTRY_VERIFY_WITH_TIMEOUT(!manager->isInstalling(manager->instances().first().dir),60000);
    }
    void copyAndGroupThroughCore(){
        QTemporaryDir data;McInstanceManager manager(data.path());auto dir=manager.createInstance("Base","1.20.1","fabric");QDir().mkpath(dir+"/saves/World");QFile f(dir+"/saves/World/level.dat");QVERIFY(f.open(QIODevice::WriteOnly));f.write("level");f.close();QDir().mkpath(dir+"/natives");
        auto copy=manager.copyInstance(dir,"Base copy");QVERIFY(copy!=dir);QCOMPARE(ModRepository::read(copy+"/instance.json")["name"].toString(),QString("Base copy"));QVERIFY(QFile::exists(copy+"/saves/World/level.dat"));QVERIFY(!QDir(copy+"/natives").exists());
        auto zip=data.path()+"/export.zip";manager.exportInstance(copy,zip);QVERIFY(QFile::exists(zip));QCOMPARE(manager.instances().size(),2);
    }
    void duplicateCreationThroughUi(){
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","en");MainWindow window;window.show();window.showPage(0);
        auto manager=window.findChild<McInstanceManager*>();QVERIFY(manager);emit manager->manifestReady(QList<McVersion>{{"1.20.1","release","https://example.invalid","2023-06-12"}});
        for(int n=0;n<2;++n){
            // The first time from the Play button of Home (no instances yet), then from the library.
            bool handled=false;window.showPage(n?MainWindow::Instances:MainWindow::Home);QTest::qWait(10);
            if(n){auto back=window.findChild<QPushButton*>("backButton");QVERIFY(back&&back->isVisible());QTest::mouseClick(back,Qt::LeftButton);QTest::qWait(10);} // creating opens the new instance; go back to the library
            QTimer::singleShot(30,&window,[&]{
                QDialog *dialog=nullptr;for(auto w:QApplication::topLevelWidgets())if(auto d=qobject_cast<QDialog*>(w);d&&d->isVisible()&&d->objectName()=="createInstanceDialog")dialog=d;QVERIFY(dialog);
                // A separate window: not modal (GNOME would glue it to the launcher) and the launcher behind it is disabled.
                QCOMPARE(dialog->windowModality(),Qt::NonModal);QVERIFY(window.centralWidget()->isEnabled());QVERIFY(dialog->isSizeGripEnabled());
                auto edits=dialog->findChildren<QLineEdit*>();QVERIFY(!edits.isEmpty());edits.first()->setText("Same instance");auto boxes=dialog->findChildren<QDialogButtonBox*>();QVERIFY(!boxes.isEmpty());handled=true;QTest::mouseClick(boxes.first()->button(QDialogButtonBox::Ok),Qt::LeftButton);});
            auto create=window.findChild<QPushButton*>(n?"newInstance":"homePlay");QVERIFY(create);QVERIFY(create->isVisible());QTest::mouseClick(create,Qt::LeftButton);QVERIFY(handled);QVERIFY(window.centralWidget()->isEnabled());
        }
        QCOMPARE(manager->instances().size(),2);QVERIFY(manager->instances()[0].dir!=manager->instances()[1].dir);
    }
    void liveCurseForgeBrowser(){
        if(!qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS"))QSKIP("Opt-in network test");
        QVERIFY2(!ModRepository::curseForgeKey().isEmpty(),"Configure a CurseForge API key before running this test");
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());qputenv("EBALIA_LANGUAGE","es");
        MainWindow window;window.show();window.showPage(MainWindow::Home);
        auto manager=window.findChild<McInstanceManager*>();QVERIFY(manager);
        CreateInstanceDialog dialog(manager,{{"1.20.1","release","","2023-06-12"}},{},{},false,&window);dialog.show();
        auto sources=dialog.findChild<QListWidget*>("instanceSources");QVERIFY(sources);
        QTest::mouseClick(sources->viewport(),Qt::LeftButton,{},sources->visualItemRect(sources->item(CreateInstanceDialog::CurseForge)).center());
        auto page=dialog.findChild<QWidget*>("curseforgePage");QVERIFY(page&&page->isVisible());
        auto key=page->findChild<QLineEdit*>("curseForgeKey");QVERIFY(key&&!key->isVisible());
        auto search=page->findChild<QLineEdit*>("packSearch");auto results=page->findChild<QListWidget*>("packResults");
        auto versions=page->findChild<QComboBox*>("packVersions");QVERIFY(search&&results&&versions);
        QTest::keyClicks(search,"SkyFactory 4");QTest::keyClick(search,Qt::Key_Return);
        QTRY_VERIFY_WITH_TIMEOUT(results->count()>0,45000);
        QTest::mouseClick(results->viewport(),Qt::LeftButton,{},results->visualItemRect(results->item(0)).center());
        QTRY_VERIFY_WITH_TIMEOUT(versions->count()>0,45000);
        const auto config=dialog.configuration();QCOMPARE(config["providerPack"].toObject()["provider"].toString(),QString("curseforge"));
        QVERIFY(!config["providerVersion"].toObject()["id"].toString().isEmpty());
        QVERIFY(dialog.findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->isEnabled());
        const auto out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(!out.isEmpty()){QDir().mkpath(out);QTest::qWait(500);QVERIFY(dialog.grab().save(out+"/curseforge-live.png"));}
        qInfo().noquote()<<"CurseForge UI:"<<results->count()<<"results,"<<versions->count()<<"versions; instance creation enabled";
        dialog.close();window.close();
    }
    void screenshots(){
        // Visual check of the main views: EBALIA_TEST_ARTIFACTS=<folder> [EBALIA_SHOT_LANGUAGE=es] [EBALIA_LIVE_TESTS=1 for provider pages].
        const QString out=qEnvironmentVariable("EBALIA_TEST_ARTIFACTS");if(out.isEmpty())QSKIP("Set EBALIA_TEST_ARTIFACTS to save screenshots");QDir().mkpath(out);
        QTemporaryDir data;qputenv("EBALIA_DATA_DIR",data.path().toUtf8());const auto language=qEnvironmentVariable("EBALIA_SHOT_LANGUAGE","es");qputenv("EBALIA_LANGUAGE",language.toUtf8());
        McInstanceManager setup(data.path());const auto now=QDateTime::currentSecsSinceEpoch();
        struct Sample{const char *name,*version,*loader,*group,*icon;bool ready;qint64 played;int mods,worlds;};
        const QList<Sample> samples{{"Survival","1.20.1","forge","Mis aventuras","creeper",true,now-7200,42,3},{"Skyblock","1.21.1","fabric","Mis aventuras","gem",true,now-864000,18,1},{"Adventure","1.21.1","fabric","Mis aventuras","book",false,0,0,0},
            {"PvP Practice","1.8.9","vanilla","Construir y jugar","pickaxe",true,now-259200,0,2},{"Creative","1.21.4","vanilla","Construir y jugar","crafting",true,now-400,0,5},{"Vanilla 1.21.4","1.21.4","vanilla","","grass",true,0,0,0}};
        QString first;
        for(const auto &sample:samples){
            auto dir=setup.createInstance(sample.name,sample.version,sample.loader);if(first.isEmpty())first=dir;auto info=ModRepository::read(dir+"/instance.json");
            info["group"]=sample.group;info["icon"]=sample.icon;info["ready"]=sample.ready;info["lastPlayed"]=sample.played;info["loaderVersion"]=QString(sample.loader)=="forge"?"47.3.0":QString(sample.loader)=="fabric"?"0.16.9":"";ModRepository::write(dir+"/instance.json",info);
            for(int n=0;n<sample.mods;++n){QFile f(dir+"/mods/mod"+QString::number(n)+".jar");QVERIFY(f.open(QIODevice::WriteOnly));}
            for(int n=0;n<sample.worlds;++n)QDir().mkpath(dir+"/saves/World "+QString::number(n));
        }
        MainWindow window;window.resize(1280,820);window.show();window.showPage(0);auto manager=window.findChild<McInstanceManager*>();
        emit manager->manifestReady(QList<McVersion>{{"1.21.4","release","","2024-12-03"},{"1.21.1","release","","2024-08-08"},{"1.20.1","release","","2023-06-12"},{"24w14a","snapshot","","2024-04-03"},{"1.8.9","release","","2015-12-09"}});
        QTest::qWait(50);QVERIFY(window.grab().save(out+"/"+language+"-home.png"));
        window.showPage(MainWindow::Instances);QTest::qWait(80);QVERIFY(window.grab().save(out+"/"+language+"-library.png"));
        QCOMPARE(window.findChildren<QFrame*>("instanceCard").size(),samples.size());
        QFrame *card=nullptr;for(auto c:window.findChildren<QFrame*>("instanceCard"))if(c->property("dir").toString()==first)card=c;QVERIFY(card);
        QTest::mouseClick(card,Qt::LeftButton,{},QPoint(20,20));QTest::qWait(80);QVERIFY(window.findChild<QWidget*>("instanceDetail")->isVisible());QVERIFY(window.grab().save(out+"/"+language+"-detail.png"));
        for(auto page:{MainWindow::Lost,MainWindow::Explore,MainWindow::Packs,MainWindow::Skins,MainWindow::News,MainWindow::Settings}){window.showPage(page);QTest::qWait(50);QVERIFY(window.grab().save(out+"/"+language+"-page"+QString::number(page)+".png"));}
        window.resize(900,600);QTest::qWait(50);window.showPage(MainWindow::Home);QTest::qWait(50);QVERIFY(window.grab().save(out+"/"+language+"-home-small.png"));window.showPage(MainWindow::Instances);QTest::qWait(50);QVERIFY(window.grab().save(out+"/"+language+"-detail-small.png"));window.resize(1280,820);
        {SetupDialog setup(&window);setup.show();QTest::qWait(1500);QVERIFY(setup.grab().save(out+"/"+language+"-setup.png"));}
        window.showPage(0);
        CreateInstanceDialog dialog(manager,{{"1.21.4","release","","2024-12-03"},{"1.21.1","release","","2024-08-08"},{"1.20.1","release","","2023-06-12"}},{}, {},false,&window);dialog.show();QTest::qWait(50);
        QVERIFY(dialog.grab().save(out+"/"+language+"-create-custom.png"));dialog.showPage(CreateInstanceDialog::Import);QTest::qWait(50);QVERIFY(dialog.grab().save(out+"/"+language+"-create-import.png"));dialog.showPage(CreateInstanceDialog::CurseForge);QTest::qWait(50);QVERIFY(dialog.grab().save(out+"/"+language+"-create-curseforge.png"));
        if(qEnvironmentVariableIsSet("EBALIA_LIVE_TESTS")){qunsetenv("EBALIA_NO_NETWORK");for(auto page:{CreateInstanceDialog::Modrinth,CreateInstanceDialog::FTB}){dialog.showPage(page);QTest::qWait(9000);auto list=dialog.findChildren<QListWidget*>("packResults");for(auto l:list)if(l->isVisible()&&l->count())l->setCurrentRow(0);QTest::qWait(4000);QVERIFY(dialog.grab().save(out+"/"+language+"-create-"+QString::number(page)+".png"));}qputenv("EBALIA_NO_NETWORK","1");}
        dialog.close();
    }
};
QTEST_MAIN(UiTests)
#include "UiTests.moc"
