#ifndef MELODY_HH
#define MELODY_HH

#include "configobject.hh"
#include <QPair>

/** A config item that encodes a melody. This can be used to configure several melodies like
 * ring tones and boot-up melodies. Have fun with it. In contrast to the common manufacturer CPSs,
 * qdmr uses a proper musical notation for that.
 * @ingroup config */
class Melody : public ConfigItem
{
  Q_OBJECT

  /** The BPM of the melody. */
  Q_PROPERTY(unsigned int bpm READ bpm WRITE setBPM)
  /** The melody in LilyPond notation. */
  Q_PROPERTY(QString melody READ toLilypond WRITE fromLilypond)

public:
  /** Encodes a note, that is tone and duration. */
  struct Note {
    /** Possible tone values. */
    enum class Tone {
      Rest, C, Cis, D, Dis, E, F, Fis, G, Gis, A, Ais, B
    };
    /** Note durations as fractions of a bar. */
    enum class Duration {
      Whole, Half, Quater, Eighth, Sixteenth
    };

    /** The note tone. */
    Tone tone;
    /** The note duration. */
    Duration duration;
    /** If @c true, the note/rest is dottet. */
    bool dotted;
    /** The octave of the note, 0 means middle. */
    int octave;

    /** Default constructor. A middle C quater note. */
    Note();

    /** Reads a note in Lilypond notation. */
    bool fromLilypond(const QString &note, Duration currentDuration);

    /** Serializes the note in Lilypond notation. */
    QString toLilypond(Duration currentDuration) const;

    /** Converts the note to a frequency in Hz and duration in seconds. */
    QPair<double, unsigned int> toTone(unsigned int bpm) const;

    /** Infers the note from the given frequency and duration in ms.
     * This is guesswork, consequently there will be some issues. The function updates the note and
     * returns the timeing error in ms. */
    unsigned int infer(double frequency, unsigned int ms, unsigned int bpm);

    /** Computes the quantization timing error for the given duration and BPM. */
    static unsigned int quantizationTimingError(unsigned int ms, unsigned int bpm);
  };

  /** Iterator over notes. */
  typedef QVector<Note>::iterator iterator;
  /** Const iterator over notes. */
  typedef QVector<Note>::const_iterator const_iterator;

public:
  /** Empty constructor. */
  Melody(unsigned int bpm=100, QObject *parent = nullptr);

  ConfigItem *clone() const;
  bool copy(const ConfigItem &other);

  /** Returns an iterator pointing at the first note. */
  iterator begin();
  /** Returns an iterator pointing right after the last note. */
  iterator end();
  /** Returns a const-iterator pointing at the first note. */
  const_iterator begin() const;
  /** Returns a const-iterator pointing right after the last note. */
  const_iterator end() const;

  /** Returns the number of notes (and rests) of this melody. */
  size_t count() const;
  /** Element access. */
  const Note &operator [](size_t index) const;
  /** Element access. */
  Note &operator [](size_t index);

  /** Returns the BPM of the melody. */
  unsigned int bpm() const;
  /** Sets the BPM of the melody. */
  void setBPM(unsigned int bpm);

  /** Parses the Lilypond notation of the melody.
   * For example,
   * @code
   * a8 b e2 des4 d
   * @endcode
   */
  bool fromLilypond(const QString &melody);
  /** Serializes the melody into Lilypond notation. */
  QString toLilypond() const;

  /** Converts the melody to a series of tones in terms of frequency and duration in ms. */
  QVector<QPair<double, unsigned int>> toTones() const;
  /** Infer melody from a vector of frequeny-duration pairs. */
  bool infer(const QVector<QPair<double, unsigned int>> &tones);

protected:
  /** Computes the absolute quantization timing error over the given melody for the given BPM. */
  static unsigned int quantizationTimingError(
      const QVector<QPair<double, unsigned int>> &tones, unsigned int bpm);

protected:
  /** Holds the beats per minute. */
  unsigned int _bpm;
  /** The actual melody. */
  QVector<Note> _melody;
};

#endif // MELODY_HH
