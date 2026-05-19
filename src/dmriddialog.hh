#ifndef DMRIDDIALOG_HH
#define DMRIDDIALOG_HH

#include <QDialog>
#include "radioid.hh"

namespace Ui {
  class DMRIDDialog;
}

class DMRIDDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DMRIDDialog(Config *context, QWidget *parent = nullptr);
  DMRIDDialog(DMRRadioID *radioid, Config *context, QWidget *parent = nullptr);
  ~DMRIDDialog();

  DMRRadioID *radioId();

protected:
  void construct();

private:
  Ui::DMRIDDialog *ui;
  DMRRadioID *_myID;
  DMRRadioID *_editID;
  Config *_config;
};

#endif // DMRIDDIALOG_HH
