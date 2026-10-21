#pragma once
#include <QSet>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <functional>

// All network operations run in the caller's worker thread; no GUI objects here.
class ModRepository {
public:
    using Transport = std::function<QByteArray(const QUrl &)>;
    explicit ModRepository(QString root, Transport transport = {});
    // kind: "mod", "resourcepack" or "shader". Mods need the instance's loader; packs only its game version.
    QJsonArray search(const QString &query, const QString &game, const QString &loader, int offset = 0, const QString &provider = "modrinth",
                      const QString &kind = "mod");
    QJsonObject details(const QString &projectId);
    // A resource pack or shader pack for this game version, verified and saved in the instance's resourcepacks/ or
    // shaderpacks/ folder. Returns {"filename", "title"}.
    QJsonObject installPack(const QString &instance, const QString &projectId, const QString &game, const QString &kind);
    QJsonObject plan(const QJsonArray &projects, const QString &game, const QString &loader, const QJsonArray &installed = {});
    void apply(const QString &instance, const QJsonObject &plan);
    QJsonObject capture(const QString &instance, const QString &name);
    QJsonArray packs() const;
    QString savePack(const QJsonObject &pack);
    static QJsonObject read(const QString &path);
    static void write(const QString &path, const QJsonObject &object);
    static bool safeName(const QString &name);
    // The Discover projects an instance already has, by project id: mods installed by the launcher (mods.json, also
    // when turned off) and resource or shader packs (packs.json), while their file is still in its folder.
    static QSet<QString> installedProjects(const QString &instance);
    // attempts and timeout: lookups nobody waits on (icons) give up sooner, so they never hold the launcher when it closes.
    static QByteArray fetch(const QUrl &url, const QMap<QByteArray,QByteArray> &headers = {}, int attempts = 3, int timeout = 70000);
    // The launcher is closing: every fetch running on any thread stops now and new ones fail, so closing never waits on
    // a slow server.
    static void cancelAll();
    // EBALIA_CURSEFORGE_API_KEY, then Provider settings, then the key built into this copy (empty when none).
    static QString curseForgeKey();
private:
    QByteArray get(const QUrl &url);
    QJsonDocument api(const QString &path, const QList<QPair<QString, QString>> &query = {});
    QJsonDocument curse(const QString &path, const QList<QPair<QString,QString>> &query = {});
    QJsonObject curseVersion(const QJsonObject &file, const QString &game, const QString &loader);
    void resolve(const QString &project, const QString &version, const QString &game,
                 const QString &loader, QMap<QString, QJsonObject> &selected, QSet<QString> &visiting);
    QString m_root;
    Transport m_transport;
};
