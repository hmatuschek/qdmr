#ifndef APRSSYSTEMDIALOG_HH
#define APRSSYSTEMDIALOG_HH

#include <QDialog>
#include "config.hh"

namespace Ui {
  class aprssystemdialog;
}

class APRSSystemDialog : public QDialog
{
  Q_OBJECT

public:
  explicit APRSSystemDialog(Config *config, QWidget *parent = nullptr);
  explicit APRSSystemDialog(Config *config, APRSSystem *aprs, QWidget *parent = nullptr);

  virtual ~APRSSystemDialog();

  APRSSystem *aprsSystem();

protected:
  void construct();

protected:
  Config *_config;
  APRSSystem *_aprs;

private:
  Ui::aprssystemdialog *ui;
};

#endif // APRSSYSTEMDIALOG_HH
