#include "melody_player.hh"
#include "melody_stream.hh"
#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioSink>
#include <QAudioFormat>


MelodyPlayer::MelodyPlayer(QObject *parent)
: QObject(parent), _stream(new MelodyStream(this)), _playbackSink(nullptr)
{
  QAudioDevice info(QMediaDevices::defaultAudioOutput());
  if (! info.isFormatSupported(_stream->audioFormat()))
    return;
  _playbackSink = new QAudioSink(_stream->audioFormat(), this);
  connect(_playbackSink, &QAudioSink::stateChanged, [this](QAudio::State state) {
    emit stateChanged(state == QAudio::ActiveState);
  });
}

MelodyPlayer::~MelodyPlayer() {
  _playbackSink->stop();
  _stream->close();
}

void
MelodyPlayer::setMelody(Melody *melody) {
  if (_stream->isOpen())
    _stream->close();
  _stream->setMelody(melody);
  _stream->open(QIODevice::ReadOnly);
}

void
MelodyPlayer::togglePlay(bool enable) {
  if (nullptr == _playbackSink)
    return;

  if (enable && (QAudio::ActiveState != _playbackSink->state()))
      _playbackSink->start(_stream);
  else if (! enable && (QAudio::ActiveState == _playbackSink->state()))
    _playbackSink->stop();
}
