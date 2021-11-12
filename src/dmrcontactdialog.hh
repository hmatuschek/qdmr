#ifndef DMRCONTACTDIALOG_HH
#define DMRCONTACTDIALOG_HH

#include <QDialog>

namespace Ui {
  class DMRContactDialog;
}

class QCompleter;
class UserDatabase;
class TalkGroupDatabase;
class DigitalContact;


class DMRContactDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DMRContactDialog(UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent=nullptr);
  explicit DMRContactDialog(DigitalContact *contact, UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent=nullptr);
  ~DMRContactDialog();

public:
  DigitalContact *contact();

protected slots:
  void onTypeChanged(int idx);
  void onCompleterActivated(const QModelIndex &idx);

protected:
  void construct();

private:
  Ui::DMRContactDialog *ui;
  DigitalContact *_myContact;
  DigitalContact *_contact;
  QCompleter *_user_completer;
  QCompleter *_tg_completer;
};

#endif // DMRCONTACTDIALOG_HH
