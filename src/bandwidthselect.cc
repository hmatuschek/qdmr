#include "bandwidthselect.hh"
#include "channel.hh"


BandwidthSelect::BandwidthSelect(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("Narrow (12.5 kHz)"), QVariant::fromValue(FMChannel::Bandwidth::Narrow));
  addItem(tr("Wide (25 kHz)"), QVariant::fromValue(FMChannel::Bandwidth::Wide));
}

void
BandwidthSelect::setBandwidth(FMChannel::Bandwidth bw) {
  setCurrentIndex((FMChannel::Bandwidth::Narrow == bw) ? 0 : 1);
}

FMChannel::Bandwidth
BandwidthSelect::bandwidth() const {
  return currentData().value<FMChannel::Bandwidth>();
}
