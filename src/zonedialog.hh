#ifndef ZONEDIALOG_HH
#define ZONEDIALOG_HH

#include <QDialog>

#include "ui_zonedialog.h"

class Config;
class Zone;


class ZoneDialog: public QDialog, private Ui::ZoneDialog
{
	Q_OBJECT

public:
	ZoneDialog(Config *config, QWidget *parent=nullptr);
	ZoneDialog(Config *config, Zone *zone, QWidget *parent=nullptr);

	Zone *zone();

protected slots:
	void onAddChannelA();
	void onRemChannelA();
	void onChannelAUp();
	void onChannelADown();

  void onAddChannelB();
	void onRemChannelB();
	void onChannelBUp();
	void onChannelBDown();

  void onHideZoneHint();

protected:
	void construct();

protected:
	Config *_config;
	Zone *_zone;
};


#endif // ZONEDIALOG_HH
