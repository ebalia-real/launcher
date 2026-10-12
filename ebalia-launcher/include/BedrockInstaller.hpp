#pragma once
#include <QList>
#include <QString>
#include <QStringList>
// Bedrock through the player's own copy only.
// Linux and macOS: the Android edition through Minecraft Linux (Flathub io.mrarm.mcpelauncher / macos-builder app).
// Its own window signs in to Google Play, checks the purchase and downloads versions; EBALIA chooses the version and
// starts it through that launcher's profile mode, which keeps its licence check and compatibility module.
// Windows: Minecraft for Windows / Minecraft Preview from the Microsoft Store, Xbox app or Game Pass.
namespace BedrockInstaller {
// Minecraft Linux on this computer. Linux: the Flatpak installation ("--user" or "--system", empty when missing);
// macOS: the app EBALIA installed (empty when missing).
QString runtime();
// Linux: installs the Flatpak for this user; macOS: installs or updates the app (Apple Silicon, macOS 15+).
// Runs on a worker thread.
void install(const QString &root);
// A newer Minecraft Linux ("v1.8.5"), or empty. Network: run on a worker thread.
QString updateAvailable(const QString &root);
// Applies that update. Runs on a worker thread.
void update(const QString &root);
// Why this computer cannot run Minecraft Linux (empty when it can): macOS needs Apple Silicon and macOS 15.
QString unsupportedReason();
// Minecraft Linux's data folder: downloaded versions, profiles, worlds and settings.
QString gameData();
struct Version { QString dir, name, abi; };
// Downloaded game versions, newest first.
QList<Version> versions();
// Opens Minecraft Linux's own window (Google sign-in, licence check, downloading versions).
bool openManager();
// Minecraft Linux is running (its window or a game): its files must not be changed meanwhile.
bool managerRunning();
// Writes the "EBALIA" profile for version dir, with the compatibility mods of the profile the player uses.
void writeProfile(const QString &dir);
// Starts the "EBALIA" profile; the log goes to logFile. Returns the process id, or 0.
qint64 play(const QString &logFile);
// What went wrong, from the end of a game log: "licence", "incompatible", "dependencies" or empty.
QString problem(const QString &log);

// Windows: installed editions ("release", "preview") with their version, and whether Gaming Services is there.
struct WindowsGames { QString release, preview; bool gamingServices = false; };
WindowsGames windowsGames();
// Removes Bedrock. Windows: the chosen edition, after copying the worlds to root/worlds-backup-…;
// Linux/macOS: Minecraft Linux, keeping worlds unless deleteWorlds. Returns the backup folder or an empty string.
// Runs on a worker thread.
QString uninstall(const QString &root, const QString &edition, bool deleteWorlds);
}
