#include "gd73_test.hh"
#include "config.hh"
#include "gd73.hh"
#include "gd73_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

GD73Test::GD73Test(QObject *parent)
  : UnitTestBase(parent)
{
  // pass...
}

void
GD73Test::testBasicConfigEncoding() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD73Test::testBasicConfigDecoding() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_basicConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
GD73Test::testChannelFrequency() {
  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&_channelFrequencyConfig, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD77: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}


void
GD73Test::testFMSignaling() {

  Config config;
  config.radioIDs()->add(new DMRRadioID("ID", 1234567));

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 1");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::CTCSS_67_0Hz);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 2");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::CTCSS_67_0Hz);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 3");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::DCS_023N);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 4");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::DCS_023N);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 5");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::DCS_023I);
    ch->setTXTone(Signaling::SIGNALING_NONE);
    config.channelList()->add(ch);
  }

  {
    FMChannel *ch = new FMChannel();
    ch->setName("Channel 6");
    ch->setRXFrequency(Frequency::fromMHz(144.0));
    ch->setTXFrequency(Frequency::fromMHz(144.6));
    ch->setRXTone(Signaling::SIGNALING_NONE);
    ch->setTXTone(Signaling::DCS_023I);
    config.channelList()->add(ch);
  }

  ErrorStack err;
  GD73Codeplug codeplug;
  if (! codeplug.encode(&config, Codeplug::Flags(), err)) {
    QFAIL(QString("Cannot encode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  Config compare;
  if (! codeplug.decode(&compare, err)) {
    QFAIL(QString("Cannot decode codeplug for Radioddity GD73: {}")
          .arg(err.format()).toStdString().c_str());
  }

  QCOMPARE(compare.channelList()->count(), 6);
  for (int i=0; i<compare.channelList()->count(); i++) {
    QVERIFY(compare.channelList()->channel(i)->is<FMChannel>());
    QCOMPARE((unsigned int)compare.channelList()->channel(i)->as<FMChannel>()->rxTone(),
             (unsigned int)config.channelList()->channel(i)->as<FMChannel>()->rxTone());
    QCOMPARE((unsigned int)compare.channelList()->channel(i)->as<FMChannel>()->txTone(),
             (unsigned int)config.channelList()->channel(i)->as<FMChannel>()->txTone());
  }
}


QTEST_GUILESS_MAIN(GD73Test)

