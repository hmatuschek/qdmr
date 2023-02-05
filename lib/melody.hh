#ifndef MELODY_HH
#define MELODY_HH

#include "configobject.hh"

class Melody : public ConfigItem
{
  Q_OBJECT

public:
  struct Note {
    enum class Tone {
      Rest, C, D, E, F, G, A, B
    };

    enum class Duration {
      Whole, Half, Quater, Eighth, Sixteenth
    };

    enum class Variant {
      None, Flat, Sharp
    };

    Tone tone;
    Duration duration;
    Variant variant;
    int octave;

    Note();

    bool fromLilipond(const QString &note, Duration currentDuration);
    double toFrequency();
  };

public:
  explicit Melody(QObject *parent = nullptr);

  bool fromLilipond(const QString &melody);

protected:
  QVector<Note> _melody;
};

#endif // MELODY_HH
