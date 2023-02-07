#include "d868uve_test.hh"
#include "config.hh"
#include "d868uv.hh"
#include "d868uv_codeplug.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>

D868UVETest::D868UVETest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
D868UVETest::initTestCase() {
  ErrorStack err;
  if (! _basicConfig.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
}

void
D868UVETest::testBasicConfigEncoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug = false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::testBasicConfigDecoding() {
  ErrorStack err;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  D868UVCodeplug codeplug;
  if (! codeplug.encode(&_basicConfig, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config config;
  if (! codeplug.decode(&config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }
}

void
D868UVETest::testAutoRepeaterOffset() {
  ErrorStack err;
  Config config;
  if (! config.readYAML(":/data/anytone_auto_repeater_extension.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1")
          .arg(err.format()).toStdString().c_str());
  }

  D868UVCodeplug codeplug;
  Codeplug::Flags flags; flags.updateCodePlug=false;
  if (! codeplug.encode(&config, flags, err)) {
    QFAIL(QString("Cannot encode codeplug for AnyTone AT-D868UVE: %1")
          .arg(err.format()).toStdString().c_str());
  }

  Config comp_config;
  if (! codeplug.decode(&comp_config, err)) {
    QFAIL(QString("Cannot decode codeplug for AnyTone AT-D878UVII: %1")
          .arg(err.format()).toStdString().c_str());
  }

  QVERIFY2(comp_config.settings()->anytoneExtension(), "Expected AnyTone settings extension.");
  AnytoneAutoRepeaterSettingsExtension *ext = comp_config.settings()->anytoneExtension()->autoRepeaterSettings();

  QCOMPARE(ext->offsets()->count(), 2);
  QCOMPARE(ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>()->offset(),  600000);
  QCOMPARE(ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>()->offset(), 7600000);

  QVERIFY(! ext->vhfRef()->isNull()); QVERIFY(! ext->uhfRef()->isNull());
  QCOMPARE(ext->vhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(0)->as<AnytoneAutoRepeaterOffset>());
  QCOMPARE(ext->uhfRef()->as<AnytoneAutoRepeaterOffset>(), ext->offsets()->get(1)->as<AnytoneAutoRepeaterOffset>());
}

QTEST_GUILESS_MAIN(D868UVETest)

