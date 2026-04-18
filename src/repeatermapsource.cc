#include "repeatermapsource.hh"
#include "logger.hh"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QUrlQuery>

#include <qtimer.h>

RepeaterMapSource::RepeaterMapSource(const QByteArray &apiToken, const QGeoCoordinate &position,
  unsigned int radius, QObject *parent)
  : DownloadableRepeaterDatabaseSource(
      "repeatermap.cache.json",
      QUrl("https://repeatermap.de/api/v1/repeaters.php"),
      5, parent), _apiToken(apiToken), _searchPosition(position), _searchRadius(radius)
{
  // pass...
}


void
RepeaterMapSource::setSearchRadius(const QGeoCoordinate &pos, unsigned int radius) {
  // Store position
  QGeoCoordinate oldPos = _searchPosition;
  _searchRadius = radius;
  _searchPosition = pos;
  // If position has moved by 10% of the search radius -> update
  if (_searchRadius && _searchPosition.isValid() && _searchPosition.distanceTo(oldPos) > 100*_searchRadius)
    QTimer::singleShot(0, this, &RepeaterMapSource::download);
}


bool
RepeaterMapSource::prepareRequest(QNetworkRequest &request) {
  // Set API token
  request.setRawHeader("X-API-Token", _apiToken.toHex().toLower());

  // If position & radius is set -> add to request
  if (_searchPosition.isValid() && _searchRadius) {
    QUrl url(request.url());
    QUrlQuery query(url);
    logDebug() << "Get repeater near " << _searchPosition.toString()
    << " (radius " << _searchRadius << "km) from repeatermap.de";
    query.addQueryItem("lat", QString::number(_searchPosition.latitude()));
    query.addQueryItem("lon", QString::number(_searchPosition.longitude()));
    query.addQueryItem("radius", QString::number(_searchRadius));
    url.setQuery(query);
    request.setUrl(url);
  } else {
    logInfo() << "No search position/radius set, request list of german repeaters from repeatermap.de.";
  }

  // Done.
  return DownloadableRepeaterDatabaseSource::prepareRequest(request);
}


bool
RepeaterMapSource::parse(const QByteArray &json) {
  QJsonParseError err;
  auto doc = QJsonDocument::fromJson(json, &err);
  if (doc.isNull()) {
    logError() << "Cannot received JSON: " << err.errorString() << ".";
    return false;
  }
  if ((! doc.isObject()) || (! doc.object().contains("repeaters"))
    || (! doc.object()["repeaters"].isArray())) {
    logError() << "Malformed result.";
    return false;
  }

  QJsonArray list = doc.object()["repeaters"].toArray();
  for (auto val: list) {
    if (! val.isObject())
      continue;
    auto repeater = val.toObject();

    auto call = repeater["call"].toString().simplified().toUpper();
    auto mode = repeater["mode"].toString();
    auto tx = Frequency::fromMHz(repeater["tx"].toDouble());
    auto rx = Frequency::fromMHz(repeater["rx"].toDouble());
    auto position = QGeoCoordinate(repeater["lat"].toDouble(), repeater["lon"].toDouble());
    auto qth = repeater["qth"].toString();

    if ("FM" == mode) {
      cache(RepeaterDatabaseEntry::fm(call, rx, tx, position, qth));
    } else if ("DMR" == mode) {
      cache(RepeaterDatabaseEntry::dmr(call, rx, tx, position, qth));
    }
  }

  logDebug() << "Loaded " << _cache.size() << " elements from " << _url.toDisplayString() << ".";

  return true;
}
