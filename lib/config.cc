#include "config.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <cmath>
#include "csvreader.hh"
#include "csvwriter.hh"
#include "userdatabase.hh"


/* ********************************************************************************************* *
 * Implementation of Config
 * ********************************************************************************************* */
Config::Config(QObject *parent)
  : QObject(parent), _modified(false), _contacts(new ContactList(this)), _rxGroupLists(new RXGroupLists(this)),
    _channels(new ChannelList(this)), _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _gpsSystems(new PositioningSystems(this)), _roaming(new RoamingZoneList(this)),
    _id(0), _name(), _introLine1(), _introLine2(), _mic_level(2),
    _speech(false)
{
  connect(_contacts, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_rxGroupLists, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_channels, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_zones, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_scanlists, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_gpsSystems, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_roaming, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(this, SIGNAL(modified()), this, SLOT(onConfigModified()));
}

bool
Config::isModified() const {
  return _modified;
}
void
Config::setModified(bool modified) {
  _modified = modified;
  if (_modified)
    emit this->modified();
}

ContactList *
Config::contacts() const {
  return _contacts;
}

RXGroupLists *
Config::rxGroupLists() const {
  return _rxGroupLists;
}

ChannelList *
Config::channelList() const {
  return _channels;
}

ZoneList *
Config::zones() const {
  return _zones;
}

ScanLists *
Config::scanlists() const {
  return _scanlists;
}

PositioningSystems *
Config::posSystems() const {
  return _gpsSystems;
}

RoamingZoneList *
Config::roaming() const {
  return _roaming;
}

uint
Config::id() const {
  return _id;
}
void
Config::setId(uint id) {
  if (id == _id)
    return;
  _id = id;
  emit modified();
}

const QString &
Config::name() const {
  return _name;
}
void
Config::setName(const QString &name) {
  if (name == _name)
    return;
  _name = name;
  emit modified();
}

const QString &
Config::introLine1() const {
  return _introLine1;
}
void
Config::setIntroLine1(const QString &line) {
  if (line == _introLine1)
    return;
  _introLine1 = line;
  emit modified();
}
const QString &
Config::introLine2() const {
  return _introLine2;
}
void
Config::setIntroLine2(const QString &line) {
  if (line == _introLine2)
    return;
  _introLine2 = line;
  emit modified();
}

uint
Config::micLevel() const {
  return _mic_level;
}
void
Config::setMicLevel(uint level) {
  level = std::min(10u, std::max(1u, level));
  if (level == _mic_level)
    return;
  _mic_level = level;
  emit modified();
}

bool
Config::speech() const {
  return _speech;
}
void
Config::setSpeech(bool enabled) {
  if (enabled == _speech)
    return;
  _speech = enabled;
  emit modified();
}

void
Config::reset() {
  // Reset lists
  _scanlists->clear();
  _zones->clear();
  _channels->clear();
  _rxGroupLists->clear();
  _contacts->clear();
  _gpsSystems->clear();
  _roaming->clear();
  _id = 0;
  _name.clear();
  _introLine1.clear();
  _introLine2.clear();
  _mic_level = 2;
  _speech    = false;
  emit modified();
}

void
Config::onConfigModified() {
  _modified = true;
}

bool
Config::readCSV(const QString &filename, QString &errorMessage) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly))
    return false;
  QTextStream stream(&file);
  return readCSV(stream, errorMessage);
}

bool
Config::readCSV(QTextStream &stream, QString &errorMessage)
{
  if (CSVReader::read(this, stream, errorMessage))
    _modified = false;
  else
    return false;
  return true;
}

bool
Config::writeCSV(const QString &filename, QString &errorMessage) {
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    errorMessage = QString("Cannot open file %1: %2").arg(filename).arg(file.errorString());
    return false;
  }
  QTextStream stream(&file);
  return writeCSV(stream, errorMessage);
}

bool
Config::writeCSV(QTextStream &stream, QString &errorMessage)
{
  if (! CSVWriter::write(this, stream, errorMessage))
    return false;

  _modified = false;
  return true;
}


