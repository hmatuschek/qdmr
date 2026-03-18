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
  GPSSystemDialog(Config *config, DMRAPRSSystem *gps, QWidget *parent=nullptr);

  DMRAPRSSystem *gpsSystem();

protected:
  void construct();

protected:
  Config *_config;
  DMRAPRSSystem *_myGPSSystem;
  DMRAPRSSystem *_gpsSystem;
};

#endif // GPSSYSTEMDIALOG_HH
