#ifndef DTMFCONTACTDIALOG_HH
#define DTMFCONTACTDIALOG_HH

#include <QDialog>

namespace Ui {
  class DTMFContactDialog;
}

class DTMFContact;


class DTMFContactDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DTMFContactDialog(QWidget *parent = nullptr);
  DTMFContactDialog(DTMFContact *contact, QWidget *parent = nullptr);
  ~DTMFContactDialog();

  DTMFContact *contact();

protected:
  void construct();

private:
  DTMFContact *_myContact;
  DTMFContact *_contact;
  Ui::DTMFContactDialog *ui;
};

#endif // DTMFCONTACTDIALOG_HH
