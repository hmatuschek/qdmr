#ifndef M17CONTACTDIALOG_HH
#define M17CONTACTDIALOG_HH

#include <QDialog>

class M17Contact;
class Config;

namespace Ui {
  class M17ContactDialog;
}

class M17ContactDialog : public QDialog
{
  Q_OBJECT

public:
  explicit M17ContactDialog(Config *config, QWidget *parent = nullptr);
  M17ContactDialog(Config *config, M17Contact *contact, QWidget *parent = nullptr);
  ~M17ContactDialog();

  M17Contact *contact() const;

protected:
  /** Assembles the GUI, populates elements. */
  void construct();

protected slots:
  void onBroadcastToggled(bool enable);

private:
  Ui::M17ContactDialog *ui;
  Config *_config;
  M17Contact *_contact;
  M17Contact *_myContact;
};

#endif // M17CONTACTDIALOG_HH
