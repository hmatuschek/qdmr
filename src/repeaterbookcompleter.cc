#include "repeaterbookcompleter.hh"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkReply>
#include "logger.hh"
#include <QStandardPaths>
#include <QDir>


/* ********************************************************************************************* *
 * RepeaterBookEntry
 * ********************************************************************************************* */
RepeaterBookEntry::RepeaterBookEntry(QObject *parent)
  : QObject(parent), _id(), _call(), _location(), _rxFrequency(0), _txFrequency(0), _isFM(false),
    _isDMR(false), _rxTone(Signaling::SIGNALING_NONE), _txTone(Signaling::SIGNALING_NONE),
    _colorCode(0), _timestamp(QDateTime::currentDateTime())
{
}

RepeaterBookEntry::RepeaterBookEntry(const RepeaterBookEntry &other)
  : QObject(other.parent()), _id(other._id), _call(other._call), _location(other._location),
    _rxFrequency(other._rxFrequency), _txFrequency(other._txFrequency), _isFM(other._isFM),
    _isDMR(other._isDMR), _rxTone(other._rxTone), _txTone(other._txTone),
    _colorCode(other._colorCode), _timestamp(other._timestamp)
{
  // pass...
}

RepeaterBookEntry &
RepeaterBookEntry::operator =(const RepeaterBookEntry &other) {
  _id = other._id;
  _call = other._call;
  _location = other._location;
  _rxFrequency = other._rxFrequency;
  _txFrequency = other._txFrequency;
  _isFM = other._isFM;
  _isDMR = other._isDMR;
  _rxTone = other._rxTone;
  _txTone = other._txTone;
  _colorCode = other._colorCode;
  _timestamp = other._timestamp;
  return *this;
}

bool
RepeaterBookEntry::isValid() const {
  return (!_call.isEmpty()) && (0!=_rxFrequency) && (0!=_txFrequency);
}

const QString &
RepeaterBookEntry::id() const {
  return _id;
}

const QString &
RepeaterBookEntry::call() const {
  return _call;
}

const QGeoCoordinate &
RepeaterBookEntry::location() const {
  return _location;
}

double
RepeaterBookEntry::rxFrequency() const {
  return _rxFrequency;
}
double
RepeaterBookEntry::txFrequency() const {
  return _txFrequency;
}

bool
RepeaterBookEntry::isFM() const {
  return _isFM;
}
bool
RepeaterBookEntry::isDMR() const {
  return _isDMR;
}

Signaling::Code
RepeaterBookEntry::rxTone() const {
  return _rxTone;
}
Signaling::Code
RepeaterBookEntry::txTone() const {
  return _txTone;
}

unsigned int
RepeaterBookEntry::colorCode() const {
  return _colorCode;
}

qint64
RepeaterBookEntry::age() const {
  return _timestamp.daysTo(QDateTime::currentDateTime());
}

bool
RepeaterBookEntry::fromRepeaterBook(const QJsonObject &obj) {
  // Handle repeater ID
  if (obj.contains("State ID"))
    _id = obj["State ID"].toString()+obj["Rptr ID"].toString();
  else if (obj.contains("id"))
    _id = obj["id"].toString();

  // Handle basic properties
  _call = obj["Callsign"].toString();
  _rxFrequency = obj["Frequency"].toString().toDouble();
  _txFrequency = obj["Input Freq"].toString().toDouble();
  _location = QGeoCoordinate(obj["Lat"].toString().toDouble(), obj["Long"].toString().toDouble());

  if (obj["FM Analog"].toString() == "Yes") {
    _isFM = true;
    if (obj.contains("PL"))
      _txTone = Signaling::fromCTCSSFrequency(obj["PL"].toString().toDouble());
    if (obj.contains("TSQ"))
      _rxTone = Signaling::fromCTCSSFrequency(obj["TSQ"].toString().toDouble());
  }
  if (obj["DMR"].toString() == "Yes") {
    _isDMR = true;
    if (obj.contains("DMR Color Code"))
      _colorCode = obj["DMR Color Code"].toString().toInt();
  }
  if (obj.contains("timestamp"))
    _timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
  return isValid();
}

bool
RepeaterBookEntry::fromCache(const QJsonObject &obj) {
  // Handle repeater ID
  _id = obj["id"].toString();
  // Handle basic properties
  _call = obj["Callsign"].toString();
  _rxFrequency = obj["Frequency"].toDouble();
  _txFrequency = obj["Input Freq"].toDouble();
  _location = QGeoCoordinate(obj["Lat"].toDouble(), obj["Long"].toDouble());
  _isFM = obj["FM Analog"].toBool();
  _isDMR = obj["DMR"].toBool();

  if (_isFM) {
    if (obj.contains("PL"))
      _txTone = Signaling::fromCTCSSFrequency(obj["PL"].toDouble());
    if (obj.contains("TSQ"))
      _rxTone = Signaling::fromCTCSSFrequency(obj["TSQ"].toDouble());
  }
  if (_isDMR) {
    if (obj.contains("DMR Color Code"))
      _colorCode = obj["DMR Color Code"].toInt();
  }
  if (obj.contains("timestamp"))
    _timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
  return isValid();
}

QJsonObject
RepeaterBookEntry::toCache() const {
  QJsonObject obj;
  obj.insert("id", _id);
  obj.insert("Callsign", _call);
  obj.insert("Frequency", _rxFrequency);
  obj.insert("Input Freq", _txFrequency);
  obj.insert("Lat", _location.latitude());
  obj.insert("Long", _location.longitude());
  obj.insert("timestamp", _timestamp.toString(Qt::ISODate));
  obj.insert("FM Analog", _isFM);
  obj.insert("DMR", _isDMR);
  if (_isFM) {
    if (Signaling::SIGNALING_NONE != _txTone)
      obj.insert("PL", Signaling::toCTCSSFrequency(_txTone));
    if (Signaling::SIGNALING_NONE != _rxTone)
      obj.insert("TSQ", Signaling::toCTCSSFrequency(_rxTone));
  }
  if (_isDMR) {
    obj.insert("DMR Color Code", (int)_colorCode);
  }
  return obj;
}


/* ********************************************************************************************* *
 * RepeaterBookList
 * ********************************************************************************************* */
RepeaterBookList::RepeaterBookList(QObject *parent)
  : QAbstractListModel(parent), _network(), _currentReply(nullptr)
{
  load();
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(onRequestFinished(QNetworkReply*)));
}

int
RepeaterBookList::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return _items.count();
}

QVariant
RepeaterBookList::data(const QModelIndex &index, int role) const {
  if (index.row() >= _items.count())
    return QVariant();
  return _items[index.row()].call();
}

QString
RepeaterBookList::cachePath() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    logError() << "Cannot create path '" << path << "'.";
    return "";
  }
  return path+"/repeaterbook.json";
}

QString
RepeaterBookList::queryPath() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    logError() << "Cannot create path '" << path << "'.";
    return "";
  }
  return path+"/repeaterbook.query.json";
}

bool
RepeaterBookList::load() {
  QFile file(cachePath());
  if (! file.open(QIODevice::ReadOnly)) {
    logInfo() << "Cannot open repeater cache '" << file.fileName() << "'.";
    return false;
  }

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
  if (doc.isNull()) {
    logError() << "Cannot parse '" << file.fileName() << "': " << err.errorString() << ".";
    return false;
  }
  file.close();

  beginResetModel();
  _items.clear();
  foreach (const QJsonValue &rep, doc.array()) {
    RepeaterBookEntry entry;
    if (! entry.fromCache(rep.toObject()))
      continue;
    if (5 < entry.age())
      continue;
    _items.append(entry);
  }
  endResetModel();

  logDebug() << "Loaded repeater cache of " << _items.count() << " entries.";

  file.setFileName(queryPath());
  if (!file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open query cache '" << file.fileName()
               << "': " << file.errorString() << ".";
    return false;
  }

  doc = QJsonDocument::fromJson(file.readAll(), &err);
  if (doc.isNull()) {
    logError() << "Cannot parse '" << file.fileName() << "': " << err.errorString() << ".";
    return false;
  }
  file.close();

  if (! doc.isArray()) {
    logError() << "Unexpected format for query cache: Expected array.";
    return false;
  }

  foreach (const QJsonValue &entry, doc.array()) {
    if (! entry.isObject())
      continue;
    QJsonObject obj = entry.toObject();
    if ((! obj.contains("query")) || (! obj.contains("timestamp")))
      continue;
    _queries[obj["query"].toString()] = QDateTime::fromString(
          obj["timestamp"].toString(), Qt::ISODate);
  }

  return true;
}

bool
RepeaterBookList::store() const {
  QFile file(cachePath());
  if (!file.open(QIODevice::WriteOnly)) {
    logError() << "Cannot open repeater cache '" << file.fileName() << "': "
               << file.errorString();
    return false;
  }

  QJsonArray array;
  foreach (const RepeaterBookEntry &entry, _items) {
    array.append(entry.toCache());
  }

  file.write(QJsonDocument(array).toJson());
  file.flush();
  file.close();

  logDebug() << "Stored repeater cache of " << array.count() << " entries.";

  file.setFileName(queryPath());
  if (! file.open(QIODevice::WriteOnly)) {
    logError() << "Cannot open repeater queries '" << file.fileName() << "': "
               << file.errorString();
    return false;
  }

  array = QJsonArray();
  QHashIterator<QString, QDateTime> iter(_queries);
  while (iter.hasNext()) {
    iter.next();
    QJsonObject obj;
    obj.insert("query", iter.key());
    obj.insert("timestamp", iter.value().toString(Qt::ISODate));
    array.append(obj);
  }

  file.write(QJsonDocument(array).toJson());
  file.flush();
  file.close();

  return true;
}

void
RepeaterBookList::search(const QString &call) {
  // Chancel running requests
  if (_currentReply)
    _currentReply->abort();

  if ((_queries.contains(call)) && (_queries[call].daysTo(QDateTime::currentDateTime())<3))
    return;

  QUrl url("https://www.repeaterbook.com/api/exportROW.php");
  QUrlQuery query;
  query.addQueryItem("callsign", QString("%1%").arg(call.toUpper()));
  url.setQuery(query);
  logDebug() << "Query RepeaterBook at " << url.toString();
  QNetworkRequest request(url);
  _currentReply = _network.get(request);
}

void
RepeaterBookList::onRequestFinished(QNetworkReply *reply) {
  if (reply->error()) {
    logError() << "Cannot download repeater list: " << reply->errorString();
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &err);
  if (doc.isNull()) {
    logError() << "Cannot parse response: " << err.errorString() << ".";
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QString query = QUrlQuery(reply->request().url()).queryItemValue("callsign", QUrl::FullyDecoded).remove("%");
  _queries[query] = QDateTime::currentDateTime();

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
    RepeaterBookEntry entry;
    if (! entry.fromRepeaterBook(rep.toObject()))
      continue;
    updateEntry(entry);
  }

  logDebug() << "Updated repeater cache with " << results.count() << " entries.";


  store();
}

bool
RepeaterBookList::updateEntry(const RepeaterBookEntry &entry) {
  for (int i=0; i<_items.count(); i++) {
    // Update entry
    if (_items[i].id() == entry.id()) {
      _items[i] = entry;
      return true;
    }
  }

  // append entry
  beginInsertRows(QModelIndex(), _items.count(), _items.count());
  _items.append(entry);
  endInsertRows();
  return true;
}


/* ********************************************************************************************* *
 * RepeaterBookCompleter
 * ********************************************************************************************* */
RepeaterBookCompleter::RepeaterBookCompleter(int minPrefixLength, QObject *parent)
  : QCompleter(parent), _repeaterList(new RepeaterBookList(this)),
    _minPrefixLength(minPrefixLength)
{
  setModel(_repeaterList);
  setCaseSensitivity(Qt::CaseInsensitive);
}

QStringList
RepeaterBookCompleter::splitPath(const QString &path) const {
  if (path.length() >= _minPrefixLength)
    _repeaterList->search(path);
  return QCompleter::splitPath(path);
}

