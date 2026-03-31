#include "timeslotselect.hh"

TimeslotSelect::TimeslotSelect(QWidget *parent)
: QComboBox(parent)
{
  addItem(tr("Slot 1"), QVariant::fromValue(DMRChannel::TimeSlot::TS1));
  addItem(tr("Slot 2"), QVariant::fromValue(DMRChannel::TimeSlot::TS2));
}


void
TimeslotSelect::setSlot(DMRChannel::TimeSlot slot) {
  setCurrentIndex((slot == DMRChannel::TimeSlot::TS1) ? 0 : 1);
}

DMRChannel::TimeSlot
TimeslotSelect::slot() const {
  return currentData().value<DMRChannel::TimeSlot>();
}
