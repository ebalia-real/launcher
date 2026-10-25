#pragma once
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
class SkinManager {
public:
    explicit SkinManager(QString root):m_root(std::move(root)){}
    QJsonObject importSkin(const QString &file,const QString &name,const QString &variant);
    QJsonArray skins() const;
    // Deletes a skin from the library (its copy and its name); the PNG it was imported from is left alone.
    void removeSkin(const QString &id);
    static QImage preview(const QImage &skin,const QString &variant,bool back=false);
    static void upload(const QString &file,const QString &variant,const QString &token);
    // Puts the skin on an instance's default player textures (a resource pack, ebalia-local-skin.zip, enabled in its
    // options.txt), in the pack format of its client (clientJar: versions/<id>/<id>.jar).
    static void applyLocal(const QString &file,const QString &variant,const QString &instance,const QString &clientJar);
    // Takes the pack off (options.txt and the file).
    static void removeLocal(const QString &instance);
    // The local skin of all instances (Skins → Apply locally to all instances): a copy in root/skins with its model, its
    // name and a stamp that changes with it. defaultLocal is empty when there is none.
    static void setDefaultLocal(const QString &root,const QString &file,const QString &variant,const QString &name);
    static void clearDefaultLocal(const QString &root);
    static QJsonObject defaultLocal(const QString &root);
    // What an instance wears, kept in ebalia-local-skin.json: "default" (the skin of all instances, with its stamp),
    // "own" (a skin put on it alone), "none" (restored), "restore" (to be restored when it starts: it was open) or
    // empty (nothing yet). An empty mode removes the mark.
    static void markLocal(const QString &instance,const QString &mode,const QString &stamp={});
    static QString localMode(const QString &instance);
    // Before an instance starts: the skin of all instances goes on it (or is brought up to date) unless it has its own
    // or was restored; a restore asked for while it was open is finished. Whether anything changed; never throws.
    static bool prepareLocal(const QString &root,const QString &instance,const QString &clientJar);
    // The face of a skin with its hat layer, at the skin's own resolution (8×8 for a normal skin): the player's head.
    static QImage head(const QImage &skin);
    // The skin a Microsoft account wears now, from Mojang's profile service, saved to file. Returns "classic" or
    // "slim"; when the service cannot be reached the file keeps the last skin seen. Network: call off the UI thread.
    static QString fetchProfileSkin(const QString &uuid,const QString &file);
    // Profiles without Minecraft (no premium) wear one of the game's default skins, chosen from the UUID as Minecraft
    // does; read from the newest downloaded client in root/mc/versions. Null when no client is downloaded yet.
    static QImage defaultSkin(const QString &root,const QString &uuid,bool *slim=nullptr,QString *name=nullptr);
private:
    QString m_root;
};
