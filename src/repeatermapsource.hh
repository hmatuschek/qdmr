#ifndef REPEATERMAPSOURCE_HH
#define REPEATERMAPSOURCE_HH

#include "repeaterdatabase.hh"


class RepeaterMapSource : public DownloadableRepeaterDatabaseSource
{
  Q_OBJECT

public:
  explicit RepeaterMapSource(const QByteArray &apiToken, const QGeoCoordinate &position,
    unsigned int radius, QObject *parent = nullptr);

  void setSearchRadius(const QGeoCoordinate &position, unsigned int radius) override;

protected:
  bool prepareRequest(QNetworkRequest &request) override;
  bool parse(const QByteArray &doc) override;

protected:
  QByteArray _apiToken;
  QGeoCoordinate _searchPosition;
  unsigned int _searchRadius;
};

#endif // REPEATERMAPSOURCE_HH
