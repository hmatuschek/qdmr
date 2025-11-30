#include "repeaterdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QtConcurrent>
#include "logger.hh"
#include "utils.hh"



/* ********************************************************************************************* *
 * Helper functions
 * ********************************************************************************************* */
static const QSet<double> _aprs_frequencies = {
  144.390, 144.575, 144.660, 144.800, 144.930, 145.175, 145.570, 432.500
};

inline QString bandName(const Frequency &F) {
  if (30 >= F.inMHz())
    return "HF";
  else if (300 >= F.inMHz())
    return "VHF";
  else if (3000 >= F.inMHz())
    return "UHF";
  else if (30000 >= F.inMHz())
    return "SHF";
  return "EHF";
}

inline QString bandName(const Frequency &rx, const Frequency & tx) {
  QString rband = bandName(rx), tband=bandName(tx);
  if ((rx == tx) && (_aprs_frequencies.contains(rx.inMHz())))
    return "APRS";
  else if (rband == tband)
    return rband;
  return QString("%1/%2").arg(rband,tband);
}



/* ********************************************************************************************* *
 * Implementation of RepeaterDatabaseEntry
 * ********************************************************************************************* */
RepeaterDatabaseEntry::RepeaterDatabaseEntry(const QString &call, const Frequency &rxFrequency,
                                             const Frequency &txFrequency,
                                             const QGeoCoordinate &location,
                                             const QString &qth,
                                             const SelectiveCall &rxTone,
                                             const SelectiveCall &txTone,
                                             const QDateTime &updated, const QDateTime& loaded)
  : _type(Type::FM), _call(call), _rxFrequency(rxFrequency), _txFrequency(txFrequency),
    _location(location), _qth(qth), _rxTone(rxTone), _txTone(txTone), _colorCode(0),
    _updated(updated), _loaded(loaded)
{
  // pass...
}

RepeaterDatabaseEntry::RepeaterDatabaseEntry(const QString &call, const Frequency &rxFrequency,
                                             const Frequency &txFrequency,
                                             const QGeoCoordinate &location, const QString &qth,
                                             unsigned int colorCode,
                                             const QDateTime &updated, const QDateTime& loaded)
  : _type(Type::DMR), _call(call), _rxFrequency(rxFrequency), _txFrequency(txFrequency),
    _location(location), _qth(qth), _rxTone(), _txTone(), _colorCode(colorCode),
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

  return (other._call == _call) && (other._rxFrequency == _rxFrequency);
}

bool
RepeaterDatabaseEntry::operator<(const RepeaterDatabaseEntry &other) const {
  return (_type < other._type) || (_call < other._call) || (_rxFrequency < other._rxFrequency);
}

QJsonValue
RepeaterDatabaseEntry::toJson() const {
  QJsonObject obj;

  switch (type()) {
  case Type::Invalid: return QJsonValue();
  case Type::FM: obj.insert("type", "FM"); break;
  case Type::DMR: obj.insert("type", "DMR"); break;
  case Type::M17: obj.insert("type", "M17"); break;
  }

  obj.insert("call", call());

  obj.insert("rx", rxFrequency().format());
  if (txFrequency().inHz())
    obj.insert("tx", txFrequency().format());

  obj.insert("latitude", location().latitude());
  obj.insert("longitude", location().longitude());
  obj.insert("qth", qth());

  if (Type::FM == type()) {
    if (rxTone().isInvalid()) obj.insert("rx-tone", rxTone().format());
    if (txTone().isInvalid()) obj.insert("tx-tone", txTone().format());
  } else if (Type::DMR == type()) {
    obj.insert("color-code", (int)colorCode());
  }

  if (updated().isValid())
    obj.insert("updated", updated().toString(Qt::ISODate));
  if (loaded().isValid())
    obj.insert("loaded", loaded().toString(Qt::ISODate));

  return obj;
}


RepeaterDatabaseEntry &
RepeaterDatabaseEntry::operator+=(const RepeaterDatabaseEntry &other) {
  if ((_type != other.type()) || (_call != other.call()))
    return *this;

  if ((!_loaded.isValid()) || (_loaded < other._loaded))
    return (*this)=other;

  if ((!_updated.isValid()) || (_updated < other._updated))
    return (*this)=other;

  if (! _location.isValid())
    _location = other._location;
  if (_qth.isEmpty())
    _qth = other.qth();

  if (Type::FM == _type) {
    if (_rxTone.isInvalid())
      _rxTone = other.rxTone();
    if (_txTone.isInvalid())
      _txTone = other.txTone();
  } else if (Type::DMR == _type) {
    if (0 == _colorCode)
      _colorCode = other.colorCode();
  }

  return *this;
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

QString
RepeaterDatabaseEntry::locator() const {
  return deg2loc(_location);
}

const QString &
RepeaterDatabaseEntry::qth() const {
  return _qth;
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
                          const Frequency &txFrequency, const QGeoCoordinate &location, const QString &qth,
                          const SelectiveCall &rxTone, const SelectiveCall &txTone,
                          const QDateTime &updated, const QDateTime& loaded)
{
  return RepeaterDatabaseEntry(
    call.simplified().toUpper(), rxFrequency, txFrequency,
        location, qth, rxTone, txTone, updated, loaded);
}

RepeaterDatabaseEntry
RepeaterDatabaseEntry::dmr(const QString &call, const Frequency &rxFrequency,
                           const Frequency &txFrequency, const QGeoCoordinate &location, const QString &qth,
                           unsigned int colorCode,
                           const QDateTime &updated, const QDateTime& loaded)
{
  return RepeaterDatabaseEntry(
    call.simplified().toUpper(), rxFrequency, txFrequency,
        location, qth, colorCode, updated, loaded);
}

RepeaterDatabaseEntry
RepeaterDatabaseEntry::fromJson(const QJsonObject &obj) {
  if (obj.isEmpty() || (! obj.contains("type")))
    return RepeaterDatabaseEntry();

  Type type = Type::Invalid;
  if ("FM" == obj.value("type").toString())
    type = Type::FM;
  else if ("DMR" == obj.value("type").toString())
    type = Type::DMR;
  else if ("M17" == obj.value("type").toString())
    type = Type::M17;

  QString call = obj.value("call").toString().simplified().toUpper();

  Frequency rx = Frequency::fromString(obj.value("rx").toString()), tx;
  if (obj.contains("tx"))
    tx = Frequency::fromString(obj.value("tx").toString());

  QGeoCoordinate location(obj.value("latitude").toDouble(),
                          obj.value("longitude").toDouble());

  QString qth = obj.value("qth").toString();

  QDateTime updated, loaded;
  if (obj.contains("updated"))
    updated = QDateTime::fromString(obj.value("updated").toString(), Qt::ISODate);
  if (obj.contains("loaded"))
    loaded = QDateTime::fromString(obj.value("loaded").toString(), Qt::ISODate);

  if (Type::FM == type) {
    SelectiveCall rxTone, txTone;
    if (obj.contains("rx-tone"))
      rxTone = SelectiveCall::parseCTCSS(obj.value("rx-tone").toString());
    if (obj.contains("tx-tone"))
      txTone = SelectiveCall::parseCTCSS(obj.value("tx-tone").toString());
    return RepeaterDatabaseEntry::fm(
          call, rx, tx, location, qth, rxTone, txTone, updated, loaded);
  } else if (Type::DMR == type) {
    unsigned int colorCode = 0;
    if (obj.contains("color-code"))
      colorCode = obj.value("color-code").toInt();
    return RepeaterDatabaseEntry::dmr(
          call, rx, tx, location, qth, colorCode, updated, loaded);
  }

  return RepeaterDatabaseEntry();
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

  _parsing = QtConcurrent::run([this](){ this->loadCache(); });
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
    _cache.append(entry);
    emit updated(entry);
  }

  logDebug() << "Loaded " << _cache.size() << " entries from '" << _cacheFile.fileName() << "'.";
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
  if (! _indices.contains(entry)) {
    _indices[entry] = _cache.size();
    _cache.append(entry);
  } else {
    _cache[_indices[entry]] += entry;
  }

  emit updated(entry);
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
 * Implementation of DownloadableRepeaterDatabaseSource
 * ********************************************************************************************* */
DownloadableRepeaterDatabaseSource::DownloadableRepeaterDatabaseSource(
    const QString &filename, const QUrl &source, unsigned int maxAge, QObject *parent)
  : CachedRepeaterDatabaseSource(filename, parent), _url(source), _maxAge(maxAge), _network(),
    _currentReply(nullptr)
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(onRequestFinished(QNetworkReply*)));

  if (needsUpdate())
    download();
}

bool
DownloadableRepeaterDatabaseSource::needsUpdate() const {
  QFileInfo info(_cacheFile);
  return (! info.exists()) || (info.lastModified().daysTo(QDateTime::currentDateTime()) > _maxAge);
}


bool
DownloadableRepeaterDatabaseSource::load(const QString &call, const QGeoCoordinate &pos) {
  Q_UNUSED(call); Q_UNUSED(pos);
  // No action needed
  return true;
}


void
DownloadableRepeaterDatabaseSource::download() {
  // Cancel running requests
  if (_currentReply)
    _currentReply->abort();

  QNetworkRequest request(_url);
  request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/115.0.0.0 Safari/537.36 Edg/114.0.1823.86");

  logDebug() << "Query " << _url.toString() << "'.";
  _currentReply = _network.get(request);
}


void
DownloadableRepeaterDatabaseSource::onRequestFinished(QNetworkReply *reply) {
  if (reply->error()) {
    logError() << "Cannot download repeater list: " << reply->errorString();
    reply->deleteLater();
    _currentReply = nullptr;
    return;
  }

  QByteArray content = reply->readAll();

  reply->deleteLater();
  _currentReply = nullptr;

  if (parse(content))
    saveCache();
}



/* ********************************************************************************************* *
 * Implementation of RepeaterDatabase
 * ********************************************************************************************* */
RepeaterDatabase::RepeaterDatabase(QObject *parent)
  : QAbstractListModel{parent}, _sources(), _indices(), _entries()
{
  // pass...
}


void
RepeaterDatabase::addSource(RepeaterDatabaseSource *source) {
  if ((nullptr == source) || _sources.contains(source))
    return;

  _sources.append(source);
  source->setParent(this);
  connect(source, &RepeaterDatabaseSource::updated, this, &RepeaterDatabase::merge);

  for (unsigned int i=0; i<source->count(); i++) {
    merge(source->get(i));
  }
}


void
RepeaterDatabase::merge(const RepeaterDatabaseEntry &entry) {
  if (_indices.contains(entry)) {
    int row = _indices[entry];
    RepeaterDatabaseEntry myEntry(_entries[row]);
    if ((! myEntry.updated().isValid()) ||
        (myEntry.updated().isValid() && entry.updated().isValid() &&
         (myEntry.updated() < entry.updated()))) {
      _entries[_indices[entry]] = entry;
      emit dataChanged(index(row), index(row));
    }
    return;
  }

  int row = _entries.size();
  beginInsertRows(QModelIndex(), row, row);
  _entries.append(entry);
  _indices[entry] = row;
  endInsertRows();
}


int
RepeaterDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _entries.size();
}

RepeaterDatabaseEntry
RepeaterDatabase::get(unsigned int idx) const {
  if (idx >= (unsigned int)_entries.size())
    return RepeaterDatabaseEntry();
  return _entries[idx];
}

QVariant
RepeaterDatabase::data(const QModelIndex &index, int role) const {
  if (index.row() >= _entries.count())
    return QVariant();

  if (Qt::EditRole == role) {
    return _entries[index.row()].call();
  } else if (Qt::DisplayRole == role) {
    return QString("%1 (%2, %3, %4)")
        .arg(_entries[index.row()].call())
        .arg(_entries[index.row()].qth())
        .arg(bandName(_entries[index.row()].rxFrequency(),
             _entries[index.row()].txFrequency()))
        .arg(_entries[index.row()].locator());
  }
  return QVariant();
}


bool
RepeaterDatabase::query(const QString &call, const QGeoCoordinate &pos) {
  for (auto source: _sources) {
    source->query(call, pos);
  }
  return true;
}



