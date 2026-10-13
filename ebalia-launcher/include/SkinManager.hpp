#pragma once
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
class SkinManager {
public:
    explicit SkinManager(QString root):m_root(std::move(root)){}
    QJsonObject importSkin(const QString &file,const QString &name,const QString &variant);
    QJsonArray skins() const;
    static QImage preview(const QImage &skin,const QString &variant,bool back=false);
    static void upload(const QString &file,const QString &variant,const QString &token);
    static void applyLocal(const QString &file,const QString &variant,const QString &instance,const QString &clientJar);
    static void removeLocal(const QString &instance);
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
