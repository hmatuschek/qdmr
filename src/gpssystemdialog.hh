#ifndef GPSSYSTEMDIALOG_HH
#define GPSSYSTEMDIALOG_HH

#include "config.hh"

#include <QDialog>
#include "ui_gpssystemdialog.h"

class GPSSystemDialog : public QDialog, private Ui::GPSSystemDialog
{
  Q_OBJECT

public:
  GPSSystemDialog(Config *config, QWidget *parent=nullptr);
  GPSSystemDialog(Config *config, GPSSystem *gps, QWidget *parent=nullptr);

  GPSSystem *gpsSystem();

protected:
  void construct();

protected:
  Config *_config;
  GPSSystem *_myGPSSystem;
  GPSSystem *_gpsSystem;
};

#endif // GPSSYSTEMDIALOG_HH
