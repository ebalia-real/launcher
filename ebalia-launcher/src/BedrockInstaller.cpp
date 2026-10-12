#include "BedrockInstaller.hpp"
#include "Download.hpp"
#include <QtCore>
#include <QRegularExpression>
#include <QVersionNumber>
#include <stdexcept>
#ifdef Q_OS_UNIX
#include <signal.h>
#endif
#ifdef Q_OS_MACOS
#include <sys/sysctl.h>
#endif

namespace {
const QString runtimeId="io.mrarm.mcpelauncher",profileName="EBALIA";
QString flatpak(){return QStandardPaths::findExecutable("flatpak");}
// EBALIA's own Qt variables (plugin and QML paths of its package) must not reach Minecraft Linux, which has its own Qt.
QProcessEnvironment cleanEnvironment() {
    auto env=QProcessEnvironment::systemEnvironment();
    for(const auto &name:{"QT_PLUGIN_PATH","QML_IMPORT_PATH","QML2_IMPORT_PATH","NIXPKGS_QT6_QML_IMPORT_PATH","QTWEBENGINE_RESOURCES_PATH","QT_QPA_PLATFORM_PLUGIN_PATH"})env.remove(name);
    return env;
}
// Runs a command and returns its output; an error when it cannot start, takes too long or fails.
QString run(const QString &exe,const QStringList &args,int timeout=1800000) {
    QProcess p;p.setProcessEnvironment(cleanEnvironment());p.start(exe,args);p.closeWriteChannel();
    if(!p.waitForStarted(10000))throw std::runtime_error("No se pudo iniciar: "+exe.toStdString());
    if(!p.waitForFinished(timeout)){p.kill();p.waitForFinished();throw std::runtime_error("La instalación superó el tiempo máximo.");}
    if(p.exitStatus()!=QProcess::NormalExit||p.exitCode()!=0)
        throw std::runtime_error(QString("Falló %1: %2").arg(exe,QString::fromUtf8(p.readAllStandardError()).right(1600)).toStdString());
    return QString::fromUtf8(p.readAllStandardOutput());
}
// The same, for quick questions: true when the command succeeded.
bool ask(const QString &exe,const QStringList &args,QString *out=nullptr) {
    if(exe.isEmpty())return false;
    QProcess p;p.setProcessEnvironment(cleanEnvironment());p.start(exe,args);p.closeWriteChannel();
    if(!p.waitForStarted(5000))return false;
    if(!p.waitForFinished(30000)){p.kill();p.waitForFinished();return false;}
    if(out)*out=QString::fromUtf8(p.readAllStandardOutput());
    return p.exitStatus()==QProcess::NormalExit&&p.exitCode()==0;
}
void removeOldManagers(const QString &root) {
    // BedrockLauncher and macOS app copies installed by EBALIA 1.1.0–1.1.6.
    for(const auto &dir:QDir(root).entryInfoList({"provider-*"},QDir::Dirs|QDir::NoDotAndDotDot))QDir(dir.filePath()).removeRecursively();
}
#ifdef Q_OS_MACOS
QString appPath(){return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)+"/bedrock/Minecraft Bedrock Launcher.app";}
QString releaseFile(){return QFileInfo(appPath()).absolutePath()+"/release.ini";}
// The newest stable macos-builder release that has the Apple Silicon app with its checksum. Listing releases, not
// /releases/latest: GitHub orders "latest" by commit date and would pick an old Intel build.
QJsonObject newestMacRelease() {
    QTemporaryDir stage;if(!stage.isValid())throw std::runtime_error("No se pudo crear la carpeta temporal.");
    Download::file(QUrl("https://api.github.com/repos/minecraft-linux/macos-builder/releases?per_page=30"),stage.path()+"/releases.json");
    QFile file(stage.path()+"/releases.json");file.open(QIODevice::ReadOnly);
    QJsonObject best;QString bestDate;
    for(const auto &value:QJsonDocument::fromJson(file.readAll()).array()) {
        const auto release=value.toObject();if(release["draft"].toBool()||release["prerelease"].toBool())continue;
        for(const auto &a:release["assets"].toArray()) {
            const auto asset=a.toObject();
            if(asset["name"].toString()!="Minecraft.Bedrock.Launcher.dmg"||!asset["digest"].toString().startsWith("sha256:"))continue;
            if(release["published_at"].toString()>bestDate){bestDate=release["published_at"].toString();best={{"tag",release["tag_name"]},{"url",asset["browser_download_url"]},{"digest",asset["digest"]}};}
        }
    }
    if(best.isEmpty())throw std::runtime_error("No se encontró una publicación estable.");
    return best;
}
#endif
}

QString BedrockInstaller::runtime() {
    if(qEnvironmentVariableIsSet("EBALIA_BEDROCK_RUNTIME"))return qEnvironmentVariable("EBALIA_BEDROCK_RUNTIME");
#if defined(Q_OS_MACOS)
    return QFileInfo::exists(appPath()+"/Contents/MacOS/mcpelauncher-ui-qt")?appPath():QString();
#elif defined(Q_OS_LINUX)
    // Always one scope at a time: without --user or --system, flatpak asks which installation to use and waits.
    for(const QString scope:{"--user","--system"})if(ask(flatpak(),{"info",scope,runtimeId}))return scope;
    return {};
#else
    return {};
#endif
}

QString BedrockInstaller::unsupportedReason() {
#ifdef Q_OS_MACOS
    int arm=0;size_t size=sizeof(arm);
    // hw.optional.arm64 is also right when EBALIA itself runs under Rosetta.
    if(sysctlbyname("hw.optional.arm64",&arm,&size,nullptr,0)!=0||arm!=1)return "intel";
    if(QOperatingSystemVersion::current()<QOperatingSystemVersion(QOperatingSystemVersion::MacOS,15))return "macos";
#endif
    return {};
}

void BedrockInstaller::install(const QString &root) {
    QDir().mkpath(root);
    QLockFile lock(root+"/install.lock");
    if(!lock.tryLock())throw std::runtime_error("Ya hay una instalación de Bedrock en curso.");
#if defined(Q_OS_LINUX)
    if(flatpak().isEmpty())throw std::runtime_error("Falta Flatpak. En NixOS habilitá services.flatpak.enable = true y reconstruí el sistema. Luego volvé a pulsar Instalar.");
    run(flatpak(),{"remote-add","--user","--if-not-exists","flathub","https://flathub.org/repo/flathub.flatpakrepo"});
    run(flatpak(),{"install","--user","--noninteractive","--assumeyes","flathub",runtimeId});
    removeOldManagers(root);
#elif defined(Q_OS_MACOS)
    if(!unsupportedReason().isEmpty())throw std::runtime_error("Sistema no compatible con la instalación automática.");
    const auto release=newestMacRelease();
    const QUrl url(release["url"].toString());
    if(url.scheme()!="https"||url.host()!="github.com"||!url.path().startsWith("/minecraft-linux/macos-builder/releases/download/"))throw std::runtime_error("Origen del paquete inesperado.");
    const auto target=appPath(),folder=QFileInfo(target).absolutePath();QDir().mkpath(folder);
    QTemporaryDir stage(folder+"/download-XXXXXX");
    if(!stage.isValid())throw std::runtime_error("No se pudo crear la carpeta temporal.");
    const auto dmg=stage.path()+"/Minecraft.Bedrock.Launcher.dmg";
    Download::file(url,dmg,release["digest"].toString().mid(7).toLatin1());
    const auto mount=stage.path()+"/mount",copy=stage.path()+"/Minecraft Bedrock Launcher.app";QDir().mkpath(mount);
    run("/usr/bin/hdiutil",{"attach","-nobrowse","-readonly","-noautoopen","-mountpoint",mount,dmg});
    try {
        const auto apps=QDir(mount).entryList({"*.app"},QDir::Dirs);
        if(apps.size()!=1)throw std::runtime_error("No se encontró una aplicación única en el DMG.");
        run("/usr/bin/ditto",{mount+"/"+apps.front(),copy});
    } catch(...) {try{run("/usr/bin/hdiutil",{"detach",mount});}catch(...){} throw;}
    run("/usr/bin/hdiutil",{"detach",mount});
    if(!QFileInfo::exists(copy+"/Contents/MacOS/mcpelauncher-ui-qt"))throw std::runtime_error("No se encontró una aplicación única en el DMG.");
    // Swapped in one step, so a failed update keeps the working copy.
    const auto old=target+".old";QDir(old).removeRecursively();
    if(QFileInfo::exists(target)&&!QDir().rename(target,old))throw std::runtime_error(("Could not install "+target).toStdString());
    if(!QDir().rename(copy,target)){QDir().rename(old,target);throw std::runtime_error(("Could not install "+target).toStdString());}
    QDir(old).removeRecursively();
    QSettings record(releaseFile(),QSettings::IniFormat);record.setValue("tag",release["tag"].toString());record.setValue("digest",release["digest"].toString());record.sync();
    removeOldManagers(root);
#else
    Q_UNUSED(root);
    throw std::runtime_error("Sistema no compatible con la instalación automática.");
#endif
}

QString BedrockInstaller::updateAvailable(const QString &root) {
    Q_UNUSED(root);
    if(qEnvironmentVariableIsSet("EBALIA_NO_NETWORK"))return {};
#if defined(Q_OS_LINUX)
    const auto scope=runtime();QString out;
    if(scope.isEmpty()||!ask(flatpak(),{"remote-ls",scope,"--updates","--app","--columns=application,version","flathub"},&out))return {};
    for(const auto &line:out.split('\n'))if(line.startsWith(runtimeId)){const auto version=line.section('\t',1).trimmed();return version.isEmpty()?QString("Flathub"):version;}
    return {};
#elif defined(Q_OS_MACOS)
    if(runtime().isEmpty())return {};
    const auto release=newestMacRelease();QSettings record(releaseFile(),QSettings::IniFormat);
    return record.value("digest").toString()!=release["digest"].toString()?release["tag"].toString():QString();
#else
    return {};
#endif
}

void BedrockInstaller::update(const QString &root) {
#if defined(Q_OS_LINUX)
    const auto scope=runtime();if(scope.isEmpty())throw std::runtime_error("Install Bedrock first.");
    if(managerRunning())throw std::runtime_error("Close Minecraft Linux first.");
    // The runtime it needs is updated with it when both are in the same installation.
    run(flatpak(),{"update",scope,"--noninteractive","--assumeyes",runtimeId});
    Q_UNUSED(root);
#elif defined(Q_OS_MACOS)
    if(managerRunning())throw std::runtime_error("Close Minecraft Linux first.");
    install(root);
#else
    Q_UNUSED(root);
#endif
}

QString BedrockInstaller::gameData() {
    if(qEnvironmentVariableIsSet("EBALIA_BEDROCK_GAMEDATA"))return qEnvironmentVariable("EBALIA_BEDROCK_GAMEDATA");
#if defined(Q_OS_MACOS)
    return QDir::homePath()+"/Library/Application Support/mcpelauncher";
#elif defined(Q_OS_LINUX)
    return QDir::homePath()+"/.var/app/"+runtimeId+"/data/mcpelauncher";
#else
    return {};
#endif
}

QList<BedrockInstaller::Version> BedrockInstaller::versions() {
    // Minecraft Linux records its downloads in versions.ini; a folder counts only when the game library is there.
    const auto root=gameData()+"/versions";QSettings names(root+"/versions.ini",QSettings::IniFormat);
    QList<Version> out;
    for(const auto &info:QDir(root).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        Version v{info.fileName(),{},{}};
        for(const auto &abi:QDir(info.filePath()+"/lib").entryList(QDir::Dirs|QDir::NoDotAndDotDot))
            if(QFileInfo::exists(info.filePath()+"/lib/"+abi+"/libminecraftpe.so")){v.abi=abi;break;}
        if(v.abi.isEmpty())continue;
        v.name=names.value(v.dir+"/versionName").toString();if(v.name.isEmpty())v.name=v.dir;
        out<<v;
    }
    std::sort(out.begin(),out.end(),[](const Version &a,const Version &b){return QVersionNumber::compare(QVersionNumber::fromString(a.name),QVersionNumber::fromString(b.name))>0;});
    return out;
}

bool BedrockInstaller::openManager() {
#if defined(Q_OS_MACOS)
    return QProcess::startDetached("/usr/bin/open",{"-n",appPath()});
#elif defined(Q_OS_LINUX)
    const auto scope=runtime();if(scope.isEmpty())return false;
    QProcess p;p.setProgram(flatpak());p.setArguments({"run",scope,runtimeId});p.setProcessEnvironment(cleanEnvironment());p.setStandardInputFile(QProcess::nullDevice());
    return p.startDetached();
#else
    return false;
#endif
}

bool BedrockInstaller::managerRunning() {
#if defined(Q_OS_MACOS)
    return ask("/usr/bin/pgrep",{"-f","Minecraft Bedrock Launcher.app/Contents/MacOS/"});
#elif defined(Q_OS_LINUX)
    QString out;return ask(flatpak(),{"ps","--columns=application"},&out)&&out.contains(runtimeId);
#else
    return false;
#endif
}

void BedrockInstaller::writeProfile(const QString &dir) {
    // Minecraft Linux rewrites its files from memory while it is open.
    if(managerRunning())throw std::runtime_error("Close Minecraft Linux before playing from EBALIA.");
    const auto all=versions();const auto it=std::find_if(all.begin(),all.end(),[&](const Version &v){return v.dir==dir;});
    if(it==all.end())throw std::runtime_error("Invalid version.");
    QDir().mkpath(gameData()+"/profiles");
    QSettings profiles(gameData()+"/profiles/profiles.ini",QSettings::IniFormat);
    // The compatibility module (mcpelauncher-updates) is enabled in the profile the player uses in Minecraft Linux.
    auto source=profiles.value("selected").toString();if(source.isEmpty()||source==profileName||!profiles.childGroups().contains(source))source="Default";
    QStringList mods;
    profiles.beginGroup(source);const int count=profiles.beginReadArray("mods");
    for(int n=0;n<count;++n){profiles.setArrayIndex(n);const auto path=profiles.value("path").toString();if(!path.isEmpty())mods<<path;}
    profiles.endArray();profiles.endGroup();
    profiles.remove(profileName);profiles.beginGroup(profileName);
    profiles.setValue("version","dir "+dir);profiles.setValue("arch",it->abi);
    profiles.beginWriteArray("mods",int(mods.size()));
    for(int n=0;n<mods.size();++n){profiles.setArrayIndex(n);profiles.setValue("path",mods[n]);}
    profiles.endArray();profiles.endGroup();profiles.sync();
    if(profiles.status()!=QSettings::NoError)throw std::runtime_error(("Could not save "+profiles.fileName()).toStdString());
}

qint64 BedrockInstaller::play(const QString &logFile) {
    QProcess p;
#if defined(Q_OS_MACOS)
    p.setProgram(appPath()+"/Contents/MacOS/mcpelauncher-ui-qt");p.setArguments({"--profile",profileName});p.setWorkingDirectory(appPath()+"/Contents/MacOS");
#elif defined(Q_OS_LINUX)
    // The profile mode of Minecraft Linux checks the purchase and loads the compatibility module like its Play button.
    const auto scope=runtime();if(scope.isEmpty())return 0;
    p.setProgram(flatpak());p.setArguments({"run",scope,runtimeId,"-p",profileName});
#else
    return 0;
#endif
    QDir().mkpath(QFileInfo(logFile).absolutePath());QFile::remove(logFile);
    p.setProcessEnvironment(cleanEnvironment());p.setStandardInputFile(QProcess::nullDevice());
    p.setStandardOutputFile(logFile,QIODevice::Append);p.setStandardErrorFile(logFile,QIODevice::Append);
    // Detached: the game keeps running when EBALIA closes or rebuilds this page.
    qint64 pid=0;return p.startDetached(&pid)?pid:0;
}

QString BedrockInstaller::problem(const QString &log) {
    // Messages of mcpelauncher-ui-qt's profile mode and of its client (exit codes do not reach EBALIA in this mode).
    if(log.contains("Something went wrong"))return "licence";
    if(log.contains("Failed to load Minecraft library")||log.contains("Incompatible Minecraft installation"))return "incompatible";
    if(log.contains("Missing launcher dependencies"))return "dependencies";
    return {};
}

BedrockInstaller::WindowsGames BedrockInstaller::windowsGames() {
    WindowsGames games;
#ifdef Q_OS_WIN
    QString out;
    ask("powershell.exe",{"-NoProfile","-NonInteractive","-Command",
        "foreach($n in 'Microsoft.MinecraftUWP','Microsoft.MinecraftWindowsBeta','Microsoft.GamingServices'){$p=@(Get-AppxPackage -Name $n);if($p.Count){$n+'|'+$p[0].Version}}"},&out);
    // Store versions write the game's last two numbers together: 1.26.5203.0 is Minecraft 1.26.52.3.
    auto game=[](const QString &version){auto parts=version.split('.');if(parts.size()>=3&&parts[2].toInt()>=100)return QString("%1.%2.%3.%4").arg(parts[0],parts[1]).arg(parts[2].toInt()/100).arg(parts[2].toInt()%100);return version;};
    for(const auto &line:out.split('\n')) {
        const auto name=line.section('|',0,0).trimmed(),version=line.section('|',1).trimmed();
        if(name=="Microsoft.MinecraftUWP")games.release=game(version);
        else if(name=="Microsoft.MinecraftWindowsBeta")games.preview=game(version);
        else if(name=="Microsoft.GamingServices")games.gamingServices=true;
    }
#endif
    return games;
}

#ifdef Q_OS_WIN
namespace {
void copyFolder(const QString &from,const QString &to) {
    QDirIterator it(from,QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDirIterator::Subdirectories);
    while(it.hasNext()) {
        const auto file=it.next();const auto target=to+"/"+QDir(from).relativeFilePath(file);
        QDir().mkpath(QFileInfo(target).absolutePath());
        if(!QFile::copy(file,target))throw std::runtime_error(("Could not copy "+file).toStdString());
    }
}
}
#endif

QString BedrockInstaller::uninstall(const QString &root,const QString &edition,bool deleteWorlds) {
    QDir().mkpath(root);
    QLockFile lock(root+"/install.lock");
    if(!lock.tryLock())throw std::runtime_error("Ya hay una instalación de Bedrock en curso.");
    QString backup;
#if defined(Q_OS_WIN)
    Q_UNUSED(deleteWorlds);
    const bool preview=edition=="preview";
    // Worlds: %APPDATA%\Minecraft Bedrock (current versions) and the package folder of older ones, which goes with it.
    const QStringList sources{qEnvironmentVariable("APPDATA")+(preview?"/Minecraft Bedrock Preview":"/Minecraft Bedrock"),
        qEnvironmentVariable("LOCALAPPDATA")+"/Packages/"+(preview?"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe":"Microsoft.MinecraftUWP_8wekyb3d8bbwe")+"/LocalState/games/com.mojang"};
    for(const auto &source:sources)if(QDir(source).exists()) {
        if(backup.isEmpty())backup=root+"/worlds-backup-"+QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
        copyFolder(source,backup+"/"+QFileInfo(source).fileName());
    }
    run("powershell.exe",{"-NoProfile","-NonInteractive","-Command",QString("Get-AppxPackage -Name %1 | Remove-AppxPackage -ErrorAction Stop").arg(preview?"Microsoft.MinecraftWindowsBeta":"Microsoft.MinecraftUWP")});
#elif defined(Q_OS_LINUX)
    Q_UNUSED(edition);
    if(managerRunning())throw std::runtime_error("Close Minecraft Linux first.");
    if(const auto scope=runtime();!scope.isEmpty())run(flatpak(),{"uninstall",scope,"--noninteractive","--assumeyes",runtimeId});
    // Downloaded game versions take gigabytes; worlds and settings stay unless the player asked to delete them.
    if(deleteWorlds)QDir(QDir::homePath()+"/.var/app/"+runtimeId).removeRecursively();
    else QDir(gameData()+"/versions").removeRecursively();
#elif defined(Q_OS_MACOS)
    Q_UNUSED(edition);
    if(managerRunning())throw std::runtime_error("Close Minecraft Linux first.");
    QDir(appPath()).removeRecursively();QFile::remove(releaseFile());
    if(deleteWorlds){QDir(gameData()).removeRecursively();QDir(QDir::homePath()+"/Library/Caches/mcpelauncher").removeRecursively();}
    else QDir(gameData()+"/versions").removeRecursively();
#else
    Q_UNUSED(edition);Q_UNUSED(deleteWorlds);
#endif
    removeOldManagers(root);
    return backup;
}
