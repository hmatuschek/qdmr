#ifndef RXGROUPLISTDIALOG_HH
#define RXGROUPLISTDIALOG_HH

#include <QListView>
#include <QListWidget>
#include <QDialog>
#include <QComboBox>

#include "rxgrouplist.hh"

#include "ui_rxgrouplistdialog.h"

class Config;


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
  void onGroupUp();
  void onGroupDown();

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
  RXGroupListBox(RXGroupLists *groups, QWidget *parent=0);

  RXGroupList *groupList();
};

void populateRXGroupListBox(QComboBox *box, RXGroupLists *groups, RXGroupList *list=nullptr);


#endif // RXGROUPLISTDIALOG_HH
