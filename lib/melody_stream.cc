#include "melody_stream.hh"

#include "logger.hh"
#include "melody.hh"
#include <qendian.h>

MelodyStream::MelodyStream(QObject *parent)
  : QBuffer(parent), _format(), _raw()
{
  _format.setSampleRate(44100.0);
  _format.setSampleFormat(QAudioFormat::Int16);
  _format.setChannelCount(1);
  setBuffer(&_raw);
}

const QAudioFormat&
MelodyStream::audioFormat() const {
  return _format;
}

void
MelodyStream::setMelody(const Melody *melody) {
  _raw.clear();
  if ((nullptr == melody) || melody->isEmpty())
    return;

  double beat = _format.sampleRate() * melody->bpm()/60.0;
  auto tones = melody->toTones();

  double duration = beat / 16; // two 1/32 silence at beginning and end

  for (const auto &tone : tones)
    duration += (tone.second * _format.sampleRate()) / 1000.;
  unsigned int samples = duration;
  // pre-allocate data
  _raw.fill(0, samples*2);

  int startSample = beat / 32;
  for (const auto &tone : tones) {
    unsigned int toneDuration = tone.second * _format.sampleRate() / 1000.0;
    // Handle silence
    if (0 == tone.first) {
      startSample += toneDuration;
      continue;
    }
    // Evaluate sin and envelope
    for (int i = -beat/32; i < toneDuration+beat/32; i++) {
      double envelope = 1./(1+std::exp(-128*i/beat))/(1+std::exp(-128*(duration-i)/beat));
      int16_t value = (1<<12) * envelope * std::sin(2*M_PI * tone.first * i / _format.sampleRate());
      *reinterpret_cast<int16_t *>(_raw.data() + 2 * (startSample + i)) += value;
    }
    startSample += toneDuration;
  }

}