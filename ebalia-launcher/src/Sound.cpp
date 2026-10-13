#include "Sound.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QHash>
#include <QSettings>
#include <QtEndian>
#include <cmath>
#if defined(Q_OS_WIN)
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#elif defined(Q_OS_MACOS)
#include <AudioToolbox/AudioToolbox.h>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStandardPaths>
#else
#include <QLibrary>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#endif
namespace {
constexpr int rate=22050;
// One note: a frequency, its length in seconds and how fast it fades.
struct Note {double frequency,length,decay;};
QByteArray render(const QList<Note> &notes,double volume,bool voice){
    QByteArray out;
    for(const auto &note:notes){
        const int count=int(note.length*rate);const qsizetype start=out.size();out.resize(start+count*2);auto samples=reinterpret_cast<qint16*>(out.data()+start);
        for(int i=0;i<count;++i){
            const double t=double(i)/rate,x=2*M_PI*note.frequency*t;
            // The voice is a soft square (odd harmonics), the chimes a bell (a sine with a little octave).
            const double wave=voice?(std::sin(x)+std::sin(3*x)/3+std::sin(5*x)/5)*0.8:std::sin(x)+0.3*std::sin(2*x);
            const double attack=std::min(1.0,t/0.003),release=std::min(1.0,(note.length-t)/0.006);
            samples[i]=qint16(qBound(-1.0,wave*volume*attack*release*std::exp(-t/note.decay),1.0)*32767);
        }
    }
    return out;
}
int key(Sound::Effect effect,int variant){return int(effect)*16+(effect==Sound::Blip?variant&3:0);}
bool silent(){
    // Tests run offscreen; EBALIA_NO_SOUND turns sounds off whatever the setting says.
    return !Sound::enabled()||qEnvironmentVariableIsSet("EBALIA_NO_SOUND")||QGuiApplication::platformName()=="offscreen"||QGuiApplication::platformName()=="minimal";
}
#if !defined(Q_OS_WIN)&&!defined(Q_OS_MACOS)
// PulseAudio's simple API, loaded when the sound is first played: no build dependency, and silence where it is missing.
struct SampleSpec {int format;quint32 rate;quint8 channels;};
struct BufferAttr {quint32 maxlength,tlength,prebuf,minreq,fragsize;};
using NewStream=void*(*)(const char*,const char*,int,const char*,const char*,const SampleSpec*,const void*,const BufferAttr*,int*);
using Write=int(*)(void*,const void*,size_t,int*);
using Drain=int(*)(void*,int*);
using Free=void(*)(void*);
// One connection, fed by a thread: blips follow each other without a new connection each time, and it closes after
// a few quiet seconds so no idle stream stays in the system's mixer.
class Player {
public:
    static Player &instance(){static auto *player=new Player;return *player;}
    void play(const QByteArray &samples){
        if(!m_ready)return;
        {std::lock_guard lock(m_mutex);if(m_queue.size()>2)m_queue.pop_front();m_queue.push_back(samples);}
        m_wake.notify_one();
    }
private:
    Player(){
        QLibrary library("pulse-simple",0);if(!library.load())return;
        m_new=reinterpret_cast<NewStream>(library.resolve("pa_simple_new"));m_write=reinterpret_cast<Write>(library.resolve("pa_simple_write"));
        m_drain=reinterpret_cast<Drain>(library.resolve("pa_simple_drain"));m_free=reinterpret_cast<Free>(library.resolve("pa_simple_free"));
        if(!m_new||!m_write||!m_drain||!m_free)return;
        m_ready=true;m_thread=std::thread([this]{run();});
        qAddPostRoutine([]{auto &p=instance();{std::lock_guard lock(p.m_mutex);p.m_stop=true;}p.m_wake.notify_one();if(p.m_thread.joinable())p.m_thread.join();});
    }
    void run(){
        void *stream=nullptr;
        for(;;){
            QByteArray samples;
            {std::unique_lock lock(m_mutex);
             if(!m_wake.wait_for(lock,std::chrono::seconds(4),[this]{return m_stop||!m_queue.empty();})){if(stream){m_drain(stream,nullptr);m_free(stream);stream=nullptr;}continue;}
             if(m_stop)break;samples=m_queue.front();m_queue.pop_front();}
            if(!stream){
                // A short buffer (40 ms) so a blip plays as its letter appears.
                const SampleSpec spec{3,rate,1};const BufferAttr attr{quint32(-1),quint32(rate*2*40/1000),quint32(-1),quint32(-1),quint32(-1)};int error=0;
                stream=m_new(nullptr,"EBALIA",1,nullptr,"Silence",&spec,nullptr,&attr,&error);if(!stream)continue;
            }
            if(m_write(stream,samples.constData(),size_t(samples.size()),nullptr)<0){m_free(stream);stream=nullptr;}
        }
        if(stream){m_drain(stream,nullptr);m_free(stream);}
    }
    NewStream m_new=nullptr;Write m_write=nullptr;Drain m_drain=nullptr;Free m_free=nullptr;bool m_ready=false,m_stop=false;
    std::thread m_thread;std::mutex m_mutex;std::condition_variable m_wake;std::deque<QByteArray> m_queue;
};
#endif
}
QByteArray Sound::pcm(Effect effect,int variant){
    switch(effect){
    case Blip:{static const double pitches[]{294,330,370,392};return render({{pitches[variant&3],0.045,0.02}},0.22,true);}
    case Next:return render({{784,0.06,0.05},{1175,0.11,0.06}},0.16,false);
    case Back:return render({{1175,0.06,0.05},{784,0.11,0.06}},0.16,false);
    case Done:return render({{523,0.07,0.06},{659,0.07,0.06},{784,0.07,0.06},{1047,0.22,0.09}},0.16,false);
    }
    return {};
}
QByteArray Sound::wav(const QByteArray &samples){
    QByteArray out(44,0);auto put32=[&](int at,quint32 v){qToLittleEndian(v,out.data()+at);};auto put16=[&](int at,quint16 v){qToLittleEndian(v,out.data()+at);};
    out.replace(0,4,"RIFF");put32(4,quint32(36+samples.size()));out.replace(8,8,"WAVEfmt ");put32(16,16);put16(20,1);put16(22,1);
    put32(24,rate);put32(28,rate*2);put16(32,2);put16(34,16);out.replace(36,4,"data");put32(40,quint32(samples.size()));
    return out+samples;
}
bool Sound::enabled(){return QSettings().value("ui/sounds",true).toBool();}
void Sound::setEnabled(bool on){QSettings().setValue("ui/sounds",on);}
void Sound::play(Effect effect,int variant){
    if(silent())return;
#if defined(Q_OS_WIN)
    // PlaySound reads the WAV from memory while it plays, so each one is kept; a new sound replaces the one playing.
    static QHash<int,QByteArray> sounds;auto &data=sounds[key(effect,variant)];if(data.isEmpty())data=wav(pcm(effect,variant));
    PlaySoundW(reinterpret_cast<LPCWSTR>(data.constData()),nullptr,SND_MEMORY|SND_ASYNC|SND_NODEFAULT);
#elif defined(Q_OS_MACOS)
    // System Sound Services plays short files: each sound is written once to the cache folder.
    static QHash<int,SystemSoundID> sounds;const int id=key(effect,variant);
    if(!sounds.contains(id)){
        const auto folder=QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/sounds";QDir().mkpath(folder);
        const auto path=QFile::encodeName(folder+"/"+QString::number(id)+".wav");QSaveFile file(QFile::decodeName(path));SystemSoundID sound=0;
        if(file.open(QIODevice::WriteOnly)&&file.write(wav(pcm(effect,variant)))>0&&file.commit()){
            if(auto url=CFURLCreateFromFileSystemRepresentation(nullptr,reinterpret_cast<const UInt8*>(path.constData()),path.size(),false)){
                if(AudioServicesCreateSystemSoundID(url,&sound)!=noErr)sound=0;CFRelease(url);}}
        sounds.insert(id,sound);
    }
    if(const auto sound=sounds.value(id))AudioServicesPlaySystemSound(sound);
#else
    static QHash<int,QByteArray> sounds;auto &data=sounds[key(effect,variant)];if(data.isEmpty())data=pcm(effect,variant);
    Player::instance().play(data);
#endif
}
