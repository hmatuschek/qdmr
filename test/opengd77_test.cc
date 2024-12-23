#include "opengd77_test.hh"
#include "config.hh"
#include "opengd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>
#include <iostream>
#include "logger.hh"


OpenGD77Test::OpenGD77Test(QObject *parent)
  : UnitTestBase(parent), _stderr(stderr)
{
  Logger::get().addHandler(new StreamLogHandler(_stderr, LogMessage::DEBUG));
}

void
OpenGD77Test::testBasicConfigEncoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for OpenGD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
OpenGD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
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
OpenGD77Test::testChannelFrequency() {
  ErrorStack err;
  OpenGD77Codeplug codeplug;
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


QTEST_GUILESS_MAIN(OpenGD77Test)

