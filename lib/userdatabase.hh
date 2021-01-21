#ifndef USERDATABASE_HH
#define USERDATABASE_HH

#include <QObject>
#include <QVector>
#include <QHash>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QGeoPositionInfoSource>

/** Auto-updating DMR user database.
 *
 * This class repesents the complete DMR user database. The user database gets downloaded from
 * https://www.radioid.net/static/users.json and kept up-to-date by re-downloading it
 * periodically (by default every 30 days). This user database gets used in the GUI application
 * to help assemble private call contacts and to assemble so-called CSV callsign databases, that
 * are programmable to some DMR radios to resolve the DMR ID to callsigns and names.
 *
 * @ingroup util */
class UserDatabase : public QAbstractTableModel
{
	Q_OBJECT

public:
	/** Represents the user information within the @c UserDatabase. */
	class User {
	public:
		/** Empty constructor. */
		User();
		/** Constructs entry from JSON object. */
		User(const QJsonObject &obj);

		/** Returns @c true if the entry is valid. */
		inline bool isValid() const { return 0 != id; }

    /** Returns the "distance" between this user and the given ID. */
    uint distance(uint id) const;

		/** The DMR ID of the user. */
		uint id;
		/** The callsign of the user. */
		QString call;
		/** The name of the user. */
		QString name;
		/** The surname of the user. */
		QString surname;
		/** The country of the user. */
		QString country;
	};

public:
	/** Constructs the user-database.
	 * The constructor will download the current user database if it was not downloaded yet or
	 * if the downloaded version is older than @c updatePeriodDays days. */
	explicit UserDatabase(uint updatePeriodDays=30, QObject *parent=nullptr);

  /** Returns the number of users. */
  qint64 count() const;

	/** Loads all entries from the downloaded user database. */
	bool load();
	/** Loads all entries from the downloaded user database at the specified location. */
	bool load(const QString &filename);

  /** Sorts users with respect to the distance to the given ID. */
  void sortUsers(uint id);

	/** Returns the user with index @c idx. */
  const User &user(int idx) const;

	/** Returns the age of the database in days. */
	uint dbAge() const;

	/** Implements the QAbstractTableModel interface, returns the number of rows (number of entries). */
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
	/** Implements the QAbstractTableModel interface, returns the number of columns. */
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
	/** Implements the QAbstractTableModel interface, return the entry data. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

signals:
  /** Gets emitted once the call-sign database has been loaded. */
  void loaded();
  /** Gets emitted if the loading of the call-sign database fails. */
  void error(const QString &msg);

public slots:
	/** Starts the download of the user database. */
	void download();

private slots:
	/** Gets called whenever the download is complete. */
	void downloadFinished(QNetworkReply *reply);

private:
	/** Holds all users sorted by their ID. */
	QVector<User>         _user;
	/** The network access used for downloading. */
	QNetworkAccessManager _network;
};


#endif // USERDATABASE_HH
