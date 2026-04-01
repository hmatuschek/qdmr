#ifndef BANDWIDTHSELECT_HH
#define BANDWIDTHSELECT_HH

#include <QComboBox>
#include "channel.hh"


class BandwidthSelect : public QComboBox
{
  Q_OBJECT

public:
  explicit BandwidthSelect(QWidget *parent=nullptr);

  void setBandwidth(FMChannel::Bandwidth bw);
  FMChannel::Bandwidth bandwidth() const;
};

#endif // BANDWIDTHSELECT_HH
