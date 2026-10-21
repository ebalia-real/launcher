#pragma once
#include <QString>
#include <QStringList>

// Removes the launcher from the computer once it has closed: a small script waits for it, then deletes only what is the
// launcher's (the files its package lists, the AppImage file or the .app bundle) and, when asked, the data folder.
namespace Uninstaller {
enum class Kind {
    WindowsInstaller, // installed with the setup: its own Uninstall.exe removes it
    Folder,           // a folder of files listed in update-files.json (Windows or Linux, portable or not)
    AppImage,         // one AppImage file
    MacApp,           // an .app bundle
    System,           // owned by the system (Nix, a package): removed with its package manager
    Unknown           // an old folder that does not list its files; it cannot tell what is safe to delete
};
struct Plan {
    Kind kind = Kind::Unknown;
    QString location;      // the folder the launcher is in, shown to the user
    bool portable = false;
    QStringList remove;    // files and folders to delete
    QStringList prune;     // folders to remove when they are left empty, deepest first
    QString uninstaller;   // run silently last (Windows setup)
    QString data;          // the data folder; in remove too when the data goes
};
// What removing this copy of the launcher takes. destination is the program folder, the AppImage file or the .app.
Plan planFor(Kind kind, const QString &destination, bool portable, const QString &dataRoot, bool removeData);
// The plan for the running launcher.
Plan plan(const QString &dataRoot, bool removeData);
// Folders never deleted as data: the root, the home folder and the user's usual folders.
bool safeDataFolder(const QString &path);
// The script that waits for the process pid to end, carries out the plan and deletes itself (PowerShell on Windows,
// sh elsewhere).
QString script(const Plan &plan, qint64 pid);
// Writes the script to a temporary file and starts it, detached. The caller then quits.
bool start(const Plan &plan);
}
