#include "BedrockPage.hpp"
#include "BedrockInstaller.hpp"
#include "GpuInfo.hpp"
#include <QtConcurrent>
#include "Language.hpp"
#include "Ui.hpp"
#include <QtWidgets>
#include <memory>
#ifdef Q_OS_UNIX
#include <signal.h>
#endif

namespace {
struct Found {
    QString runtime,unsupported;QList<BedrockInstaller::Version> versions;
    BedrockInstaller::WindowsGames windows;bool virtualOnly=false;
};
bool alive(qint64 pid) {
#ifdef Q_OS_UNIX
    return pid>0&&::kill(pid_t(pid),0)==0;
#else
    Q_UNUSED(pid);return false;
#endif
}
}

BedrockPage::BedrockPage(const QString &dataRoot, QWidget *parent) : QWidget(parent) {
    setObjectName("bedrockLabPage");
    auto tr = [](const char *es, const char *en) {
        return Language::text(es, en, en);
    };
#if defined(Q_OS_WIN)
    const bool windows=true,mac=false;
#elif defined(Q_OS_MACOS)
    const bool windows=false,mac=true;
#else
    const bool windows=false,mac=false;
#endif
    const QString root = dataRoot + "/bedrock";
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
        : tr("EBALIA instala Minecraft Linux, que ejecuta la edición Android de Bedrock, lo mantiene actualizado y abre la versión que elijas. Las versiones salen de tu compra de Google Play: iniciás sesión una vez en la ventana de Minecraft Linux y las descargás ahí.",
             "EBALIA installs Minecraft Linux, which runs the Android edition of Bedrock, keeps it updated and opens the version you choose. Versions come from your Google Play purchase: you sign in once in the Minecraft Linux window and download them there."));
    auto box = new QFrame;box->setObjectName("bedrockProviderCard");
    box->setStyleSheet("#bedrockProviderCard{background:#202b26;border:1px solid #416748;border-radius:12px;}");layout->addWidget(box);
    auto card = new QVBoxLayout(box);card->setContentsMargins(20,20,20,20);card->setSpacing(12);
    auto name = new QLabel(windows ? tr("Minecraft para Windows","Minecraft for Windows") : QString("Minecraft Bedrock · Minecraft Linux"));
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
    auto manager = new QPushButton(windows ? tr("Game Pass (app Xbox) ↗","Game Pass (Xbox app) ↗") : tr("Descargar versiones…","Download versions…"));
    manager->setObjectName("bedrockManager");actions->addWidget(manager);
    auto update = new QPushButton;update->setObjectName("bedrockUpdate");update->setProperty("primary",true);actions->addWidget(update);
    actions->addStretch();
    auto more = new QHBoxLayout;card->addLayout(more);
    auto detect = new QPushButton(tr("Comprobar de nuevo","Check again"));detect->setObjectName("bedrockDetect");more->addWidget(detect);
    more->addStretch();
    auto uninstall = new QPushButton(tr("Desinstalar","Uninstall"));uninstall->setObjectName("bedrockUninstall");more->addWidget(uninstall);
    for(auto b:{manager,update,uninstall})b->hide();

    paragraph(windows
        ? tr("Necesitás Minecraft para Windows o Game Pass en tu cuenta Microsoft. Tus mundos los guarda el juego.",
             "You need Minecraft for Windows or Game Pass on your Microsoft account. The game keeps your worlds.")
        : tr("Necesitás Minecraft comprado en Google Play (la licencia de Windows no sirve para Android). Al abrir versiones nuevas, Minecraft Linux puede pedirte que apruebes tus credenciales de Google en una ventana suya: aceptá ahí.",
             "You need Minecraft bought on Google Play (a Windows licence does not work for Android). When newer versions start, Minecraft Linux may ask you to approve your Google credentials in a window of its own: accept there."));
    auto xbox = new QPushButton(tr("Iniciar sesión en Xbox","Signing in to Xbox"));xbox->setObjectName("bedrockXboxHelp");layout->addWidget(xbox,0,Qt::AlignLeft);
    auto trial = new QPushButton(tr("Prueba gratuita oficial ↗","Official free trial ↗"));layout->addWidget(trial,0,Qt::AlignLeft);
    auto docs = new QPushButton(tr("Instalación y requisitos ↗","Installation and requirements ↗"));layout->addWidget(docs,0,Qt::AlignLeft);
    connect(docs,&QPushButton::clicked,this,[windows] {
        QDesktopServices::openUrl(QUrl(windows ? "https://www.minecraft.net/download" : "https://minecraft-linux.github.io/"));
    });
    connect(trial,&QPushButton::clicked,this,[] {QDesktopServices::openUrl(QUrl("https://www.minecraft.net/free-trial"));});
    connect(xbox,&QPushButton::clicked,this,[=,this] {
        QMessageBox box(QMessageBox::Information,"Xbox",windows
            ? tr("Un jugador y LAN funcionan sin iniciar sesión. Para servidores, Realms, amigos y Marketplace, Bedrock usa la cuenta de la app Xbox de Windows: iniciá sesión ahí con la misma cuenta Microsoft que usás en EBALIA.",
                 "Single-player and LAN work without signing in. For servers, Realms, friends and the Marketplace, Bedrock uses the account of the Windows Xbox app: sign in there with the same Microsoft account you use in EBALIA.")
            : tr("Un jugador y LAN funcionan sin iniciar sesión. Para servidores, Realms y amigos, pulsá Iniciar sesión dentro del juego. Si la ventana de Microsoft falla, activá en el juego Ajustes > Cuenta > «Usar conexión remota», reiniciá, pulsá Iniciar sesión y escribí el código en aka.ms/remoteconnect.",
                 "Single-player and LAN work without signing in. For servers, Realms and friends, press Sign In inside the game. If the Microsoft window fails, turn on Settings > Account > \"Use Remote Connect\" in the game, restart it, press Sign In and enter the code at aka.ms/remoteconnect."),
            QMessageBox::Close,this);
        auto open=windows?nullptr:box.addButton("aka.ms/remoteconnect ↗",QMessageBox::ActionRole);
        Ui::openWindow(box);
        if(open&&box.clickedButton()==open)QDesktopServices::openUrl(QUrl("https://aka.ms/remoteconnect"));
    });
    paragraph(tr("Tus instancias Java permanecen separadas de Bedrock.","Your Java instances remain separate from Bedrock."));
    layout->addStretch();

    auto busy = std::make_shared<bool>(false);
    auto task = std::make_shared<QPointer<QWidget>>();
    auto found = std::make_shared<Found>();
    auto game = std::make_shared<qint64>(0);
    auto setBusy = [=,this](const QString &text) {
        *busy=!text.isEmpty();
        for(auto b:{install,manager,update,detect,uninstall})b->setEnabled(!*busy);
        launch->setEnabled(!*busy&&version->count()>0&&!*game);version->setEnabled(!*busy);
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
    // Shows what was found: what can be played and the buttons that make sense on this computer.
    auto show = [=](const Found &f) {
        *found=f;setBusy({});version->clear();update->hide();
        const auto chosen=settings->value("game/version").toString();
        if(windows) {
            const auto &w=f.windows;
            if(!w.release.isEmpty())version->addItem(QString("Minecraft  ·  %1").arg(w.release),"release");
            if(!w.preview.isEmpty())version->addItem(QString("Minecraft Preview  ·  %1").arg(w.preview),"preview");
            const bool installed=version->count()>0;
            install->setVisible(!installed||!w.gamingServices);
            install->setText(installed?tr("Instalar Gaming Services ↗","Install Gaming Services ↗"):tr("Conseguir Minecraft para Windows ↗","Get Minecraft for Windows ↗"));
            manager->setVisible(!installed);uninstall->setVisible(installed);
            QString text=!installed
                ? tr("Minecraft para Windows no está instalado. Conseguilo en Microsoft Store (o con Game Pass) y volvé: EBALIA lo detecta solo.",
                     "Minecraft for Windows is not installed. Get it from the Microsoft Store (or with Game Pass) and come back: EBALIA detects it by itself.")
                : !w.gamingServices
                ? tr("Falta Gaming Services de Microsoft, que Minecraft necesita para abrir. Instalalo desde la Store.","Microsoft Gaming Services, which Minecraft needs to start, is missing. Install it from the Store.")
                : tr("Listo para jugar. Microsoft Store actualiza el juego sola.","Ready to play. The Microsoft Store keeps the game updated.");
            if(installed&&f.virtualOnly)text+="\n"+tr("Esta computadora no tiene una tarjeta gráfica real (máquina virtual): Bedrock usa DirectX y puede no abrir o ir muy lento. Activá la aceleración 3D de la máquina virtual.",
                "This computer has no real graphics card (virtual machine): Bedrock uses DirectX and may not start or run very slowly. Turn on the virtual machine's 3D acceleration.");
            status->setText(text);
        } else {
            for(const auto &v:f.versions)version->addItem(v.name==v.dir?v.name:QString("%1  ·  %2").arg(v.name,v.dir),v.dir);
            const bool ready=!f.runtime.isEmpty();
            install->setVisible(!ready&&f.unsupported.isEmpty());install->setText(tr("Instalar Bedrock","Install Bedrock"));
            manager->setVisible(ready);manager->setText(f.versions.isEmpty()?tr("Iniciar sesión y descargar versiones…","Sign in and download versions…"):tr("Descargar versiones…","Download versions…"));
            uninstall->setVisible(ready);
            if(f.unsupported=="intel")status->setText(tr("Las versiones actuales de Bedrock en Mac necesitan un Mac con chip Apple (M1 o más nuevo).","Current Bedrock versions on Mac need a Mac with an Apple chip (M1 or newer)."));
            else if(f.unsupported=="macos")status->setText(tr("Minecraft Linux necesita macOS 15 o más nuevo. Actualizá macOS y volvé.","Minecraft Linux needs macOS 15 or newer. Update macOS and come back."));
            else if(!mac&&QStandardPaths::findExecutable("flatpak").isEmpty())
                status->setText(tr("Falta Flatpak, que EBALIA usa para instalar el motor de Bedrock. En NixOS habilitá services.flatpak.enable = true y reconstruí el sistema.",
                                   "Flatpak is missing; EBALIA uses it to install the Bedrock runtime. On NixOS enable services.flatpak.enable = true and rebuild the system."));
            else if(!ready)status->setText(tr("Pulsá Instalar Bedrock: EBALIA descarga Minecraft Linux y lo deja listo.","Press Install Bedrock: EBALIA downloads Minecraft Linux and sets it up."));
            else if(f.versions.isEmpty())status->setText(tr("Falta el juego. Pulsá «Iniciar sesión y descargar versiones», entrá con la cuenta de Google que compró Minecraft, descargá una versión y cerrá esa ventana: EBALIA la detecta al volver.",
                                                            "The game is missing. Press \"Sign in and download versions\", sign in with the Google account that bought Minecraft, download a version and close that window: EBALIA finds it when you come back."));
            else status->setText(tr("Listo para jugar. Elegí la versión.","Ready to play. Choose the version."));
        }
        const int at=version->findData(chosen);if(at>=0)version->setCurrentIndex(at);
        version->setVisible(version->count()>1);
        launch->setEnabled(version->count()>0&&!*game);
    };
    auto finder = new QFutureWatcher<Found>(this);
    auto updater = new QFutureWatcher<QString>(this);
    connect(finder,&QFutureWatcher<Found>::finished,this,[=]{
        const auto f=finder->result();show(f);
        // A newer Minecraft Linux: checked once per page, in the background.
        if(!windows&&!f.runtime.isEmpty()&&!updater->isRunning()&&!updater->property("checked").toBool()){updater->setProperty("checked",true);updater->setFuture(QtConcurrent::run([root]{try{return BedrockInstaller::updateAvailable(root);}catch(...){return QString();}}));}
    });
    connect(updater,&QFutureWatcher<QString>::finished,this,[=]{
        const auto available=updater->result();if(available.isEmpty()||*busy)return;
        update->setText(tr("Actualizar Minecraft Linux (%1)","Update Minecraft Linux (%1)").arg(available));update->show();
    });
    m_refresh = [=] {
        if(*busy||finder->isRunning())return;
        launch->setEnabled(false);detect->setEnabled(false);status->setText(tr("Buscando Bedrock…","Looking for Bedrock…"));
        finder->setFuture(QtConcurrent::run([windows] {
            Found f;
            if(windows) {
                f.windows=BedrockInstaller::windowsGames();
                const auto gpus=GpuInfo::detect();
                f.virtualOnly=!gpus.isEmpty()&&std::all_of(gpus.begin(),gpus.end(),[](const GpuInfo::Gpu &g){return g.virtualAdapter;});
            } else {
                f.unsupported=BedrockInstaller::unsupportedReason();
                if(f.unsupported.isEmpty()&&!(f.runtime=BedrockInstaller::runtime()).isEmpty())f.versions=BedrockInstaller::versions();
            }
            return f;
        }));
    };
    // Long jobs (install, update, uninstall) run on a worker; they return an error ("ERROR: …") or a message.
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
    connect(install,&QPushButton::clicked,this,[=] {
        if(windows){
            // The Store pages of Minecraft for Windows and of Gaming Services; the web page if the Store app is missing.
            const bool services=!found->windows.release.isEmpty()||!found->windows.preview.isEmpty();
            if(!QDesktopServices::openUrl(QUrl(services?"ms-windows-store://pdp/?ProductId=9MWPM2CQNLHN":"ms-windows-store://pdp/?ProductId=9NBLGGH2JHXJ")))
                QDesktopServices::openUrl(QUrl("https://www.xbox.com/games/store/minecraft-for-windows/9NBLGGH2JHXJ"));
            return;
        }
        job(tr("Instalando Bedrock… Puede tardar unos minutos.","Installing Bedrock… This can take a few minutes."),[root]{BedrockInstaller::install(root);return QString();},
            [=](const QString &){if(BedrockInstaller::openManager())status->setText(tr("Se abrió Minecraft Linux: iniciá sesión con Google, descargá una versión y cerrá esa ventana. Después pulsá Comprobar de nuevo.",
                "Minecraft Linux opened: sign in with Google, download the version and close that window. Then press Check again."));});
    });
    connect(manager,&QPushButton::clicked,this,[=] {
        if(windows){QDesktopServices::openUrl(QUrl("msxbox://game/?productId=9NBLGGH2JHXJ"));return;}
        // Only Minecraft Linux's own window signs in to Google Play and checks the purchase.
        status->setText(BedrockInstaller::openManager()
            ? tr("Se abrió Minecraft Linux: iniciá sesión con Google, descargá una versión y cerrá esa ventana. Después pulsá Comprobar de nuevo.",
                 "Minecraft Linux opened: sign in with Google, download the version and close that window. Then press Check again.")
            : tr("No se pudo abrir el gestor. Comprobá la instalación y sus dependencias.","Could not open the manager. Check its installation and dependencies."));
    });
    connect(update,&QPushButton::clicked,this,[=] {
        update->hide();
        job(tr("Actualizando Minecraft Linux…","Updating Minecraft Linux…"),[root]{BedrockInstaller::update(root);return QString();},
            [=](const QString &){updater->setProperty("checked",false);});
    });
    connect(uninstall,&QPushButton::clicked,this,[=,this] {
        const auto edition=version->currentData().toString();
        QMessageBox ask(QMessageBox::Question,"Bedrock",windows
            ? tr("¿Desinstalar Minecraft para Windows? Antes, EBALIA copia tus mundos a su carpeta de datos.",
                 "Uninstall Minecraft for Windows? EBALIA copies your worlds to its data folder first.")
            : tr("¿Desinstalar Bedrock? Se borran el motor y las versiones descargadas del juego.",
                 "Uninstall Bedrock? The runtime and the downloaded game versions are removed."),QMessageBox::Yes|QMessageBox::Cancel,this);
        QCheckBox worlds(tr("Borrar también mis mundos y ajustes de Bedrock","Also delete my Bedrock worlds and settings"));
        if(!windows)ask.setCheckBox(&worlds);
        ask.button(QMessageBox::Yes)->setText(tr("Desinstalar","Uninstall"));ask.setDefaultButton(QMessageBox::Cancel);
        if(Ui::openWindow(ask)!=QMessageBox::Yes)return;
        const bool deleteWorlds=worlds.isChecked();
        job(tr("Desinstalando Bedrock…","Uninstalling Bedrock…"),[root,edition,deleteWorlds]{return BedrockInstaller::uninstall(root,edition,deleteWorlds);},[=,this](const QString &backup){
            QMessageBox::information(this,"Bedrock",backup.isEmpty()?tr("Bedrock se desinstaló.","Bedrock was uninstalled.")
                :tr("Bedrock se desinstaló. Tus mundos quedaron en %1","Bedrock was uninstalled. Your worlds are in %1").arg(QDir::toNativeSeparators(backup)));
        });
    });
    connect(detect,&QPushButton::clicked,this,[this]{refresh();});
    connect(version,&QComboBox::activated,this,[=]{settings->setValue("game/version",version->currentData());});
    // While the game is open: the button waits, and when it closes the end of its log says what went wrong, if anything.
    auto watch = new QTimer(this);watch->setInterval(2000);
    const QString log=root+"/last-game.log";
    connect(watch,&QTimer::timeout,this,[=]{
        if(alive(*game))return;
        watch->stop();*game=0;launch->setEnabled(version->count()>0);
        QFile file(log);QString tail;if(file.open(QIODevice::ReadOnly)){if(file.size()>65536)file.seek(file.size()-65536);tail=QString::fromUtf8(file.readAll());}
        const auto problem=BedrockInstaller::problem(tail);
        status->setText(problem=="licence"
            ? tr("Minecraft Linux no confirmó tu compra. Pulsá «Descargar versiones…», iniciá sesión con la cuenta de Google que compró Minecraft y volvé a jugar.",
                 "Minecraft Linux did not confirm your purchase. Press \"Download versions…\", sign in with the Google account that bought Minecraft and play again.")
            : problem=="incompatible"
            ? tr("Esta versión no funciona con tu Minecraft Linux. Actualizalo, o elegí otra versión. Si Minecraft Linux ofrece «DRM Update», aceptalo en su ventana.",
                 "This version does not work with your Minecraft Linux. Update it or choose another version. If Minecraft Linux offers \"DRM Update\", accept it in its window.")
            : problem=="dependencies"
            ? tr("A Minecraft Linux le faltan componentes. Pulsá Comprobar de nuevo o reinstalalo.","Minecraft Linux is missing components. Press Check again or reinstall it.")
            : tr("Bedrock se cerró. El registro está en %1","Bedrock closed. The log is in %1").arg(QDir::toNativeSeparators(log)));
    });
    connect(launch,&QPushButton::clicked,this,[=] {
        if(version->count()==0)return;
        settings->setValue("game/version",version->currentData());
        if(windows) {
            const bool preview=version->currentData().toString()=="preview";
            status->setText(QDesktopServices::openUrl(QUrl(preview?"minecraft-preview:":"minecraft:"))
                ? tr("Bedrock se está abriendo…","Bedrock is starting…")
                : tr("No se pudo abrir el juego. Pulsá Comprobar de nuevo o reinstalalo.","Could not open the game. Press Check again or reinstall it."));
            return;
        }
        const auto dir=version->currentData().toString();
        job(tr("Abriendo Bedrock…","Opening Bedrock…"),[dir,log]{
            BedrockInstaller::writeProfile(dir);
            const auto pid=BedrockInstaller::play(log);
            if(!pid)throw std::runtime_error("Could not open the game. Press Check again or reinstall it.");
            return QString::number(pid);
        },[=](const QString &pid){
            *game=pid.toLongLong();launch->setEnabled(false);watch->start();
            status->setText(tr("Bedrock se está abriendo… Si Minecraft Linux pide aprobar tus credenciales de Google, aceptá en su ventana.",
                               "Bedrock is starting… If Minecraft Linux asks you to approve your Google credentials, accept in its window."));
        });
    });
}

// The fields emit signals while they are destroyed: none may reach the page then.
BedrockPage::~BedrockPage() {for(auto child:findChildren<QObject*>())child->disconnect(this);}

void BedrockPage::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    refresh();
}
