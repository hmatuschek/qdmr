#include "rd5r_test.hh"
#include "config.hh"
#include "rd5r.hh"
#include "rd5r_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

RD5RTest::RD5RTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
RD5RTest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
  if (! _channelFrequencyConfig.readYAML(":/data/channel_frequency_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RD5RTest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
  _channelFrequencyConfig.clear();
}

void
RD5RTest::testBasicConfigEncoding() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RD5RTest::testBasicConfigDecoding() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }
}

void
RD5RTest::testChannelFrequency() {
  ErrorStack err;
  RD5RCodeplug codeplug;
  codeplug.clear();
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity RD5R: {}")
          .arg(err.format()).toStdString().c_str());
  }

  /*QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           123456780ULL);
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           1234567890ULL);*/
}

QTEST_GUILESS_MAIN(RD5RTest)

