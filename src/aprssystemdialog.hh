#ifndef APRSSYSTEMDIALOG_HH
#define APRSSYSTEMDIALOG_HH

#include <QDialog>
#include <QImage>
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
  QIcon aprsIcon(APRSSystem::Icon icon);

protected:
  Config *_config;
  APRSSystem *_aprs;

private:
  Ui::aprssystemdialog *ui;
  QImage _icons0;
};

#endif // APRSSYSTEMDIALOG_HH
