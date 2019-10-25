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

class ContactDialog: public QDialog, private Ui::ContactDialog
{
	Q_OBJECT

public:
	ContactDialog(QWidget *parent=nullptr);
	ContactDialog(Contact *contact, QWidget *parent=nullptr);

	Contact *contact();

protected slots:
	void onTypeChanged(int idx);

protected:
	void construct();

protected:
	Contact *_contact;
};


class DigitalContactBox: public QComboBox
{
  Q_OBJECT

public:
  DigitalContactBox(ContactList *contacts, QWidget *parent=0);

  DigitalContact *contact();
};


#endif // CONTACTDIALOG_HH
