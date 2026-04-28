#include "opengd77_test.hh"
#include "opengd77_limits.hh"
#include "config.hh"
#include "opengd77_codeplug.hh"
#include "errorstack.hh"
#include <QTest>
#include <QtEndian>
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

  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  Config *intermediate = codeplug.preprocess(&config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot encode codeplug for OpenGD77.";
    return false;
  }

  if (! codeplug.encode(intermediate, flags, err)) {
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

  config.channelList()->channel(0)->as<DMRChannel>()->setGroupList(nullptr);
  config.channelList()->channel(0)->as<DMRChannel>()->setContact(nullptr);
  config.channelList()->channel(1)->as<DMRChannel>()->setContact(nullptr);
  config.channelList()->channel(2)->as<DMRChannel>()->setGroupList(nullptr);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QVERIFY(decoded.channelList()->channel(0)->as<DMRChannel>()->groupListRef()->isNull());
  QVERIFY(decoded.channelList()->channel(0)->as<DMRChannel>()->contactRef()->isNull());

  QVERIFY(! decoded.channelList()->channel(1)->as<DMRChannel>()->groupListRef()->isNull());
  QVERIFY(decoded.channelList()->channel(1)->as<DMRChannel>()->contactRef()->isNull());

  QVERIFY(decoded.channelList()->channel(2)->as<DMRChannel>()->groupListRef()->isNull());
  QVERIFY(! decoded.channelList()->channel(2)->as<DMRChannel>()->contactRef()->isNull());

  QVERIFY(! decoded.channelList()->channel(3)->as<DMRChannel>()->groupListRef()->isNull());
  QVERIFY(decoded.channelList()->channel(3)->as<DMRChannel>()->contactRef()->isNull());
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
  config.channelList()->channel(0)->as<DMRChannel>()->setRadioId(id2);
  config.channelList()->channel(1)->as<DMRChannel>()->setRadioId(id2);

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.radioIDs()->count(), 2);
  QVERIFY(! decoded.channelList()->channel(0)->as<DMRChannel>()->radioIdRef()->isNull());
  QCOMPARE(decoded.channelList()->channel(0)->as<DMRChannel>()->radioId()->number(), 1234567);
  QVERIFY(! decoded.channelList()->channel(1)->as<DMRChannel>()->radioIdRef()->isNull());
  QCOMPARE(decoded.channelList()->channel(1)->as<DMRChannel>()->radioId()->number(), 1234567);
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
  auto sys = decoded.posSystems()->get(0)->as<FMAPRSSystem>();
  QCOMPARE(sys->source(), "DM3MAT");

  FMChannel *channel = decoded.posSystems()->get(0)->as<FMAPRSSystem>()
                         ->revertChannelRef()->as<FMChannel>();
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

  config.channelList()->channel(0)->setTimeout(Interval::fromSeconds(60));

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.channelList()->count(), 2);
  QVERIFY(! decoded.channelList()->channel(0)->timeoutDisabled());
  QCOMPARE(decoded.channelList()->channel(0)->timeout(), Interval::fromSeconds(60));
  QVERIFY(decoded.channelList()->channel(1)->timeoutDisabled());
}


void
OpenGD77Test::testConfigVerification() {
  ErrorStack err;
  Config config;

  if (! config.readYAML(":/data/opengd77_simple_config.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  RadioLimitContext ctx;
  OpenGD77Limits().verifyConfig(&config, ctx);

  QStringList messages;
  for (int i=0; i<ctx.count(); i++)
    messages.append(ctx.message(i).format());

  if (RadioLimitIssue::Severity::Warning <= ctx.maxSeverity())
    QFAIL(messages.join("; ").toLatin1().constData());
}


void
OpenGD77Test::testBootMelody() {
  uint8_t note[2];
  OpenGD77BaseCodeplug::NoteElement elm(note);
  elm.setPause(); QCOMPARE(note[0], 0u);
  elm.setFrequency(110); QCOMPARE(note[0],1u);
  elm.setFrequency(440); QCOMPARE(note[0],25u);
  note[0] = 13; QCOMPARE(elm.frequency(), 220);
  note[0] = 16; QCOMPARE(elm.frequency(), 261.62);
  note[0] = 17; QCOMPARE(elm.frequency(), 277.18);
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/opengd77_boot_melody.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  QCOMPARE(config.settings()->tone()->bootMelody()->toLilypond(), "c4 e g c e g c e g c e g c d e g c1");

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.settings()->tone()->bootMelody()->toLilypond(), "c4 e g c e g c e g c e g c d e g c1");
}


void
OpenGD77Test::testBootMelodyPauses() {
  ErrorStack err;
  Config config, decoded;

  if (! config.readYAML(":/data/opengd77_boot_melody.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toLocal8Bit().constData());
  }

  config.settings()->tone()->bootMelody()->fromLilypond("c4 r16 c4 g4 e4 r4 c16 g16 e16 r16 c4");

  if (! encodeDecode(config, decoded, err))
    QFAIL(err.format().toLocal8Bit().constData());

  QCOMPARE(decoded.settings()->tone()->bootMelody()->toLilypond(), "c4 r16 c4 g e r c16 g e r c4");
}


QTEST_GUILESS_MAIN(OpenGD77Test)

