#ifndef ROAMINGZONEDIALOG_HH
#define ROAMINGZONEDIALOG_HH

#include <QComboBox>
#include <QDialog>
#include "roamingzone.hh"
#include "ui_roamingzonedialog.h"

class Config;

class RoamingZoneDialog : public QDialog, private Ui_RoamingZoneDialog
{
  Q_OBJECT
public:
  explicit RoamingZoneDialog(Config *config, QWidget *parent = nullptr);
  RoamingZoneDialog(Config *config, RoamingZone *zone, QWidget *parent=nullptr);

  RoamingZone *zone();

protected slots:
  void construct();

  void onAddRoamingChannel();
  void onAddDMRChannel();
  void onRemChannel();

protected:
  Config *_config;
  RoamingZone *_myZone;
  RoamingZone *_zone;
};


class RoamingZoneSelect: public QComboBox
{
  Q_OBJECT

public:
  RoamingZoneSelect(Config *config, QWidget *parent=nullptr);

  void setRoamingZone(RoamingZone *z);
  RoamingZone *roamingZone() const;
};

#endif // ROAMINGZONEDIALOG_HH
