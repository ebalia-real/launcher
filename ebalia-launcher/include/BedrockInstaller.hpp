#pragma once
#include <QString>
#include <QStringList>
namespace BedrockInstaller {
// Runs on a worker thread. Linux: installs the Minecraft Linux runtime (Flatpak) and returns "flatpak";
// macOS: installs its app and returns the .app path.
QString install(const QString &root);
// Windows: the version of Minecraft for Windows (Microsoft Store / Xbox app), "-" when only its minecraft: link is
// registered, empty when it is not installed. Runs PowerShell: call it on a worker thread.
QString windowsGame();
// Linux: the Minecraft Linux runtime is installed in Flatpak (for this user or system-wide).
bool runtimeInstalled();
// Linux: the runtime's own data folder (worlds, settings and downloaded game versions).
QString gameData();
// Linux: game versions ready to play (folders with the extracted game), newest first.
QStringList versions();
// Linux: extracts the Minecraft APK files the user owns (.apk, or a .apks/.xapk/.zip bundle) into a new version
// and returns its name. Runs on a worker thread.
QString importGame(const QStringList &files);
// Removes Bedrock and the managers EBALIA installed. Windows: the Store edition keeps its worlds inside the package,
// so they are copied to root/worlds-backup-… first. Linux: worlds stay unless deleteWorlds. Returns the backup folder
// or an empty string. Runs on a worker thread.
QString uninstall(const QString &root, bool deleteWorlds);
// Windows: BedrockLauncher refuses to start without the Visual C++ 2015-2022 x64 runtime (14.14.26405 or later)
// registered system-wide. True elsewhere.
bool vcRuntimeReady();
// Downloads Microsoft's installer and runs it; Windows asks for administrator permission. Runs on a worker thread.
void installVcRuntime(const QString &root);
}
