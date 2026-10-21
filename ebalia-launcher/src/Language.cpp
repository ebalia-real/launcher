#include "Language.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QRegularExpression>
#include <algorithm>
QString Language::current="es";
namespace {
const QMap<QString,QJsonObject> &catalogs(){
    static const auto all=[] {QMap<QString,QJsonObject> out;for(const auto &code:Language::available()){QFile f(":/locales/"+code+".json");if(f.open(QIODevice::ReadOnly))out[code]=QJsonDocument::fromJson(f.readAll()).object();}return out;}();return all;
}
}
QStringList Language::available(){return {"es","en","pt","de","fr","it","ru","ja","ko","zh"};}
QString Language::text(const char *es,const char *en,const char *pt){
    auto translated=catalogs().value(current).value(QString::fromUtf8(en)).toString();
    if(!translated.isEmpty())return translated;
    return QString::fromUtf8(current=="es"?es:current=="pt"?pt:en);
}
QString Language::message(QString value){
    const auto es=catalogs().value("es"),en=catalogs().value("en");
    for(auto it=es.begin();it!=es.end();++it)if(it.value().toString()==value)return catalogs().value(current).value(it.key()).toString(value);
    for(auto it=en.begin();it!=en.end();++it)if(it.key()==value)return catalogs().value(current).value(it.key()).toString(value);
    // Messages with details inside ("Recursos 3/10", "Could not write C:/…"): an entry whose %1, %2… stand for the
    // details, or whose text ends with a space and is followed by one (usually a file name or a host).
    struct Pattern {QRegularExpression re;QString key;QList<int> order;int literal;};
    static const auto patterns=[] {
        QList<Pattern> out;const auto es=catalogs().value("es"),en=catalogs().value("en");
        for(auto it=en.begin();it!=en.end();++it)for(const auto &source:{it.key(),es.value(it.key()).toString()}) {
            static const QRegularExpression slot("%([1-9])");QString re="^";QList<int> order;qsizetype from=0;
            for(auto m=slot.globalMatch(source);m.hasNext();) {auto s=m.next();re+=QRegularExpression::escape(source.mid(from,s.capturedStart()-from))+"(.*?)";order<<s.captured(1).toInt();from=s.capturedEnd();}
            const int literal=int(source.size()-2*order.size());
            if(order.isEmpty()&&source.endsWith(' ')&&source.trimmed().size()>=5){re+=QRegularExpression::escape(source)+"(.+)";order<<0;}
            else if(order.isEmpty()||literal<5)continue;
            else re+=QRegularExpression::escape(source.mid(from));
            out.push_back({QRegularExpression(re+"$",QRegularExpression::DotMatchesEverythingOption),it.key(),order,literal});
        }
        std::stable_sort(out.begin(),out.end(),[](const Pattern &a,const Pattern &b){return a.literal>b.literal;});return out;
    }();
    for(const auto &p:patterns) {
        const auto m=p.re.match(value);if(!m.hasMatch())continue;
        const auto translated=catalogs().value(current).value(p.key).toString();if(translated.isEmpty())break;
        if(p.order==QList<int>{0})return translated+m.captured(1);
        QString out;static const QRegularExpression slot("%([1-9])");qsizetype from=0;
        for(auto s=slot.globalMatch(translated);s.hasNext();) {auto t=s.next();const int at=int(p.order.indexOf(t.captured(1).toInt()));out+=translated.mid(from,t.capturedStart()-from)+(at<0?t.captured():m.captured(at+1));from=t.capturedEnd();}
        return out+translated.mid(from);
    }
    return value; // Remote API errors and game logs retain their original diagnostic text.
}
QString Language::standard(const QString &source){
    static const QMap<QString,QStringList> labels{
        {"OK",{"Aceptar","OK","OK","OK","OK","OK","ОК","OK","확인","确定"}},
        {"Cancel",{"Cancelar","Cancel","Cancelar","Abbrechen","Annuler","Annulla","Отмена","キャンセル","취소","取消"}},
        {"Save",{"Guardar","Save","Salvar","Speichern","Enregistrer","Salva","Сохранить","保存","저장","保存"}},
        {"Yes",{"Sí","Yes","Sim","Ja","Oui","Sì","Да","はい","예","是"}},
        {"No",{"No","No","Não","Nein","Non","No","Нет","いいえ","아니요","否"}},
        {"Close",{"Cerrar","Close","Fechar","Schließen","Fermer","Chiudi","Закрыть","閉じる","닫기","关闭"}},
        {"Open",{"Abrir","Open","Abrir","Öffnen","Ouvrir","Apri","Открыть","開く","열기","打开"}}};
    QString clean=source;clean.remove('&');auto it=labels.find(clean);if(it==labels.end())return {};
    int index=available().indexOf(current);return it.value().value(qMax(0,index));
}

QString Language::key(const QString &source){auto button=standard(source);if(!button.isEmpty())return button;return catalogs().value(current).value(source).toString(source);}
