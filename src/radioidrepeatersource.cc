#include "radioidrepeatersource.hh"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonParseError>

#include "logger.hh"



RadioidRepeaterSource::RadioidRepeaterSource(QObject *parent)
  : DownloadableRepeaterDatabaseSource(
      "radioidrepeater.cache.json", QUrl("https://radioid.net/static/map.json"), 5, parent)
{
  // pass...
}


bool
RadioidRepeaterSource::parse(const QByteArray &json) {
  QJsonParseError err;
  auto doc = QJsonDocument::fromJson(json, &err);
  if (doc.isNull()) {
    logError() << "Cannot received JSON: " << err.errorString() << ".";
    return false;
  }

  if ((! doc.isObject()) || (! doc.object().contains("markers")) || (! doc.object()["markers"].isArray())) {
    logError() << "Malformed result.";
    return false;
  }

  QJsonArray list = doc.object()["markers"].toArray();
  for (auto val: list) {
    if (! val.isObject())
      continue;

    auto repeater = val.toObject();
    if ("ACTIVE" != repeater["status"].toString())
      continue;

    auto call = repeater["callsign"].toString().simplified().toUpper();
    auto rx = Frequency::fromMHz(repeater["frequency"].toString().toDouble());
    auto tx = Frequency::fromMHz(rx.inMHz() +  repeater["offset"].toString().toDouble());
    auto colorCode = repeater["color_code"].toInt();
    auto position = QGeoCoordinate(repeater["lat"].toString().toDouble(),
        repeater["lng"].toString().toDouble());
    auto qth = repeater["city"].toString();

    cache(RepeaterDatabaseEntry::dmr(call, rx, tx, position, qth, colorCode));
  }

  logDebug() << "Loaded " << _cache.size() << " elements from " << _url.toDisplayString() << ".";
  return true;
}
