#ifndef APRSSELECT_HH
#define APRSSELECT_HH

#include <QComboBox>
class PositionReportingSystem;
class FMAPRSSystem;
class Config;


class APRSSelect : public QComboBox
{
  Q_OBJECT

public:
  APRSSelect(Config *config, QWidget *parent=nullptr);

  void setAPRSSystem(PositionReportingSystem *sys);
  PositionReportingSystem *aprsSystem() const;
};



class FMAPRSSelect : public QComboBox
{
  Q_OBJECT

public:
  FMAPRSSelect(Config *config, QWidget *parent=nullptr);

  void setAPRSSystem(FMAPRSSystem *sys);
  FMAPRSSystem *aprsSystem() const;
};

#endif // APRSSELECT_HH
