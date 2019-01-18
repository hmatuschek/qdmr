#ifndef REPEATERDATABASE_HH
#define REPEATERDATABASE_HH

#include <QObject>
#include <QVector>
#include <QHash>
#include <QJsonObject>
#include <QNetworkAccessManager>


class RepeaterDatabase : public QObject
{
	Q_OBJECT

public:
	RepeaterDatabase(QObject *parent=nullptr);

	bool load();
	bool load(const QString &filename);

public slots:
	void download();

protected slots:
	void downloadFinished(QNetworkReply *reply);

protected:
	QVector<QJsonObject>  _repeater;
	QHash<QString, uint>  _callsigns;
	QNetworkAccessManager _network;
};

#endif // REPEATERDATABASE_HH
