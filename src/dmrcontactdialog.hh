#ifndef DMRCONTACTDIALOG_HH
#define DMRCONTACTDIALOG_HH

#include <QDialog>

namespace Ui {
  class DMRContactDialog;
}

class QCompleter;
class UserDatabase;
class TalkGroupDatabase;
class DMRContact;
class Config;

class DMRContactDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DMRContactDialog(UserDatabase *users, TalkGroupDatabase *tgs, Config *context, QWidget *parent=nullptr);
  explicit DMRContactDialog(DMRContact *contact, UserDatabase *users, TalkGroupDatabase *tgs, Config *context, QWidget *parent=nullptr);
  ~DMRContactDialog();

public:
  DMRContact *contact();

protected slots:
  void onTypeChanged(int idx);
  void onCompleterActivated(const QModelIndex &idx);

protected:
  void construct();

private:
  DMRContact *_myContact;
  DMRContact *_contact;
  QCompleter *_user_completer;
  QCompleter *_tg_completer;
  Config *_config;
  Ui::DMRContactDialog *ui;
};

#endif // DMRCONTACTDIALOG_HH
