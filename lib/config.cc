#include "config.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <cmath>
#include "csvreader.hh"
#include "csvwriter.hh"


/* ********************************************************************************************* *
 * Implementation of Config
 * ********************************************************************************************* */
Config::Config(QObject *parent)
  : QObject(parent), _modified(false), _contacts(new ContactList(this)), _rxGroupLists(new RXGroupLists(this)),
    _channels(new ChannelList(this)), _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _id(0), _name(), _introLine1(), _introLine2(), _vox_level(3), _mic_level(2)
{
  connect(_contacts, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_rxGroupLists, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_channels, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_zones, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_scanlists, SIGNAL(modified()), this, SIGNAL(modified()));
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
Config::voxLevel() const {
  return _vox_level;
}
void
Config::setVoxLevel(uint level) {
  level = std::min(10u, std::max(1u, level));
  if (level == _vox_level)
    return;
  _vox_level = level;
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

void
Config::reset() {
  // Reset lists
  _scanlists->clear();
  _zones->clear();
  _channels->clear();
  _rxGroupLists->clear();
  _contacts->clear();
  _id = 0;
  _name.clear();
  _introLine1.clear();
  _introLine2.clear();
  _vox_level = 3;
  _mic_level = 2;
}

void
Config::onConfigModified() {
  _modified = true;
}

bool
Config::readCSV(QTextStream &stream)
{
  if (CSVReader::read(this, stream))
    _modified = false;
  else
    return false;
  return true;
}

bool
Config::writeCSV(QTextStream &stream)
{
  if (! CSVWriter::write(this, stream))
    return false;

  _modified = false;
  return true;
}


