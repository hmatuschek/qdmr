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

/** Represents the complete downloaded repeater database from http://repeatermap.de.
 * @ingroup util */
class RepeaterDatabase : public QAbstractTableModel
{
	Q_OBJECT

public:
	/** Constructs a new repeater database.
	 * The contructor will also start the download of the repeater database if the database was not
	 * downloaded yet or the downloaded database is older than @c updatePeriodDays days.
	 *
	 * The repeater database will be sorted with respect to the distance to the specified QTH. */
	explicit RepeaterDatabase(const QGeoCoordinate &qth, unsigned updatePeriodDays=5, QObject *parent=nullptr);

	/** Loads the downloaded repeater database. */
	bool load();
	/** Loads the downloaded repeater database from the specified location. */
	bool load(const QString &filename);

	/** Returns the repeater at the specified index. */
  const QJsonObject &repeater(int idx) const;

	/** Returns the age of the downloaded repeater database in days. */
	unsigned dbAge() const;

	/** Implements the QAbstractTableModel, return the number of rows (entries). */
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
	/** Implements the QAbstractTableModel, return the number of columns. */
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
	/** Implements the QAbstractTableModel, return the cell data. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

public slots:
	/** Starts the download of the repeater database from https://repeatermap.de */
	void download();

private slots:
	/** Internal callback on completed download. */
	void downloadFinished(QNetworkReply *reply);

private:
	/** My location. */
	QGeoCoordinate _qth;
	/** All repeaters sorted with respect to the distance to QTH. */
	QVector<QJsonObject>  _repeater;
	/** Table of callsigns. */
	QHash<QString, unsigned>  _callsigns;
	/** Network access. */
	QNetworkAccessManager _network;
};


/** A filter proxy for DMR repeaters.
 * @ingroup util */
class DMRRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
	/** Constructor. */
  explicit DMRRepeaterFilter(QObject *parent=nullptr);
};


/** A filter proxy for analog FM repeaters.
 * @ingroup util */
class FMRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
	/** Constructor. */
  explicit FMRepeaterFilter(QObject *parent=nullptr);
};

#endif // REPEATERDATABASE_HH
