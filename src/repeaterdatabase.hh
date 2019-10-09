#ifndef REPEATERDATABASE_HH
#define REPEATERDATABASE_HH

#include <QObject>
#include <QVector>
#include <QHash>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QGeoPositionInfoSource>


class RepeaterDatabase : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit RepeaterDatabase(QObject *parent=nullptr);

	bool load();
	bool load(const QString &filename);

  const QJsonObject &repeater(int idx) const;

  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

public slots:
	void download();

protected slots:
	void downloadFinished(QNetworkReply *reply);

protected:
	QVector<QJsonObject>  _repeater;
	QHash<QString, uint>  _callsigns;
	QNetworkAccessManager _network;
};


class DMRRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit DMRRepeaterFilter(QObject *parent=nullptr);

protected:
  //bool lessThan(const QModelIndex &src_left, const QModelIndex &src_right) const;
};


class FMRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit FMRepeaterFilter(QObject *parent=nullptr);

protected:
  //bool lessThan(const QModelIndex &src_left, const QModelIndex &src_right) const;
};

#endif // REPEATERDATABASE_HH
