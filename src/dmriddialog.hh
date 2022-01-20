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
  explicit DMRIDDialog(QWidget *parent = nullptr);
  DMRIDDialog(DMRRadioID *radioid, QWidget *parent = nullptr);
  ~DMRIDDialog();

  DMRRadioID *radioId();

protected:
  void construct();

private:
  Ui::DMRIDDialog *ui;
  DMRRadioID *_myID;
  DMRRadioID *_editID;
};

#endif // DMRIDDIALOG_HH
