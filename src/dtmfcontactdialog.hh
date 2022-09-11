#ifndef DTMFCONTACTDIALOG_HH
#define DTMFCONTACTDIALOG_HH

#include <QDialog>

namespace Ui {
  class DTMFContactDialog;
}

class DTMFContact;
class Config;

class DTMFContactDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DTMFContactDialog(Config *context, QWidget *parent = nullptr);
  DTMFContactDialog(DTMFContact *contact, Config *context, QWidget *parent = nullptr);
  ~DTMFContactDialog();

  DTMFContact *contact();

protected:
  void construct();

private:
  DTMFContact *_myContact;
  DTMFContact *_contact;
  Config *_config;
  Ui::DTMFContactDialog *ui;
};

#endif // DTMFCONTACTDIALOG_HH
