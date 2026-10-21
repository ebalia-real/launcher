#pragma once
#include "VersionManager.hpp"
#include <functional>
namespace LostInstaller {
void install(const VersionInfo &version,const QString &destination,std::function<void(qint64,qint64)> progress={});
// Installations made by earlier launchers hold the whole package; this removes what the game does not use (see
// LostNative::needed), keeping its game folder with the worlds and options, the private home and the log.
void tidy(const VersionInfo &version,const QString &installDir);
// .exe/.bat/.cmd packages: Windows only, or Wine elsewhere.
bool windowsPackage(const VersionInfo &version);
// Java 8 for archive packages: a detected Java 8, or Mojang's official jre-legacy. Under Wine this is the Windows runtime.
QString java(const QString &dataDir,bool windowsPackage,std::function<void(int,int)> progress={});
QString winePrefix(const QString &dataDir);
QString wine(); // wine, or wine64 on distributions that only ship the 64-bit loader
}
