#ifndef ZONE_HH
#define ZONE_HH

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QWidget>
#include <QListView>
#include <QDialog>
#include <QListWidget>

class Channel;
class Config;

#include "ui_zonedialog.h"


class Zone : public QAbstractListModel
{
	Q_OBJECT

public:
	explicit Zone(const QString &name, QObject *parent = nullptr);

	int count() const;
	void clear();

	const QString &name() const;
	bool setName(const QString &name);

	Channel *channel(int idx) const;
	bool addChannel(Channel *channel);
	bool remChannel(int idx);
	bool remChannel(Channel *channel);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
	void onChannelDeleted(QObject *obj);

protected:
	QString _name;
	QVector<Channel *> _channels;
};


class ZoneList : public QAbstractListModel
{
	Q_OBJECT

public:
	explicit ZoneList(QObject *parent = nullptr);

	int count() const;
	void clear();

	Zone *zone(int idx) const;
	bool addZone(Zone *zone);
	bool remZone(int idx);
	bool remZone(Zone *zone);

  bool moveUp(int row);
  bool moveDown(int row);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
	void onZoneDeleted(QObject *obj);

protected:
	QVector<Zone *> _zones;
};


class ZoneDialog: public QDialog, private Ui::ZoneDialog
{
	Q_OBJECT

public:
	ZoneDialog(Config *config, QWidget *parent=nullptr);
	ZoneDialog(Config *config, Zone *zone, QWidget *parent=nullptr);

	Zone *zone();

protected slots:
	void onAddChannel();
	void onRemChannel();
	void onChannelUp();
	void onChannelDown();

protected:
	void construct();

protected:
	Config *_config;
	Zone *_zone;
};

#endif // ZONE_HH
