#include "InstanceContent.hpp"
#include "Archive.hpp"
#include "ModRepository.hpp"
#include <QCryptographicHash>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSet>
#include <QTemporaryDir>
#include <algorithm>
#include <optional>
#include <stdexcept>
namespace {
[[noreturn]] void fail(const QString &s){throw std::runtime_error(s.toStdString());}
// Names come from the folder listing, but a name with a path inside would reach outside the instance.
void checkName(const QString &file){if(file.isEmpty()||file=="."||file==".."||file.contains('/')||file.contains('\\'))fail("Invalid file name.");}
void save(const QString &path,const QByteArray &data){QDir().mkpath(QFileInfo(path).absolutePath());QSaveFile f(path);if(!f.open(QIODevice::WriteOnly)||f.write(data)!=data.size()||!f.commit())fail("Could not write "+path);}
QByteArray read(const QString &path){QFile f(path);if(!f.exists())return {};if(!f.open(QIODevice::ReadOnly))fail("Could not read "+path);return f.readAll();}
// Text of a pack description: a plain string with § colour codes, or a text component (object, list, "extra").
QString plain(const QJsonValue &value){
    if(value.isString())return value.toString();
    if(value.isArray()){QString out;for(const auto &part:value.toArray())out+=plain(part);return out;}
    if(value.isObject()){const auto o=value.toObject();auto out=o["text"].toString();if(out.isEmpty()&&o.contains("translate"))out=o["fallback"].toString(o["translate"].toString());return out+plain(o["extra"]);}
    return {};
}
// At most 256 pixels: pack pictures go from 16 to 1024, the window shows them at 48.
QImage picture(const QByteArray &data){QImage image;if(data.isEmpty()||!image.loadFromData(data))return {};return image.width()>256||image.height()>256?image.scaled(256,256,Qt::KeepAspectRatio,Qt::SmoothTransformation):image;}
// Iris and Oculus keep their choice in a Java properties file under config/.
QString shaderConfig(const QString &instance){
    for(const auto &jar:QDir(instance+"/mods").entryList({"*.jar"},QDir::Files))if(jar.contains("oculus",Qt::CaseInsensitive))return instance+"/config/oculus.properties";
    return instance+"/config/iris.properties";
}
QString unescape(const QString &s){
    QString out;for(qsizetype n=0;n<s.size();++n){
        if(s[n]!='\\'||n+1>=s.size()){out+=s[n];continue;}const auto c=s[++n];
        if(c=='u'&&n+4<s.size()){bool ok;const auto code=s.mid(n+1,4).toUShort(&ok,16);if(ok){out+=QChar(code);n+=4;continue;}}
        out+=c=='n'?QChar('\n'):c=='t'?QChar('\t'):c=='r'?QChar('\r'):c;
    }return out;
}
QString escape(const QString &s){
    QString out;for(qsizetype n=0;n<s.size();++n){const auto c=s[n];
        if(c=='\\'||c=='='||c==':'||c=='#'||c=='!'||(c==' '&&n==0)){out+='\\';out+=c;}
        else if(c.unicode()<0x20||c.unicode()>0x7e)out+=QString("\\u%1").arg(uint(c.unicode()),4,16,QChar(u'0'));else out+=c;
    }return out;
}
std::optional<QString> property(const QStringList &lines,const QString &key){
    static const QRegularExpression pair("^([A-Za-z0-9_.]+)\\s*[=:]\\s*(.*)$");
    for(const auto &line:lines){const auto m=pair.match(line.trimmed());if(m.hasMatch()&&m.captured(1)==key)return unescape(m.captured(2));}
    return std::nullopt;
}
void setProperty(QStringList &lines,const QString &key,const QString &value){
    static const QRegularExpression pair("^([A-Za-z0-9_.]+)\\s*[=:]");const auto line=key+"="+escape(value);
    for(auto &existing:lines){const auto m=pair.match(existing.trimmed());if(m.hasMatch()&&m.captured(1)==key){existing=line;return;}}
    lines.append(line);
}
}
QList<InstanceContent::Entry> InstanceContent::list(const QString &instance,const QString &folder){
    QList<Entry> out;const QDir dir(instance+"/"+folder);if(!dir.exists())return out;
    QSet<QString> on;QString shaderOn;
    if(folder=="resourcepacks")for(const auto &line:read(instance+"/options.txt").split('\n'))if(line.startsWith("resourcePacks:"))for(const auto &v:QJsonDocument::fromJson(line.mid(14).trimmed()).array())on<<v.toString();
    if(folder=="shaderpacks")shaderOn=shader(instance);
    for(const auto &info:dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name|QDir::IgnoreCase)){
        Entry e;e.file=info.fileName();e.directory=info.isDir();e.modified=info.lastModified();e.size=e.directory?0:info.size();
        if(folder=="saves"){if(!e.directory||!QFile::exists(info.filePath()+"/level.dat"))continue;e.title=e.file;e.modified=QFileInfo(info.filePath()+"/level.dat").lastModified();out<<e;continue;}
        // Iris keeps each pack's settings next to it as "<pack>.txt"; those and other loose files are not packs.
        if(!e.directory&&!e.file.endsWith(".zip",Qt::CaseInsensitive))continue;
        if(folder=="resourcepacks"){
            // The local skin pack belongs to the Skins page.
            if(e.file=="ebalia-local-skin.zip"||(e.directory&&!QFile::exists(info.filePath()+"/pack.mcmeta")))continue;
            e.active=on.contains("file/"+e.file);
        }else e.active=!shaderOn.isEmpty()&&shaderOn==e.file;
        e.title=e.directory?e.file:e.file.chopped(4);out<<e;
    }
    // Worlds: the last one played first, as in the game.
    if(folder=="saves")std::stable_sort(out.begin(),out.end(),[](const Entry &a,const Entry &b){return a.modified>b.modified;});
    return out;
}
QImage InstanceContent::modrinthIcon(const QString &file,const QString &cache){
    QImage image;if(cache.isEmpty()||qEnvironmentVariableIsSet("EBALIA_NO_NETWORK"))return image;
    QFile in(file);QCryptographicHash sha(QCryptographicHash::Sha1);if(!in.open(QIODevice::ReadOnly)||!sha.addData(&in))return image;
    const auto hash=QString::fromLatin1(sha.result().toHex()),cached=cache+"/"+hash+".png";
    if(QFile::exists(cached)){image.load(cached);return image;}if(QFile::exists(cached+".none"))return image;
    QDir().mkpath(cache);auto none=[&]{QFile mark(cached+".none");mark.open(QIODevice::WriteOnly);};
    try{
        const auto version=QJsonDocument::fromJson(ModRepository::fetch(QUrl("https://api.modrinth.com/v2/version_file/"+hash+"?algorithm=sha1"))).object();
        const auto project=version["project_id"].toString();if(!QRegularExpression("^[A-Za-z0-9]+$").match(project).hasMatch()){none();return {};}
        const QUrl icon(QJsonDocument::fromJson(ModRepository::fetch(QUrl("https://api.modrinth.com/v2/project/"+project))).object()["icon_url"].toString());
        if(icon.scheme()=="https"&&image.loadFromData(ModRepository::fetch(icon))){image=image.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);image.save(cached,"PNG");return image;}
        none(); // on Modrinth, without an icon
    }catch(const std::exception &e){
        // Not on Modrinth is remembered; a failed connection is not, so the next look tries again.
        if(QString::fromUtf8(e.what()).contains("Not Found",Qt::CaseInsensitive))none();
    }
    return {};
}
InstanceContent::Entry InstanceContent::describe(const QString &instance,const QString &folder,Entry entry,const QString &iconCache){
    const auto path=instance+"/"+folder+"/"+entry.file;
    try{
        if(folder=="saves"){entry.icon=picture(read(path+"/icon.png"));return entry;}
        if(folder=="shaderpacks"){if(!entry.directory)entry.icon=modrinthIcon(path,iconCache);return entry;}
        if(folder!="resourcepacks")return entry;
        const auto meta=entry.directory?read(path+"/pack.mcmeta"):Archive::readEntry(path,"pack.mcmeta",1<<20);
        entry.icon=picture(entry.directory?read(path+"/pack.png"):Archive::readEntry(path,"pack.png",4<<20));
        auto text=meta;if(text.startsWith("\xEF\xBB\xBF"))text=text.mid(3);
        static const QRegularExpression codes("§.");
        entry.description=plain(QJsonDocument::fromJson(text).object()["pack"].toObject()["description"]).remove(codes).simplified();
        if(entry.icon.isNull()&&!entry.directory)entry.icon=modrinthIcon(path,iconCache);
    }catch(...){}
    return entry;
}
void InstanceContent::setResourcePack(const QString &instance,const QString &file,bool on){
    checkName(file);const auto text=read(instance+"/options.txt");auto lines=text.isEmpty()?QList<QByteArray>{}:text.split('\n');const auto id="file/"+file;
    auto edit=[&](const QByteArray &key,bool add){
        QJsonArray packs;int at=-1;bool cr=false;
        for(int n=0;n<lines.size();++n)if(lines[n].startsWith(key+":")){at=n;cr=lines[n].endsWith('\r');packs=QJsonDocument::fromJson(lines[n].mid(key.size()+1).trimmed()).array();}
        if(at<0&&!add)return;
        for(auto n=packs.size()-1;n>=0;--n)if(packs[n].toString()==id)packs.removeAt(n);
        if(add){if(packs.isEmpty())packs.append("vanilla");packs.append(id);}
        auto line=key+":"+QJsonDocument(packs).toJson(QJsonDocument::Compact)+(cr?"\r":"");if(at<0)lines.append(line);else lines[at]=line;
    };
    edit("resourcePacks",on);edit("incompatibleResourcePacks",false);
    save(instance+"/options.txt",lines.join('\n'));
}
QString InstanceContent::shader(const QString &instance){
    const auto lines=QString::fromLatin1(read(shaderConfig(instance))).split('\n');
    if(property(lines,"enableShaders").value_or("true").trimmed()=="false")return {};
    return property(lines,"shaderPack").value_or(QString());
}
void InstanceContent::setShader(const QString &instance,const QString &file){
    if(!file.isEmpty())checkName(file);const auto path=shaderConfig(instance);const auto text=QString::fromLatin1(read(path));
    auto lines=text.isEmpty()?QStringList{}:text.split('\n');for(auto &line:lines)if(line.endsWith('\r'))line.chop(1);
    if(!lines.isEmpty()&&lines.last().isEmpty())lines.removeLast();
    setProperty(lines,"enableShaders",file.isEmpty()?"false":"true");if(!file.isEmpty())setProperty(lines,"shaderPack",file);
    save(path,(lines.join('\n')+'\n').toLatin1());
}
QString InstanceContent::add(const QString &instance,const QString &folder,const QString &source){
    const QFileInfo from(source);if(!from.isFile())fail("The file does not exist.");
    if(folder!="resourcepacks"&&folder!="shaderpacks"&&folder!="saves")fail("Invalid file name.");
    const QDir target(instance+"/"+folder);QDir().mkpath(target.path());
    if(!from.fileName().endsWith(".zip",Qt::CaseInsensitive))fail("Choose a ZIP file.");
    if(folder!="saves"){
        if(folder=="resourcepacks"&&Archive::readEntry(source,"pack.mcmeta",1<<20).isEmpty())fail("This ZIP is not a resource pack: it has no pack.mcmeta at the top.");
        const auto name=from.fileName();if(target.exists(name))fail("A file with that name is already there.");
        if(!QFile::copy(source,target.filePath(name)))fail("Could not copy the file.");
        return name;
    }
    // Worlds: unpack inside the instance first, then move the folder holding level.dat into saves.
    QTemporaryDir stage(instance+"/.import-XXXXXX");if(!stage.isValid())fail("Could not copy the file.");
    Archive::extract(source,stage.path());
    QString world;if(QFile::exists(stage.path()+"/level.dat"))world=stage.path();
    else for(const auto &d:QDir(stage.path()).entryList(QDir::Dirs|QDir::NoDotAndDotDot))if(QFile::exists(stage.path()+"/"+d+"/level.dat")){world=stage.path()+"/"+d;break;}
    if(world.isEmpty())fail("This ZIP has no Minecraft world (level.dat).");
    const auto base=world==stage.path()?from.completeBaseName():QFileInfo(world).fileName();auto name=base;
    for(int n=2;target.exists(name);++n)name=QString("%1 (%2)").arg(base).arg(n);
    if(!QDir().rename(world,target.filePath(name)))fail("Could not copy the file.");
    return name;
}
void InstanceContent::remove(const QString &instance,const QString &folder,const QString &file){
    checkName(file);const auto from=instance+"/"+folder+"/"+file;if(!QFileInfo::exists(from))fail("The file does not exist.");
    if(folder=="resourcepacks")setResourcePack(instance,file,false);
    if(folder=="shaderpacks"&&shader(instance)==file)setShader(instance,{});
    const auto bin=instance+"/removed-"+(folder=="saves"?QString("worlds"):folder);QDir().mkpath(bin);
    if(!QDir().rename(from,bin+"/"+QString::number(QDateTime::currentMSecsSinceEpoch())+"-"+file))fail("Could not move the file.");
}
void InstanceContent::exportWorld(const QString &instance,const QString &world,const QString &zip){
    checkName(world);const QDir saves(instance+"/saves");if(!QFile::exists(saves.filePath(world+"/level.dat")))fail("The file does not exist.");
    // Every other world stays out, so the ZIP holds "<world>/level.dat"; session.lock is the game's own lock.
    auto skip=saves.entryList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);skip.removeAll(world);
    Archive::compress(saves.path(),zip,skip);
}
