#include "Uninstaller.hpp"
#include <QtCore>
#include <algorithm>

namespace {
#ifdef Q_OS_WIN
constexpr auto pathCase=Qt::CaseInsensitive;
#else
constexpr auto pathCase=Qt::CaseSensitive;
#endif
QString clean(const QString &path){return path.isEmpty()?QString():QDir::cleanPath(QDir::fromNativeSeparators(QFileInfo(path).absoluteFilePath()));}
// The folders between a listed file and the program folder, so the ones left empty go too.
void addParents(QStringList &prune,const QString &root,QString relative){
    for(relative=QFileInfo(relative).path();!relative.isEmpty()&&relative!=".";relative=QFileInfo(relative).path())
        if(const auto folder=root+"/"+relative;!prune.contains(folder))prune<<folder;
}
QString shell(const QString &text){auto quoted=text;quoted.replace("'","'\\''");return "'"+quoted+"'";}
QString powershell(const QString &text){auto quoted=QDir::toNativeSeparators(text);quoted.replace("'","''");return "'"+quoted+"'";}
}

namespace Uninstaller {
bool safeDataFolder(const QString &path){
    const auto folder=clean(path);
    if(folder.isEmpty()||QDir(folder).isRoot()||folder.count('/')<2)return false;
    QStringList kept{QDir::homePath(),QDir::rootPath(),QDir::tempPath()};
    for(auto location:{QStandardPaths::DesktopLocation,QStandardPaths::DocumentsLocation,QStandardPaths::DownloadLocation,QStandardPaths::MusicLocation,QStandardPaths::PicturesLocation,
                       QStandardPaths::MoviesLocation,QStandardPaths::GenericDataLocation,QStandardPaths::GenericConfigLocation,QStandardPaths::GenericCacheLocation,QStandardPaths::ApplicationsLocation})
        kept<<QStandardPaths::standardLocations(location);
    for(const auto &other:kept)if(!other.isEmpty()&&clean(other).compare(folder,pathCase)==0)return false;
    // It has to look like the launcher's: one of the things it keeps there (or nothing at all).
    if(!QFileInfo::exists(folder))return true;
    for(const auto name:{"mc","launcher.lock","news.json","skins","updates","cache","settings"})if(QFileInfo::exists(folder+"/"+name))return true;
    return QDir(folder).isEmpty(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System);
}

Plan planFor(Kind kind,const QString &destination,bool portable,const QString &dataRoot,bool removeData){
    Plan plan;plan.kind=kind;plan.portable=portable;plan.data=clean(dataRoot);const auto target=clean(destination);
    switch(kind){
    case Kind::WindowsInstaller:plan.location=target;plan.uninstaller=target+"/Uninstall.exe";break;
    case Kind::Folder:{
        plan.location=target;QStringList files;
        QFile manifest(target+"/update-files.json");
        if(manifest.open(QIODevice::ReadOnly))for(const auto &entry:QJsonDocument::fromJson(manifest.readAll()).object()["files"].toArray())files<<entry.toString();
        if(files.isEmpty()){plan.kind=Kind::Unknown;break;}
        files<<"update-files.json";if(portable)files<<"portable.txt";
        for(const auto &file:files){
            const auto relative=QDir::cleanPath(QDir::fromNativeSeparators(file));
            if(relative.isEmpty()||relative=="."||relative==".."||relative.startsWith("../")||QDir::isAbsolutePath(relative)||relative.contains(':'))continue; // only what is inside the folder
            if(const auto path=target+"/"+relative;!plan.remove.contains(path)){plan.remove<<path;addParents(plan.prune,target,relative);}
        }
        std::stable_sort(plan.prune.begin(),plan.prune.end(),[](const QString &a,const QString &b){return a.count('/')>b.count('/');});
        plan.prune<<target; // the folder itself, when nothing else is in it
        break;}
    case Kind::AppImage:case Kind::MacApp:
        plan.location=QFileInfo(target).absolutePath();plan.remove<<target;
        if(portable){plan.remove<<plan.location+"/portable.txt";plan.prune<<plan.location;}
        break;
    case Kind::System:case Kind::Unknown:plan.location=target;break;
    }
    if(removeData&&!plan.data.isEmpty()&&safeDataFolder(plan.data))plan.remove<<plan.data;
    return plan;
}

Plan plan(const QString &dataRoot,bool removeData){
    const bool portable=qEnvironmentVariableIsSet("EBALIA_PORTABLE");
#if defined(Q_OS_MACOS)
    if(const auto program=QCoreApplication::applicationDirPath();program.endsWith(".app/Contents/MacOS")){
        const auto app=QDir::cleanPath(program+"/../..");
        return planFor(QFileInfo(QFileInfo(app).absolutePath()).isWritable()?Kind::MacApp:Kind::System,app,portable,dataRoot,removeData);
    }
    const auto folder=QCoreApplication::applicationDirPath(); // a program outside a bundle (a build folder)
#elif defined(Q_OS_LINUX)
    const QFileInfo image(qEnvironmentVariable("APPIMAGE"));
    if(!image.filePath().isEmpty()&&image.isFile())return planFor(QFileInfo(image.absolutePath()).isWritable()?Kind::AppImage:Kind::System,image.absoluteFilePath(),portable,dataRoot,removeData);
    if(QFileInfo(QCoreApplication::applicationFilePath()).canonicalFilePath().startsWith("/nix/store/"))return planFor(Kind::System,QCoreApplication::applicationDirPath(),portable,dataRoot,removeData);
    const auto folder=QDir::cleanPath(QCoreApplication::applicationDirPath()+"/.."); // bin/ inside the folder
#else
    const auto folder=QCoreApplication::applicationDirPath();
    if(!portable&&QFileInfo::exists(folder+"/Uninstall.exe"))return planFor(Kind::WindowsInstaller,folder,portable,dataRoot,removeData);
#endif
    if(!QFileInfo(folder).isWritable())return planFor(Kind::System,folder,portable,dataRoot,removeData);
    return planFor(QFileInfo::exists(folder+"/update-files.json")?Kind::Folder:Kind::Unknown,folder,portable,dataRoot,removeData);
}

QString script(const Plan &plan,qint64 pid){
    QStringList lines;
#ifdef Q_OS_WIN
    lines<<"# EBALIA Launcher: removes the launcher once it has closed, then this script."<<"$ErrorActionPreference = 'SilentlyContinue'"
         <<QString("Wait-Process -Id %1").arg(pid)<<"Start-Sleep -Milliseconds 500";
    for(const auto &path:plan.remove)lines<<"Remove-Item -LiteralPath "+powershell(path)+" -Recurse -Force";
    for(const auto &folder:plan.prune)lines<<"if (-not (Get-ChildItem -LiteralPath "+powershell(folder)+" -Force)) { Remove-Item -LiteralPath "+powershell(folder)+" -Force }";
    if(!plan.uninstaller.isEmpty())lines<<"if (Test-Path -LiteralPath "+powershell(plan.uninstaller)+") { Start-Process -FilePath "+powershell(plan.uninstaller)+" -ArgumentList '/S' -Wait }";
    lines<<"Remove-Item -LiteralPath $PSCommandPath -Force";
#else
    lines<<"#!/bin/sh"<<"# EBALIA Launcher: removes the launcher once it has closed, then this script."
         <<QString("while kill -0 %1 2>/dev/null; do sleep 0.2; done").arg(pid);
    for(const auto &path:plan.remove)lines<<"rm -rf -- "+shell(path);
    for(const auto &folder:plan.prune)lines<<"rmdir -- "+shell(folder)+" 2>/dev/null";
    lines<<"rm -f -- \"$0\"";
#endif
    return lines.join('\n')+'\n';
}

bool start(const Plan &plan){
    if(plan.remove.isEmpty()&&plan.uninstaller.isEmpty())return false;
#ifdef Q_OS_WIN
    const auto path=QDir::tempPath()+"/ebalia-uninstall-"+QUuid::createUuid().toString(QUuid::WithoutBraces)+".ps1";
    QFile file(path);if(!file.open(QIODevice::WriteOnly))return false;
    file.write("\xEF\xBB\xBF");file.write(script(plan,QCoreApplication::applicationPid()).toUtf8());file.close(); // with a BOM, so Windows PowerShell reads the paths as UTF-8
    return QProcess::startDetached("powershell.exe",{"-NoProfile","-ExecutionPolicy","Bypass","-WindowStyle","Hidden","-File",QDir::toNativeSeparators(path)});
#else
    const auto path=QDir::tempPath()+"/ebalia-uninstall-"+QUuid::createUuid().toString(QUuid::WithoutBraces)+".sh";
    QFile file(path);if(!file.open(QIODevice::WriteOnly))return false;
    file.write(script(plan,QCoreApplication::applicationPid()).toUtf8());file.close();file.setPermissions(QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner);
    return QProcess::startDetached("/bin/sh",{path});
#endif
}
}
