#include "BedrockPage.hpp"
#include "BedrockInstaller.hpp"
#include <QtConcurrent>
#include "Language.hpp"
#include "Ui.hpp"
#include <QtWidgets>
#include <memory>

namespace Bedrock {
Platform hostPlatform() {
#if defined(Q_OS_WIN)
    return Platform::Windows;
#elif defined(Q_OS_MACOS)
    return Platform::MacOS;
#elif defined(Q_OS_LINUX)
    return Platform::Linux;
#else
    return Platform::Unsupported;
#endif
}
Command localCommand(const QString &path, Platform platform) {
    if (platform == Platform::Unsupported || !QDir::isAbsolutePath(path)) return {};
    const QFileInfo file(path);
    if (platform == Platform::MacOS && file.isDir() && path.endsWith(".app", Qt::CaseInsensitive))
        return {"/usr/bin/open", {"-a", file.absoluteFilePath()}};
    if (!file.isFile()) return {};
    if (platform == Platform::Windows) {
        if (!path.endsWith(".exe", Qt::CaseInsensitive)) return {};
    } else if (!file.isExecutable()) return {};
    return {file.absoluteFilePath(), {}};
}
}

namespace {
struct Found {QString windowsGame;bool runtime=false;QStringList versions;};
}

BedrockPage::BedrockPage(const QString &dataRoot, QWidget *parent) : QWidget(parent) {
    setObjectName("bedrockLabPage");
    auto tr = [](const char *es, const char *en) {
        return Language::text(es, en, en);
    };
    const auto platform = Bedrock::hostPlatform();
    const bool windows = platform == Bedrock::Platform::Windows, onLinux = platform == Bedrock::Platform::Linux;
    const QString root = dataRoot + "/bedrock", flatpak = QStandardPaths::findExecutable("flatpak");
    auto settings = new QSettings(dataRoot + "/bedrock-lab.ini", QSettings::IniFormat, this);
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(28,24,28,24);layout->setSpacing(16);
    auto title = new QLabel("Bedrock · Beta");title->setObjectName("pageTitle");layout->addWidget(title);
    auto paragraph = [layout](const QString &text) {
        auto label = new QLabel(text);label->setWordWrap(true);label->setTextFormat(Qt::PlainText);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);layout->addWidget(label);return label;
    };
    paragraph(windows
        ? tr("EBALIA abre Minecraft para Windows (Bedrock) directamente, sin otro launcher. Si todavía no lo tenés, conseguilo en Microsoft Store o con Game Pass.",
             "EBALIA opens Minecraft for Windows (Bedrock) directly, with no other launcher. If you do not have it yet, get it from the Microsoft Store or with Game Pass.")
        : onLinux
        ? tr("EBALIA instala el motor de Minecraft Linux y abre Bedrock directamente desde acá. El juego es la edición Android: importá el APK de tu compra de Google Play, o descargalo una sola vez con tu cuenta de Google.",
             "EBALIA installs the Minecraft Linux runtime and starts Bedrock directly from here. The game is the Android edition: import the APK you bought on Google Play, or download it once with your Google account.")
        : tr("Integración experimental. Abre un gestor externo de Bedrock; instala el gestor y abre su asistente para obtener el juego.",
             "Experimental integration. Opens an external Bedrock manager; it installs the manager and opens its setup to obtain the game."));
    auto box = new QFrame;box->setObjectName("bedrockProviderCard");
    box->setStyleSheet("#bedrockProviderCard{background:#202b26;border:1px solid #416748;border-radius:12px;}");layout->addWidget(box);
    auto card = new QVBoxLayout(box);card->setContentsMargins(20,20,20,20);card->setSpacing(12);
    auto name = new QLabel(windows ? tr("Minecraft para Windows","Minecraft for Windows") : onLinux ? QString("Minecraft Bedrock · Android") : QString("Minecraft Linux · mcpelauncher"));
    name->setStyleSheet("font-size:20px;font-weight:700;");card->addWidget(name);
    auto status = new QLabel;status->setObjectName("bedrockProviderStatus");status->setWordWrap(true);
    status->setTextFormat(Qt::PlainText);card->addWidget(status);
    auto version = new QComboBox;version->setObjectName("bedrockVersion");version->hide();card->addWidget(version);
    auto launch = new QPushButton(tr("▶  Jugar Bedrock","▶  Play Bedrock"));
    launch->setObjectName("bedrockPlay");launch->setProperty("primary",true);launch->setProperty("play",true);launch->setMinimumHeight(48);launch->setEnabled(false);card->addWidget(launch);
    // Two short rows, so the page fits narrow windows in every language.
    auto actions = new QHBoxLayout;card->addLayout(actions);
    auto install = new QPushButton(windows ? tr("Conseguir Minecraft para Windows ↗","Get Minecraft for Windows ↗") : tr("Instalar Bedrock","Install Bedrock"));
    install->setObjectName("bedrockInstall");actions->addWidget(install);
    auto importApk = new QPushButton(tr("Importar APK…","Import APK…"));importApk->setObjectName("bedrockImportApk");actions->addWidget(importApk);
    auto googlePlay = new QPushButton(tr("Descargar con Google Play","Download with Google Play"));googlePlay->setObjectName("bedrockGooglePlay");actions->addWidget(googlePlay);
    actions->addStretch();
    auto more = new QHBoxLayout;card->addLayout(more);
    auto detect = new QPushButton(tr("Comprobar de nuevo","Check again"));detect->setObjectName("bedrockDetect");more->addWidget(detect);
    more->addStretch();
    auto uninstall = new QPushButton(tr("Desinstalar","Uninstall"));uninstall->setObjectName("bedrockUninstall");more->addWidget(uninstall);
    for(auto b:{importApk,googlePlay,uninstall})b->hide();

    // Another manager (BedrockLauncher, a custom mcpelauncher build…), for those who prefer one.
    auto advancedToggle = new QPushButton(tr("Usar otro gestor ▸","Use another manager ▸"));advancedToggle->setObjectName("bedrockAdvanced");advancedToggle->setFlat(true);
    card->addWidget(advancedToggle,0,Qt::AlignLeft);
    auto advanced = new QWidget;auto advancedLayout = new QHBoxLayout(advanced);advancedLayout->setContentsMargins(0,0,0,0);card->addWidget(advanced);
    auto path = new QLineEdit(settings->value("provider/path").toString());
    path->setObjectName("bedrockProviderPath");
    path->setPlaceholderText(tr("Ruta del gestor instalado (opcional)", "Installed manager path (optional)"));
    auto browse = new QPushButton(tr("Elegir gestor…","Choose manager…"));
    advancedLayout->addWidget(path,1);advancedLayout->addWidget(browse);
    advanced->setVisible(!path->text().isEmpty());
    connect(advancedToggle,&QPushButton::clicked,advanced,[advanced]{advanced->setVisible(!advanced->isVisible());});

    paragraph(windows
        ? tr("Necesitás Minecraft para Windows o Game Pass en tu cuenta Microsoft. Tus mundos los guarda el juego.",
             "You need Minecraft for Windows or Game Pass on your Microsoft account. The game keeps your worlds.")
        : tr("Linux/macOS: necesitás Minecraft comprado en Google Play. La licencia de Windows no sustituye la de Android.",
             "Linux/macOS: Google Play ownership is required. A Windows license does not replace Android ownership."));
    auto docs = new QPushButton(tr("Instalación y requisitos ↗","Installation and requirements ↗"));layout->addWidget(docs,0,Qt::AlignLeft);
    connect(docs,&QPushButton::clicked,this,[windows] {
        QDesktopServices::openUrl(QUrl(windows ? "https://www.minecraft.net/download" : "https://minecraft-onLinux.github.io/"));
    });
    auto trial = new QPushButton(tr("Prueba gratuita oficial ↗","Official free trial ↗"));layout->addWidget(trial,0,Qt::AlignLeft);
    connect(trial,&QPushButton::clicked,this,[] {
        QDesktopServices::openUrl(QUrl("https://www.minecraft.net/free-trial"));
    });
    paragraph(tr("Tus instancias Java permanecen separadas de Bedrock.","Your Java instances remain separate from Bedrock."));
    layout->addStretch();

    // What the Play button starts.
    auto play = std::make_shared<std::function<bool()>>();
    auto busy = std::make_shared<bool>(false);
    auto task = std::make_shared<QPointer<QWidget>>();
    auto setBusy = [=,this](const QString &text) {
        *busy=!text.isEmpty();
        for(auto b:{launch,install,importApk,googlePlay,detect,uninstall,browse})b->setEnabled(!*busy);
        path->setEnabled(!*busy);version->setEnabled(!*busy);
        if(*task){if(auto main=qobject_cast<QMainWindow*>(window()))main->statusBar()->removeWidget(*task);(*task)->deleteLater();*task=nullptr;}
        if(text.isEmpty())return;
        status->setText(text);
        if(auto main=qobject_cast<QMainWindow*>(window())) {
            auto widget=new QWidget(main);widget->setMaximumWidth(300);widget->setObjectName("bedrockInstallProgress");
            auto taskLayout=new QVBoxLayout(widget);taskLayout->setContentsMargins(8,4,8,4);
            taskLayout->addWidget(new QLabel(text.section('\n',0,0).left(60)));
            auto bar=new QProgressBar;bar->setRange(0,0);bar->setFixedHeight(8);bar->setTextVisible(false);taskLayout->addWidget(bar);
            main->statusBar()->addPermanentWidget(widget);*task=widget;
        }
    };
    auto show = [=](const Found &found) {
        setBusy({});*play=nullptr;version->hide();importApk->hide();googlePlay->hide();uninstall->hide();install->show();
        const bool oldManagers=!QDir(root).entryList({"provider-*"},QDir::Dirs|QDir::NoDotAndDotDot).isEmpty();
        const auto configured=path->text().trimmed();
        if(!configured.isEmpty()) {
            auto local=Bedrock::localCommand(configured,platform);
            if(local.valid())*play=[local]{QProcess p;p.setProgram(local.program);p.setArguments(local.arguments);p.setWorkingDirectory(QFileInfo(local.program).absolutePath());return p.startDetached();};
            status->setText(local.valid()?tr("Gestor seleccionado. Compatibilidad del juego sin verificar.","Manager selected. Game compatibility unverified.")
                                         :tr("Ruta inválida: elegí un ejecutable compatible con tu sistema.","Invalid path: choose an executable compatible with your OS."));
            install->hide();
        } else if(windows) {
            const bool game=!found.windowsGame.isEmpty();
            install->setText(tr("Conseguir Minecraft para Windows ↗","Get Minecraft for Windows ↗"));install->setVisible(!game);uninstall->setVisible(game||oldManagers);
            if(game)*play=[]{return QDesktopServices::openUrl(QUrl("minecraft:"));};
            status->setText(game
                ? (found.windowsGame=="-"?tr("Minecraft para Windows está instalado.","Minecraft for Windows is installed.")
                                         :tr("Minecraft para Windows %1 está instalado.","Minecraft for Windows %1 is installed.").arg(found.windowsGame))
                : tr("Minecraft para Windows no está instalado. Conseguilo en Microsoft Store (o con Game Pass) y volvé: EBALIA lo detecta solo.",
                     "Minecraft for Windows is not installed. Get it from the Microsoft Store (or with Game Pass) and come back: EBALIA detects it by itself."));
        } else if(onLinux) {
            install->setText(tr("Instalar Bedrock","Install Bedrock"));install->setVisible(!found.runtime);
            importApk->setVisible(found.runtime);googlePlay->setVisible(found.runtime);uninstall->setVisible(found.runtime||oldManagers);
            if(flatpak.isEmpty())
                status->setText(tr("Falta Flatpak, que EBALIA usa para instalar el motor de Bedrock. En NixOS habilitá services.flatpak.enable = true y reconstruí el sistema.",
                                   "Flatpak is missing; EBALIA uses it to install the Bedrock runtime. On NixOS enable services.flatpak.enable = true and rebuild the system."));
            else if(!found.runtime)
                status->setText(tr("Pulsá Instalar Bedrock: EBALIA descarga el motor (unos cientos de MB) y queda listo para importar el juego.",
                                   "Press Install Bedrock: EBALIA downloads the runtime (a few hundred MB) and is then ready to import the game."));
            else if(found.versions.isEmpty())
                status->setText(tr("Falta el juego. Importá el APK de Minecraft que compraste en Google Play, o descargalo una vez con tu cuenta de Google y volvé.",
                                   "The game is missing. Import the Minecraft APK you bought on Google Play, or download it once with your Google account and come back."));
            else {
                const auto chosen=settings->value("game/version").toString();
                version->clear();version->addItems(found.versions);version->setVisible(found.versions.size()>1);
                if(found.versions.contains(chosen))version->setCurrentText(chosen);
                status->setText(tr("Listo para jugar.","Ready to play."));
                *play=[=]{
                    settings->setValue("game/version",version->currentText());
                    return QProcess::startDetached(flatpak,{"run","--command=mcpelauncher-client","io.mrarm.mcpelauncher","-dg",BedrockInstaller::gameData()+"/versions/"+version->currentText()});
                };
            }
        } else {
            install->setText(tr("Instalar gestor Bedrock","Install Bedrock manager"));uninstall->setVisible(oldManagers);
            status->setText(tr("No se encontró un gestor. Instalalo siguiendo los requisitos o elegí su ejecutable.","No manager found. Follow installation requirements or choose its executable."));
        }
        launch->setEnabled(bool(*play));
    };
    auto finder = new QFutureWatcher<Found>(this);
    connect(finder,&QFutureWatcher<Found>::finished,this,[=]{show(finder->result());});
    m_refresh = [=] {
        if(*busy||finder->isRunning())return;
        settings->setValue("provider/path",path->text().trimmed());
        launch->setEnabled(false);detect->setEnabled(false);status->setText(tr("Buscando Bedrock…","Looking for Bedrock…"));
        finder->setFuture(QtConcurrent::run([windows,onLinux] {
            Found found;
            if(windows)found.windowsGame=BedrockInstaller::windowsGame();
            if(onLinux&&(found.runtime=BedrockInstaller::runtimeInstalled()))found.versions=BedrockInstaller::versions();
            return found;
        }));
    };
    // Long jobs (install, import, uninstall) run on a worker; they return an error ("ERROR: …") or a message.
    auto job = [=,this](const QString &text,std::function<QString()> work,std::function<void(const QString &)> done={}) {
        setBusy(text);
        auto watcher=new QFutureWatcher<QString>(this);
        connect(watcher,&QFutureWatcher<QString>::finished,this,[=,this]{
            const auto result=watcher->result();watcher->deleteLater();setBusy({});
            if(result.startsWith("ERROR: "))QMessageBox::warning(this,"Bedrock",Language::message(result.mid(7)));
            else if(done)done(result);
            refresh();
        });
        watcher->setFuture(QtConcurrent::run([work]{try{return work();}catch(const std::exception &e){return QString("ERROR: ")+QString::fromUtf8(e.what());}}));
    };
    connect(install,&QPushButton::clicked,this,[=,this] {
        if(windows){
            // The Store's own page of Minecraft for Windows; the web page if the Store app is missing.
            if(!QDesktopServices::openUrl(QUrl("ms-windows-store://pdp/?ProductId=9NBLGGH2JHXJ")))
                QDesktopServices::openUrl(QUrl("https://www.xbox.com/games/store/minecraft-for-windows/9NBLGGH2JHXJ"));
            return;
        }
        job(tr("Instalando Bedrock… Puede tardar unos minutos.","Installing Bedrock… This can take a few minutes."),[root]{
            auto result=BedrockInstaller::install(root);
            return result=="flatpak"?QString():result;
        },[=](const QString &app){if(!app.isEmpty()){path->setText(app);settings->setValue("provider/path",app);}});
    });
    connect(importApk,&QPushButton::clicked,this,[=,this] {
        const auto files=QFileDialog::getOpenFileNames(this,tr("Elegir el APK de Minecraft","Choose the Minecraft APK"),QDir::homePath(),
            tr("Minecraft para Android","Minecraft for Android")+" (*.apk *.apks *.xapk *.zip)");
        if(files.isEmpty())return;
        job(tr("Importando Minecraft…","Importing Minecraft…"),[files]{return BedrockInstaller::importGame(files);},
            [=](const QString &name){settings->setValue("game/version",name);});
    });
    connect(googlePlay,&QPushButton::clicked,this,[=] {
        // Only Minecraft Linux's own window can sign in to Google Play; after downloading, EBALIA plays the game itself.
        const bool started=QProcess::startDetached(flatpak,{"run","io.mrarm.mcpelauncher"});
        status->setText(started
            ? tr("Se abrió Minecraft Linux: iniciá sesión con Google, descargá la versión y cerrá esa ventana. Después pulsá Comprobar de nuevo.",
                 "Minecraft Linux opened: sign in with Google, download the version and close that window. Then press Check again.")
            : tr("No se pudo abrir el gestor. Comprobá la instalación y sus dependencias.","Could not open the manager. Check its installation and dependencies."));
    });
    connect(uninstall,&QPushButton::clicked,this,[=,this] {
        QMessageBox ask(QMessageBox::Question,"Bedrock",windows
            ? tr("¿Desinstalar Minecraft para Windows? Antes, EBALIA copia tus mundos a su carpeta de datos.",
                 "Uninstall Minecraft for Windows? EBALIA copies your worlds to its data folder first.")
            : tr("¿Desinstalar Bedrock? Se borran el motor y las versiones descargadas del juego.",
                 "Uninstall Bedrock? The runtime and the downloaded game versions are removed."),QMessageBox::Yes|QMessageBox::Cancel,this);
        QCheckBox worlds(tr("Borrar también mis mundos y ajustes de Bedrock","Also delete my Bedrock worlds and settings"));
        if(onLinux)ask.setCheckBox(&worlds);
        ask.button(QMessageBox::Yes)->setText(tr("Desinstalar","Uninstall"));ask.setDefaultButton(QMessageBox::Cancel);
        if(Ui::openWindow(ask)!=QMessageBox::Yes)return;
        const bool deleteWorlds=worlds.isChecked();
        if(path->text().trimmed().startsWith(root)){path->clear();settings->remove("provider/path");}
        job(tr("Desinstalando Bedrock…","Uninstalling Bedrock…"),[root,deleteWorlds]{return BedrockInstaller::uninstall(root,deleteWorlds);},[=,this](const QString &backup){
            QMessageBox::information(this,"Bedrock",backup.isEmpty()?tr("Bedrock se desinstaló.","Bedrock was uninstalled.")
                :tr("Bedrock se desinstaló. Tus mundos quedaron en %1","Bedrock was uninstalled. Your worlds are in %1").arg(QDir::toNativeSeparators(backup)));
        });
    });
    connect(detect,&QPushButton::clicked,this,[this]{refresh();});
    connect(path,&QLineEdit::textEdited,this,[=]{launch->setEnabled(false);*play=nullptr;});
    connect(path,&QLineEdit::editingFinished,this,[this]{refresh();});
    connect(browse,&QPushButton::clicked,this,[=, this] {
        QString selected;
        if(platform==Bedrock::Platform::MacOS)
            selected=QFileDialog::getExistingDirectory(this,tr("Elegir aplicación .app","Choose .app bundle"));
        else selected=QFileDialog::getOpenFileName(this,tr("Elegir gestor Bedrock","Choose Bedrock manager"),{},windows?tr("Programas","Programs")+" (*.exe)":"*");
        if(!selected.isEmpty()){path->setText(selected);refresh();}
    });
    connect(launch,&QPushButton::clicked,this,[=, this] {
        if(!*play) return;
        // BedrockLauncher stops at start without the Visual C++ runtime: install it first (Windows asks for permission).
        if(windows&&path->text().endsWith("BedrockLauncher.exe",Qt::CaseInsensitive)&&!BedrockInstaller::vcRuntimeReady()) {
            job(tr("Instalando Microsoft Visual C++ (lo necesita el gestor). Windows va a pedir permiso.","Installing Microsoft Visual C++ (the manager needs it). Windows will ask for permission."),
                [root]{BedrockInstaller::installVcRuntime(root);return QString();});
            return;
        }
        status->setText((*play)()
            ? tr("Bedrock se está abriendo…","Bedrock is starting…")
            : tr("No se pudo abrir el juego. Pulsá Comprobar de nuevo o reinstalalo.","Could not open the game. Press Check again or reinstall it."));
    });
}

// The fields emit signals while they are destroyed (the path field loses focus): none may reach the page then.
BedrockPage::~BedrockPage() {for(auto child:findChildren<QObject*>())child->disconnect(this);}

void BedrockPage::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    refresh();
}
