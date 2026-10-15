#pragma once
#include <QDialog>
#include <QStringList>
#include <functional>
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QTimer;
// A game's log, live: the output the game writes while it runs appears as it comes (the view follows it unless you
// scrolled up), errors in red and warnings in yellow, with a filter. It reads the file only while it is on screen.
class LogView:public QWidget {
public:
    explicit LogView(QWidget *parent=nullptr);
    // folder: an instance or lost version, whose launcher.log is the game's output (logs/latest.log when there is none).
    // Another folder starts the view over.
    void setFolder(const QString &folder);
    QString folder() const {return m_folder;}
    // Whether its game is open now.
    std::function<bool()> running;
    // The row above the text, where the view's owner adds its own buttons.
    QHBoxLayout *bar() const {return m_bar;}
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
private:
    void poll();
    void append(const QStringList &lines);
    void refilter();
    QString m_folder,m_file;
    qint64 m_position=-1;QByteArray m_partial;QStringList m_lines;
    QPlainTextEdit *m_view;QLabel *m_state;QLineEdit *m_filter;QTimer *m_timer;QHBoxLayout *m_bar;
};
// The log in a window of its own, which does not block the launcher behind it.
class LogWindow:public QDialog {
public:
    LogWindow(const QString &folder,const QString &title,std::function<bool()> running,QWidget *parent);
};
