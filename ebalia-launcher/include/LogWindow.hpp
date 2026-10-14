#pragma once
#include <QDialog>
#include <QStringList>
#include <functional>
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QTimer;
// A game's log, live: the output the game writes while it runs appears as it comes (the view follows it unless you
// scrolled up), errors in red and warnings in yellow, with a filter. It does not block the launcher behind it.
class LogWindow:public QDialog {
public:
    // folder: an instance or lost version, whose launcher.log is the game's output (logs/latest.log when there is none).
    // running: whether its game is open now.
    LogWindow(const QString &folder,const QString &title,std::function<bool()> running,QWidget *parent);
private:
    void poll();
    void append(const QStringList &lines);
    void refilter();
    QString m_folder,m_file;std::function<bool()> m_running;
    qint64 m_position=-1;QByteArray m_partial;QStringList m_lines;
    QPlainTextEdit *m_view;QLabel *m_state;QLineEdit *m_filter;QTimer *m_timer;
};
