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
	explicit RepeaterDatabase(const QGeoCoordinate &qth, uint updatePeriodDays=5, QObject *parent=nullptr);

	bool load();
	bool load(const QString &filename);

  const QJsonObject &repeater(int idx) const;

	uint dbAge() const;

  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

public slots:
	void download();

protected slots:
	void downloadFinished(QNetworkReply *reply);

protected:
	QGeoCoordinate _qth;
	QVector<QJsonObject>  _repeater;
	QHash<QString, uint>  _callsigns;
	QNetworkAccessManager _network;
};


class DMRRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit DMRRepeaterFilter(QObject *parent=nullptr);
};


class FMRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit FMRepeaterFilter(QObject *parent=nullptr);
};

#endif // REPEATERDATABASE_HH
