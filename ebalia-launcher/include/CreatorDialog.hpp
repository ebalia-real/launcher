#pragma once
#include <QDialog>
#include <QJsonObject>
#include <QPixmap>
#include <functional>
// One of EBALIA's mods. background: cover art. text: the description's key, the same one the site uses (description is the
// launcher's own sentence, or the store's summary for a mod new to the catalog). page, the store pages, icon, downloads and
// whether it is a collaboration (credit: "collabWith" or "remakeBy", with the other authors) come from the live data
// (site-src/live.py) when it has them.
struct CreatorMod {
    QString name, slug, description; bool soon; QString background;
    QString page, icon; qint64 downloads = 0;
    QString text, curseforge, modrinth; bool collab = false; QString credit, creditNames;
    QString url() const;
    QString about() const; // the description in the launcher's language
    QString by() const;    // "With PKFL · TingKH", "Remake by FLYover67"; empty for EBALIA's own
};
// EBALIA's mods as last known, its own first: the bundled ones, or the live list once read.
QList<CreatorMod> creatorMods();
// Takes EBALIA's mods (its own and the collaborations) from the live data, kept to https addresses on the stores' own hosts.
// Whether anything changed.
bool setCreatorMods(const QJsonObject &live);
// Back to the bundled list (tests).
void resetCreatorMods();
// Where the live data is: refreshed every hour by .github/workflows/live-data.yml.
inline constexpr auto creatorModsUrl = "https://raw.githubusercontent.com/ebalia-real/launcher/live/mods.json";
// A mod's picture over darkened launcher art, 640 × 360, so it fills a cover like a news image (Home and the mods window).
QPixmap creatorCover(const QPixmap &art, const QPixmap &picture);

class CreatorDialog:public QDialog {
public:
    // picture loads a mod's picture from the store (the launcher's own image loader); without it the bundled ones show.
    using PictureLoader = std::function<void(const QString &url, std::function<void(const QPixmap &)> done)>;
    explicit CreatorDialog(bool patreonOnly,QWidget *parent=nullptr,PictureLoader picture={});
};
