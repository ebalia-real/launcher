#pragma once
#include <QByteArray>
// Silence's little sounds in the guide. They are made here (no sound files) and played without Qt Multimedia:
// PlaySound on Windows, System Sound Services on macOS, PulseAudio (or PipeWire's) on Linux when it is there.
namespace Sound {
enum Effect{Blip,Next,Back,Done};
// Plays without waiting. Blips come in a few pitches (variant), like a voice. Silent when sounds are off, in tests
// (offscreen) or when the system has no way to play them.
void play(Effect effect,int variant=0);
bool enabled();
void setEnabled(bool on);
// The samples of an effect: mono, 16-bit, 22050 Hz.
QByteArray pcm(Effect effect,int variant=0);
// The same samples as a WAV file.
QByteArray wav(const QByteArray &pcm);
}
