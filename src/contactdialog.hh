#ifndef CONTACTDIALOG_HH
#define CONTACTDIALOG_HH

#include "contact.hh"
#include <QWidget>
#include <QAbstractTableModel>
#include <QVector>
#include <QTableView>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_contactdialog.h"
class UserDatabase;
class TalkGroupDatabase;


class ContactDialog: public QDialog, private Ui::ContactDialog
{
	Q_OBJECT

public:
  ContactDialog(UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent=nullptr);
  ContactDialog(Contact *contact, UserDatabase *users, TalkGroupDatabase *tgs, QWidget *parent=nullptr);

	Contact *contact();

protected slots:
	void onTypeChanged(int idx);
  void onCompleterActivated(const QModelIndex &idx);

protected:
	void construct();

protected:
  Contact *_myContact;
	Contact *_contact;
  QCompleter *_user_completer;
  QCompleter *_tg_completer;
};


class DigitalContactBox: public QComboBox
{
  Q_OBJECT

public:
  DigitalContactBox(ContactList *contacts, QWidget *parent=0);

  DigitalContact *contact();
};


#endif // CONTACTDIALOG_HH
