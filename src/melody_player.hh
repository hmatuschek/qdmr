#ifndef QDMR_MELODY_PLAYER_HH
#define QDMR_MELODY_PLAYER_HH

#include <QObject>


class Melody;
class MelodyStream;
class QAudioSink;

/** Plays some melody. */
class MelodyPlayer: public QObject
{
  Q_OBJECT

public:
  explicit MelodyPlayer(QObject *parent = nullptr);
  virtual ~MelodyPlayer();

public slots:
  void setMelody(Melody *melody);
  void togglePlay(bool play);

signals:
  void stateChanged(bool playing);

protected:
  MelodyStream *_stream;
  QAudioSink *_playbackSink;
};

#endif //QDMR_MELODY_PLAYER_HH
