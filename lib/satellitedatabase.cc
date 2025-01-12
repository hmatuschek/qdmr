#include "satellitedatabase.hh"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of Satellite
 * ********************************************************************************************* */
Satellite::Satellite()
  : OrbitalElement(), _name(),
    _fmUplink(), _fmDownlink(), _fmUplinkTone(), _fmDownlinkTone(),
    _aprsUplink(), _aprsDownlink(), _aprsUplinkTone(), _aprsDownlinkTone(), _beacon()
{
  // pass...
}

Satellite::Satellite(const OrbitalElement &orbit)
  : OrbitalElement(orbit), _name(orbit.name()),
    _fmUplink(), _fmDownlink(), _fmUplinkTone(), _fmDownlinkTone(),
    _aprsUplink(), _aprsDownlink(), _aprsUplinkTone(), _aprsDownlinkTone(), _beacon()
{
  // pass...
}


const QString &
Satellite::name() const {
  return _name;
}

void
Satellite::setName(const QString &name) {
  _name = name.simplified();
}


const Frequency &
Satellite::fmUplink() const {
  return _fmUplink;
}

void
Satellite::setFMUplink(const Frequency &f) {
  _fmUplink = f;
}


const Frequency &
Satellite::fmDownlink() const {
  return _fmDownlink;
}

void
Satellite::setFMDownlink(const Frequency &f) {
  _fmDownlink = f;
}


const SelectiveCall &
Satellite::fmUplinkTone() const {
  return _fmUplinkTone;
}

void
Satellite::setFMUplinkTone(const SelectiveCall &tone) {
  _fmUplinkTone = tone;
}


const SelectiveCall &
Satellite::fmDownlinkTone() const {
  return _fmDownlinkTone;
}

void
Satellite::setFMDownlinkTone(const SelectiveCall &tone) {
  _fmDownlinkTone = tone;
}


const Frequency &
Satellite::aprsUplink() const {
  return _aprsUplink;
}

void
Satellite::setAPRSUplink(const Frequency &f) {
  _aprsUplink = f;
}

const Frequency &
Satellite::aprsDownlink() const {
  return _aprsDownlink;
}

void
Satellite::setAPRSDownlink(const Frequency &f) {
  _aprsDownlink = f;
}


const SelectiveCall &
Satellite::aprsUplinkTone() const {
  return _aprsUplinkTone;
}

void
Satellite::setAPRSUplinkTone(const SelectiveCall &tone) {
  _aprsUplinkTone = tone;
}

const SelectiveCall &
Satellite::aprsDownlinkTone() const {
  return _aprsDownlinkTone;
}

void
Satellite::setAPRSDownlinkTone(const SelectiveCall &tone) {
  _aprsDownlinkTone = tone;
}

const Frequency &
Satellite::beacon() const {
  return _beacon;
}

void
Satellite::setBeacon(const Frequency &f) {
  _beacon = f;
}


QJsonObject
Satellite::toJson() const {
  if (! isValid())
    return QJsonObject();

  QJsonObject o = QJsonObject();
  o.insert("norad", QJsonValue(qint64(id())));
  o.insert("name", name());

  if (0 != fmUplink().inHz())
    o.insert("fm_uplink", fmUplink().format());
  if (fmUplinkTone().isValid())
    o.insert("fm_uplink_tone", fmUplinkTone().format());

  if (0 != fmDownlink().inHz())
    o.insert("fm_downlink", fmDownlink().format());
  if (fmDownlinkTone().isValid())
    o.insert("fm_downlink_tone", fmDownlinkTone().format());

  if (0 != aprsUplink().inHz())
    o.insert("aprs_uplink", aprsUplink().format());
  if (aprsUplinkTone().isValid())
    o.insert("aprs_uplink_tone", aprsUplinkTone().format());

  if (0 != aprsDownlink().inHz())
    o.insert("aprs_downlink", aprsDownlink().format());
  if (aprsDownlinkTone().isValid())
    o.insert("aprs_downlink_tone", aprsDownlinkTone().format());

  if (0 != beacon().inHz())
    o.insert("beacon", beacon().format());

  return o;
}


Satellite
Satellite::fromJson(const QJsonObject &obj, const OrbitalElementsDatabase &db) {
  unsigned int id = obj.value("norad").toInt();
  QString name = obj.value("name").toString();

  if (! db.contains(id))
    return Satellite();

  Satellite sat(db.getById(id));
  sat._name = name;

  if (obj.contains("fm_uplink"))
    sat._fmUplink.parse(obj.value("fm_uplink").toString());
  if (obj.contains("fm_uplink_tone"))
    sat._fmUplinkTone = SelectiveCall::parseCTCSS(obj.value("fm_uplink_tone").toString());
  if (obj.contains("fm_downlink"))
    sat._fmDownlink.parse(obj.value("fm_downlink").toString());
  if (obj.contains("fm_downlink_tone"))
    sat._fmDownlinkTone = SelectiveCall::parseCTCSS(obj.value("fm_downlink_tone").toString());

  if (obj.contains("aprs_uplink"))
    sat._aprsUplink.parse(obj.value("aprs_uplink").toString());
  if (obj.contains("aprs_uplink_tone"))
    sat._aprsUplinkTone = SelectiveCall::parseCTCSS(obj.value("aprs_uplink_tone").toString());
  if (obj.contains("aprs_downlink"))
    sat._aprsDownlink.parse(obj.value("aprs_downlink").toString());
  if (obj.contains("aprs_downlink_tone"))
    sat._aprsDownlinkTone = SelectiveCall::parseCTCSS(obj.value("aprs_downlink_tone").toString());

  if (obj.contains("beacon"))
    sat._beacon.parse(obj.value("beacon").toString());

  return sat;
}



/* ********************************************************************************************* *
 * Implementation of SatelliteDatabase
 * ********************************************************************************************* */
SatelliteDatabase::SatelliteDatabase(unsigned int updatePeriod, QObject *parent)
  : QAbstractTableModel{parent}, _satellites(), _orbitalElements(false, updatePeriod),
    _transponders(false, updatePeriod)
{
  connect(&_orbitalElements, &OrbitalElementsDatabase::loaded,
          this, &SatelliteDatabase::load);

  _orbitalElements.load();
  _transponders.load();
}


const OrbitalElementsDatabase  &
SatelliteDatabase::orbitalElements() const {
  return _orbitalElements;
}

const TransponderDatabase  &
SatelliteDatabase::transponders() const {
  return _transponders;
}

OrbitalElementsDatabase  &
SatelliteDatabase::orbitalElements() {
  return _orbitalElements;
}

TransponderDatabase  &
SatelliteDatabase::transponders() {
  return _transponders;
}

unsigned int
SatelliteDatabase::count() const {
  return _satellites.count();
}

const Satellite &
SatelliteDatabase::getAt(unsigned int idx) const {
  return _satellites[idx];
}

void
SatelliteDatabase::add(const Satellite &sat) {
  if (! sat.isValid())
    return;

  beginInsertRows(QModelIndex(), _satellites.count(), _satellites.count());
  _satellites.append(sat);
  endInsertRows();
}


bool
SatelliteDatabase::removeRows(int row, int count, const QModelIndex &parent) {
  if ((row >= _satellites.count()) || ((row+count) > _satellites.count()))
    return false;

  if (0 == count)
    return true;

  beginRemoveRows(parent, row, row+count-1);
  _satellites.remove(row, count);
  endRemoveRows();

  return true;
}


int
SatelliteDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _satellites.size();
}

int
SatelliteDatabase::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 11;
}

QVariant
SatelliteDatabase::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::Horizontal != orientation) || (Qt::DisplayRole != role))
    return QVariant();

  switch (section) {
  case 0: return tr("NORAD");
  case 1: return tr("Name");
  case 2: return tr("FM Downlink Frequency");
  case 3: return tr("FM Uplink Frequency");
  case 4: return tr("FM Downlink Tone");
  case 5: return tr("FM Uplink Tone");
  case 6: return tr("APRS Downlink Frequency");
  case 7: return tr("APRS Uplink Frequency");
  case 8: return tr("APRS Downlink Tone");
  case 9: return tr("APRS Uplink Tone");
  case 10: return tr("Beacon Frequency");
  }

  return QVariant();
}


Qt::ItemFlags
SatelliteDatabase::flags(const QModelIndex &index) const {
  Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

  // Name
  if (1 == index.column())
    f |= Qt::ItemIsEditable;
  // FM up/downlink frequencies
  if ((2 == index.column()) || (3 == index.column()))
    f |= Qt::ItemIsEditable;
  // FM up/downlink sub tones
  if ((4 == index.column()) || (5 == index.column()))
    f |= Qt::ItemIsEditable;
  // APRS up/downlink frequencies
  if ((6 == index.column()) || (7 == index.column()))
    f |= Qt::ItemIsEditable;
  // APRS up/downlink sub tones
  if ((8 == index.column()) || (9 == index.column()))
    f |= Qt::ItemIsEditable;
  // Beacon
  if (10 == index.column())
    f |= Qt::ItemIsEditable;

  return f;
}


QVariant
SatelliteDatabase::data(const QModelIndex &index, int role) const {
  if (index.row() >= _satellites.count())
    return QVariant();

  if (Qt::DisplayRole == role) {
    switch (index.column()) {
    case 0: return _satellites.at(index.row()).id();
    case 1: return _satellites.at(index.row()).name();
    case 2:
      return (0 == _satellites.at(index.row()).fmDownlink().inHz()) ?
            tr("None") : _satellites.at(index.row()).fmDownlink().format();
    case 3:
      return (0 == _satellites.at(index.row()).fmUplink().inHz()) ?
            tr("None") : _satellites.at(index.row()).fmUplink().format();
    case 4: return _satellites.at(index.row()).fmDownlinkTone().format();
    case 5: return _satellites.at(index.row()).fmUplinkTone().format();
    case 6:
      return (0 == _satellites.at(index.row()).aprsDownlink().inHz()) ?
            tr("None") : _satellites.at(index.row()).aprsDownlink().format();
    case 7:
      return (0 == _satellites.at(index.row()).aprsUplink().inHz()) ?
            tr("None") : _satellites.at(index.row()).aprsUplink().format();
    case 8: return _satellites.at(index.row()).aprsDownlinkTone().format();
    case 9: return _satellites.at(index.row()).aprsUplinkTone().format();
    case 10:
      return (0 == _satellites.at(index.row()).beacon().inHz()) ?
            tr("None") : _satellites.at(index.row()).beacon().format();
    }
  } else if (Qt::EditRole == role) {
    switch (index.column()) {
    case 0: return _satellites.at(index.row()).id();
    case 1: return _satellites.at(index.row()).name();
    case 2: return QVariant::fromValue(_satellites.at(index.row()).fmDownlink());
    case 3: return QVariant::fromValue(_satellites.at(index.row()).fmUplink());
    case 4: return QVariant::fromValue(_satellites.at(index.row()).fmDownlinkTone());
    case 5: return QVariant::fromValue(_satellites.at(index.row()).fmUplinkTone());
    case 6: return QVariant::fromValue(_satellites.at(index.row()).aprsDownlink());
    case 7: return QVariant::fromValue(_satellites.at(index.row()).aprsUplink());
    case 8: return QVariant::fromValue(_satellites.at(index.row()).aprsDownlinkTone());
    case 9: return QVariant::fromValue(_satellites.at(index.row()).aprsUplinkTone());
    case 10: return QVariant::fromValue(_satellites.at(index.row()).beacon());
    }
  }

  return QVariant();
}


bool
SatelliteDatabase::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (Qt::EditRole != role)
    return false;
  if (index.row() >= _satellites.count())
    return false;

  switch (index.column()) {
  case 1: _satellites[index.row()].setName(value.toString().simplified()); return true;
  case 2: _satellites[index.row()].setFMDownlink(value.value<Frequency>()); return true;
  case 3: _satellites[index.row()].setFMUplink(value.value<Frequency>()); return true;
  case 4: _satellites[index.row()].setFMDownlinkTone(value.value<SelectiveCall>()); return true;
  case 5: _satellites[index.row()].setFMUplinkTone(value.value<SelectiveCall>()); return true;
  case 6: _satellites[index.row()].setAPRSDownlink(value.value<Frequency>()); return true;
  case 7: _satellites[index.row()].setAPRSUplink(value.value<Frequency>()); return true;
  case 8: _satellites[index.row()].setAPRSDownlinkTone(value.value<SelectiveCall>()); return true;
  case 9: _satellites[index.row()].setAPRSUplinkTone(value.value<SelectiveCall>()); return true;
  case 10: _satellites[index.row()].setBeacon(value.value<Frequency>()); return true;
  }

  return false;
}


void
SatelliteDatabase::update() {
  _orbitalElements.download();
  _transponders.download();
}

void
SatelliteDatabase::load() {
  QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/satellites.json";
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    QString msg = QString("Cannot open satellites '%1': %2").arg(filename).arg(file.errorString());
    logError() << msg;
    emit error(msg);
    return;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(data, &err);
  if (doc.isEmpty()) {
    QString msg = "Failed to load satellites: " + err.errorString();
    logError() << msg;
    emit error(msg);
    return;
  }

  if (! doc.isArray()) {
    QString msg = "Failed to load satellites: JSON document is not an array!";
    logError() << msg;
    emit error(msg);
    return;
  }

  beginResetModel();

  QJsonArray array = doc.array();
  _satellites.clear();
  _satellites.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    Satellite element = Satellite::fromJson(array.at(i).toObject(), _orbitalElements);
    if (element.isValid()) {
      _satellites.append(element);
    }
  }

  // Done.
  endResetModel();

  logDebug() << "Loaded satellites with " << _satellites.size() << " entries from " << filename << ".";

  emit loaded();
}


bool
SatelliteDatabase::save(const ErrorStack &err) const {
  QJsonArray satellites;
  for (auto sat: _satellites)
    satellites.append(sat.toJson());
  QJsonDocument doc(satellites);

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path + "/satellites.json");

  QDir directory;
  if ((! directory.exists(path)) && (! directory.mkpath(path))) {
    errMsg(err) << "Cannot create path '" << path << "'.";
    return false;
  }

  if (! file.open(QIODevice::WriteOnly)) {
    errMsg(err) << "Cannot save satellites at '" << file.fileName() << "'.";
    return false;
  }

  file.write(doc.toJson());
  file.flush();
  file.close();

  return true;
}
