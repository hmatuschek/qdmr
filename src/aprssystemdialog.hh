#ifndef APRSSYSTEMDIALOG_HH
#define APRSSYSTEMDIALOG_HH

#include <QDialog>
#include <QImage>
#include "config.hh"

namespace Ui {
  class APRSSystemDialog;
}

class APRSSystemDialog : public QDialog
{
  Q_OBJECT

public:
  explicit APRSSystemDialog(Config *config, QWidget *parent = nullptr);
  explicit APRSSystemDialog(Config *config, FMAPRSSystem *aprs, QWidget *parent = nullptr);

  virtual ~APRSSystemDialog();

  FMAPRSSystem *aprsSystem();

protected:
  void closeEvent(QCloseEvent *event);
  void construct();
  QIcon aprsIcon(FMAPRSSystem::Icon icon);

protected:
  Config *_config;
  FMAPRSSystem *_myAPRS;
  FMAPRSSystem *_aprs;

private:
  Ui::APRSSystemDialog *ui;
  QImage _icons0;
};

#endif // APRSSYSTEMDIALOG_HH
