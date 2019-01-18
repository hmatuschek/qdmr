#include "repeaterdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QNetworkReply>

#include "settings.hh"

RepeaterDatabase::RepeaterDatabase(QObject *parent)
  : QObject(parent), _repeater(), _callsigns(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if ((! load()) || Settings().repeaterUpdateNeeded())
    download();
}

bool
RepeaterDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return load(path+"/repeater.json");
}

bool
RepeaterDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    qDebug() << "Cannot open repeater list" << filename;
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (! doc.isObject())
    return false;
  if (! doc.object().contains("repeaters"))
    return false;
  if (! doc.object()["repeaters"].isArray())
    return false;

  _repeater.clear();
  _callsigns.clear();

  QJsonArray array = doc.object()["repeaters"].toArray();
  _repeater.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    QJsonObject repeater = array.at(i).toObject();
    _repeater.append(repeater);
    _callsigns[repeater["callsign"].toString()] = i;
  }
  qDebug() << "Loaded repeater list with" << _repeater.size() << "entries.";
  return true;
}

void
RepeaterDatabase::download() {
  QUrl url("https://ham-digital.org/status/rptrs.json");
  QNetworkRequest request(url);
  _network.get(request);
}

void
RepeaterDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    qDebug() << "Cannot download repeater list:" << reply->errorString();
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/repeater.json");
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    qDebug() << "Cannot create path" << path;
    return;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    qDebug() << "Cannot save repeaterlist at" << (path+"/repeater.json");
    return;
  }

  file.write(reply->readAll());
  file.flush();
  file.close();

  load();

  qDebug() << "Updated repeater list with" << _repeater.size() << "entries.";
  Settings().repeaterUpdated();

  reply->deleteLater();
}
