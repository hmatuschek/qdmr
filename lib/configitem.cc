#include "configitem.hh"
#include <logger.hh>

using namespace Configuration;


/* ********************************************************************************************* *
 * Implementation of ConfigDeclRadioId
 * ********************************************************************************************* */
RadioId::Declaration *RadioId::Declaration::_instance = nullptr;

RadioId::Declaration::Declaration()
  : Object::Declaration(tr("Specifies a radio ID, that is a pair of DMR ID and name for the radio."))
{
  _name = "RadioId";
  add("name", StringItem::Declaration::get(), true, tr("Specifies the name of the radio ID."));
  add("number", DMRId::Declaration::get(), true, tr("Specifies the radio DMR ID."));
}

Item *
RadioId::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new RadioId();
}

RadioId::Declaration *
RadioId::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRadioIdItem
 * ********************************************************************************************* */
RadioId::RadioId(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "id%1";
}

const QString &
RadioId::name() const {
  return get("name")->as<StringItem>()->value();
}

void
RadioId::setName(const QString &nm) {
  get("name")->as<StringItem>()->setValue(nm);
}

uint32_t
RadioId::number() const {
  return get("number")->as<DMRId>()->value();
}

void
RadioId::setNumber(uint32_t id) {
  return get("number")->as<DMRId>()->setValue(id);
}


/* ********************************************************************************************* *
 * Implementation of ConfigContactItem::Declaration
 * ********************************************************************************************* */
Contact::Declaration::Declaration(const QString &description)
  : Object::Declaration(description)
{
  _name = "Contact";
  add("name", StringItem::Declaration::get(), true, tr("Specifies the name of the contact."));
  add("ring", BoolItem::Declaration::get(), false, tr("Enables ring tone (optional)."));
}

/* ********************************************************************************************* *
 * Implementation of ConfigContactItem
 * ********************************************************************************************* */
Contact::Contact(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "con%1";
}

const QString &
Contact::name() const {
  return get("name")->as<StringItem>()->value();
}

void
Contact::setName(const QString &nm) {
  return get("name")->as<StringItem>()->setValue(nm);
}

bool
Contact::ring() const {
  return get("ring")->as<BoolItem>()->value();
}

void
Contact::setRing(bool enable) {
  return get("ring")->as<BoolItem>()->setValue(enable);
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem::Declaration
 * ********************************************************************************************* */
Configuration::DigitalContact::Declaration::Declaration(const QString &description)
  : Contact::Declaration(description)
{
  _name = "DigitalContact";
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalContactItem
 * ********************************************************************************************* */
Configuration::DigitalContact::DigitalContact(Declaration *declaraion, QObject *parent)
  : Contact(declaraion, parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem::Declaration
 * ********************************************************************************************* */
PrivateCall::Declaration *PrivateCall::Declaration::_instance = nullptr;

PrivateCall::Declaration::Declaration()
  : DigitalContact::Declaration(tr("Specifies a private call."))
{
  _name = "PrivateCall";
  add("number", DMRId::Declaration::get(), true, tr("Specifies the number of the contact"));
}

Item *
PrivateCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new PrivateCall();
}

PrivateCall::Declaration *
PrivateCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigPrivateCallItem
 * ********************************************************************************************* */
PrivateCall::PrivateCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
PrivateCall::number() const {
  return get("number")->as<DMRId>()->value();
}

void
PrivateCall::setNumber(uint32_t num) {
  get("number")->as<DMRId>()->setValue(num);
}


/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem::Declaration
 * ********************************************************************************************* */
GroupCall::Declaration *GroupCall::Declaration::_instance = nullptr;

GroupCall::Declaration::Declaration()
  : DigitalContact::Declaration(tr("Specifies a group call."))
{
  _name = "GroupCall";
  add("number", DMRId::Declaration::get(), true, tr("Specifies the number of the contact"));
}

Item *
GroupCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new GroupCall();
}

GroupCall::Declaration *
GroupCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new GroupCall::Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupCallItem
 * ********************************************************************************************* */
GroupCall::GroupCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}

uint32_t
GroupCall::number() const {
  return get("number")->as<DMRId>()->value();
}
void
GroupCall::setNumber(uint32_t num) {
  get("number")->as<DMRId>()->setValue(num);
}

/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem::Declaration
 * ********************************************************************************************* */
AllCall::Declaration *AllCall::Declaration::_instance = nullptr;

AllCall::Declaration::Declaration()
  : DigitalContact::Declaration(tr("Specifies an all-call."))
{
  _name = "AllCall";
}

Item *
AllCall::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new AllCall();
}

AllCall::Declaration *
AllCall::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAllCallItem
 * ********************************************************************************************* */
AllCall::AllCall(QObject *parent)
  : DigitalContact(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DigitalContactList::Declaration
 * ********************************************************************************************* */
DigitalContactList::Declaration *DigitalContactList::Declaration::_instance = nullptr;

DigitalContactList::Declaration::Declaration()
  : RefList::Declaration(
      Reference::Declaration::get<DigitalContact>(),
      [](const Item *item)->bool { return (nullptr != dynamic_cast<const DigitalContact *>(item)); },
      tr("The list of group calls forming the group list."))
{
  // pass...
}

DigitalContactList::Declaration *
DigitalContactList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new DigitalContactList::Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of DigitalContactList
 * ********************************************************************************************* */
DigitalContactList::DigitalContactList(QObject *parent)
  : RefList(Declaration::get(), parent)
{
  // pass...
}

Configuration::DigitalContact *
DigitalContactList::get(int i) const {
  Item *item = RefList::get(i);
  if (item && item->is<DigitalContact>())
    return item->as<DigitalContact>();
  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of GroupListItem::Declaration
 * ********************************************************************************************* */
GroupList::Declaration *GroupList::Declaration::_instance = nullptr;

GroupList::Declaration::Declaration()
  : Object::Declaration(tr("Defines a group list."))
{
  _name = "GroupList";
  add("name", StringItem::Declaration::get(), true, tr("Specifies the name of the group list."));
  add("members", DigitalContactList::Declaration::get(), true, tr("Specifies list of group calls."));
}

Item *
GroupList::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new GroupList();
}


GroupList::Declaration *
GroupList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigGroupListItem::Declaration
 * ********************************************************************************************* */
GroupList::GroupList(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "grp%1";
}

const QString &
GroupList::name() const {
  return get("name")->as<StringItem>()->value();
}

DigitalContactList *
GroupList::members() const {
  if (! has("members"))
    return nullptr;
  return get("members")->as<DigitalContactList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::Channel::Declaration::Declaration(const QString &description)
  : Object::Declaration(description)
{
  _name = "Channel";
  add("name", StringItem::Declaration::get(),
      true, tr("Specifies the name of the channel."));
  add("rx", new FloatItem::Declaration(0, 10e3),
      true, tr("Specifies the RX frequency of the channel in MHz."));
  add("tx", new FloatItem::Declaration(-10e3, 10e3),
      false, tr("Specifies the TX frequency of the channel or offset in MHz."));
  add("power", new Enum::Declaration(
    { {"min", ::Channel::MinPower},
      {"low", ::Channel::LowPower},
      {"mid", ::Channel::MidPower},
      {"high", ::Channel::HighPower},
      {"max", ::Channel::MaxPower} }), true, tr("Specifies the transmit power on the channel."));
  add("tx-timeout", new IntItem::Declaration(0, 10000),
      false, tr("Specifies the transmit timeout in seconds. None if 0 or omitted."));
  add("rx-only", BoolItem::Declaration::get(),
      false, tr("If true, TX is disabled for this channel."));
  add("scan-list", Reference::Declaration::get<ScanList>(),
      false, tr("References the scanlist associated with this channel."));
}

/* ********************************************************************************************* *
 * Implementation of ConfigChannelItem
 * ********************************************************************************************* */
Configuration::Channel::Channel(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "ch%1";
}

const QString &
Configuration::Channel::name() const {
  return get("name")->as<StringItem>()->value();
}

double
Configuration::Channel::rx() const {
  return get("rx")->as<FloatItem>()->value();
}

double
Configuration::Channel::tx() const {
  return get("tx")->as<FloatItem>()->value();
}

::Channel::Power
Configuration::Channel::power() const {
  return get("power")->as<Enum>()->as<::Channel::Power>(::Channel::MaxPower);
}

qint64
Configuration::Channel::txTimeout() const {
  return get("tx-timeout")->as<IntItem>()->value();
}

bool
Configuration::Channel::rxOnly() const {
  return get("rx-only")->as<BoolItem>()->value();
}

Configuration::ScanList *
Configuration::Channel::scanList() const {
  return get("scan-list")->as<Configuration::ScanList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::DigitalChannel::Declaration *
Configuration::DigitalChannel::Declaration::_instance = nullptr;

Configuration::DigitalChannel::Declaration::Declaration()
  : Channel::Declaration(tr("Specifies a digial channel."))
{
  _name = "DigitalChannel";
  add("admit", new Enum::Declaration(
    { {"always", ::DigitalChannel::AdmitNone},
      {"free", ::DigitalChannel::AdmitFree},
      {"color-code", ::DigitalChannel::AdmitColorCode} }), true, tr("Specifies the transmit admit criterion."));
  add("color-code", new IntItem::Declaration(1,16), true, tr("Specifies the color-code of the channel."));
  add("time-slot", new IntItem::Declaration(1,2), true, tr("Specifies the time-slot of the channel."));
  add("group-list", Reference::Declaration::get<GroupList>(), true, tr("References the RX group list of the channel."));
  add("tx-contact", Reference::Declaration::get<DigitalContact>(), false, tr("References the default TX contact of the channel."));
  add("aprs", Reference::Declaration::get<Positioning>(), false, tr("References the positioning system used by this channel."));
  add("roaming-zone", Reference::Declaration::get<RoamingZone>(), false, tr("References the roaming zone used by this channel."));
  add("dmr-id", Reference::Declaration::get<RadioId>(), false, tr("Specifies the DMR ID to use on this channel."));
}

Item *
Configuration::DigitalChannel::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new DigitalChannel();
}

Configuration::DigitalChannel::Declaration *
Configuration::DigitalChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDigitalChannelItem
 * ********************************************************************************************* */
Configuration::DigitalChannel::DigitalChannel(QObject *parent)
  : Channel(Declaration::get(), parent)
{
  // pass...
}

::DigitalChannel::Admit
Configuration::DigitalChannel::admit() const {
  return get("admit")->as<Enum>()->as<::DigitalChannel::Admit>(::DigitalChannel::AdmitColorCode);
}

uint8_t
Configuration::DigitalChannel::colorCode() const {
  if (! has("color-code"))
    return 1;
  return get("color-code")->as<IntItem>()->value();
}

::DigitalChannel::TimeSlot
Configuration::DigitalChannel::timeSlot() const {
  if (! has("time-slot"))
    return ::DigitalChannel::TimeSlot1;
  return get("time-slot")->as<Enum>()->as<::DigitalChannel::TimeSlot>(::DigitalChannel::TimeSlot1);
}

GroupList *
Configuration::DigitalChannel::groupList() const {
  return getRef<GroupList>("group-list");
}

Configuration::DigitalContact *
Configuration::DigitalChannel::txContact() const {
  return getRef<Configuration::DigitalContact>("tx-contact");
}

Configuration::Positioning *
Configuration::DigitalChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<Configuration::Positioning>();
}

Configuration::RoamingZone *
Configuration::DigitalChannel::roamingZone() const {
  return getRef<Configuration::RoamingZone>("roaming-zone");
}

RadioId *
Configuration::DigitalChannel::radioId() const {
  return getRef<RadioId>("dmr-id");
}


/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem::Declaration
 * ********************************************************************************************* */
Configuration::AnalogChannel::Declaration *
Configuration::AnalogChannel::Declaration::_instance = nullptr;

Configuration::AnalogChannel::Declaration::Declaration()
  : Channel::Declaration(tr("Specifies a digial channel."))
{
  _name = "AnalogChannel";
  add("admit", new Enum::Declaration(
    {{"always", ::AnalogChannel::AdmitNone},
     {"free", ::AnalogChannel::AdmitFree},
     {"tone", ::AnalogChannel::AdmitTone} }), false, tr("Specifies the transmit admit criterion."));
  add("squelch", new IntItem::Declaration(1,10), true, tr("Specifies the squelch level."));
  add("rx-tone", new DispatchDeclaration(
  { {"ctcss", new FloatItem::Declaration(0,300, tr("Specifies the CTCSS frequency."))},
    {"dcs", new IntItem::Declaration(-300,300, tr("Specifies the DCS code."))} }),
      false, tr("Specifies the DCS/CTCSS signaling for RX."));
  add("tx-tone", new DispatchDeclaration(
  { {"ctcss", new FloatItem::Declaration(0,300, tr("Specifies the CTCSS frequency."))},
    {"dcs", new IntItem::Declaration(-300,300, tr("Specifies the DCS code."))} }),
      false, tr("Specifies the DCS/CTCSS signaling for TX."));
  add("band-width", new Enum::Declaration(
    {{"narrow", ::AnalogChannel::BWNarrow},
     {"wide", ::AnalogChannel::BWWide}}), true, tr("Specifies the bandwidth of the channel."));
  add("aprs", Reference::Declaration::get<APRSPositioning>(), false, tr("References the APRS system used by this channel."));
}

Item *
Configuration::AnalogChannel::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new AnalogChannel();
}

Configuration::AnalogChannel::Declaration *
Configuration::AnalogChannel::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAnalogChannelItem
 * ********************************************************************************************* */
Configuration::AnalogChannel::AnalogChannel(QObject *parent)
  : Channel(Declaration::get(), parent)
{
  // pass...
}

::AnalogChannel::Admit
Configuration::AnalogChannel::admit() const {
  return get("admit")->as<Enum>()->as<::AnalogChannel::Admit>(::AnalogChannel::AdmitNone);
}

uint
Configuration::AnalogChannel::squelch() const {
  return get("squelch")->as<IntItem>()->value();
}

::Signaling::Code
Configuration::AnalogChannel::rxSignalling() const {
  if (! has("rx-tone"))
    return Signaling::SIGNALING_NONE;

  if (get("rx-tone")->is<FloatItem>()) {
    double f = get("rx-tone")->as<FloatItem>()->value();
    return Signaling::fromCTCSSFrequency(f);
  } else if (get("rx-tone")->is<IntItem>()) {
    // handle as DCS
    qint64 num = get("rx-tone")->as<IntItem>()->value();
    return Signaling::fromDCSNumber(std::abs(num), num<0);
  }

  return Signaling::SIGNALING_NONE;
}


Signaling::Code
Configuration::AnalogChannel::txSignalling() const {
  if (! has("rx-tone"))
    return ::Signaling::SIGNALING_NONE;
  if (get("rx-tone")->is<FloatItem>()) {
    double f = get("rx-tone")->as<FloatItem>()->value();
    return Signaling::fromCTCSSFrequency(f);
  } else if (get("rx-tone")->is<IntItem>()) {
    // handle as DCS
    qint64 num = get("rx-tone")->as<IntItem>()->value();
    return Signaling::fromDCSNumber(std::abs(num), num<0);
  }
  return Signaling::SIGNALING_NONE;
}

::AnalogChannel::Bandwidth
Configuration::AnalogChannel::bandWidth() const {
  return get("band-width")->as<Enum>()->as<::AnalogChannel::Bandwidth>(::AnalogChannel::BWNarrow);
}

APRSPositioning *
Configuration::AnalogChannel::aprs() const {
  if (! has("aprs"))
    return nullptr;
  return get("aprs")->as<APRSPositioning>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigZoneItem::Declaration
 * ********************************************************************************************* */
Zone::Declaration *Zone::Declaration::_instance = nullptr;

Zone::Declaration::Declaration()
  : Object::Declaration(tr("Defines a zone within the codeplug."))
{
  _name = "Zone";
  add("name", StringItem::Declaration::get(), true, tr("Specifies the name of the zone."));
  add("A", RefList::Declaration::get<Channel>(), true, tr("Channel references."));
  add("B", RefList::Declaration::get<Channel>(), false, tr("Channel references."));
}

Item *
Zone::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Zone();
}

Zone::Declaration *
Zone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigZone
 * ********************************************************************************************* */
Zone::Zone(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "zone%1";
}


/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem::Declaration
 * ********************************************************************************************* */
Configuration::ScanList::Declaration *
Configuration::ScanList::Declaration::_instance = nullptr;

Configuration::ScanList::Declaration::Declaration()
  : Object::Declaration(tr("Defines a scan list"))
{
  _name = "ScanList";
  add("name", StringItem::Declaration::get(), true, tr("Name of the scan list."));
  add("primary", Reference::Declaration::get<Channel>(), false, tr("Reference to the primary priority channel."));
  add("secondary", Reference::Declaration::get<Channel>(), false, tr("Reference to the secondary priority channel."));
  add("revert", Reference::Declaration::get<Channel>(), false, tr("Reference to the revert (transmit) channel."));
  add("channels", RefList::Declaration::get<Channel>(), true, tr("Reference to a channel."));
}

Item *
Configuration::ScanList::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Configuration::ScanList();
}

Configuration::ScanList::Declaration *
Configuration::ScanList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigScanListItem
 * ********************************************************************************************* */
Configuration::ScanList::ScanList(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "scan%1";
}

const QString &
Configuration::ScanList::name() const {
  return get("name")->as<StringItem>()->value();
}

Configuration::Channel *
Configuration::ScanList::primary() const {
  return getRef<Configuration::Channel>("primary");
}

Configuration::Channel *
Configuration::ScanList::secondary() const {
  return getRef<Configuration::Channel>("secondary");
}

Configuration::Channel *
Configuration::ScanList::revert() const {
  return getRef<Configuration::Channel>("revert");
}

RefList *
Configuration::ScanList::channels() const {
  return get("channels")->as<RefList>();
}


/* ********************************************************************************************* *
 * Implementation of ConfigPositioning::Declaration
 * ********************************************************************************************* */
Positioning::Declaration::Declaration(const QString &description)
  : Object::Declaration(description)
{
  _name = "Positioning";
  add("name", StringItem::Declaration::get(), true, tr("Specifies the name of the positioning system."));
  add("period", new IntItem::Declaration(1, 1000), true, tr("Specifies the update period in seconds."));
}

/* ********************************************************************************************* *
 * Implementation of ConfigPositioning
 * ********************************************************************************************* */
Positioning::Positioning(Declaration *declaraion, QObject *parent)
  : Object(declaraion, parent)
{
  _idTemplate = "pos%1";
}

const QString &
Positioning::name() const {
  return get("name")->as<StringItem>()->value();
}

uint32_t
Positioning::period() const {
  return get("period")->as<IntItem>()->value();
}


/* ********************************************************************************************* *
 * Implementation of ConfigDMRPosItem::Declaration
 * ********************************************************************************************* */
DMRPositioning::Declaration *DMRPositioning::Declaration::_instance = nullptr;

DMRPositioning::Declaration::Declaration()
  : Positioning::Declaration(tr("Specifies a DMR positioning system."))
{
  _name = "DMRPositioning";
  add("destination", Reference::Declaration::get<DigitalContact>(),
      true, tr("Specifies the destination contact."));
  add("channel", Reference::Declaration::get<DigitalChannel>(),
      false, tr("Specifies the optional revert channel."));
}

Item *
DMRPositioning::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new DMRPositioning();
}

DMRPositioning::Declaration *
DMRPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigDMRPositioning
 * ********************************************************************************************* */
DMRPositioning::DMRPositioning(QObject *parent)
  : Positioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPosItem::Declaration
 * ********************************************************************************************* */
APRSPositioning::Declaration *APRSPositioning::Declaration::_instance = nullptr;

APRSPositioning::Declaration::Declaration()
  : Positioning::Declaration(tr("Specifies an APRS positioning system."))
{
  _name = "APRSPositioning";
  add("source", StringItem::Declaration::get(), true, tr("Specifies the source call and SSID."));
  add("destination", StringItem::Declaration::get(), true, tr("Specifies the destination call and SSID."));
  add("channel", Reference::Declaration::get<AnalogChannel>(), false, tr("Specifies the optional revert channel."));
  add("path", List::Declaration::get<StringItem>(StringItem::Declaration::get()),
      true, tr("Specifies a path element of the APRS packet."));
  add("icon", StringItem::Declaration::get(), true, tr("Specifies the icon name."));
  add("message", StringItem::Declaration::get(), false, tr("Specifies the optional APRS message."));
}

Item *
APRSPositioning::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new APRSPositioning();
}

APRSPositioning::Declaration *
APRSPositioning::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigAPRSPositioning
 * ********************************************************************************************* */
APRSPositioning::APRSPositioning(QObject *parent)
  : Positioning(Declaration::get(), parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZoneItem::Declaration
 * ********************************************************************************************* */
Configuration::RoamingZone::Declaration *
Configuration::RoamingZone::Declaration::_instance = nullptr;

Configuration::RoamingZone::Declaration::Declaration()
  : Object::Declaration(tr("Specifies a roaming zone."))
{
  _name = "RoamingZone";
  add("name", StringItem::Declaration::get(), true, tr("Specifies name of the roaming zone."));
  add("channels", RefList::Declaration::get<Configuration::DigitalChannel>(), false, tr("References a channel"));
}

Item *
Configuration::RoamingZone::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Configuration::RoamingZone();
}

Configuration::RoamingZone::Declaration *
Configuration::RoamingZone::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of ConfigRoamingZone
 * ********************************************************************************************* */
Configuration::RoamingZone::RoamingZone(QObject *parent)
  : Object(Declaration::get(), parent)
{
  _idTemplate = "roam%1";
}

QString
Configuration::RoamingZone::name() const {
  return get("name")->as<StringItem>()->value();
}

RefList *
Configuration::RoamingZone::channels() const {
  return get("channels")->as<RefList>();
}


/* ********************************************************************************************* *
 * Implementation of ChannelList::Declaration
 * ********************************************************************************************* */
Configuration::ChannelList::Declaration *
Configuration::ChannelList::Declaration::_instance = nullptr;

Configuration::ChannelList::Declaration::Declaration()
  : List::Declaration(new DispatchDeclaration({ {"analog", AnalogChannel::Declaration::get()},
                                                {"digital", DigitalChannel::Declaration::get()} }),
                      [](const Item *item)->bool { return (nullptr != dynamic_cast<const Configuration::Channel *>(item)); },
                      tr("The list of all channels within the codeplug."))
{
  // pass...
}

Configuration::ChannelList::Declaration *
Configuration::ChannelList::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of Configuration::ChannelList
 * ********************************************************************************************* */
Configuration::ChannelList::ChannelList(QObject *parent)
  : List(Declaration::get(), parent)
{
  // pass...
}

bool
Configuration::ChannelList::add(Item *item) {
  if (nullptr == item)
    return false;
  if (! item->is<Configuration::Channel>())
    return false;
  return List::add(item);
}

Configuration::Channel *
Configuration::ChannelList::getChannel(int i) const {
  return get(i)->as<Configuration::Channel>();
}


/* ********************************************************************************************* *
 * Implementation of Configuration::Config::Declaration
 * ********************************************************************************************* */
Configuration::Config::Declaration *
Configuration::Config::Declaration::_instance = nullptr;

Configuration::Config::Declaration::Declaration()
  : Map::Declaration(tr("Defines the entire codeplug."))
{
  _name = "Config";
  add("version", StringItem::Declaration::get(), false, tr("Specifies the optional version number of the config format."));
  add("intro-line1", StringItem::Declaration::get(), false, tr("Specifies the optional first boot display line."));
  add("intro-line2", StringItem::Declaration::get(), false, tr("Specifies the optional second boot display line."));

  add("radio-ids", List::Declaration::get<RadioId>(),
      true, tr("This list specifies the DMR IDs and names for the radio."));

  add("channels", ChannelList::Declaration::get(),
      true, tr("List of channel definitions."));

  add("zones", List::Declaration::get<Zone>(),
      true, tr("Defines the list of zone definitions."));

  add("scan-lists", List::Declaration::get<ScanList>(),
      false, tr("Defines the list of all scan lists."));

  add("contacts",
      List::Declaration::get<Contact>(
        new DispatchDeclaration({ {"private", PrivateCall::Declaration::get()},
                                  {"group", GroupCall::Declaration::get()},
                                  {"all", AllCall::Declaration::get()} },
                                tr("One of the contact types, 'private', 'group', 'all'."))),
      true, tr("Specifies the list of contacts."));

  add("group-lists", List::Declaration::get<GroupList>(),
      true, tr("Lists all RX group lists within the codeplug."));

  add("positioning",
      List::Declaration::get<Positioning>(
        new DispatchDeclaration({ {"dmr", DMRPositioning::Declaration::get()},
                                  {"aprs", APRSPositioning::Declaration::get()} })),
      false, tr("List of all positioning systems."));

  add("roaming", List::Declaration::get<RoamingZone>(),
      false, tr("List of all roaming zones."));
}

bool
Configuration::Config::Declaration::verify(const YAML::Node &doc, QString &message) {
  QSet<QString> ctx;
  if (! this->verifyForm(doc, ctx, message))
    return false;
  if (! this->verifyReferences(doc, ctx, message))
    return false;
  return true;
}

bool
Configuration::Config::Declaration::parse(Config *config, YAML::Node &doc, QString &message) {
  if (! verify(doc, message))
    return false;

  QHash<QString, Config::Item *> ctx;
  if (! parsePopulate(config, doc, ctx, message))
    return false;
  if (! parseLink(config, doc, ctx, message))
    return false;

  return true;
}

Configuration::Config *
Configuration::Config::Declaration::parse(YAML::Node &doc, QString &message) {
  Item *item = parseAllocate(doc, message);
  if (nullptr == item)
    return nullptr;
  if (! parse(item->as<Config>(), doc, message)) {
    item->deleteLater();
    return nullptr;
  }
  return item->as<Config>();
}

Item *
Configuration::Config::Declaration::parseAllocate(const YAML::Node &node, QString &msg) const {
  return new Config();
}

Configuration::Config::Declaration *
Configuration::Config::Declaration::get() {
  if (nullptr == _instance)
    _instance = new Config::Declaration();
  return _instance;
}

/* ********************************************************************************************* *
 * Implementation of Configuration
 * ********************************************************************************************* */
Configuration::Config::Config(QObject *parent)
  : Map(Declaration::get(), parent)
{
  // pass...
}

YAML::Node
Configuration::Config::generate(QString &message) const {
  QHash<const Item *, QString> ctx;
  if (! this->generateIds(ctx, message))
    return YAML::Node();
  return this->generateNode(ctx, message);
}
