#include "libdmrconfigtest.hh"

UnitTestBase::UnitTestBase(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
UnitTestBase::initTestCase() {
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
UnitTestBase::cleanupTestCase() {
  // clear codeplug
  _basicConfig.clear();
  _channelFrequencyConfig.clear();
}


