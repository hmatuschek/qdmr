#ifndef TIMESLOTSELECT_HH
#define TIMESLOTSELECT_HH

#include <QComboBox>
#include "channel.hh"


class TimeslotSelect : public QComboBox
{
  Q_OBJECT

public:
  explicit TimeslotSelect(QWidget *parent=nullptr);

  void setSlot(DMRChannel::TimeSlot slot);
  DMRChannel::TimeSlot slot() const;
};

#endif // TIMESLOTSELECT_HH
