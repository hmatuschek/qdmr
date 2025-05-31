#include "repeatermapsource.hh"
#include "logger.hh"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


RepeaterMapSource::RepeaterMapSource(QObject *parent)
  : DownloadableRepeaterDatabaseSource(
      "repeatermap.cache.json", QUrl("https://repeatermap.de/apinew.php"), 5, parent)
{
  // pass...
}


bool
RepeaterMapSource::parse(const QByteArray &json) {
  QJsonParseError err;
  auto doc = QJsonDocument::fromJson(json, &err);
  if (doc.isNull()) {
    logError() << "Cannot received JSON: " << err.errorString() << ".";
    return false;
  }
  if ((! doc.isObject()) || (! doc.object().contains("relais")) || (! doc.object()["relais"].isArray())) {
    logError() << "Malformed result.";
    return false;
  }

  QJsonArray list = doc.object()["relais"].toArray();
  for (auto val: list) {
    if (! val.isObject())
      continue;
    auto repeater = val.toObject();

    auto call = repeater["call"].toString().simplified().toUpper();
    auto mode = repeater["mode"].toString();
    auto rx = Frequency::fromMHz(repeater["tx"].toDouble());
    auto tx = Frequency::fromMHz(repeater["rx"].toDouble());
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
