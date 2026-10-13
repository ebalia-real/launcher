#pragma once
#include <QList>
#include <QWidget>
class QLabel;
class QMainWindow;
class QToolButton;
// The window's own title bar, for desktops that leave the title bar to Qt (GNOME and others on Wayland without
// server-side decorations), where Qt's lacks the double-click to maximize. Windows, macOS, X11 and compositors that
// draw their own title bars (KDE, Sway…) keep the system's, which already does it.
class TitleBar:public QWidget {
public:
    // Puts the bar on window when this desktop needs it; EBALIA_TITLE_BAR=system or custom forces either one.
    static bool install(QMainWindow *window);
    explicit TitleBar(QMainWindow *window);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;
private:
    void toggleMaximized();
    void sync();
    QMainWindow *m_window;QLabel *m_title;QToolButton *m_maximize;QList<QWidget*> m_grips;QPoint m_press;bool m_pressed=false;
};
