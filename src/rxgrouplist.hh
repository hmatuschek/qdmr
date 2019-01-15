#ifndef RXGROUPLIST_HH
#define RXGROUPLIST_HH

#include <QObject>
#include <QAbstractListModel>
#include <QListView>
#include <QListWidget>
#include "contact.hh"

#include "ui_rxgrouplistdialog.h"

class Config;

class RXGroupList: public QAbstractListModel
{
	Q_OBJECT

public:
	explicit RXGroupList(const QString &name, QObject *parent=nullptr);

  int count() const;

  void clear();

	const QString &name() const;
	bool setName(const QString &name);

	DigitalContact *contact(int idx) const;

	int addContact(DigitalContact *contact);
	bool remContact(DigitalContact *contact);
	bool remContact(int idx);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
	void onContactDeleted(QObject *obj);

protected:
	QString _name;
	QVector<DigitalContact *> _contacts;
};


class RXGroupLists: public QAbstractListModel
{
	Q_OBJECT

public:
	explicit RXGroupLists(QObject *parent=nullptr);

  int count() const;

  void clear();

  int indexOf(RXGroupList *list) const;
	RXGroupList *list(int idx) const;

	int addList(RXGroupList *list);
	bool remList(RXGroupList *list);
	bool remList(int idx);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
  void onListDeleted(QObject *);

protected:
	QVector<RXGroupList *> _lists;
};


class RXGroupListDialog: public QDialog, private Ui::RXGroupListDialog
{
	Q_OBJECT

public:
	explicit RXGroupListDialog(Config *config, QWidget *parent=nullptr);
	RXGroupListDialog(Config *config, RXGroupList *list, QWidget *parent=nullptr);

	RXGroupList *groupList();

protected slots:
	void onAddGroup();
	void onRemGroup();

protected:
	void construct();

protected:
	Config *_config;
  RXGroupList *_list;
};


class RXGroupListBox: public QComboBox
{
  Q_OBJECT

public:
  RXGroupListBox(RXGroupLists *lists, QWidget *parent=0);

  RXGroupList *groupList();
};


#endif // RXGROUPLIST_HH
