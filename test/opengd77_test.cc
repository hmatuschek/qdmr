#include "opengd77_test.hh"
#include "config.hh"
#include "opengd77_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>
#include <QtEndian>
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


bool
OpenGD77Test::encodeDecode(Config &config, Config &decoded, const ErrorStack &err) {
  OpenGD77Codeplug codeplug;
  codeplug.clear();

  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot encode codeplug for OpenGD77.";
    return false;
  }

  if (! codeplug.encode(intermediate, Codeplug::Flags(), err)) {
    errMsg(err) << "Cannot encode codeplug for OpenGD77.";
    return false;
  }
  delete intermediate;

  if (! codeplug.decode(&decoded, err)) {
    errMsg(err) << "Cannot decode codeplug for OpenGD77.";
    return false;
  }

  if (! codeplug.postprocess(&decoded, err)) {
    errMsg(err) << "Cannot decode codeplug for OpenGD77.";
    return false;
  }

  return true;
}


void
OpenGD77Test::testBasicConfigDecoding() {
  ErrorStack err;
  Config decoded;
  if (! encodeDecode(_basicConfig, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());
}


void
OpenGD77Test::testChannelFrequency() {
  ErrorStack err;
  Config config;

  if (! encodeDecode(_channelFrequencyConfig, config, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(config.channelList()->channel(0)->rxFrequency(),
           Frequency::fromHz(123456780ULL));
  QCOMPARE(config.channelList()->channel(0)->txFrequency(),
           Frequency::fromHz(999999990ULL));
}


void
OpenGD77Test::testChannelGroupList() {
  ErrorStack err;

  Config config, decoded;
  if (! config.readYAML(":/data/config_test.yaml", err))
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());

  config.channelList()->channel(0)->as<DMRChannel>()->setGroupListObj(nullptr);
  config.channelList()->channel(0)->as<DMRChannel>()->setTXContactObj(nullptr);
  config.channelList()->channel(1)->as<DMRChannel>()->setTXContactObj(nullptr);
  config.channelList()->channel(2)->as<DMRChannel>()->setGroupListObj(nullptr);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QVERIFY(decoded.channelList()->channel(0)->as<DMRChannel>()->groupList()->isNull());
  QVERIFY(decoded.channelList()->channel(0)->as<DMRChannel>()->contact()->isNull());

  QVERIFY(! decoded.channelList()->channel(1)->as<DMRChannel>()->groupList()->isNull());
  QVERIFY(decoded.channelList()->channel(1)->as<DMRChannel>()->contact()->isNull());

  QVERIFY(decoded.channelList()->channel(2)->as<DMRChannel>()->groupList()->isNull());
  QVERIFY(! decoded.channelList()->channel(2)->as<DMRChannel>()->contact()->isNull());

  QVERIFY(! decoded.channelList()->channel(3)->as<DMRChannel>()->groupList()->isNull());
  QVERIFY(decoded.channelList()->channel(3)->as<DMRChannel>()->contact()->isNull());
}


void
OpenGD77Test::testChannelPowerSettings() {
  ErrorStack err;

  Config config, decoded;
  if (! config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  config.channelList()->channel(0)->setDefaultPower();
  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());
  QCOMPARE(decoded.channelList()->channel(0)->defaultPower(), true);

  config.channelList()->channel(0)->setPower(Channel::Power::Low);
  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());
  QCOMPARE(decoded.channelList()->channel(0)->defaultPower(), false);
  QCOMPARE(decoded.channelList()->channel(0)->power(), Channel::Power::Low);

  config.channelList()->channel(0)->setPower(Channel::Power::High);
  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());
  QCOMPARE(decoded.channelList()->channel(0)->defaultPower(), false);
  QCOMPARE(decoded.channelList()->channel(0)->power(), Channel::Power::High);
}


void
OpenGD77Test::testOverrideChannelRadioId() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  // add another DMR ID and assign it to two channels
  auto id2 = new DMRRadioID("Test", 1234567);
  config.radioIDs()->add(id2);
  config.channelList()->channel(0)->as<DMRChannel>()->setRadioIdObj(id2);
  config.channelList()->channel(1)->as<DMRChannel>()->setRadioIdObj(id2);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.radioIDs()->count(), 2);
  QVERIFY(! decoded.channelList()->channel(0)->as<DMRChannel>()->radioId()->isNull());
  QCOMPARE(decoded.channelList()->channel(0)->as<DMRChannel>()->radioIdObj()->number(), 1234567);
  QVERIFY(! decoded.channelList()->channel(1)->as<DMRChannel>()->radioId()->isNull());
  QCOMPARE(decoded.channelList()->channel(1)->as<DMRChannel>()->radioIdObj()->number(), 1234567);
}


void
OpenGD77Test::testChannelSubTones() {
  ErrorStack err;
  Config config, decoded;

  char enc[] = {0x00, 0x10};
  uint16_t dec = *(quint16 *)enc;
  SelectiveCall decTone = OpenGD77BaseCodeplug::decodeSelectiveCall(
        qFromLittleEndian(dec));
  QVERIFY(decTone.isValid());
  QVERIFY(decTone.isCTCSS());
  QCOMPARE(decTone.Hz(), 100.0);

  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  config.channelList()->channel(0)->as<FMChannel>()->setTXTone(SelectiveCall(67.0));
  config.channelList()->channel(0)->as<FMChannel>()->setRXTone(SelectiveCall(123.0));

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  SelectiveCall txTone = decoded.channelList()->channel(0)->as<FMChannel>()->txTone(),
      rxTone = decoded.channelList()->channel(0)->as<FMChannel>()->rxTone();

  QVERIFY(txTone.isValid());
  QVERIFY(txTone.isCTCSS());
  QCOMPARE(txTone.Hz(), 67.0);

  QVERIFY(rxTone.isValid());
  QVERIFY(rxTone.isCTCSS());
  QCOMPARE(rxTone.Hz(), 123.0);
}


void
OpenGD77Test::testChannelFixedLocation() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  auto ext = new OpenGD77ChannelExtension();
  ext->setLocator("JO62jl24");
  config.channelList()->channel(0)->setOpenGD77ChannelExtension(ext);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QVERIFY(decoded.channelList()->channel(0)->openGD77ChannelExtension());
  QCOMPARE(decoded.channelList()->channel(0)->openGD77ChannelExtension()->locator(), "JO62jl24");

  ext->setLocator("JO59gw73");
  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());
  QCOMPARE(decoded.channelList()->channel(0)->openGD77ChannelExtension()->locator(), "JO59gw73");
}


void
OpenGD77Test::testAPRSSourceCall() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.posSystems()->count(), 1);
  auto sys = decoded.posSystems()->aprsSystem(0);
  QCOMPARE(sys->source(), "DM3MAT");

  FMChannel *channel = decoded.posSystems()->aprsSystem(0)->revert()->as<FMChannel>();
  QVERIFY(channel->name() == "2m APRS");
}

void
OpenGD77Test::testDTMFContacts() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/dtmf_contact.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.contacts()->count(), 1);
  QVERIFY(decoded.contacts()->contact(0)->is<DTMFContact>());
  auto cont = decoded.contacts()->contact(0)->as<DTMFContact>();
  QCOMPARE(cont->name(), "DTMF");
  QCOMPARE(cont->number(), "*ABC123#");
}


void
OpenGD77Test::testChannelTransmitTimeout() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/fm_aprs_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  config.channelList()->channel(0)->setTimeout(60);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.channelList()->count(), 2);
  QVERIFY(! decoded.channelList()->channel(0)->timeoutDisabled());
  QCOMPARE(decoded.channelList()->channel(0)->timeout(), 60);
  QVERIFY(decoded.channelList()->channel(1)->timeoutDisabled());
}

QTEST_GUILESS_MAIN(OpenGD77Test)

