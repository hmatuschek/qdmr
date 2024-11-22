#include "hearhamrepeatersource.hh"

#include "logger.hh"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


HearhamRepeaterSource::HearhamRepeaterSource(QObject *parent)
  : DownloadableRepeaterDatabaseSource(
      "hearham.cache.json", QUrl("https://hearham.com/api/repeaters/v1"), 5, parent)
{
  // pass...
}


bool
HearhamRepeaterSource::parse(const QByteArray &json) {
  QJsonParseError err;
  auto doc = QJsonDocument::fromJson(json, &err);
  if (doc.isNull()) {
    logError() << "Cannot received JSON: " << err.errorString() << ".";
    return false;
  }
  if (! doc.isArray()) {
    logError() << "Malformed result.";
    return false;
  }

  QRegularExpression ccPattern(R"(CC\w*([0-9]{1,2}))");
  QRegularExpression dcsPattern(R"(DCS\w*([0-9]{3})\w*(N|I|))");

  for (const QJsonValue &val: doc.array()) {
    if (! val.isObject())
      continue;
    auto obj = val.toObject();

    auto call = obj["callsign"].toString().simplified().toUpper();
    auto mode = obj["mode"].toString();
    auto rx = Frequency::fromHz(obj["frequency"].toInt()),
        tx = Frequency::fromHz(rx.inHz() + obj["offset"].toInt());
    auto location = QGeoCoordinate(obj["latitude"].toDouble(), obj["longitude"].toDouble());

    if ("FM" == mode) {
      SelectiveCall rxTone, txTone;
      auto rxDCS = dcsPattern.match(obj["decode"].toString());
      auto txDCS = dcsPattern.match(obj["encode"].toString());

      if (rxDCS.isValid())
        rxTone = SelectiveCall(rxDCS.captured(1).toUInt(), "I" == rxDCS.captured(2));
      else if (0 != obj["decode"].toString().toDouble())
        rxTone = SelectiveCall(obj["decode"].toString().toDouble());

      if (txDCS.isValid())
        txTone = SelectiveCall(txDCS.captured(1).toUInt(), "I" == txDCS.captured(2));
      else if (0 != obj["encode"].toString().toDouble())
        txTone = SelectiveCall(obj["encode"].toString().toDouble());

      cache(RepeaterDatabaseEntry::fm(call, rx, tx, location, rxTone, txTone));
    } else if ("DMR" == mode) {
      unsigned int colorCode = 0;
      auto CC = ccPattern.match(obj["encode"].toString());
      if (CC.isValid())
        colorCode = CC.captured(1).toUInt();

      cache(RepeaterDatabaseEntry::dmr(call, rx, tx, location, colorCode));
    }
  }

  return true;
}
