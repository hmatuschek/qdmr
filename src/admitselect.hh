#ifndef ADMITSELECT_HH
#define ADMITSELECT_HH

#include <QComboBox>
#include "channel.hh"


class FMAdmitSelect : public QComboBox
{
  Q_OBJECT

public:
  explicit FMAdmitSelect(QWidget *parent=nullptr);

  void setAdmit(FMChannel::Admit admit);
  FMChannel::Admit admit() const;
};


class DMRAdmitSelect : public QComboBox
{
  Q_OBJECT

public:
  explicit DMRAdmitSelect(QWidget *parent=nullptr);

  void setAdmit(DMRChannel::Admit admit);
  DMRChannel::Admit admit() const;
};


#endif // ADMITSELECT_HH
