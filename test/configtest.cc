#include "configtest.hh"
#include "config.hh"
#include <QTest>


ConfigTest::ConfigTest(QObject *parent) : QObject(parent)
{
  // pass...
}


void
ConfigTest::initTestCase() {
  _config.readYAML(":/config_test.yaml");
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
  QTEST_ASSERT(0 == clone->compare(*_config.channelList()->channel(0)));
}


QTEST_GUILESS_MAIN(ConfigTest)

