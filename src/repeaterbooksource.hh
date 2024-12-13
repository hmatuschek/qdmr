#ifndef REPEATERBOOKSOURCE_HH
#define REPEATERBOOKSOURCE_HH

#include "repeaterdatabase.hh"
#include <QNetworkAccessManager>


class RepeaterBookSource : public CachedRepeaterDatabaseSource
{
  Q_OBJECT

public:
  enum Region {
    World, NorthAmerica
  };
  Q_ENUM(Region)

public:
  explicit RepeaterBookSource(QObject *parent = nullptr);

protected slots:
  void onRequestFinished(QNetworkReply *reply);

protected:
  bool load(const QString &call, const QGeoCoordinate &pos);

protected:
  QNetworkAccessManager _network;
  QNetworkReply *_currentReply;
  QRegularExpression _callsignPattern;
};

#endif // REPEATERBOOKSOURCE_HH
