#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <functional>
class QWidget;
class QDialog;
class UpdateChecker:public QObject {
 Q_OBJECT
public:
 explicit UpdateChecker(QObject *parent=nullptr);
 void check();
 QString releaseUrl() const{return m_releaseUrl;}
 QJsonObject release() const{return m_release;}
 // The prompt for a new version, with what changed since this one (notes from releaseNotes, Markdown).
 static QDialog *showUpdatePrompt(QWidget *parent,const QString &version, std::function<void()> install = {},const QString &notes = {},bool translated = false);
 // The sections of a release's notes (the whole CHANGELOG, "# x.y.z" per version) newer than current, up to target.
 static QString releaseNotes(const QJsonObject &release,const QString &current,const QString &target);
 static QString releaseNotes(const QString &markdown,const QString &current,const QString &target);
 // The notes of the release found, in the launcher's language when that version's translation covers every version
 // since this one (translated says which); otherwise the English notes of the release.
 QString notesFor(const QString &current,const QString &target,bool *translated = nullptr) const;
 static bool newerStable(QString tag,const QString &current);
 static QString releasePage(const QJsonObject &release,bool gitgud);
signals:
 void updateAvailable(const QString &version);
 void upToDate();
 void checkFailed(const QString &error);
private:
 void fetch(bool gitgud);
 void fetchTranslation(bool gitgud,const QString &tag);
 QNetworkAccessManager m_nam;
 QString m_releaseUrl;
 QJsonObject m_release;
 QString m_translated; // the new version's changelog in the launcher's language, from its repository
 bool m_busy=false;
};
