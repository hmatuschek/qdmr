#include "melody_stream.hh"

#include "logger.hh"
#include "melody.hh"
#include <qendian.h>

MelodyStream::MelodyStream(QObject *parent)
  : QBuffer(parent), _melody(), _format(), _raw()
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
MelodyStream::setMelody(Melody *melody) {
  _raw.clear();
  if (!_melody.isNull())
    disconnect(_melody, &ConfigItem::modified, this, &MelodyStream::reload);
  _melody = melody;
  if (_melody.isNull())
    return;
  connect(_melody, &ConfigItem::modified, this, &MelodyStream::reload);
  reload();
}

void
MelodyStream::reload() {
  if (_melody.isNull() || _melody->isEmpty())
    return;
  double beat = _format.sampleRate() * _melody->bpm()/60.0;
  auto tones = _melody->toTones();

  double duration = beat / 16; // two 1/32 silence at beginning and end

  for (const auto &tone : tones)
    duration += (tone.second * _format.sampleRate()) / 1000.;
  unsigned int samples = duration;
  // pre-allocate data
  _raw.fill(0, samples*2);

  int head = beat/64, tail=beat/32, startSample = head;
  for (const auto &tone : tones) {
    int toneDuration = tone.second * _format.sampleRate() / 1000.0;
    // Handle silence
    if (0 == tone.first) {
      startSample += toneDuration;
      continue;
    }
    // Evaluate sin and envelope
    for (int i = -head; i < toneDuration+tail; i++) {
      double envelope = 1.;
      if (i <= 0)
        envelope = double(head+i)/head;
      else if (i >= toneDuration)
        envelope = 1 - double(i-toneDuration)/tail;
      int16_t value = (1<<12) * envelope * std::sin(2*M_PI * tone.first * i / _format.sampleRate());
      *reinterpret_cast<int16_t *>(_raw.data() + 2 * (startSample + i)) += value;
    }
    startSample += toneDuration;
  }
}