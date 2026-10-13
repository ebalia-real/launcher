#pragma once
#include <QDateTime>
#include <QImage>
#include <QList>
#include <QString>
// What an instance holds besides mods: resource packs, shader packs and worlds, and the switches the game reads for them.
namespace InstanceContent {
struct Entry {QString file,title,description;QImage icon;qint64 size=0;QDateTime modified;bool active=false,directory=false;};
// The entries of "resourcepacks", "shaderpacks" or "saves", by name. Cheap: no archive is opened.
QList<Entry> list(const QString &instance,const QString &folder);
// Fills in what needs reading the pack or world: the description in pack.mcmeta and the picture (pack.png, icon.png).
Entry describe(const QString &instance,const QString &folder,Entry entry);
// Turns a resource pack on or off in options.txt, as the game's pack screen does; the newest one goes on top.
void setResourcePack(const QString &instance,const QString &file,bool on);
// The shader pack Iris or Oculus loads, or empty when shaders are off.
QString shader(const QString &instance);
// Chooses the shader pack for Iris or Oculus; an empty file turns shaders off.
void setShader(const QString &instance,const QString &file);
// Copies a pack ZIP, or a world ZIP holding level.dat, into the instance; returns its name there.
QString add(const QString &instance,const QString &folder,const QString &source);
// Moves an entry to removed-<folder> inside the instance, so it can be recovered by hand.
void remove(const QString &instance,const QString &folder,const QString &file);
// Writes one world into a ZIP with its folder at the top, the way other launchers and servers expect it.
void exportWorld(const QString &instance,const QString &world,const QString &zip);
}
