#include "openuv380_test.hh"
#include "config.hh"
#include "openuv380_codeplug.hh"
#include "errorstack.hh"
#include <QTest>
#include "logger.hh"


OpenUV380Test::OpenUV380Test(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
OpenUV380Test::testBasicConfigEncoding() {
  ErrorStack err;
  OpenUV380Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenUV380Test::testBasicConfigDecoding() {
  ErrorStack err;
  OpenUV380Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenUV380Test::testChannelFrequency() {
  ErrorStack err;
  OpenUV380Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}



QTEST_GUILESS_MAIN(OpenUV380Test)

