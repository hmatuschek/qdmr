#include "repeaterdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of RepeaterDatabaseEntry
 * ********************************************************************************************* */
RepeaterDatabaseEntry::RepeaterDatabaseEntry(const QString &call, const Frequency &rxFrequency,
                                             const Frequency &txFrequency,
                                             const QGeoCoordinate &location,
                                             const SelectiveCall &rxTone,
                                             const SelectiveCall &txTone,
                                             const QDateTime &updated, const QDateTime& loaded)
  : _type(Type::FM), _call(call), _rxFrequency(rxFrequency), _txFrequency(txFrequency),
    _location(location), _rxTone(rxTone), _txTone(txTone), _colorCode(0),
    _updated(updated), _loaded(loaded)
{
  // pass...
}

RepeaterDatabaseEntry::RepeaterDatabaseEntry(const QString &call, const Frequency &rxFrequency,
                                             const Frequency &txFrequency,
                                             const QGeoCoordinate &location,
                                             unsigned int colorCode,
                                             const QDateTime &updated, const QDateTime& loaded)
  : _type(Type::FM), _call(call), _rxFrequency(rxFrequency), _txFrequency(txFrequency),
    _location(location), _rxTone(), _txTone(), _colorCode(colorCode),
    _updated(updated), _loaded(loaded)
{
  // pass...
}

RepeaterDatabaseEntry::RepeaterDatabaseEntry()
  : _type(Type::Invalid), _call(), _rxFrequency(), _txFrequency(), _location(),
    _rxTone(), _txTone(), _colorCode(0), _updated()
{
  // pass...
}

bool
RepeaterDatabaseEntry::operator==(const RepeaterDatabaseEntry &other) const {
  if (other._type != _type)
    return false;
  // Invalid entries are all equal
  if (Type::Invalid == _type)
    return true;

  return (other._call == _call) && (other._rxFrequency == _rxFrequency)
      && (other._txFrequency == _txFrequency);
}

bool
RepeaterDatabaseEntry::isValid() const {
  return (Type::Invalid != _type) && (! _call.isEmpty()) && (_rxFrequency.inHz())
      && (_txFrequency.inHz());
}

RepeaterDatabaseEntry::Type
RepeaterDatabaseEntry::type() const {
  return _type;
}

const QString &
RepeaterDatabaseEntry::call() const {
  return _call;
}

const Frequency &
RepeaterDatabaseEntry::rxFrequency() const {
  return _rxFrequency;
}
const Frequency &
RepeaterDatabaseEntry::txFrequency() const {
  return _txFrequency;
}

const QGeoCoordinate &
RepeaterDatabaseEntry::location() const {
  return _location;
}

const SelectiveCall &
RepeaterDatabaseEntry::rxTone() const {
  return _rxTone;
}
const SelectiveCall &
RepeaterDatabaseEntry::txTone() const {
  return _txTone;
}

unsigned int
RepeaterDatabaseEntry::colorCode() const {
  return _colorCode;
}

const QDateTime &
RepeaterDatabaseEntry::updated() const {
  return _updated;
}
const QDateTime &
RepeaterDatabaseEntry::loaded() const {
  return _loaded;
}


RepeaterDatabaseEntry
RepeaterDatabaseEntry::fm(const QString &call, const Frequency &rxFrequency,
                          const Frequency &txFrequency, const QGeoCoordinate &location,
                          const SelectiveCall &rxTone, const SelectiveCall &txTone,
                          const QDateTime &updated, const QDateTime& loaded)
{
  return RepeaterDatabaseEntry{
    call.simplified().toUpper(), rxFrequency, txFrequency,
        location, rxTone, txTone, updated, loaded};
}

RepeaterDatabaseEntry
RepeaterDatabaseEntry::dmr(const QString &call, const Frequency &rxFrequency,
                           const Frequency &txFrequency, const QGeoCoordinate &location,
                           unsigned int colorCode,
                           const QDateTime &updated, const QDateTime& loaded)
{
  return RepeaterDatabaseEntry{
    call.simplified().toUpper(), rxFrequency, txFrequency,
        location, colorCode, updated, loaded};
}



/* ********************************************************************************************* *
 * Implementation of RepeaterDatabaseSource
 * ********************************************************************************************* */
RepeaterDatabaseSource::RepeaterDatabaseSource(QObject *parent)
  : QObject{parent}
{
  // pass...
}

bool
RepeaterDatabaseSource::query(const QString &call, const QGeoCoordinate &location) {
  return this->load(call, location);
}

unsigned int
RepeaterDatabaseSource::count() const {
  return 0;
}

RepeaterDatabaseEntry
RepeaterDatabaseSource::get(unsigned int idx) const {
  Q_UNUSED(idx);
  return RepeaterDatabaseEntry();
}



/* ********************************************************************************************* *
 * Implementation of CachedRepeaterDatabaseSource
 * ********************************************************************************************* */
CachedRepeaterDatabaseSource::CachedRepeaterDatabaseSource(const QString &filename, QObject *parent)
  : RepeaterDatabaseSource{parent}
{
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    logError() << "Cannot create path '" << path << "'.";
    return;
  }

  _cacheFile.setFileName(path + "/" + filename);

  loadCache();
}


void
CachedRepeaterDatabaseSource::loadCache() {
  if (! _cacheFile.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open cache '" << _cacheFile.fileName()
               << "': " << _cacheFile.errorString() << ".";
    return;
  }

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(_cacheFile.readAll(), &error);
  _cacheFile.close();

  if (doc.isNull()) {
    logError() << "Cannot parse cache '" << _cacheFile.fileName()
               << "': " << error.errorString() << ".";
    return;
  }

  if (! doc.isArray()) {
    logError() << "Malformed cache file.";
    return;
  }

  _cache.clear();

  for (QJsonValue obj: doc.array()) {
    if (! obj.isObject())
      continue;
    RepeaterDatabaseEntry entry = RepeaterDatabaseEntry::fromJson(obj.toObject());
    if (! entry.isValid())
      continue;
    int idx = _cache.size();
    _cache.append(entry);
    emit updated(idx);
  }
}


void
CachedRepeaterDatabaseSource::saveCache() {
  if (! _cacheFile.open(QIODevice::WriteOnly)) {
    logError() << "Cannot open cache '" << _cacheFile.fileName()
               << "': " << _cacheFile.errorString() << ".";
    return;
  }

  QJsonArray entries;
  for (auto entry: _cache)
    entries.append(entry.toJson());
  if (! _cacheFile.write(QJsonDocument(entries).toJson())) {
    logError() << "Cannot write cache '" << _cacheFile.fileName()
               << "': " << _cacheFile.errorString() << ".";
  }
  _cacheFile.flush();
  _cacheFile.close();
}


void
CachedRepeaterDatabaseSource::cache(const RepeaterDatabaseEntry &entry) {
  unsigned int idx=0; auto iter = _cache.begin();
  for (; iter != _cache.end(); ++iter, idx++) {
    if (entry == *iter) {
      *iter = entry;
      break;
    }
  }

  if (idx != (unsigned int)_cache.size()) {
    _cache.append(entry);
  }

  emit updated(idx);
}


unsigned int
CachedRepeaterDatabaseSource::count() const {
  return _cache.size();
}

RepeaterDatabaseEntry
CachedRepeaterDatabaseSource::get(unsigned int idx) const {
  return _cache.value(idx, RepeaterDatabaseEntry());
}

bool
CachedRepeaterDatabaseSource::query(const QString &call, const QGeoCoordinate &location) {
  QString query = call.simplified().toUpper();
  QDateTime newest;
  for (const RepeaterDatabaseEntry &entry: _cache) {
    if (entry.call().startsWith(query) && entry.loaded().isValid())
      if ((! newest.isValid()) || (newest < entry.loaded()))
        newest = entry.loaded();
  }

  if (newest.isValid() && (newest.daysTo(QDateTime::currentDateTime()) <= _maxAge))
    return true;

  return RepeaterDatabaseSource::query(call, location);
}



/* ********************************************************************************************* *
 * Implementation of RepeaterDatabase
 * ********************************************************************************************* */
RepeaterDatabase::RepeaterDatabase(QObject *parent)
  : QObject{parent}
{

}
