#include "transponderdatabase.hh"

#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of Transponder
 * ********************************************************************************************* */
Transponder::Transponder()
  : _satellite(0), _type(Type::Transmitter), _mode(Mode::CW), _name(), _downlink(), _uplink()
{
  // pass...
}

unsigned int
Transponder::satellite() const {
  return _satellite;
}

bool
Transponder::isValid() const {
  return 0 != _satellite;
}

Transponder::Type
Transponder::type() const {
  return _type;
}

Transponder::Mode
Transponder::mode() const {
  return _mode;
}

const QString &
Transponder::name() const {
  return _name;
}

const Frequency &
Transponder::uplink() const {
  return _uplink;
}

const Frequency &
Transponder::downlink() const {
  return _downlink;
}


Transponder
Transponder::fromSATNOGS(const QJsonObject &obj) {
  Transponder t;
  t._satellite = obj.value("norad_cat_id").toInt();
  t._name = obj.value("description").toString();
  if ((! obj.contains("alive")) || (! obj.value("alive").toBool())) {
    //logInfo() << "Skip transponder '" << t._name << "': no alive flag set or false.";
    return Transponder();
  }
  if ((! obj.contains("mode")) || obj.value("mode").isNull()) {
    //logInfo() << "Skip transponder '" << t._name << "': no mode set.";
    return Transponder();
  }
  if ((! obj.contains("downlink_low")) || (obj.value("downlink_low").isNull())) {
    //logInfo() << "Skip transponder '" << t._name << "': no downlink frequency given.";
    return Transponder();
  }

  QString tn = obj.value("type").toString();
  if (("Transmitter" == tn) || ("Beacon" == tn)) {
    t._type = Type::Transmitter;
  } else if ("Transceiver" == tn) {
    t._type = Type::Transponder;
  } else {
    //logInfo() << "Skip transponder '" << t._name << "': unknown type '" << tn << "'.";
    return Transponder();
  }

  QString mode = obj.value("mode").toString();
  if ("FM" == mode) {
    t._mode = Mode::FM;
  } else if ("AFSK" == mode) {
    t._mode = Mode::APRS;
  } else if ("CW" == mode) {
    t._mode = Mode::CW;
  } else if ("BPSK" == mode) {
    t._mode = Mode::BPSK;
  } else {
    //logInfo() << "Skip transponder '" << t._name << "': unknown mode '" << mode << "'.";
    return Transponder();
  }

  t._downlink = Frequency::fromHz(obj.value("downlink_low").toInt());
  if (obj.contains("uplink_low") && (! obj.value("uplink_low").isNull()))
    t._uplink = Frequency::fromHz(obj.value("uplink_low").toInt());

  return t;
}



/* ********************************************************************************************* *
 * Implementation of TransponderDatabase
 * ********************************************************************************************* */
TransponderDatabase::TransponderDatabase(bool autoLoad, unsigned int updatePeriod, QObject *parent)
  : QAbstractTableModel{parent}, _updatePeriod(updatePeriod), _transponders(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if (autoLoad) load();
}

const Transponder &
TransponderDatabase::getAt(unsigned int idx) const {
  return _transponders[idx];
}

int
TransponderDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _transponders.size();
}

int
TransponderDatabase::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 5;
}

QVariant
TransponderDatabase::data(const QModelIndex &index, int role) const {
  if (index.row() >= _transponders.size())
    return QVariant();

  if (Qt::DisplayRole == role) {
    if (0 == index.column()) {
      switch (_transponders.at(index.row()).type()) {
      case Transponder::Type::Transmitter: return "Transmitter";
      case Transponder::Type::Transponder: return "Transponder";
      }
    } else if (1 == index.column()) {
      switch (_transponders.at(index.row()).mode()) {
      case Transponder::Mode::CW: return "CW";
      case Transponder::Mode::BPSK: return "BPSK";
      case Transponder::Mode::FM: return "FM";
      case Transponder::Mode::APRS: return "APRS";
      }
    } else if (2 == index.column()) {
      return _transponders.at(index.row()).name();
    } else if (3 == index.column()) {
      return _transponders.at(index.row()).uplink().format();
    } else if (4 == index.column()) {
      return _transponders.at(index.row()).downlink().format();
    }
  }

  return QVariant();
}


TransponderDatabase::const_iterator
TransponderDatabase::begin() const {
  return _transponders.begin();
}

TransponderDatabase::const_iterator
TransponderDatabase::end() const {
  return _transponders.end();
}


unsigned
TransponderDatabase::dbAge() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/transponders.json";
  QFileInfo info(path);
  if (! info.exists())
    return -1;
  return info.lastModified().daysTo(QDateTime::currentDateTime());
}


void
TransponderDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/transponders.json";
  if ((! load(path)) || (_updatePeriod < dbAge()))
    download();
}


bool
TransponderDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    QString msg = QString("Cannot open transponders '%1': %2").arg(filename).arg(file.errorString());
    logError() << msg;
    emit error(msg);
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(data, &err);
  if (doc.isEmpty()) {
    QString msg = "Failed to load transponders: " + err.errorString();
    logError() << msg;
    emit error(msg);
    return false;
  }

  if (! doc.isArray()) {
    QString msg = "Failed to load transponders: JSON document is not an array!";
    logError() << msg;
    emit error(msg);
    return false;
  }

  beginResetModel();

  QJsonArray array = doc.array();
  _transponders.clear();
  _transponders.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    auto tranponder = Transponder::fromSATNOGS(array.at(i).toObject());
    if (tranponder.isValid()) {
      _transponders.append(tranponder);
    }
  }
  // Sort repeater w.r.t. their sat ID
  std::stable_sort(_transponders.begin(), _transponders.end(),
                   [](const Transponder &a, const Transponder &b){ return a.satellite() < b.satellite(); });

  // Done.
  endResetModel();

  logDebug() << "Loaded transponder with " << _transponders.size() << " entries from " << filename << ".";

  emit loaded();
  return true;
}


void
TransponderDatabase::download() {
  QUrl url("https://db.satnogs.org/api/transmitters/?format=json");
  QNetworkRequest request(url);
  _network.get(request);
}

void
TransponderDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    QString msg = QString("Cannot download transponders: %1").arg(reply->errorString());
    logError() << msg;
    emit error(msg);
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/transponders.json");

  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    QString msg = QString("Cannot create path '%1'.").arg(path);
    logError() << msg;
    emit error(msg);
    return;
  }

  if (! file.open(QIODevice::WriteOnly)) {
    QString msg = QString("Cannot save transponders at '%1'.").arg(file.fileName());
    logError() << msg;
    emit error(msg);
    return;
  }

  file.write(reply->readAll());
  file.flush();
  file.close();

  load();
  reply->deleteLater();
}


