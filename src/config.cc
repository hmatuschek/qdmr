#include "config.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include <QTextStream>
#include <QDebug>
#include <QFormLayout>
#include <QIntValidator>
#include <QDateTime>
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of Config
 * ********************************************************************************************* */
Config::Config(QObject *parent)
  : QObject(parent), _modified(false), _contacts(new ContactList(this)), _rxGroupLists(new RXGroupLists(this)),
    _channels(new ChannelList(this)), _zones(new ZoneList(this)), _scanlists(new ScanLists(this)),
    _id(0), _name(), _introLine1(), _introLine2()
{
  connect(_contacts, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_rxGroupLists, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_channels, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_zones, SIGNAL(modified()), this, SIGNAL(modified()));
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
  _id = id;
  emit modified();
}

const QString &
Config::name() const {
  return _name;
}
void
Config::setName(const QString &name) {
  _name = name;
  emit modified();
}

const QString &
Config::introLine1() const {
  return _introLine1;
}
void
Config::setIntroLine1(const QString &line) {
  _introLine1 = line;
  emit modified();
}
const QString &
Config::introLine2() const {
  return _introLine2;
}
void
Config::setIntroLine2(const QString &line) {
  _introLine2 = line;
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
}

void
Config::onConfigModified() {
  _modified = true;
}

bool
Config::readCSV(QTextStream &stream)
{
  // If I cannot seek in stream -> oops.
  if (! stream.seek(0))
    return false;

  reset();

  QMap<int, Channel *> channels;
  QMap<int, DigitalContact *> contacts;
  QMap<int, RXGroupList *> rxgroups;
  QMap<int, Zone *> zones;
  QMap<int, ScanList *> scanlists;
  if (! readCSVDefine(stream, channels, contacts, rxgroups, zones, scanlists))
    return false;
  if (! readCSVLink(stream, channels, contacts, rxgroups, zones, scanlists))
    return false;

  _modified = false;
  return true;
}

bool
Config::readCSVDefine(QTextStream &stream, QMap<int, Channel *> &channels,
                      QMap<int, DigitalContact *> &contacts, QMap<int, RXGroupList *> &rxgroups,
                      QMap<int, Zone *> &zones, QMap<int, ScanList *> &scanlists)
{
  stream.seek(0);
  enum {S_START, S_DIGITAL, S_ANALOG, S_CONTACT, S_GROUP, S_ZONE, S_SCAN} state = S_START;

  // Read contacts first
  for (QString line = stream.readLine(); !line.isNull(); line=stream.readLine())
  {
    if (line.startsWith("#")) {
      continue;
    } else if ((S_START == state) && line.simplified().isEmpty()) {
      continue;
    } else if ((S_START == state) && line.startsWith("ID: ")) {
      _id = line.mid(4).simplified().toUInt();
    } else if ((S_START == state) && line.startsWith("Name: ")) {
      _name = line.mid(6).simplified();
    } else if ((S_START == state) && line.startsWith("Intro Line 1: ")) {
      _introLine1 = line.mid(14).simplified();
    } else if ((S_START == state) && line.startsWith("Intro Line 2: ")) {
      _introLine2 = line.mid(14).simplified();
    } else if ((S_START == state) && line.startsWith("Name: ")) {
      _name = line.mid(4).simplified();
    } else if ((S_START == state) && line.startsWith("Digital")) {
      state = S_DIGITAL;
    } else if ((S_START == state) && line.startsWith("Analog")) {
      state = S_ANALOG;
    } else if ((S_START == state) && line.startsWith("Contact")) {
      state = S_CONTACT;
    } else if ((S_START == state) && line.startsWith("Grouplist")) {
      state = S_GROUP;
    } else if ((S_START == state) && line.startsWith("Zone")) {
      state = S_ZONE;
    } else if ((S_START == state) && line.startsWith("Scanlist")) {
      state = S_SCAN;
    } else if ((S_DIGITAL == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_DIGITAL == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<13)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      double rx = row.at(2).toDouble();
      double tx = row.at(3).toDouble();
      if (row.at(3).startsWith('+') || row.at(3).startsWith('-'))
        tx = rx+tx;
      Channel::Power power = Channel::HighPower;
      if ("Low" == row.at(4))
        power = Channel::LowPower;
      int timeout = 0;
      if ("-" != row.at(6))
        timeout = row.at(6).toUInt();
      bool rxOnly = false;
      if ("+" == row.at(7))
        rxOnly = true;
      DigitalChannel::Admit admit = DigitalChannel::AdmitNone;
      if ("Free" == row.at(8))
        admit = DigitalChannel::AdmitFree;
      else if ("Color" == row.at(8))
        admit = DigitalChannel::AdmitColorCode;
      uint colorcode = row.at(9).toUInt();
      DigitalChannel::TimeSlot slot = DigitalChannel::TimeSlot1;
      if ("2" == row.at(10))
        slot = DigitalChannel::TimeSlot2;
      int rxGrpIdx = -1;
      if ("-" != row.at(11))
        rxGrpIdx = row.at(11).toUInt();
      int txCntIdx = -1;
      if ("-" != row.at(12))
        txCntIdx = row.at(12).toUInt();
      channels[idx] = new DigitalChannel(name, rx, tx, power, timeout, rxOnly, admit, colorcode, slot,
                                         nullptr, nullptr, nullptr);
    } else if ((S_ANALOG == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_ANALOG == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<13)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      double rx = row.at(2).toDouble();
      double tx = row.at(3).toDouble();
      if (row.at(3).startsWith('+') || row.at(3).startsWith('-'))
        tx = rx+tx;
      Channel::Power power = Channel::HighPower;
      if ("Low" == row.at(4))
        power = Channel::LowPower;
      int timeout = 0;
      if ("-" != row.at(6))
        timeout = row.at(6).toUInt();
      bool rxOnly = false;
      if ("+" == row.at(7))
        rxOnly = true;
      AnalogChannel::Admit admit = AnalogChannel::AdmitNone;
      if ("Free" == row.at(8))
        admit = AnalogChannel::AdmitFree;
      else if ("Tone" == row.at(8))
        admit = AnalogChannel::AdmitTone;
      uint squelch = row.at(9).toUInt();
      double rxTone = 0;
      if ("-" != row.at(10))
        rxTone = row.at(10).toDouble();
      double txTone = 0;
      if ("-" != row.at(11))
        txTone = row.at(11).toDouble();
      AnalogChannel::Bandwidth bw = AnalogChannel::BWNarrow;
      if ("12.5" != row.at(12))
        bw = AnalogChannel::BWWide;
      channels[idx] = new AnalogChannel(name, rx, tx, power, timeout, rxOnly, admit, squelch, rxTone,
                                         txTone, bw, nullptr);
    } else if ((S_CONTACT == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_CONTACT == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<5)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      DigitalContact::Type type = DigitalContact::PrivateCall;
      if ("Private" == row.at(2))
        type = DigitalContact::PrivateCall;
      else if ("Group" == row.at(2))
        type = DigitalContact::GroupCall;
      else if ("AllCall" == row.at(2))
        type = DigitalContact::AllCall;
      uint number = row.at(3).toUInt();
      bool rxTone = ("+" == row.at(4));
      contacts[idx] = new DigitalContact(type, name, number, rxTone);
    } else if ((S_GROUP == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_GROUP == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<3)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      rxgroups[idx] = new RXGroupList(name);
    } else if ((S_ZONE == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_ZONE == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<3)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      zones[idx] = new Zone(name);
    } else if ((S_SCAN == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_SCAN == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<6)
        continue;
      int idx = row.at(0).toUInt();
      QString name = row.at(1).simplified(); name.replace('_',' ');
      scanlists[idx] = new ScanList(name);
    }
  }

  return true;
}


bool
Config::readCSVLink(QTextStream &stream, QMap<int, Channel *> &channels,
                    QMap<int, DigitalContact *> &contacts, QMap<int, RXGroupList *> &rxgroups,
                    QMap<int, Zone *> &zones, QMap<int, ScanList *> &scanlists)
{
  stream.seek(0);
  enum {S_START, S_DIGITAL, S_ANALOG, S_CONTACT, S_GROUP, S_ZONE, S_SCAN} state = S_START;

  // Read contacts first
  for (QString line = stream.readLine(); !line.isNull(); line=stream.readLine())
  {
    if (line.startsWith("#")) {
      continue;
    } else if ((S_START == state) && line.simplified().isEmpty()) {
      continue;
    } else if ((S_START == state) && line.startsWith("Digital")) {
      state = S_DIGITAL;
    } else if ((S_START == state) && line.startsWith("Analog")) {
      state = S_ANALOG;
    } else if ((S_START == state) && line.startsWith("Contact")) {
      state = S_CONTACT;
    } else if ((S_START == state) && line.startsWith("Grouplist")) {
      state = S_GROUP;
    } else if ((S_START == state) && line.startsWith("Zone")) {
      state = S_ZONE;
    } else if ((S_START == state) && line.startsWith("Scanlist")) {
      state = S_SCAN;
    } else if ((S_DIGITAL == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_DIGITAL == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<13)
        continue;
      int idx = row.at(0).toUInt();
      if ((!channels.contains(idx)) || (!channels[idx]->is<DigitalChannel>()))
        continue;
      if ("-" != row.at(5)) {
        int slIdx = row.at(5).toUInt();
        if (scanlists.contains(slIdx))
          channels[idx]->setScanList(scanlists[slIdx]);
      }
      if ("-" != row.at(11)) {
        int rxGrpIdx = row.at(11).toUInt();
        if (rxgroups.contains(rxGrpIdx))
          channels[idx]->as<DigitalChannel>()->setRXGroupList(rxgroups[rxGrpIdx]);
      }
      if ("-" != row.at(12)) {
        int txCntIdx = row.at(12).toUInt();
        if (contacts.contains(txCntIdx) && contacts[txCntIdx]->is<DigitalContact>()) {
          DigitalContact *digi = contacts[txCntIdx]->as<DigitalContact>();
          channels[idx]->as<DigitalChannel>()->setTXContact(digi);
        }
      }
      //_channels->addChannel(channels[idx]);
    } else if ((S_ANALOG == state) && line.simplified().isEmpty()) {
      // at the end of the analog channel list -> store channels according to their index
      QList<int> indices = channels.keys();
      qSort(indices);
      foreach (int index, indices) {
        _channels->addChannel(channels[index]);
      }
      state = S_START;
    } else if (S_ANALOG == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<13)
        continue;
      int idx = row.at(0).toUInt();
      if ((!channels.contains(idx)) || (!channels[idx]->is<AnalogChannel>()))
        continue;
      //_channels->addChannel(channels[idx]);
    } else if ((S_CONTACT == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_CONTACT == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<5)
        continue;
      int idx = row.at(0).toUInt();
      if (! contacts.contains(idx))
        continue;
      _contacts->addContact(contacts[idx]);
    } else if ((S_GROUP == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_GROUP == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<3)
        continue;
      int idx = row.at(0).toUInt();
      if (! rxgroups.contains(idx))
        continue;
      RXGroupList *list = rxgroups[idx];
      QStringList entries = row[2].split(",", QString::SkipEmptyParts);
      foreach(QString entry, entries) {
        if (entry.contains("-")) {
          QStringList range = entry.split("-");
          if (range.size() != 2)
            continue;
          for (int i=range[0].toInt(); i<range[1].toInt();i++) {
            if ((!contacts.contains(i)) || (!contacts[i]->is<DigitalContact>()))
              continue;
            list->addContact(contacts[i]->as<DigitalContact>());
          }
        } else {
          int i = entry.toUInt();
          if ((!contacts.contains(i)) || (!contacts[i]->is<DigitalContact>()))
            continue;
          list->addContact(contacts[i]->as<DigitalContact>());
        }
      }
      _rxGroupLists->addList(list);
    } else if ((S_ZONE == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_ZONE == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<3)
        continue;
      int idx = row.at(0).toUInt();
      if (! zones.contains(idx))
        continue;
      Zone *zone = zones[idx];
      QStringList entries = row[2].split(",", QString::SkipEmptyParts);
      foreach(QString entry, entries) {
        if (entry.contains("-")) {
          QStringList range = entry.split("-");
          if (range.size() != 2)
            continue;
          for (int i=range[0].toInt(); i<range[1].toInt();i++) {
            if (! channels.contains(i))
              continue;
            zone->addChannel(channels[i]);
          }
        } else {
          int i = entry.toUInt();
          if (! channels.contains(i))
            continue;
          zone->addChannel(channels[i]);
        }
      }
      _zones->addZone(zone);
    } else if ((S_SCAN == state) && line.simplified().isEmpty()) {
      state = S_START;
    } else if (S_SCAN == state) {
      QStringList row = line.split(" ", QString::SkipEmptyParts);
      if (row.size()<6)
        continue;
      int idx = row.at(0).toUInt();
      if (! zones.contains(idx))
        continue;
      ScanList *scanlist = scanlists[idx];
      QStringList entries = row[5].split(",", QString::SkipEmptyParts);
      foreach(QString entry, entries) {
        if (entry.contains("-")) {
          QStringList range = entry.split("-");
          if (range.size() != 2)
            continue;
          for (int i=range[0].toInt(); i<range[1].toInt();i++) {
            if (! channels.contains(i))
              continue;
            scanlist->addChannel(channels[i]);
          }
        } else {
          int i = entry.toUInt();
          if (! channels.contains(i))
            continue;
          scanlist->addChannel(channels[i]);
        }
      }
      _scanlists->addScanList(scanlist);
    }
  }

  return true;
}

inline QString formatName(const QString &name) {
  QString tmp = name;
  tmp.replace(' ', "_");
  return tmp;
}

inline QString formatFrequency(float f) {
  int val = std::round(f*10000);
  return QString::number(double(val)/10000, 'f', 4);
}

bool
Config::writeCSV(QTextStream &stream)
{
  stream << "#\n"
         << "# Configuration generated " << QDateTime::currentDateTime().toString() << "by qdrm, version 0.1.0\n"
         << "#\n\n";

  stream << "ID: " << id() << "\n"
         << "Name: " << name() << "\n\n"
         << "Intro Line 1: " << introLine1() << "\n"
         << "Intro Line 2: " << introLine2() << "\n\n";

  stream << "Digital Name             Receive   Transmit  Power Scan TOT RO Admit  Color Slot RxGL TxContact\n";
  for (int i=0; i<channelList()->count(); i++) {
    if (channelList()->channel(i)->is<AnalogChannel>())
      continue;
    DigitalChannel *digi = channelList()->channel(i)->as<DigitalChannel>();
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(digi->name())
           << qSetFieldWidth(10) << left << formatFrequency(digi->rxFrequency());
    if (digi->txFrequency()<digi->rxFrequency())
      stream << qSetFieldWidth(10) << left << formatFrequency(digi->txFrequency()-digi->rxFrequency());
    else
      stream << qSetFieldWidth(10) << left << formatFrequency(digi->txFrequency());
    stream << qSetFieldWidth(6)  << left << ( (Channel::HighPower == digi->power()) ? "High" : "Low" )
           << qSetFieldWidth(5)  << left << ( nullptr != digi->scanList() ?
          QString::number(scanlists()->indexOf(digi->scanList())+1) : QString("-") )
           << qSetFieldWidth(4)  << left << digi->txTimeout()
           << qSetFieldWidth(3)  << left << (digi->rxOnly() ? '+' : '-')
           << qSetFieldWidth(7)  << left << (DigitalChannel::AdmitNone ? "-" : (DigitalChannel::AdmitFree ? "Free" : "Color"))
           << qSetFieldWidth(6)  << left << digi->colorCode()
           << qSetFieldWidth(5)  << left << (DigitalChannel::TimeSlot1==digi->timeslot() ? "1" : "2");
    if (nullptr == digi->rxGroupList())
      stream << qSetFieldWidth(5)  << left << '-';
    else
      stream << qSetFieldWidth(5)  << left << (rxGroupLists()->indexOf(digi->rxGroupList())+1);
    if (nullptr == digi->txContact())
      stream << qSetFieldWidth(5)  << left << '-';
    else
      stream << qSetFieldWidth(5)  << left << (contacts()->indexOf(digi->txContact())+1)
             << "# " << digi->txContact()->name();
    stream << qSetFieldWidth(0) << "\n";
  }
  stream << "\n";

  stream << "Analog  Name             Receive   Transmit Power Scan TOT RO Admit  Squelch RxTone TxTone Width\n";
  for (int i=0; i<channelList()->count(); i++) {
    if (channelList()->channel(i)->is<DigitalChannel>())
      continue;
    AnalogChannel *analog = channelList()->channel(i)->as<AnalogChannel>();
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(analog->name())
           << qSetFieldWidth(10) << left << formatFrequency(analog->rxFrequency());
    if (analog->txFrequency()<analog->rxFrequency())
      stream << qSetFieldWidth(10) << left << formatFrequency(analog->txFrequency()-analog->rxFrequency());
    else
      stream << qSetFieldWidth(10) << left << formatFrequency(analog->txFrequency());
    stream << qSetFieldWidth(6)  << left << ( (Channel::HighPower == analog->power()) ? "High" : "Low" )
           << qSetFieldWidth(5)  << left << ( nullptr != analog->scanList() ?
          QString::number(scanlists()->indexOf(analog->scanList())+1) : QString("-") )
           << qSetFieldWidth(4)  << left << analog->txTimeout()
           << qSetFieldWidth(3)  << left << (analog->rxOnly() ? '+' : '-')
           << qSetFieldWidth(7)  << left << (AnalogChannel::AdmitNone ? "-" : (AnalogChannel::AdmitFree ? "Free" : "Tone"))
           << qSetFieldWidth(8)  << left << analog->squelch();
    if (0 == analog->rxTone())
      stream << qSetFieldWidth(7)  << left << "-";
    else
      stream << qSetFieldWidth(7)  << left << analog->rxTone();
    if (0 == analog->txTone())
      stream << qSetFieldWidth(7)  << left << "-";
    else
      stream << qSetFieldWidth(7)  << left << analog->txTone();
    stream << qSetFieldWidth(5) << left << (AnalogChannel::BWWide == analog->bandwidth() ? 25.0 : 12.5);
    stream << qSetFieldWidth(0) << "\n";
  }
  stream << "\n";

  stream << "Zone    Name             Channels\n";
  for (int i=0; i<zones()->count(); i++) {
    Zone *zone = zones()->zone(i);
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(zone->name());
    QStringList tmp;
    for (int j=0; j<zone->count(); j++) {
      tmp.append(QString::number(channelList()->indexOf(zone->channel(j))+1));
    }
    stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
  }
  stream << "\n";

  stream << "Scanlist Name            PCh1 PCh2 TxCh Channels\n";
  for (int i=0; i<scanlists()->count(); i++) {
    ScanList *list = scanlists()->scanlist(i);
    stream << qSetFieldWidth(9)  << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(list->name())
           << qSetFieldWidth(5)  << left << "-"
           << qSetFieldWidth(5)  << left << "-"
           << qSetFieldWidth(5)  << left << "Sel";
    QStringList tmp;
    for (int j=0; j<list->count(); j++) {
      tmp.append(QString::number(channelList()->indexOf(list->channel(j))+1));
    }
    stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
  }
  stream << "\n";

  stream << "Contact Name             Type    ID       RxTone\n";
  for (int i=0; i<contacts()->count(); i++) {
    if (! contacts()->contact(i)->is<DigitalContact>())
      continue;
    DigitalContact *contact = contacts()->contact(i)->as<DigitalContact>();
    stream << qSetFieldWidth(8)  << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(contact->name())
           << qSetFieldWidth(8)  << left << (DigitalContact::PrivateCall == contact->type() ?
                                               "Private" : (DigitalContact::GroupCall == contact->type() ?
                                                              "Group" : "All"))
           << qSetFieldWidth(9)  << left << contact->number()
           << qSetFieldWidth(6)  << left << (contact->rxTone() ? "+" : "-");
    stream << qSetFieldWidth(0) << "\n";
  }
  stream << "\n";

  stream << "Grouplist Name             Contacts\n";
  for (int i=0; i<rxGroupLists()->count(); i++) {
    RXGroupList *list = rxGroupLists()->list(i);
    stream << qSetFieldWidth(10) << left << (i+1)
           << qSetFieldWidth(17) << left << formatName(list->name());
    QStringList tmp;
    for (int j=0; j<list->count(); j++) {
      tmp.append(QString::number(contacts()->indexOf(list->contact(j))+1));
    }
    stream << qSetFieldWidth(0) << tmp.join(",") << "\n";
  }
  stream << "\n";

  _modified = false;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ConfigView
 * ********************************************************************************************* */
ConfigView::ConfigView(Config *conf, QWidget *parent)
  : QWidget(parent), _config(conf)
{
  _id = new QLineEdit(QString::number(_config->id()));
  _id->setValidator(new QIntValidator());
  _name = new QLineEdit(_config->name());
  _intro1 = new QLineEdit(_config->introLine1());
  _intro2 = new QLineEdit(_config->introLine2());

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("ID"), _id);
  layout->addRow(tr("Name"), _name);
  layout->addRow(tr("Intro Line 1"), _intro1);
  layout->addRow(tr("Intro Line 2"), _intro2);

  this->setLayout(layout);

  connect(_config, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_id, SIGNAL(editingFinished()), this, SLOT(onIdModified()));
  connect(_name, SIGNAL(editingFinished()), this, SLOT(onNameModified()));
  connect(_intro1, SIGNAL(editingFinished()), this, SLOT(onIntro1Modified()));
  connect(_intro2, SIGNAL(editingFinished()), this, SLOT(onIntro2Modified()));
}

void
ConfigView::onConfigModified() {
  if (_id->text().toUInt() != _config->id())
    _id->setText(QString::number(_config->id()));
  if (_name->text() != _config->name())
    _name->setText(_config->name());
  if (_intro1->text() != _config->introLine1())
    _intro1->setText(_config->introLine1());
  if (_intro2->text() != _config->introLine2())
    _intro2->setText(_config->introLine2());
}

void
ConfigView::onIdModified() {
  _config->setId(_id->text().toUInt());
}

void
ConfigView::onNameModified() {
  _config->setName(_name->text().simplified());
}

void
ConfigView::onIntro1Modified() {
  _config->setIntroLine1(_intro1->text().simplified());
}

void
ConfigView::onIntro2Modified() {
  _config->setIntroLine2(_intro2->text().simplified());
}

