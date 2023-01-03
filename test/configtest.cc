#include "configtest.hh"
#include "config.hh"
#include "errorstack.hh"
#include <iostream>
#include <QTest>


ConfigTest::ConfigTest(QObject *parent) : QObject(parent)
{
  // pass...
}


void
ConfigTest::initTestCase() {
  ErrorStack err;
  if (! _config.readYAML(":/data/config_test.yaml", err)) {
    QFAIL(QString("Cannot open codeplug file: %1").arg(err.format()).toStdString().c_str());
  }
}

void
ConfigTest::cleanupTestCase() {
  // clear codeplug
  _config.clear();
}

void
ConfigTest::testCloneChannelBasic() {
  // Check if a channel can be cloned
  Channel *clone = _config.channelList()->channel(0)->clone()->as<Channel>();
  // Check if channels are the same
  QCOMPARE(clone->compare(*_config.channelList()->channel(0)), 0);
}


QTEST_GUILESS_MAIN(ConfigTest)

