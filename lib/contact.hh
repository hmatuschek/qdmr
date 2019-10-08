#ifndef CONTACT_HH
#define CONTACT_HH

#include <QObject>
#include <QWidget>
#include <QAbstractTableModel>
#include <QVector>
#include <QTableView>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_contactdialog.h"

class Config;


class Contact: public QObject
{
	Q_OBJECT

protected:
	Contact(const QString &name, bool rxTone=true, QObject *parent=nullptr);

public:
	const QString &name() const;
	bool setName(const QString &name);

  bool rxTone() const;
  void setRXTone(bool enable);

	template <class T>
	bool is() const {
		return 0 != dynamic_cast<const T *>(this);
	}

	template<class T>
	T *as() {
		return dynamic_cast<T *>(this);
	}

	template<class T>
	const T *as() const {
		return dynamic_cast<const T *>(this);
	}

signals:
	void modified();

protected:
	QString _name;
  bool _rxTone;
};


class DTMFContact: public Contact
{
	Q_OBJECT

public:
	DTMFContact(const QString &name, const QString &number, bool rxTone=false, QObject *parent=nullptr);

	const QString &number() const;
	bool setNumber(const QString &number);

protected:
	QString _number;
};


class DigitalContact: public Contact
{
	Q_OBJECT

public:
	typedef enum {
		PrivateCall, GroupCall, AllCall
	} Type;

public:
	DigitalContact(Type type, const QString &name, uint number, bool rxTone=false, QObject *parent=nullptr);

	Type type() const;
  void setType(Type type);

	uint number() const;
	bool setNumber(uint number);

protected:
	Type _type;
	uint _number;
};


class ContactList: public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit ContactList(QObject *parent=nullptr);

	int count() const;

  void clear();

  Contact *contact(int idx) const;
  int indexOf(Contact *contact) const;

  int addContact(Contact *contact);

  bool remContact(int idx);
	bool remContact(Contact *contact);

  bool moveUp(int row);
  bool moveDown(int row);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &index) const;
	int columnCount(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
	void onContactDeleted(QObject *contact);
  void onContactEdited();

protected:
	QVector<Contact *> _contacts;
};


class ContactDialog: public QDialog, private Ui::ContactDialog
{
	Q_OBJECT

public:
	ContactDialog(QWidget *parent=nullptr);
	ContactDialog(Contact *contact, QWidget *parent=nullptr);

	Contact *contact();

protected slots:
	void onTypeChanged();

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


#endif // CONTACT_HH
