#ifndef SCANLISTDIALOG_HH
#define SCANLISTDIALOG_HH

#include <QDialog>

#include "ui_scanlistdialog.h"

class Config;
class ScanList;


class ScanListDialog: public QDialog, private Ui::ScanListDialog
{
	Q_OBJECT

public:
	ScanListDialog(Config *config, QWidget *parent=nullptr);
	ScanListDialog(Config *config, ScanList *list, QWidget *parent=nullptr);

	ScanList *scanlist();

protected slots:
	void onAddChannel();
	void onRemChannel();

protected:
	void construct();

protected:
	Config *_config;
  ScanList *_myScanList;
  ScanList *_scanlist;
};


#endif // SCANLISTDIALOG_HH
