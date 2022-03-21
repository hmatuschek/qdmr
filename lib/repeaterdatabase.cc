#include "repeaterdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <algorithm>
#include "logger.hh"
#include <QSet>

static const QSet<double> _aprs_frequencies = {
  144.390, 144.575, 144.660, 144.800, 144.930, 145.175, 145.570, 432.500
};

inline QString bandName(double MHz) {
  if (30 >= MHz)
    return "HF";
  else if (300 >= MHz)
    return "VHF";
  else if (3000 >= MHz)
    return "UHF";
  else if (30000 >= MHz)
    return "SHF";
  return "EHF";
}

inline QString bandName(double rx, double tx) {
  QString rband = bandName(rx), tband=bandName(tx);
  if ((rx == tx) && (_aprs_frequencies.contains(rx)))
    return "APRS";
  else if (rband == tband)
    return rband;
  return QString("%1/%2").arg(rband,tband);
}


/// @cond with_internal_docs
/** "is-less" Operator for sorting repeaters with respect to the distace. */
class DistanceIsLess
{
public:
  /** Constructor, @c qth specifies the QTH location. */
  inline DistanceIsLess(const QGeoCoordinate &qth): _qth(qth) { }

  /** Compares two repeaters with respect to the distance to the QTH. */
  inline bool operator()(const QJsonObject &a, const QJsonObject &b) {
    QGeoCoordinate al(a.value("lat").toDouble(), a.value("lon").toDouble());
    QGeoCoordinate bl(b.value("lat").toDouble(), b.value("lon").toDouble());
    return _qth.distanceTo(al) < _qth.distanceTo(bl);
  }

private:
  /** QTH location. */
  QGeoCoordinate  _qth;
};
/// @endcond


RepeaterDatabase::RepeaterDatabase(const QGeoCoordinate &qth, unsigned updatePeriodDays, QObject *parent)
  : QAbstractTableModel(parent), _qth(qth), _repeater(), _callsigns(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if ((! load()) || (updatePeriodDays < dbAge()))
    download();
}

bool
RepeaterDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return load(path+"/repeater.json");
}

const QJsonObject &
RepeaterDatabase::repeater(int idx) const {
  return _repeater[idx];
}

bool
RepeaterDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open repeater list '" << filename << "'.";
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (! doc.isObject()) {
    logError() << "Failed to load repeater DB: JSON document is not an object!";
    return false;
  }
  if (! doc.object().contains("relais")) {
    logError() << "Failed to load repeater DB: JSON object does not contain 'relais' item.";
    return false;
  }
  if (! doc.object()["relais"].isArray()) {
    logError() << "Failed to load repeater DB: 'relais' item is not an array.";
    return false;
  }

  beginResetModel();
  _repeater.clear();
  _callsigns.clear();

  QJsonArray array = doc.object()["relais"].toArray();
  _repeater.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    QJsonObject repeater = array.at(i).toObject();
    _repeater.append(repeater);
    _callsigns[repeater["callsign"].toString()] = i;
  }
  // Sort repeater w.r.t. distance to me
  if (_qth.isValid())
    std::stable_sort(_repeater.begin(), _repeater.end(), DistanceIsLess(_qth));
  // Done.
  endResetModel();

  logDebug() << "Loaded repeater database with " << _repeater.size() << " entries from " << filename << ".";

  return true;
}

void
RepeaterDatabase::download() {
  QUrl url("https://repeatermap.de/api.php");
  QNetworkRequest request(url);
  _network.get(request);
}

void
RepeaterDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    logError() << "Cannot download repeater list: " << reply->errorString();
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/repeater.json");
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    logError() << "Cannot create path '" << path << "'.";
    return;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    logError() << "Cannot save repeaterlist at '" << (path+"/repeater.json") << "'.";
    return;
  }

  file.write(reply->readAll());
  file.flush();
  file.close();

  load();
  reply->deleteLater();
}

unsigned
RepeaterDatabase::dbAge() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/repeater.json";
  QFileInfo info(path);
  if (! info.exists())
    return -1;
  return info.lastModified().daysTo(QDateTime::currentDateTime());
}

int
RepeaterDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _repeater.size();
}

int
RepeaterDatabase::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 7;
}

QVariant
RepeaterDatabase::data(const QModelIndex &index, int role) const {
  if ((Qt::EditRole != role) && ((Qt::DisplayRole != role)))
    return QVariant();

  if (index.row() >= _repeater.size())
    return QVariant();

  if (0 == index.column()) {
    // Call
    if (Qt::DisplayRole == role)
      return tr("%1 (%2, %3, %4)")
          .arg(_repeater[index.row()].value("call").toString())
          .arg(bandName(_repeater[index.row()].value("rx").toDouble(),
                 _repeater[index.row()].value("tx").toDouble()))
          .arg(_repeater[index.row()].value("qth").toString())
          .arg(_repeater[index.row()].value("locator").toString());
      else
          return _repeater[index.row()].value("call").toString();
  } else if (1 == index.column()) {
    // Mode
    return _repeater[index.row()].value("mode").toString();
  } else if (2 == index.column()) {
    // Repeater TX
    if (Qt::DisplayRole == role)
      return _repeater[index.column()].value("tx").toString();
    else
      return _repeater[index.column()].value("tx").toDouble();
  } else if (3 == index.column()) {
    // Repeater RX
    if (Qt::DisplayRole == role)
      return _repeater[index.column()].value("rx").toString();
    else
      return _repeater[index.column()].value("rx").toDouble();
  } else if (4 == index.column()) {
    // Locator
    return _repeater[index.row()].value("locator").toString();
  } else if (5 == index.column()) {
    // Repeater position lon
    if (Qt::DisplayRole == role)
      return _repeater[index.column()].value("lon").toString();
    else
      return _repeater[index.column()].value("lon").toDouble();
  } else if (6 == index.column()) {
    // Repeater position lat
    if (Qt::DisplayRole == role)
      return _repeater[index.column()].value("lat").toString();
    else
      return _repeater[index.column()].value("lat").toDouble();
  }

  return QVariant();
}

