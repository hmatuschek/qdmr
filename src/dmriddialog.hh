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
  DMRIDDialog(RadioID *radioid, QWidget *parent = nullptr);
  ~DMRIDDialog();

  RadioID *radioId();

protected:
  void construct();

private:
  Ui::DMRIDDialog *ui;
  RadioID *_myID;
  RadioID *_editID;
};

#endif // DMRIDDIALOG_HH
