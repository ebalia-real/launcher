#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
class AccountManager;
class McInstanceManager;
class QLabel;
class QListWidget;
class SkinViewer;
class QPushButton;
class SkinsPage : public QWidget {
public:
    using Work=std::function<void(const QString &,std::function<QJsonObject()>,std::function<void(QJsonObject)>)>;
    SkinsPage(QString root,AccountManager *accounts,McInstanceManager *instances,Work work,QWidget *parent=nullptr);
private:
    void refresh();void showSkin();void showDefaultSkin(const QString &name,const QString &uuid);void importSkin();void apply(bool local);void removeSkin();
    QString m_root;AccountManager *m_accounts;McInstanceManager *m_instances;Work m_work;
    QListWidget *m_list;SkinViewer *m_preview;QLabel *m_details;QPushButton *m_remove=nullptr;QJsonArray m_skins;
};
