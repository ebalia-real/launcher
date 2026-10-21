#pragma once
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QWidget>
#include <functional>
class QBoxLayout;
class QDialog;
class QWidget;
// Shared look of the launcher: Lucide icons (resources/icons/ui, ISC license), the dark theme and window helpers.
#include <QColor>
#include <QList>
class QThreadPool;
class QLineEdit;
class QGridLayout;
namespace Ui {
// Cards in as many columns as fit (up to three), each row as tall as its longest card.
class CardGrid:public QWidget {
public:
    explicit CardGrid(int cardWidth,QWidget *parent=nullptr);
    void add(QWidget *card);
    int count() const {return int(m_cards.size());}
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    void arrange(int width);
    int m_cardWidth,m_columns=0;QGridLayout *m_grid;QList<QWidget*> m_cards;
};
// A row that becomes a column before its controls need to squeeze together.
class ResponsiveRow:public QWidget {
public:
    explicit ResponsiveRow(int breakpoint=740,QWidget *parent=nullptr);
    QBoxLayout *box() const {return m_box;}
    QSize minimumSizeHint() const override;
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    QBoxLayout *m_box;int m_breakpoint;
};
QPixmap pixmap(const QString &name,int size,const QColor &color=QColor(232,232,236));
QIcon icon(const QString &name,const QColor &color=QColor(232,232,236));
QIcon navigationIcon(const QString &name);
QString styleSheet();
// Colour themes: the same layout with another accent and tinted dark backgrounds (setting "ui/theme").
struct Theme { QString id, name; int accentHue; int backgroundHue; double backgroundSaturation; };
QList<Theme> themes();
QString theme();
void setTheme(const QString &id);
// Theme "custom": colours chosen by the player.
QColor customAccent();
QColor customBackground();
void setCustomTheme(const QColor &accent,const QColor &background);
// A colour of the EBALIA (green) theme in the current theme.
QColor themed(const QColor &color);
QColor accent();
// The magnifier at the start of a search field. Qt places the icon a fixed distance from the border; the field's
// left padding then only separates it from the text, so search fields get a smaller one and the icon sits centred.
void addSearchIcon(QLineEdit *field,const QColor &color);
// Landscape art cropped to size, with rounded corners when radius > 0.
QPixmap art(const QString &resource,QSize size,int radius=0);
// Decode and resize artwork on a bounded worker pool. Callbacks run on the UI thread
// only while the receiver is alive; identical requests share one job and a memory cache.
void loadArt(const QString &resource,QSize bounds,QObject *receiver,std::function<void(const QPixmap &)> ready);
QPixmap cachedArt(const QString &resource,QSize bounds);
// The bounds to load artwork with for an area of this many pixels (logical size × device pixel ratio): one of a few
// sizes, so areas of similar size share a picture in the cache and none is decoded much larger than it is shown.
QSize artBounds(QSizeF pixels);
// Drops the decoded artwork kept for later and gives free memory back to the system (the window was minimized).
void releaseMemory();
// A few threads for work nobody waits on (icons, pack descriptions, lookups on Modrinth, skin heads): it can be slow
// or wait on the network without delaying anything the person asked for, which runs on Qt's own pool.
QThreadPool *backgroundPool();
// Any picture cropped to fill size (centered horizontally, top kept), with rounded corners when radius > 0.
QPixmap cover(const QPixmap &source,QSize size,int radius=0);
// Round avatar with the player's initial; an empty name gives the generic user icon.
QPixmap avatar(const QString &name,int size);
QString artFor(const QString &key);
QString lostArtFor(const QString &id,const QString &category);
// Opens a dialog as an ordinary window and waits for it. Window managers such as GNOME attach modal
// dialogs to their parent (both move together and the dialog cannot be resized), so the dialog is not
// window-modal; the window behind it is disabled instead.
int openWindow(QDialog &dialog);
int openWindows();
void fitToScreen(QWidget *window,QSize preferred);
}
