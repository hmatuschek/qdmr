#ifndef ROAMINGZONEDIALOG_HH
#define ROAMINGZONEDIALOG_HH

#include <QDialog>
#include "roaming.hh"
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

  void onAddChannel();
  void onRemChannel();

protected:
  Config *_config;
  RoamingZone *_myZone;
  RoamingZone *_zone;
};

#endif // ROAMINGZONEDIALOG_HH
