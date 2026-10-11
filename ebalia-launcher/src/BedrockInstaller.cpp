#include "BedrockInstaller.hpp"
#include "Download.hpp"
#include "Archive.hpp"
#include <QtCore>
#include <QRegularExpression>
#include <stdexcept>
#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#endif
namespace {
const QString runtimeId="io.mrarm.mcpelauncher";
void run(const QString &exe,const QStringList &args) {
    QProcess p;p.start(exe,args);
    if(!p.waitForStarted(10000))throw std::runtime_error("No se pudo iniciar: "+exe.toStdString());
    if(!p.waitForFinished(1800000)){p.kill();p.waitForFinished();throw std::runtime_error("La instalación superó el tiempo máximo.");}
    if(p.exitStatus()!=QProcess::NormalExit||p.exitCode()!=0)
        throw std::runtime_error(QString("Falló %1: %2").arg(exe,QString::fromUtf8(p.readAllStandardError()).right(1600)).toStdString());
}
}
bool BedrockInstaller::vcRuntimeReady() {
#ifdef Q_OS_WIN
    // The same registry value BedrockLauncher reads at start.
    QSettings key(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64)",QSettings::Registry64Format);
    auto version=key.value("Version").toString();version.remove('v');
    return key.value("Installed").toInt()==1&&QVersionNumber::compare(QVersionNumber::fromString(version),QVersionNumber(14,14,26405))>=0;
#else
    return true;
#endif
}
void BedrockInstaller::installVcRuntime(const QString &root) {
#ifdef Q_OS_WIN
    if(vcRuntimeReady())return;
    QDir().mkpath(root);const auto installer=root+"/vc_redist.x64.exe";
    // Microsoft's permanent link to the latest Visual C++ 2015-2022 redistributable.
    Download::file(QUrl("https://aka.ms/vs/17/release/vc_redist.x64.exe"),installer);
    const auto file=QDir::toNativeSeparators(installer).toStdWString();const std::wstring parameters=L"/install /passive /norestart";
    SHELLEXECUTEINFOW info{};info.cbSize=sizeof(info);info.fMask=SEE_MASK_NOCLOSEPROCESS;info.lpVerb=L"runas";info.lpFile=file.c_str();info.lpParameters=parameters.c_str();info.nShow=SW_SHOWNORMAL;
    if(!ShellExecuteExW(&info)||!info.hProcess){QFile::remove(installer);throw std::runtime_error("Visual C++ was not installed: Windows did not get permission to run the installer.");}
    WaitForSingleObject(info.hProcess,INFINITE);DWORD code=1;GetExitCodeProcess(info.hProcess,&code);CloseHandle(info.hProcess);QFile::remove(installer);
    // 3010: installed, restart pending; 1638: a newer version is already there.
    if((code!=0&&code!=3010&&code!=1638)||!vcRuntimeReady())throw std::runtime_error(QString("The Visual C++ installer did not finish (code %1).").arg(code).toStdString());
#else
    Q_UNUSED(root);
#endif
}
QString BedrockInstaller::install(const QString &root) {
    QDir().mkpath(root);
    QLockFile lock(root+"/install.lock");
    if(!lock.tryLock())throw std::runtime_error("Ya hay una instalación de Bedrock en curso.");
#if defined(Q_OS_LINUX)
    auto flatpak=QStandardPaths::findExecutable("flatpak");
    if(flatpak.isEmpty())throw std::runtime_error("Falta Flatpak. En NixOS habilitá services.flatpak.enable = true y reconstruí el sistema. Luego volvé a pulsar Instalar.");
    run(flatpak,{"remote-add","--user","--if-not-exists","flathub","https://flathub.org/repo/flathub.flatpakrepo"});
    run(flatpak,{"install","--user","--noninteractive","--assumeyes","flathub",runtimeId});
    return "flatpak";
#elif defined(Q_OS_MACOS)
    const QString repo="minecraft-linux/macos-builder",suffix=".dmg";
    QTemporaryDir stage(root+"/download-XXXXXX");
    if(!stage.isValid())throw std::runtime_error("No se pudo crear la carpeta temporal.");
    Download::file(QUrl("https://api.github.com/repos/"+repo+"/releases/latest"),stage.path()+"/release.json");
    QFile metadata(stage.path()+"/release.json");metadata.open(QIODevice::ReadOnly);
    auto release=QJsonDocument::fromJson(metadata.readAll()).object();
    if(release.isEmpty()||release["draft"].toBool()||release["prerelease"].toBool())throw std::runtime_error("No se encontró una publicación estable.");
    QJsonObject asset;
    for(auto v:release["assets"].toArray())if(v.toObject()["name"].toString().endsWith(suffix,Qt::CaseInsensitive)){if(!asset.isEmpty())throw std::runtime_error("Hay varios paquetes: selección automática no disponible.");asset=v.toObject();}
    auto url=QUrl(asset["browser_download_url"].toString());
    if(url.scheme()!="https"||url.host()!="github.com"||!url.path().startsWith("/"+repo+"/releases/download/"))throw std::runtime_error("Origen del paquete inesperado.");
    const auto file=stage.path()+"/package"+suffix;
    auto digest=asset["digest"].toString();
    QByteArray hash;
    if(digest.startsWith("sha256:"))hash=digest.mid(7).toLatin1();
    Download::file(url,file,hash);
    const auto target=root+"/provider-"+QUuid::createUuid().toString(QUuid::WithoutBraces);
    const auto mount=stage.path()+"/mount";
    QDir().mkpath(mount);
    run("/usr/bin/hdiutil",{"attach","-nobrowse","-readonly","-mountpoint",mount,file});
    QString app;
    try {
        auto entries=QDir(mount).entryList({"*.app"},QDir::Dirs);
        if(entries.size()!=1)throw std::runtime_error("No se encontró una aplicación única en el DMG.");
        QDir().mkpath(target);app=target+"/"+entries.front();
        run("/usr/bin/ditto",{mount+"/"+entries.front(),app});
    } catch(...) { try {run("/usr/bin/hdiutil",{"detach",mount});} catch(...) {} throw; }
    run("/usr/bin/hdiutil",{"detach",mount});
    return app;
#else
    throw std::runtime_error("Sistema no compatible con la instalación automática.");
#endif
}
QString BedrockInstaller::windowsGame() {
#ifdef Q_OS_WIN
    QProcess p;p.start("powershell.exe",{"-NoProfile","-NonInteractive","-Command","(Get-AppxPackage -Name Microsoft.MinecraftUWP).Version"});
    if(p.waitForStarted(5000)&&p.waitForFinished(20000)) {
        const auto version=QString::fromUtf8(p.readAllStandardOutput()).trimmed().section('\n',0,0).trimmed();
        if(!version.isEmpty())return version;
    } else p.kill();
    // Without PowerShell: the game registers the minecraft: link when it is installed.
    QSettings link(R"(HKEY_CLASSES_ROOT\minecraft)",QSettings::NativeFormat);
    return link.contains("URL Protocol")?"-":QString();
#else
    return {};
#endif
}
bool BedrockInstaller::runtimeInstalled() {
    const auto flatpak=QStandardPaths::findExecutable("flatpak");if(flatpak.isEmpty())return false;
    QProcess p;p.start(flatpak,{"info",runtimeId});
    return p.waitForStarted(5000)&&p.waitForFinished(15000)&&p.exitStatus()==QProcess::NormalExit&&p.exitCode()==0;
}
QString BedrockInstaller::gameData() {return QDir::homePath()+"/.var/app/"+runtimeId+"/data/mcpelauncher";}
QStringList BedrockInstaller::versions() {
    QStringList out;
    for(const auto &dir:QDir(gameData()+"/versions").entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Time))
        if(!QDir(dir.filePath()+"/lib").entryList({"*"},QDir::Dirs|QDir::NoDotAndDotDot).isEmpty())out<<dir.fileName();
    return out;
}
QString BedrockInstaller::importGame(const QStringList &files) {
    const auto flatpak=QStandardPaths::findExecutable("flatpak");
    if(flatpak.isEmpty()||!runtimeInstalled())throw std::runtime_error("Install Bedrock first.");
    // The runtime runs in a sandbox that only sees its own folder: the APKs are copied there to be extracted.
    QDir().mkpath(gameData()+"/versions");
    QTemporaryDir stage(gameData()+"/import-XXXXXX");
    if(!stage.isValid())throw std::runtime_error("No se pudo crear la carpeta temporal.");
    QStringList apks;int n=0;
    for(const auto &file:files) {
        const auto copy=stage.path()+QString("/%1.apk").arg(++n);
        if(file.endsWith(".apk",Qt::CaseInsensitive)){if(!QFile::copy(file,copy))throw std::runtime_error(("Could not copy "+file).toStdString());apks<<copy;continue;}
        const auto bundle=stage.path()+QString("/bundle-%1").arg(n);Archive::extract(file,bundle);
        QDirIterator it(bundle,{"*.apk"},QDir::Files,QDirIterator::Subdirectories);
        while(it.hasNext())apks<<it.next();
    }
    if(apks.isEmpty())throw std::runtime_error("No Minecraft APK was found in the chosen files.");
    QString name=QFileInfo(files.front()).completeBaseName();
    name.replace(QRegularExpression("[^A-Za-z0-9._-]+"),"-");if(name.isEmpty()||name.startsWith('.'))name="minecraft";
    QString version=name;for(int i=2;QFileInfo::exists(gameData()+"/versions/"+version);++i)version=name+"-"+QString::number(i);
    const auto target=gameData()+"/versions/"+version;
    run(flatpak,QStringList{"run","--command=mcpelauncher-extract",runtimeId}+apks+QStringList{target});
    QDirIterator game(target+"/lib",{"libminecraftpe.so"},QDir::Files,QDirIterator::Subdirectories);
    if(!game.hasNext()){QDir(target).removeRecursively();throw std::runtime_error("These files are not Minecraft Bedrock for Android (libminecraftpe.so is missing). Choose the Minecraft APK, including its x86_64 or arm64 part.");}
    return version;
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
QString BedrockInstaller::uninstall(const QString &root,bool deleteWorlds) {
    QDir().mkpath(root);
    QLockFile lock(root+"/install.lock");
    if(!lock.tryLock())throw std::runtime_error("Ya hay una instalación de Bedrock en curso.");
    QString backup;
#if defined(Q_OS_WIN)
    Q_UNUSED(deleteWorlds);
    const auto worlds=qEnvironmentVariable("LOCALAPPDATA")+"/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang";
    if(QDir(worlds).exists()) {
        backup=root+"/worlds-backup-"+QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
        copyFolder(worlds,backup+"/com.mojang");
    }
    if(!windowsGame().isEmpty())
        run("powershell.exe",{"-NoProfile","-NonInteractive","-Command","Get-AppxPackage -Name Microsoft.MinecraftUWP | Remove-AppxPackage -ErrorAction Stop"});
#elif defined(Q_OS_LINUX)
    if(runtimeInstalled()) {
        const auto flatpak=QStandardPaths::findExecutable("flatpak");
        QProcess user;user.start(flatpak,{"info","--user",runtimeId});
        const bool mine=user.waitForStarted(5000)&&user.waitForFinished(15000)&&user.exitCode()==0;
        run(flatpak,{"uninstall",mine?"--user":"--system","--noninteractive","--assumeyes",runtimeId});
    }
    // Downloaded game versions take gigabytes; worlds and settings stay unless the user asked to delete them.
    if(deleteWorlds)QDir(QDir::homePath()+"/.var/app/"+runtimeId).removeRecursively();
    else QDir(gameData()+"/versions").removeRecursively();
#else
    Q_UNUSED(deleteWorlds);
#endif
    // Managers installed by earlier versions of EBALIA (BedrockLauncher, the macOS app).
    for(const auto &dir:QDir(root).entryInfoList({"provider-*"},QDir::Dirs|QDir::NoDotAndDotDot))QDir(dir.filePath()).removeRecursively();
    return backup;
}
