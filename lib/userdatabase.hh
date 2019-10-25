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


class UserDatabase : public QAbstractTableModel
{
	Q_OBJECT

public:
	class User {
	public:
		User();
		User(const QJsonObject &obj);

		inline bool isValid() const { return 0 != id; }

		uint id;
		QString call;
		QString name;
		QString surname;
		QString country;
	};

public:
	explicit UserDatabase(uint updatePeriodDays=30, QObject *parent=nullptr);

	bool load();
	bool load(const QString &filename);

  const User &user(int idx) const;

	uint dbAge() const;

  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

public slots:
	void download();

protected slots:
	void downloadFinished(QNetworkReply *reply);

protected:
	QVector<User>         _user;
	QNetworkAccessManager _network;
};


#endif // REPEATERDATABASE_HH
