#ifndef MELODY_STREAM_HH
#define MELODY_STREAM_HH

#include <QAudioFormat>
#include <QBuffer>
#include <QPointer>
class Melody;

/** Represents a raw stream playing a melody. */
class MelodyStream : public QBuffer
{
Q_OBJECT

public:
  /** Default constructor. */
  explicit MelodyStream(QObject *parent = nullptr);
  /** Returns the audio format of the stream. */
  const QAudioFormat &audioFormat() const;
  /** Sets the melody. */
  void setMelody(Melody *melody);

protected slots:
  void reload();

protected:
  QPointer<Melody> _melody;
  QAudioFormat _format;
  QByteArray _raw;
};



#endif //QDMR_MELODY_STREAM_HH
