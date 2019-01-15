#ifndef SCANLIST_HH
#define SCANLIST_HH

#include <QObject>
#include <QAbstractListModel>
#include <QWidget>
#include <QDialog>
#include <QListView>
#include <QListWidget>
#include <QLineEdit>

class Channel;
class Config;


class ScanList : public QAbstractListModel
{
	Q_OBJECT

public:
	ScanList(const QString &name, QObject *parent=nullptr);

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

class ScanLists: public QAbstractListModel
{
	Q_OBJECT

public:
	explicit ScanLists(QObject *parent = nullptr);

	int count() const;
  int indexOf(ScanList *list) const;

	void clear();

	ScanList *scanlist(int idx) const;
	bool addScanList(ScanList *list);
	bool remScanList(int idx);
	bool remScanList(ScanList *list);

	// Implementation of QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
	void onScanListDeleted(QObject *obj);

protected:
	QVector<ScanList *> _scanlists;
};


class ScanListsView: public QWidget
{
	Q_OBJECT

public:
	ScanListsView(Config *config, QWidget *parent=nullptr);

protected slots:
	void onAddScanList();
	void onRemScanList();
	void onEditScanList(const QModelIndex &idx);

protected:
	Config *_config;
	QListView *_view;
};


class ScanListDialog: public QDialog
{
	Q_OBJECT

public:
	ScanListDialog(Config *config, QWidget *parent=nullptr);
	ScanListDialog(Config *config, ScanList *list, QWidget *parent=nullptr);

	ScanList *scanlist();

protected slots:
	void onAddChannel();
	void onRemChannel();
	void onChannelUp();
	void onChannelDown();

protected:
	void construct();

protected:
	Config *_config;
	ScanList *_scanlist;
	QLineEdit *_name;
	QListWidget *_list;
};


#endif // SCANLIST_HH
