#include "repeaterbooksource.hh"

#include <QNetworkReply>
#include <QRegularExpressionMatch>
#include <QUrlQuery>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "logger.hh"
#include "settings.hh"
#include "config.h"


RepeaterBookSource::RepeaterBookSource(QObject *parent)
  : CachedRepeaterDatabaseSource("repeaterbook.cache.json", parent), _network(),
    _currentReply(nullptr),
    _callsignPattern(R"re(([a-z]|[a-z0-9][a-z]|[a-z][a-z0-9])[0-9]+[a-z]*)re",
                     QRegularExpression::CaseInsensitiveOption)
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(onRequestFinished(QNetworkReply*)));
}


bool
RepeaterBookSource::load(const QString &queryCall, const QGeoCoordinate &pos) {
  Q_UNUSED(pos);

  // Cancel running requests
  if (_currentReply)
    _currentReply->abort();

  QRegularExpressionMatch match = _callsignPattern.match(queryCall);
  if (! match.hasMatch())
    return false;
  QString call = match.captured().toUpper();
  logDebug() << "Search for (partial) call '" << call << "'.";

  QUrl url;
  if (Region::World == Settings().repeaterBookRegion())
    url = QUrl("https://www.repeaterbook.com/api/exportROW.php");
  else
    url = QUrl("https://www.repeaterbook.com/api/export.php");

  QUrlQuery query;
  query.addQueryItem("callsign", QString("%1%").arg(call));
  url.setQuery(query);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::UserAgentHeader,
                    "qdmr " VERSION_STRING " https://dm3mat.darc.de/qdmr/");
  logDebug() << "Query RepeaterBook at " << url.toString()
             << " as '" << request.header(QNetworkRequest::UserAgentHeader).toString() << "'.";

  _currentReply = _network.get(request);

  return true;
}


void
RepeaterBookSource::onRequestFinished(QNetworkReply *reply) {
  if (reply->error()) {
    logError() << "Cannot download repeater list: " << reply->errorString();
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QByteArray content = reply->readAll();
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(content, &err);
  if (doc.isNull()) {
    logError() << "Cannot parse response: " << err.errorString() << ".";
    logDebug() << "Got '" << content << "'.";
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QString query = QUrlQuery(reply->request().url())
      .queryItemValue("callsign", QUrl::FullyDecoded).remove("%");

  reply->deleteLater();
  _currentReply = nullptr;

  if ((! doc.isObject()) || (! doc.object().contains("results")) || (! doc.object()["results"].isArray())) {
    logError() << "Cannot parse response: Unexpected structure.";
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QJsonArray results = doc.object()["results"].toArray();
  foreach (const QJsonValue &rep, results) {
    if (! rep.isObject())
      continue;
    QJsonObject obj = rep.toObject();

    // Handle basic properties
    auto call = obj["Callsign"].toString().simplified().toUpper();
    auto rxFrequency = Frequency::fromMHz(obj["Frequency"].toString().toDouble());
    auto txFrequency = Frequency::fromMHz(obj["Input Freq"].toString().toDouble());
    auto location = QGeoCoordinate(obj["Lat"].toString().toDouble(), obj["Long"].toString().toDouble());
    auto qth = obj["Nearest City"].toString();

    auto type = RepeaterDatabaseEntry::Type::Invalid;
    SelectiveCall rxTone, txTone;
    unsigned int colorCode = 0;
    QDateTime updated;

    if (obj["FM Analog"].toString() == "Yes") {
      type = RepeaterDatabaseEntry::Type::FM;
      if (obj.contains("PL"))
        txTone = SelectiveCall(obj["PL"].toString().toDouble());
      if (obj.contains("TSQ"))
        rxTone = SelectiveCall(obj["TSQ"].toString().toDouble());
    }

    if (obj["DMR"].toString() == "Yes") {
      type = RepeaterDatabaseEntry::Type::FM;
      if (obj.contains("DMR Color Code"))
        colorCode = obj["DMR Color Code"].toString().toInt();
    }

    if (obj.contains("timestamp"))
      updated = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);

    if (RepeaterDatabaseEntry::Type::FM == type) {
      cache(RepeaterDatabaseEntry::fm(call, rxFrequency, txFrequency, location, qth,
                                      rxTone, txTone, updated));
    } else if (RepeaterDatabaseEntry::Type::DMR == type) {
      cache(RepeaterDatabaseEntry::dmr(call, rxFrequency, txFrequency, location, qth,
                                       colorCode, updated));
    }
  }

  logDebug() << "Updated repeater cache with " << results.count() << " entries.";

  saveCache();
}
