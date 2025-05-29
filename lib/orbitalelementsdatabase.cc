#include "orbitalelementsdatabase.hh"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFileInfo>
#include <QNetworkReply>
#include <QDir>
#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of OrbitalElement::Epoch
 * ********************************************************************************************* */
OrbitalElement::Epoch::Epoch()
  : year(0), month(0), day(0), hour(0), minute(0), second(0), microsecond(0)
{
  // pass...
}

OrbitalElement::Epoch::Epoch(unsigned int pyear, unsigned int pmonth, unsigned int pday,
                        unsigned int phour, unsigned int pminute, unsigned int psecond,
                        unsigned int pmicrosecond)
  : year(pyear), month(pmonth), day(pday), hour(phour), minute(pminute), second(psecond),
    microsecond(pmicrosecond)
{
  // pass...
}


OrbitalElement::Epoch
OrbitalElement::Epoch::parse(const QString &datetime) {
  QRegularExpression pattern("([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{6})");
  QRegularExpressionMatch match = pattern.match(datetime);
  if (! match.isValid())
    return Epoch();
  return Epoch{
    match.captured(1).toUInt(),
    match.captured(2).toUInt(),
    match.captured(3).toUInt(),
    match.captured(4).toUInt(),
    match.captured(5).toUInt(),
    match.captured(6).toUInt(),
    match.captured(7).toUInt()
  };
}

double
OrbitalElement::Epoch::toEpoch() const {
  QDate date(year, month, day);
  double res = microsecond;
  res = (res / 1e6) + second;
  res = (res / 60)  + minute;
  res = (res / 60)  + hour;
  res = (res / 24)  + date.dayOfYear();
  return res;
}

QString
OrbitalElement::Epoch::toString() const {
  return QString("%1-%2-%3T%4:%5:%6.%7")
                 .arg(year, 4, 10, QChar('0'))
                 .arg(month, 2, 10, QChar('0'))
                 .arg(day, 2, 10, QChar('0'))
                 .arg(hour, 2, 10, QChar('0'))
                 .arg(minute, 2, 10, QChar('0'))
                 .arg(second, 2, 10, QChar('0'))
                 .arg(microsecond, 6, 10, QChar('0'));
}



/* ********************************************************************************************* *
 * Implementation of OrbitalElement
 * ********************************************************************************************* */
OrbitalElement::OrbitalElement()
  : _id(0), _name(), _epoch(), _meanMotion(0.0), _meanMotionDerivative(0.0),
    _inclination(0.0), _ascension(0.0), _eccentricity(0.0), _perigee(0.0), _meanAnomaly(0.0),
    _revolutionNumber(0)
{
  // pass...
}

OrbitalElement::OrbitalElement(unsigned int id)
  : _id(id), _name(), _epoch(), _meanMotion(0.0), _meanMotionDerivative(0.0),
    _inclination(0.0), _ascension(0.0), _eccentricity(0.0), _perigee(0.0), _meanAnomaly(0.0),
    _revolutionNumber(0)
{
  // pass...
}


bool
OrbitalElement::isValid() const {
  return 0 != _id;
}

unsigned int
OrbitalElement::id() const {
  return _id;
}

const QString &
OrbitalElement::name() const {
  return _name;
}

const OrbitalElement::Epoch &
OrbitalElement::epoch() const {
  return _epoch;
}

double
OrbitalElement::meanMotion() const {
  return _meanMotion;
}

double
OrbitalElement::meanMotionDerivative() const {
  return _meanMotionDerivative;
}

double
OrbitalElement::inclination() const {
  return _inclination;
}

double
OrbitalElement::ascension() const {
  return _ascension;
}

double
OrbitalElement::eccentricity() const {
  return _eccentricity;
}

double
OrbitalElement::perigee() const {
  return _perigee;
}

double
OrbitalElement::meanAnomaly() const {
  return _meanAnomaly;
}

unsigned int
OrbitalElement::revolutionNumber() const {
  return _revolutionNumber;
}


OrbitalElement
OrbitalElement::fromCelesTrak(const QJsonObject &obj) {
  OrbitalElement el;

  el._name = obj.value("OBJECT_NAME").toString();
  el._epoch = Epoch::parse(obj.value("EPOCH").toString());
  el._meanMotion = obj.value("MEAN_MOTION").toDouble();
  el._meanMotionDerivative = obj.value("MEAN_MOTION_DOT").toDouble();
  el._inclination = obj.value("INCLINATION").toDouble();
  el._ascension = obj.value("RA_OF_ASC_NODE").toDouble();
  el._eccentricity = obj.value("ECCENTRICITY").toDouble();
  el._perigee = obj.value("ARG_OF_PERICENTER").toDouble();
  el._meanAnomaly = obj.value("MEAN_ANOMALY").toDouble();
  el._revolutionNumber = obj.value("REV_AT_EPOCH").toInt();

  el._id = obj.value("NORAD_CAT_ID").toInt();

  return el;
}



/* ********************************************************************************************* *
 * Implementation of OrbitalElementsDatabase
 * ********************************************************************************************* */
OrbitalElementsDatabase::OrbitalElementsDatabase(bool autoLoad, unsigned int updatePeriod, QObject *parent)
  : QAbstractTableModel{parent}, _updatePeriod(updatePeriod), _elements(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if (autoLoad) load();
}


bool
OrbitalElementsDatabase::contains(unsigned int id) const {
  return _idIndexMap.contains(id);
}

OrbitalElement
OrbitalElementsDatabase::getById(unsigned int id) const {
  return _elements.at(_idIndexMap.value(id));
}

const OrbitalElement &
OrbitalElementsDatabase::getAt(unsigned int idx) const {
  return _elements[idx];
}

OrbitalElement &
OrbitalElementsDatabase::getAt(unsigned int idx) {
  return _elements[idx];
}


int
OrbitalElementsDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _elements.size();
}

int
OrbitalElementsDatabase::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 3;
}

QVariant
OrbitalElementsDatabase::data(const QModelIndex &index, int role) const {
  if (index.row() >= _elements.size())
    return QVariant();

  if (Qt::DisplayRole == role) {
    if (0 == index.column())
      return _elements.at(index.row()).id();
    if (1 == index.column())
      return _elements.at(index.row()).name();
    if (2 == index.column())
      return _elements.at(index.row()).epoch().toString();
  }

  return QVariant();
}

QVariant
OrbitalElementsDatabase::headerData(int section, Qt::Orientation orientation,
                                int role) const {
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    return QVariant();

  switch (section) {
      case 0: return QStringLiteral("NORAD");
      case 1: return QStringLiteral("Name");
      case 2: return QStringLiteral("Epoch");
  }

  return QVariant();
}

unsigned
OrbitalElementsDatabase::dbAge() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/elements.json";
  QFileInfo info(path);
  if (! info.exists())
    return -1;
  return info.lastModified().daysTo(QDateTime::currentDateTime());
}


void
OrbitalElementsDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/elements.json";
  if ((! load(path)) || (_updatePeriod < dbAge()))
    download();
}


bool
OrbitalElementsDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    QString msg = QString("Cannot open orbital elements '%1': %2").arg(filename).arg(file.errorString());
    logError() << msg;
    emit error(msg);
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(data, &err);
  if (doc.isEmpty()) {
    QString msg = "Failed to load orbital elements: " + err.errorString();
    logError() << msg;
    emit error(msg);
    return false;
  }

  if (! doc.isArray()) {
    QString msg = "Failed to load orbital elements: JSON document is not an array!";
    logError() << msg;
    emit error(msg);
    return false;
  }

  beginResetModel();

  QJsonArray array = doc.array();
  _elements.clear();
  _elements.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    OrbitalElement element = OrbitalElement::fromCelesTrak(array.at(i).toObject());
    if (element.isValid()) {
      _elements.append(element);
    }
  }
  // Sort repeater w.r.t. their IDs
  std::stable_sort(_elements.begin(), _elements.end(),
                   [](const OrbitalElement &a, const OrbitalElement &b){ return a.id() < b.id(); });

  _idIndexMap.clear();
  for (unsigned int i=0; i<(unsigned int)_elements.size(); i++)
    _idIndexMap.insert(_elements.at(i).id(), i);

  // Done.
  endResetModel();

  logDebug() << "Loaded orbital elements with " << _elements.size() << " entries from " << filename << ".";

  emit loaded();

  return true;
}


void
OrbitalElementsDatabase::download() {
  QUrl url("https://celestrak.org/NORAD/elements/gp.php?GROUP=amateur&FORMAT=json");
  QNetworkRequest request(url);
  _network.get(request);
}

void
OrbitalElementsDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    QString msg = QString("Cannot download orbital elements: %1").arg(reply->errorString());
    logError() << msg;
    emit error(msg);
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/elements.json");

  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    QString msg = QString("Cannot create path '%1'.").arg(path);
    logError() << msg;
    emit error(msg);
    return;
  }

  if (! file.open(QIODevice::WriteOnly)) {
    QString msg = QString("Cannot save orbital elements at '%1'.").arg(file.fileName());
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

