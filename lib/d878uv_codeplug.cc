#include "d878uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"
#include "channel.hh"

#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::NameColor
 * ******************************************************************************************** */
AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::NameColor::decode(uint8_t code) {
  switch((CodedColor) code) {
  case White: return AnytoneDisplaySettingsExtension::Color::White;
  case Orange: return AnytoneDisplaySettingsExtension::Color::Orange;
  case Red: return AnytoneDisplaySettingsExtension::Color::Red;
  case Yellow: return AnytoneDisplaySettingsExtension::Color::Yellow;
  case Green: return AnytoneDisplaySettingsExtension::Color::Green;
  case Turquoise: return AnytoneDisplaySettingsExtension::Color::Turquoise;
  case Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  default: break;
  }
  return AnytoneDisplaySettingsExtension::Color::White;
}

uint8_t
D878UVCodeplug::NameColor::encode(AnytoneDisplaySettingsExtension::Color color) {
  switch(color) {
  case AnytoneDisplaySettingsExtension::Color::White: return (uint8_t) White;
  case AnytoneDisplaySettingsExtension::Color::Orange: return (uint8_t) Orange;
  case AnytoneDisplaySettingsExtension::Color::Red: return (uint8_t) Red;
  case AnytoneDisplaySettingsExtension::Color::Yellow: return (uint8_t) Yellow;
  case AnytoneDisplaySettingsExtension::Color::Green: return (uint8_t) Green;
  case AnytoneDisplaySettingsExtension::Color::Turquoise: return (uint8_t) Turquoise;
  case AnytoneDisplaySettingsExtension::Color::Blue: return (uint8_t) Blue;
  default: break;
  }
  return (uint8_t) White;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::TextColor
 * ******************************************************************************************** */
AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::TextColor::decode(uint8_t code) {
  switch((CodedColor) code) {
  case White: return AnytoneDisplaySettingsExtension::Color::White;
  case Orange: return AnytoneDisplaySettingsExtension::Color::Orange;
  case Red: return AnytoneDisplaySettingsExtension::Color::Red;
  case Yellow: return AnytoneDisplaySettingsExtension::Color::Yellow;
  case Green: return AnytoneDisplaySettingsExtension::Color::Green;
  case Turquoise: return AnytoneDisplaySettingsExtension::Color::Turquoise;
  case Blue: return AnytoneDisplaySettingsExtension::Color::Blue;
  default: break;
  }
  return AnytoneDisplaySettingsExtension::Color::White;
}

uint8_t
D878UVCodeplug::TextColor::encode(AnytoneDisplaySettingsExtension::Color color) {
  switch(color) {
  case AnytoneDisplaySettingsExtension::Color::White: return (uint8_t) White;
  case AnytoneDisplaySettingsExtension::Color::Orange: return (uint8_t) Orange;
  case AnytoneDisplaySettingsExtension::Color::Red: return (uint8_t) Red;
  case AnytoneDisplaySettingsExtension::Color::Yellow: return (uint8_t) Yellow;
  case AnytoneDisplaySettingsExtension::Color::Green: return (uint8_t) Green;
  case AnytoneDisplaySettingsExtension::Color::Turquoise: return (uint8_t) Turquoise;
  case AnytoneDisplaySettingsExtension::Color::Blue: return (uint8_t) Blue;
  default: break;
  }
  return (uint8_t) White;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : D868UVCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : D868UVCodeplug::ChannelElement(ptr, ChannelElement::size())
{
  // pass...
}

void
D878UVCodeplug::ChannelElement::clear() {
  D868UVCodeplug::ChannelElement::clear();
  setPTTIDSetting(PTTId::Off);
}

D878UVCodeplug::ChannelElement::PTTId
D878UVCodeplug::ChannelElement::pttIDSetting() const {
  return (PTTId)getUInt2(Offset::pttIDSetting(), 0);
}
void
D878UVCodeplug::ChannelElement::setPTTIDSetting(PTTId ptt) {
  setUInt2(Offset::pttIDSetting(), 0, (unsigned)ptt);
}

bool
D878UVCodeplug::ChannelElement::roamingEnabled() const {
  // inverted
  return !getBit(Offset::roamingEnabled(), 2);
}
void
D878UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  // inverted
  setBit(Offset::roamingEnabled(), 2, !enable);
}
bool
D878UVCodeplug::ChannelElement::dataACK() const {
  // inverted
  return !getBit(Offset::dataACK(), 3);
}
void
D878UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  // inverted
  setBit(Offset::dataACK(), 3, !enable);
}

bool
D878UVCodeplug::ChannelElement::txDigitalAPRS() const {
  return 2 == getUInt2(Offset::txDMRAPRS(), 0);
}
void
D878UVCodeplug::ChannelElement::enableTXDigitalAPRS(bool enable) {
  setUInt2(Offset::txDMRAPRS(), 0, (enable ? 0x02 : 0x00));
}
bool
D878UVCodeplug::ChannelElement::txAnalogAPRS() const {
  return 1 == getUInt2(Offset::txDMRAPRS(), 0);
}
void
D878UVCodeplug::ChannelElement::enableTXAnalogAPRS(bool enable) {
  setUInt2(Offset::txDMRAPRS(), 0, (enable ? 0x01 : 0x00));
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::analogAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(Offset::fmAPRSPTTSetting());
}
void
D878UVCodeplug::ChannelElement::setAnalogAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(Offset::fmAPRSPTTSetting(), (unsigned)ptt);
}

D878UVCodeplug::ChannelElement::APRSPTT
D878UVCodeplug::ChannelElement::digitalAPRSPTTSetting() const {
  return (APRSPTT)getUInt8(Offset::dmrAPRSPTTSetting());
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSPTTSetting(APRSPTT ptt) {
  setUInt8(Offset::dmrAPRSPTTSetting(), (unsigned)ptt);
}

unsigned
D878UVCodeplug::ChannelElement::digitalAPRSSystemIndex() const {
  return getUInt8(Offset::dmrAPRSSystemIndex());
}
void
D878UVCodeplug::ChannelElement::setDigitalAPRSSystemIndex(unsigned idx) {
  setUInt8(Offset::dmrAPRSSystemIndex(), idx);
}

int
D878UVCodeplug::ChannelElement::frequenyCorrection() const {
  return ((int)getInt8(Offset::frequenyCorrection()))*10;
}
void
D878UVCodeplug::ChannelElement::setFrequencyCorrection(int corr) {
  setInt8(Offset::frequenyCorrection(), corr/10);
}

unsigned int
D878UVCodeplug::ChannelElement::fmAPRSFrequencyIndex() const {
  return getUInt8(Offset::fmAPRSFrequencyIndex());
}
void
D878UVCodeplug::ChannelElement::setFMAPRSFrequencyIndex(unsigned int idx) {
  setUInt8(Offset::fmAPRSFrequencyIndex(), std::min(7U, idx));
}


Channel *
D878UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = D868UVCodeplug::ChannelElement::toChannelObj(ctx);

  if (nullptr == ch)
    return nullptr;

  // Get extensions
  AnytoneChannelExtension *ext = nullptr;
  if (DMRChannel *dch = ch->as<DMRChannel>()) {
    ext = dch->anytoneChannelExtension();
  } else if (FMChannel *fch = ch->as<FMChannel>()){
    ext = fch->anytoneChannelExtension();
  }

  // If extension is present, update
  if (nullptr != ext) {
    ext->setFrequencyCorrection(frequenyCorrection());
  }

  return ch;
}

bool
D878UVCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  if (! D868UVCodeplug::ChannelElement::linkChannelObj(c, ctx))
    return false;

  if (c->is<DMRChannel>()) {
    DMRChannel *dc = c->as<DMRChannel>();
    // Link to GPS system
    if (txDigitalAPRS() && ctx.has<GPSSystem>(digitalAPRSSystemIndex()))
      dc->setAPRSObj(ctx.get<GPSSystem>(digitalAPRSSystemIndex()));
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      dc->setAPRSObj(ctx.get<APRSSystem>(0));
    // If roaming is not disabled -> link to default roaming zone
    if (roamingEnabled())
      dc->setRoamingZone(DefaultRoamingZone::get());
    if (auto *ext = dc->anytoneChannelExtension()) {
      // If not default FM APRS frequency
      if (0 != fmAPRSFrequencyIndex()) {
        if (ctx.has<AnytoneAPRSFrequency>(fmAPRSFrequencyIndex()))
          ext->fmAPRSFrequency()->set(ctx.get<AnytoneAPRSFrequency>(fmAPRSFrequencyIndex()));
      }
    }
  } else if (c->is<FMChannel>()) {
    FMChannel *ac = c->as<FMChannel>();
    // Link APRS system if one is defined
    //  There can only be one active APRS system, hence the index is fixed to one.
    if (txAnalogAPRS() && ctx.has<APRSSystem>(0))
      ac->setAPRSSystem(ctx.get<APRSSystem>(0));
    if (auto *ext = ac->anytoneChannelExtension()) {
      // If not default FM APRS frequency
      if (0 != fmAPRSFrequencyIndex()) {
        if (ctx.has<AnytoneAPRSFrequency>(fmAPRSFrequencyIndex()))
          ext->fmAPRSFrequency()->set(ctx.get<AnytoneAPRSFrequency>(fmAPRSFrequencyIndex()));
      }
    }
  }

  return true;
}

bool
D878UVCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  if (! D868UVCodeplug::ChannelElement::fromChannelObj(c, ctx))
    return false;

  if (const DMRChannel *dc = c->as<DMRChannel>()) {
    // Set GPS system index
    enableRXAPRS(false);
    if (dc->aprsObj() && dc->aprsObj()->is<GPSSystem>()) {
      enableRXAPRS(true);
      enableTXDigitalAPRS(true);
      setDigitalAPRSSystemIndex(ctx.index(dc->aprsObj()->as<GPSSystem>()));
    } else if (dc->aprsObj() && dc->aprsObj()->is<APRSSystem>()) {
      enableTXAnalogAPRS(true);
    }
    // Enable roaming
    if (dc->roaming())
      enableRoaming(true);
    // Apply extension settings, if present
    if (AnytoneDMRChannelExtension *ext = dc->anytoneChannelExtension()) {
      setFrequencyCorrection(ext->frequencyCorrection());
      /// Handles bug in AnyTone firmware.
      /// @todo Remove once fixed by AnyTone.
      enableRXAPRS(! ext->sms());
    }
  } else if (const FMChannel *ac = c->as<FMChannel>()) {
    // Set APRS system
    enableRXAPRS(false);
    if (nullptr != ac->aprsSystem()) {
      enableTXAnalogAPRS(true);
      if (ac == ac->aprsSystem()->revertChannel()) {
        enableRXAPRS(true);
      }
    }
    // Apply extension settings
    if (AnytoneFMChannelExtension *ext = ac->anytoneChannelExtension()) {
      setFrequencyCorrection(ext->frequencyCorrection());
      if (! ext->fmAPRSFrequency()->isNull()) {
        int idx = ctx.index(ext->fmAPRSFrequency()->as<AnytoneAPRSFrequency>());
        if ((0 <= idx) && (7 >= idx))
          setFMAPRSFrequencyIndex(idx);
        else
          setFMAPRSFrequencyIndex(0);
      } else {
        // Use default
        setFMAPRSFrequencyIndex(0);
      }
    }
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::FMAPRSFrequencyNamesElement
 * ******************************************************************************************** */
D878UVCodeplug::FMAPRSFrequencyNamesElement::FMAPRSFrequencyNamesElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::FMAPRSFrequencyNamesElement::FMAPRSFrequencyNamesElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
D878UVCodeplug::FMAPRSFrequencyNamesElement::clear() {
  memset(_data, 0xff, size());
}

QString
D878UVCodeplug::FMAPRSFrequencyNamesElement::name(unsigned int n) const {
  n = std::min(n, 7U);
  return readASCII(n*Offset::betweenNames(), Limit::nameLength(), 0xff);
}

void
D878UVCodeplug::FMAPRSFrequencyNamesElement::setName(unsigned int n, const QString &name) {
  n = std::min(n, 7U);
  writeASCII(n*Offset::betweenNames(), name, Limit::nameLength(), 0xff);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingChannelElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingChannelElement::RoamingChannelElement(uint8_t *ptr)
  : Element(ptr, RoamingChannelElement::size())
{
  // pass...
}

void
D878UVCodeplug::RoamingChannelElement::clear() {
  memset(_data, 0x00, _size);
}

unsigned
D878UVCodeplug::RoamingChannelElement::rxFrequency() const {
  return getBCD8_be(Offset::rxFrequency())*10;
}
void
D878UVCodeplug::RoamingChannelElement::setRXFrequency(unsigned hz) {
  setBCD8_be(Offset::rxFrequency(), hz/10);
}
unsigned
D878UVCodeplug::RoamingChannelElement::txFrequency() const {
  return getBCD8_be(Offset::txFrequency())*10;
}
void
D878UVCodeplug::RoamingChannelElement::setTXFrequency(unsigned hz) {
  setBCD8_be(Offset::txFrequency(), hz/10);
}

bool
D878UVCodeplug::RoamingChannelElement::hasColorCode() const {
  return ColorCodeValue::Disabled == getUInt8(Offset::colorCode());
}
unsigned
D878UVCodeplug::RoamingChannelElement::colorCode() const {
  return std::min(15u, (unsigned)getUInt8(Offset::colorCode()));
}
void
D878UVCodeplug::RoamingChannelElement::setColorCode(unsigned cc) {
  setUInt8(Offset::colorCode(), cc);
}
void
D878UVCodeplug::RoamingChannelElement::disableColorCode() {
  setUInt8(Offset::colorCode(), ColorCodeValue::Disabled);
}

DMRChannel::TimeSlot
D878UVCodeplug::RoamingChannelElement::timeSlot() const {
  switch (getUInt8(Offset::timeSlot())) {
  case TimeSlotValue::TS1: return DMRChannel::TimeSlot::TS1;
  case TimeSlotValue::TS2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
D878UVCodeplug::RoamingChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::timeSlot(), TimeSlotValue::TS1); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::timeSlot(), TimeSlotValue::TS2); break;
  }
}

QString
D878UVCodeplug::RoamingChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
D878UVCodeplug::RoamingChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

bool
D878UVCodeplug::RoamingChannelElement::fromChannel(const RoamingChannel* ch) {
  setName(ch->name());
  setRXFrequency(ch->rxFrequency().inHz());
  setTXFrequency(ch->txFrequency().inHz());
  if (ch->colorCodeOverridden())
    setColorCode(ch->colorCode());
  else
    disableColorCode();
  setTimeSlot(ch->timeSlot());
  return true;
}

RoamingChannel *
D878UVCodeplug::RoamingChannelElement::toChannel(Context &ctx) {
  RoamingChannel *roam = new RoamingChannel();
  roam->setName(name());
  roam->setRXFrequency(Frequency::fromHz(rxFrequency()));
  roam->setTXFrequency(Frequency::fromHz(txFrequency()));
  if (hasColorCode())
    roam->setColorCode(colorCode());
  else
    roam->overrideColorCode(false);
  roam->overrideTimeSlot(true);
  roam->setTimeSlot(timeSlot());

  ctx.config()->roamingChannels()->add(roam);

  return roam;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingChannelBitmapElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingChannelBitmapElement::RoamingChannelBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingChannelBitmapElement::RoamingChannelBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, RoamingChannelBitmapElement::size())
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingZoneElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingZoneElement::RoamingZoneElement(uint8_t *ptr)
  : Element(ptr, RoamingZoneElement::size())
{
  // pass...
}

void
D878UVCodeplug::RoamingZoneElement::clear() {
  memset(_data, 0x00, _size);
  memset(_data+Offset::members(), 0xff, Limit::numMembers());
}

bool
D878UVCodeplug::RoamingZoneElement::hasMember(unsigned n) const {
  return (0xff != member(n));
}
unsigned
D878UVCodeplug::RoamingZoneElement::member(unsigned n) const {
  return getUInt8(Offset::members() + n*Offset::betweenMembers());
}
void
D878UVCodeplug::RoamingZoneElement::setMember(unsigned n, unsigned idx) {
  if (n >= Limit::numMembers())
    return;
  setUInt8(Offset::members() + n*Offset::betweenMembers(), idx);
}
void
D878UVCodeplug::RoamingZoneElement::clearMember(unsigned n) {
  if (n >= Limit::numMembers())
    return;
  setMember(Offset::members() + n*Offset::betweenMembers(), 0xff);
}

QString
D878UVCodeplug::RoamingZoneElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
D878UVCodeplug::RoamingZoneElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

bool
D878UVCodeplug::RoamingZoneElement::fromRoamingZone(RoamingZone *zone, Context &ctx, const ErrorStack& err)
{
  Q_UNUSED(err)

  clear();
  setName(zone->name());
  for (unsigned int i=0; i<std::min(Limit::numMembers(), (unsigned int)zone->count()); i++) {
    setMember(i, ctx.index(zone->channel(i)));
  }
  return true;
}

RoamingZone *
D878UVCodeplug::RoamingZoneElement::toRoamingZone(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new RoamingZone(name());
}

bool
D878UVCodeplug::RoamingZoneElement::linkRoamingZone(RoamingZone *zone, Context &ctx, const ErrorStack &err)
{
  for (uint8_t i=0; (i<Limit::numMembers())&&hasMember(i); i++) {
    if (ctx.has<RoamingChannel>(member(i))) {
      zone->addChannel(ctx.get<RoamingChannel>(member(i)));
    } else {
      errMsg(err) << "Cannot link roaming zone '" << zone->name()
                  << "': Roaming channel index " << member(i) << " is not defined.";
      return false;
    }
  }
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RoamingZoneBitmapElement
 * ******************************************************************************************** */
D878UVCodeplug::RoamingZoneBitmapElement::RoamingZoneBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::RoamingZoneBitmapElement::RoamingZoneBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, RoamingZoneBitmapElement::size())
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsElement::KeyFunction
 * ******************************************************************************************** */
uint8_t
D878UVCodeplug::GeneralSettingsElement::KeyFunction::encode(AnytoneKeySettingsExtension::KeyFunction func) {
  switch (func) {
  case AnytoneKeySettingsExtension::KeyFunction::Off:               return (uint8_t)KeyFunction::Off;
  case AnytoneKeySettingsExtension::KeyFunction::Voltage:           return (uint8_t)KeyFunction::Voltage;
  case AnytoneKeySettingsExtension::KeyFunction::Power:             return (uint8_t)KeyFunction::Power;
  case AnytoneKeySettingsExtension::KeyFunction::Repeater:          return (uint8_t)KeyFunction::Repeater;
  case AnytoneKeySettingsExtension::KeyFunction::Reverse:           return (uint8_t)KeyFunction::Reverse;
  case AnytoneKeySettingsExtension::KeyFunction::Encryption:        return (uint8_t)KeyFunction::Encryption;
  case AnytoneKeySettingsExtension::KeyFunction::Call:              return (uint8_t)KeyFunction::Call;
  case AnytoneKeySettingsExtension::KeyFunction::VOX:               return (uint8_t)KeyFunction::VOX;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleVFO:         return (uint8_t)KeyFunction::ToggleVFO;
  case AnytoneKeySettingsExtension::KeyFunction::SubPTT:            return (uint8_t)KeyFunction::SubPTT;
  case AnytoneKeySettingsExtension::KeyFunction::Scan:              return (uint8_t)KeyFunction::Scan;
  case AnytoneKeySettingsExtension::KeyFunction::WFM:               return (uint8_t)KeyFunction::WFM;
  case AnytoneKeySettingsExtension::KeyFunction::Alarm:             return (uint8_t)KeyFunction::Alarm;
  case AnytoneKeySettingsExtension::KeyFunction::RecordSwitch:      return (uint8_t)KeyFunction::RecordSwitch;
  case AnytoneKeySettingsExtension::KeyFunction::Record:            return (uint8_t)KeyFunction::Record;
  case AnytoneKeySettingsExtension::KeyFunction::SMS:               return (uint8_t)KeyFunction::SMS;
  case AnytoneKeySettingsExtension::KeyFunction::Dial:              return (uint8_t)KeyFunction::Dial;
  case AnytoneKeySettingsExtension::KeyFunction::Monitor:           return (uint8_t)KeyFunction::Monitor;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel: return (uint8_t)KeyFunction::ToggleMainChannel;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey1:           return (uint8_t)KeyFunction::HotKey1;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey2:           return (uint8_t)KeyFunction::HotKey2;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey3:           return (uint8_t)KeyFunction::HotKey3;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey4:           return (uint8_t)KeyFunction::HotKey4;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey5:           return (uint8_t)KeyFunction::HotKey5;
  case AnytoneKeySettingsExtension::KeyFunction::HotKey6:           return (uint8_t)KeyFunction::HotKey6;
  case AnytoneKeySettingsExtension::KeyFunction::WorkAlone:         return (uint8_t)KeyFunction::WorkAlone;
  case AnytoneKeySettingsExtension::KeyFunction::SkipChannel:       return (uint8_t)KeyFunction::SkipChannel;
  case AnytoneKeySettingsExtension::KeyFunction::DMRMonitor:        return (uint8_t)KeyFunction::DMRMonitor;
  case AnytoneKeySettingsExtension::KeyFunction::SubChannel:        return (uint8_t)KeyFunction::SubChannel;
  case AnytoneKeySettingsExtension::KeyFunction::PriorityZone:      return (uint8_t)KeyFunction::PriorityZone;
  case AnytoneKeySettingsExtension::KeyFunction::VFOScan:           return (uint8_t)KeyFunction::VFOScan;
  case AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality:   return (uint8_t)KeyFunction::MICSoundQuality;
  case AnytoneKeySettingsExtension::KeyFunction::LastCallReply:     return (uint8_t)KeyFunction::LastCallReply;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelType:       return (uint8_t)KeyFunction::ChannelType;
  case AnytoneKeySettingsExtension::KeyFunction::Roaming:           return (uint8_t)KeyFunction::Roaming;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelRanging:    return (uint8_t)KeyFunction::ChannelRanging;
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch:    return (uint8_t)KeyFunction::APRSType;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::Mute:              return (uint8_t)KeyFunction::Mute;
  case AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet:       return (uint8_t)KeyFunction::CtcssDcsSet;
  case AnytoneKeySettingsExtension::KeyFunction::TBSTSend:          return (uint8_t)KeyFunction::TBSTSend;
  case AnytoneKeySettingsExtension::KeyFunction::Bluetooth:         return (uint8_t)KeyFunction::Bluetooth;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelName:       return (uint8_t)KeyFunction::ChannelName;
  case AnytoneKeySettingsExtension::KeyFunction::CDTScan:           return (uint8_t)KeyFunction::CDTScan;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSInfo:          return (uint8_t)KeyFunction::APRSInfo;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::KeyFunction::decode(uint8_t code) {
  switch ((KeyFunctionCode)code) {
  case KeyFunction::Off:               return AnytoneKeySettingsExtension::KeyFunction::Off;
  case KeyFunction::Voltage:           return AnytoneKeySettingsExtension::KeyFunction::Voltage;
  case KeyFunction::Power:             return AnytoneKeySettingsExtension::KeyFunction::Power;
  case KeyFunction::Repeater:          return AnytoneKeySettingsExtension::KeyFunction::Repeater;
  case KeyFunction::Reverse:           return AnytoneKeySettingsExtension::KeyFunction::Reverse;
  case KeyFunction::Encryption:        return AnytoneKeySettingsExtension::KeyFunction::Encryption;
  case KeyFunction::Call:              return AnytoneKeySettingsExtension::KeyFunction::Call;
  case KeyFunction::VOX:               return AnytoneKeySettingsExtension::KeyFunction::VOX;
  case KeyFunction::ToggleVFO:         return AnytoneKeySettingsExtension::KeyFunction::ToggleVFO;
  case KeyFunction::SubPTT:            return AnytoneKeySettingsExtension::KeyFunction::SubPTT;
  case KeyFunction::Scan:              return AnytoneKeySettingsExtension::KeyFunction::Scan;
  case KeyFunction::WFM:               return AnytoneKeySettingsExtension::KeyFunction::WFM;
  case KeyFunction::Alarm:             return AnytoneKeySettingsExtension::KeyFunction::Alarm;
  case KeyFunction::RecordSwitch:      return AnytoneKeySettingsExtension::KeyFunction::RecordSwitch;
  case KeyFunction::Record:            return AnytoneKeySettingsExtension::KeyFunction::Record;
  case KeyFunction::SMS:               return AnytoneKeySettingsExtension::KeyFunction::SMS;
  case KeyFunction::Dial:              return AnytoneKeySettingsExtension::KeyFunction::Dial;
  case KeyFunction::Monitor:           return AnytoneKeySettingsExtension::KeyFunction::Monitor;
  case KeyFunction::ToggleMainChannel: return AnytoneKeySettingsExtension::KeyFunction::ToggleMainChannel;
  case KeyFunction::HotKey1:           return AnytoneKeySettingsExtension::KeyFunction::HotKey1;
  case KeyFunction::HotKey2:           return AnytoneKeySettingsExtension::KeyFunction::HotKey2;
  case KeyFunction::HotKey3:           return AnytoneKeySettingsExtension::KeyFunction::HotKey3;
  case KeyFunction::HotKey4:           return AnytoneKeySettingsExtension::KeyFunction::HotKey4;
  case KeyFunction::HotKey5:           return AnytoneKeySettingsExtension::KeyFunction::HotKey5;
  case KeyFunction::HotKey6:           return AnytoneKeySettingsExtension::KeyFunction::HotKey6;
  case KeyFunction::WorkAlone:         return AnytoneKeySettingsExtension::KeyFunction::WorkAlone;
  case KeyFunction::SkipChannel:       return AnytoneKeySettingsExtension::KeyFunction::SkipChannel;
  case KeyFunction::DMRMonitor:        return AnytoneKeySettingsExtension::KeyFunction::DMRMonitor;
  case KeyFunction::SubChannel:        return AnytoneKeySettingsExtension::KeyFunction::SubChannel;
  case KeyFunction::PriorityZone:      return AnytoneKeySettingsExtension::KeyFunction::PriorityZone;
  case KeyFunction::VFOScan:           return AnytoneKeySettingsExtension::KeyFunction::VFOScan;
  case KeyFunction::MICSoundQuality:   return AnytoneKeySettingsExtension::KeyFunction::MICSoundQuality;
  case KeyFunction::LastCallReply:     return AnytoneKeySettingsExtension::KeyFunction::LastCallReply;
  case KeyFunction::ChannelType:       return AnytoneKeySettingsExtension::KeyFunction::ChannelType;
  case KeyFunction::Roaming:           return AnytoneKeySettingsExtension::KeyFunction::Roaming;
  case KeyFunction::ChannelRanging:    return AnytoneKeySettingsExtension::KeyFunction::ChannelRanging;
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::APRSType:          return AnytoneKeySettingsExtension::KeyFunction::APRSTypeSwitch;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::Mute:              return AnytoneKeySettingsExtension::KeyFunction::Mute;
  case KeyFunction::CtcssDcsSet:       return AnytoneKeySettingsExtension::KeyFunction::CtcssDcsSet;
  case KeyFunction::TBSTSend:          return AnytoneKeySettingsExtension::KeyFunction::TBSTSend;
  case KeyFunction::Bluetooth:         return AnytoneKeySettingsExtension::KeyFunction::Bluetooth;
  case KeyFunction::GPS:               return AnytoneKeySettingsExtension::KeyFunction::GPS;
  case KeyFunction::ChannelName:       return AnytoneKeySettingsExtension::KeyFunction::ChannelName;
  case KeyFunction::CDTScan:           return AnytoneKeySettingsExtension::KeyFunction::CDTScan;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  case KeyFunction::APRSInfo:          return AnytoneKeySettingsExtension::KeyFunction::APRSInfo;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsElement::TimeZone
 * ******************************************************************************************** */
QVector<QTimeZone>
D878UVCodeplug::GeneralSettingsElement::TimeZone::_timeZones = {
  QTimeZone(-43200), QTimeZone(-39600), QTimeZone(-36000), QTimeZone(-32400),
  QTimeZone(-28800), QTimeZone(-25200), QTimeZone(-21600), QTimeZone(-18000),
  QTimeZone(-14400), QTimeZone(-12600), QTimeZone(-10800), QTimeZone(- 7200),
  QTimeZone(- 3600), QTimeZone(     0), QTimeZone(  3600), QTimeZone(  7200),
  QTimeZone( 10800), QTimeZone( 12600), QTimeZone(-28800), QTimeZone( 14400),
  QTimeZone( 16200), QTimeZone( 18000), QTimeZone( 19800), QTimeZone( 20700),
  QTimeZone( 21600), QTimeZone( 25200), QTimeZone( 28600), QTimeZone( 30600),
  QTimeZone( 32400), QTimeZone( 36000), QTimeZone( 39600), QTimeZone( 43200),
  QTimeZone( 46800) };

QTimeZone
D878UVCodeplug::GeneralSettingsElement::TimeZone::decode(uint8_t code) {
  if (code >= _timeZones.size())
    return _timeZones.back();
  return _timeZones.at(code);
}
uint8_t
D878UVCodeplug::GeneralSettingsElement::TimeZone::encode(const QTimeZone &zone) {
  if (! _timeZones.contains(zone))
    return 13; //<- UTC
  return _timeZones.indexOf(zone);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D878UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D868UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D868UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

void
D878UVCodeplug::GeneralSettingsElement::clear() {
  AnytoneCodeplug::GeneralSettingsElement::clear();
}

QTimeZone
D878UVCodeplug::GeneralSettingsElement::gpsTimeZone() const {
  return TimeZone::decode(getUInt8(Offset::gpsTimeZone()));
}
void
D878UVCodeplug::GeneralSettingsElement::setGPSTimeZone(const QTimeZone &zone) {
  setUInt8(Offset::gpsTimeZone(), TimeZone::encode(zone)); // <- Set to UTC
}

unsigned
D878UVCodeplug::GeneralSettingsElement::transmitTimeout() const {
  return ((unsigned)getUInt8(Offset::transmitTimeout()))*30;
}
void
D878UVCodeplug::GeneralSettingsElement::setTransmitTimeout(unsigned tot) {
  setUInt8(Offset::transmitTimeout(), tot/30);
}

AnytoneDisplaySettingsExtension::Language
D878UVCodeplug::GeneralSettingsElement::language() const {
  return (AnytoneDisplaySettingsExtension::Language)getUInt8(Offset::language());
}
void
D878UVCodeplug::GeneralSettingsElement::setLanguage(AnytoneDisplaySettingsExtension::Language lang) {
  setUInt8(Offset::language(), (unsigned)lang);
}

Frequency
D878UVCodeplug::GeneralSettingsElement::vfoFrequencyStep() const {
  switch (getUInt8(Offset::vfoFrequencyStep())) {
  case FREQ_STEP_2_5kHz: return Frequency::fromkHz(2.5);
  case FREQ_STEP_5kHz: return Frequency::fromkHz(5);
  case FREQ_STEP_6_25kHz: return Frequency::fromkHz(6.25);
  case FREQ_STEP_10kHz: return Frequency::fromkHz(10);
  case FREQ_STEP_12_5kHz: return Frequency::fromkHz(12.5);
  case FREQ_STEP_20kHz: return Frequency::fromkHz(20);
  case FREQ_STEP_25kHz: return Frequency::fromkHz(25);
  case FREQ_STEP_50kHz: return Frequency::fromkHz(50);
  }
  return Frequency::fromkHz(2.5);
}
void
D878UVCodeplug::GeneralSettingsElement::setVFOFrequencyStep(Frequency freq) {
  if (freq.inkHz() <= 2.5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_2_5kHz);
  else if (freq.inkHz() <= 5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_5kHz);
  else if (freq.inkHz() <= 6.25)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_6_25kHz);
  else if (freq.inkHz() <= 10)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_10kHz);
  else if (freq.inkHz() <= 12.5)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_12_5kHz);
  else if (freq.inkHz() <= 20)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_20kHz);
  else if (freq.inkHz() <= 25)
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_25kHz);
  else
    setUInt8(Offset::vfoFrequencyStep(), FREQ_STEP_50kHz);
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyAShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyAShort()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyAShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyAShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyBShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBShort()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyBShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyCShort() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCShort()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyCShort(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCShort(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKey1Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Short()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKey1Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKey2Short() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Short()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKey2Short(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Short(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyALong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyALong()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyALong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyALong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyBLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyBLong()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyBLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyBLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKeyCLong() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKeyCLong()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKeyCLong(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKeyCLong(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKey1Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey1Long()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKey1Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey1Long(), KeyFunction::encode(func));
}

AnytoneKeySettingsExtension::KeyFunction
D878UVCodeplug::GeneralSettingsElement::funcKey2Long() const {
  return KeyFunction::decode(getUInt8(Offset::progFuncKey2Long()));
}
void
D878UVCodeplug::GeneralSettingsElement::setFuncKey2Long(AnytoneKeySettingsExtension::KeyFunction func) {
  setUInt8(Offset::progFuncKey2Long(), KeyFunction::encode(func));
}

AnytoneSettingsExtension::STEType
D878UVCodeplug::GeneralSettingsElement::steType() const {
  return (AnytoneSettingsExtension::STEType)getUInt8(Offset::steType());
}
void
D878UVCodeplug::GeneralSettingsElement::setSTEType(AnytoneSettingsExtension::STEType type) {
  setUInt8(Offset::steType(), (unsigned)type);
}
double
D878UVCodeplug::GeneralSettingsElement::steFrequency() const {
  switch ((STEFrequency)getUInt8(Offset::steFrequency())) {
  case STEFrequency::Off: return 0;
  case STEFrequency::Hz55_2: return 55.2;
  case STEFrequency::Hz259_2: return 259.2;
  }
  return 0;
}
void
D878UVCodeplug::GeneralSettingsElement::setSTEFrequency(double freq) {
  if (0 >= freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Off);
  } else if (100 > freq) {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz55_2);
  } else {
    setUInt8(Offset::steFrequency(), (unsigned)STEFrequency::Hz259_2);
  }
}

Interval
D878UVCodeplug::GeneralSettingsElement::groupCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::groupCallHangTime()));
}
void
D878UVCodeplug::GeneralSettingsElement::setGroupCallHangTime(Interval intv) {
  setUInt8(Offset::groupCallHangTime(), intv.seconds());
}
Interval
D878UVCodeplug::GeneralSettingsElement::privateCallHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::privateCallHangTime()));
}
void
D878UVCodeplug::GeneralSettingsElement::setPrivateCallHangTime(Interval intv) {
  setUInt8(Offset::privateCallHangTime(), intv.seconds());
}
Interval
D878UVCodeplug::GeneralSettingsElement::preWaveDelay() const {
  return Interval::fromMilliseconds((unsigned)getUInt8(Offset::preWaveDelay())*20);
}
void
D878UVCodeplug::GeneralSettingsElement::setPreWaveDelay(Interval intv) {
  setUInt8(Offset::preWaveDelay(), intv.milliseconds()/20);
}
Interval
D878UVCodeplug::GeneralSettingsElement::wakeHeadPeriod() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::wakeHeadPeriod()))*20);
}
void
D878UVCodeplug::GeneralSettingsElement::setWakeHeadPeriod(Interval intv) {
  setUInt8(Offset::wakeHeadPeriod(), intv.milliseconds()/20);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::wfmChannelIndex() const {
  return getUInt8(Offset::wfmChannelIndex());
}
void
D878UVCodeplug::GeneralSettingsElement::setWFMChannelIndex(unsigned idx) {
  setUInt8(Offset::wfmChannelIndex(), idx);
}
bool
D878UVCodeplug::GeneralSettingsElement::wfmVFOEnabled() const {
  return getUInt8(Offset::wfmVFOEnabled());
}
void
D878UVCodeplug::GeneralSettingsElement::enableWFMVFO(bool enable) {
  setUInt8(Offset::wfmVFOEnabled(), (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::dtmfToneDuration() const {
  switch (getUInt8(Offset::dtmfToneDuration())) {
  case DTMF_DUR_50ms:  return 50;
  case DTMF_DUR_100ms: return 100;
  case DTMF_DUR_200ms: return 200;
  case DTMF_DUR_300ms: return 300;
  case DTMF_DUR_500ms: return 500;
  }
  return 50;
}
void
D878UVCodeplug::GeneralSettingsElement::setDTMFToneDuration(unsigned ms) {
  if (ms<=50) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_50ms);
  } else if (ms<=100) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_100ms);
  } else if (ms<=200) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_200ms);
  } else if (ms<=300) {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_300ms);
  } else {
    setUInt8(Offset::dtmfToneDuration(), DTMF_DUR_500ms);
  }
}

bool
D878UVCodeplug::GeneralSettingsElement::manDown() const {
  return getUInt8(Offset::manDown());
}
void
D878UVCodeplug::GeneralSettingsElement::enableManDown(bool enable) {
  setUInt8(Offset::manDown(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::wfmMonitor() const {
  return getUInt8(Offset::wfmMonitor());
}
void
D878UVCodeplug::GeneralSettingsElement::enableWFMMonitor(bool enable) {
  setUInt8(Offset::wfmMonitor(), (enable ? 0x01 : 0x00));
}

Frequency
D878UVCodeplug::GeneralSettingsElement::tbstFrequency() const {
  switch ((TBSTFrequency)getUInt8(Offset::tbstFrequency())) {
  case TBSTFrequency::Hz1000: return Frequency::fromHz(1000);
  case TBSTFrequency::Hz1450: return Frequency::fromHz(1450);
  case TBSTFrequency::Hz1750: return Frequency::fromHz(1750);
  case TBSTFrequency::Hz2100: return Frequency::fromHz(2100);
  }
  return Frequency::fromHz(1750);
}
void
D878UVCodeplug::GeneralSettingsElement::setTBSTFrequency(Frequency freq) {
  if (1000 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1000);
  } else if (1450 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1450);
  } else if (1750 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  } else if (2100 == freq.inHz()) {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz2100);
  } else {
    setUInt8(Offset::tbstFrequency(), (unsigned)TBSTFrequency::Hz1750);
  }
}

bool
D878UVCodeplug::GeneralSettingsElement::proMode() const {
  return getUInt8(Offset::proMode());
}
void
D878UVCodeplug::GeneralSettingsElement::enableProMode(bool enable) {
  setUInt8(Offset::proMode(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::filterOwnID() const {
  return getUInt8(Offset::filterOwnID());
}
void
D878UVCodeplug::GeneralSettingsElement::enableFilterOwnID(bool enable) {
  setUInt8(Offset::filterOwnID(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::remoteStunKill() const {
  return getUInt8(Offset::remoteStunKill());
}
void
D878UVCodeplug::GeneralSettingsElement::enableRemoteStunKill(bool enable) {
  setUInt8(Offset::remoteStunKill(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::remoteMonitor() const {
  return getUInt8(Offset::remoteMonitor());
}
void
D878UVCodeplug::GeneralSettingsElement::enableRemoteMonitor(bool enable) {
  setUInt8(Offset::remoteMonitor(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::SlotMatch
D878UVCodeplug::GeneralSettingsElement::monitorSlotMatch() const {
  return (AnytoneDMRSettingsExtension::SlotMatch)getUInt8(Offset::monSlotMatch());
}
void
D878UVCodeplug::GeneralSettingsElement::setMonitorSlotMatch(AnytoneDMRSettingsExtension::SlotMatch match) {
  setUInt8(Offset::monSlotMatch(), (unsigned)match);
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorColorCodeMatch() const {
  return getUInt8(Offset::monColorCodeMatch());
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorColorCodeMatch(bool enable) {
  setUInt8(Offset::monColorCodeMatch(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorIDMatch() const {
  return getUInt8(Offset::monIDMatch());
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorIDMatch(bool enable) {
  setUInt8(Offset::monIDMatch(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::monitorTimeSlotHold() const {
  return getUInt8(Offset::monTimeSlotHold());
}
void
D878UVCodeplug::GeneralSettingsElement::enableMonitorTimeSlotHold(bool enable) {
  setUInt8(Offset::monTimeSlotHold(), (enable ? 0x01 : 0x00));
}

Interval
D878UVCodeplug::GeneralSettingsElement::manDownDelay() const {
  return Interval::fromSeconds(getUInt8(Offset::manDownDelay()));
}
void
D878UVCodeplug::GeneralSettingsElement::setManDownDelay(Interval sec) {
  setUInt8(Offset::manDownDelay(), sec.seconds());
}

unsigned
D878UVCodeplug::GeneralSettingsElement::fmCallHold() const {
  return getUInt8(Offset::fmCallHold());
}
void
D878UVCodeplug::GeneralSettingsElement::setFMCallHold(unsigned sec) {
  setUInt8(Offset::fmCallHold(), sec);
}

bool
D878UVCodeplug::GeneralSettingsElement::gpsMessageEnabled() const {
  return getUInt8(Offset::enableGPSMessage());
}
void
D878UVCodeplug::GeneralSettingsElement::enableGPSMessage(bool enable) {
  setUInt8(Offset::enableGPSMessage(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::maintainCallChannel() const {
  return getUInt8(Offset::maintainCallChannel());
}
void
D878UVCodeplug::GeneralSettingsElement::enableMaintainCallChannel(bool enable) {
  setUInt8(Offset::maintainCallChannel(), (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::priorityZoneAIndex() const {
  return getUInt8(Offset::priorityZoneA());
}
void
D878UVCodeplug::GeneralSettingsElement::setPriorityZoneAIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneA(), idx);
}
unsigned
D878UVCodeplug::GeneralSettingsElement::priorityZoneBIndex() const {
  return getUInt8(Offset::priorityZoneB());
}
void
D878UVCodeplug::GeneralSettingsElement::setPriorityZoneBIndex(unsigned idx) {
  setUInt8(Offset::priorityZoneB(), idx);
}

bool
D878UVCodeplug::GeneralSettingsElement::bluetooth() const {
  return 0 != getUInt8(Offset::bluetooth());
}
void
D878UVCodeplug::GeneralSettingsElement::enableBluetooth(bool enable) {
  setUInt8(Offset::bluetooth(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::GeneralSettingsElement::btAndInternalMic() const {
  return 0 != getUInt8(Offset::btAndInternalMic());
}
void
D878UVCodeplug::GeneralSettingsElement::enableBTAndInternalMic(bool enable) {
  setUInt8(Offset::btAndInternalMic(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::GeneralSettingsElement::btAndInternalSpeaker() const {
  return 0 != getUInt8(Offset::btAndInternalSpeaker());
}
void
D878UVCodeplug::GeneralSettingsElement::enableBTAndInternalSpeaker(bool enable) {
  setUInt8(Offset::btAndInternalSpeaker(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::GeneralSettingsElement::pluginRecTone() const {
  return 0 != getUInt8(Offset::pluginRecTone());
}
void
D878UVCodeplug::GeneralSettingsElement::enablePluginRecTone(bool enable) {
  setUInt8(Offset::pluginRecTone(), enable ? 0x01 : 0x00);
}

Interval
D878UVCodeplug::GeneralSettingsElement::gpsUpdatePeriod() const {
  return Interval::fromSeconds(getUInt8(Offset::gpsRangingInterval()));
}
void
D878UVCodeplug::GeneralSettingsElement::setGPSUpdatePeriod(Interval intv) {
  setUInt8(Offset::gpsRangingInterval(), intv.seconds());
}

unsigned int
D878UVCodeplug::GeneralSettingsElement::btMicGain() const {
  return (getUInt8(Offset::btMicGain())+1)*2;
}
void
D878UVCodeplug::GeneralSettingsElement::setBTMicGain(unsigned int gain) {
  gain = std::min(10U, std::max(2U, gain));
  setUInt8(Offset::btMicGain(), gain/2-1);
}

unsigned int
D878UVCodeplug::GeneralSettingsElement::btSpeakerGain() const {
  return (getUInt8(Offset::btSpeakerGain())+1)*2;
}
void
D878UVCodeplug::GeneralSettingsElement::setBTSpeakerGain(unsigned int gain) {
  gain = std::min(10U, std::max(2U, gain));
  setUInt8(Offset::btSpeakerGain(), gain/2-1);
}

bool
D878UVCodeplug::GeneralSettingsElement::displayChannelNumber() const {
  return getUInt8(Offset::showChannelNumber());
}
void
D878UVCodeplug::GeneralSettingsElement::enableDisplayChannelNumber(bool enable) {
  setUInt8(Offset::showChannelNumber(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::showCurrentContact() const {
  return getUInt8(Offset::showCurrentContact());
}
void
D878UVCodeplug::GeneralSettingsElement::enableShowCurrentContact(bool enable) {
  setUInt8(Offset::showCurrentContact(), (enable ? 0x01 : 0x00));
}

Interval
D878UVCodeplug::GeneralSettingsElement::autoRoamPeriod() const {
  return Interval::fromMinutes(getUInt8(Offset::autoRoamPeriod()));
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRoamPeriod(Interval intv) {
  setUInt8(Offset::autoRoamPeriod(), intv.minutes());
}

bool
D878UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(Offset::keyToneLevel()))*10/15;
}
void
D878UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(Offset::keyToneLevel(), level*10/15);
}
void
D878UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(Offset::keyToneLevel(), 0);
}

AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return NameColor::decode(getUInt8(Offset::callColor()));
}
void
D878UVCodeplug::GeneralSettingsElement::setCallDisplayColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::callColor(), NameColor::encode(color));
}

bool
D878UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(Offset::gpsUnits());
}
void
D878UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(Offset::gpsUnits(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(Offset::knobLock(), 0);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(Offset::knobLock(), 0, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(Offset::keypadLock(), 1);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(Offset::keypadLock(), 1, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(Offset::sideKeyLock(), 3);
}
void
D878UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(Offset::sideKeyLock(), 3, enable);
}
bool
D878UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(Offset::forceKeyLock(), 4);
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(Offset::forceKeyLock(), 4, enable);
}

Interval
D878UVCodeplug::GeneralSettingsElement::autoRoamDelay() const {
  return Interval::fromSeconds(getUInt8(Offset::autoRoamDelay()));
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRoamDelay(Interval intv) {
  setUInt8(Offset::autoRoamDelay(), intv.seconds());
}

AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::GeneralSettingsElement::standbyTextColor() const {
  return TextColor::decode(getUInt8(Offset::standbyTextColor()));
}
void
D878UVCodeplug::GeneralSettingsElement::setStandbyTextColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::standbyTextColor(), TextColor::encode(color));
}

D878UVCodeplug::GeneralSettingsElement::BackgroundImage
D878UVCodeplug::GeneralSettingsElement::standbyBackgroundImage() const {
  return (D878UVCodeplug::GeneralSettingsElement::BackgroundImage)getUInt8(Offset::standbyBackground());
}
void
D878UVCodeplug::GeneralSettingsElement::setStandbyBackgroundImage(D878UVCodeplug::GeneralSettingsElement::BackgroundImage img) {
  setUInt8(Offset::standbyBackground(), (unsigned)img);
}

bool
D878UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(Offset::showLastHeard());
}
void
D878UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(Offset::showLastHeard(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::SMSFormat
D878UVCodeplug::GeneralSettingsElement::smsFormat() const {
  return (AnytoneDMRSettingsExtension::SMSFormat) getUInt8(Offset::smsFormat());
}
void
D878UVCodeplug::GeneralSettingsElement::setSMSFormat(AnytoneDMRSettingsExtension::SMSFormat fmt) {
  setUInt8(Offset::smsFormat(), (unsigned)fmt);
}

Frequency
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinVHF())*10);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinVHF(), freq.inHz()/10);
}
Frequency
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxVHF())*10);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxVHF(), freq.inHz()/10);
}

Frequency
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMinUHF())*10);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMinUHF(), freq.inHz()/10);
}
Frequency
D878UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return Frequency::fromHz(getUInt32_le(Offset::autoRepMaxUHF())*10);
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(Frequency freq) {
  setUInt32_le(Offset::autoRepMaxUHF(), freq.inHz()/10);
}

AnytoneAutoRepeaterSettingsExtension::Direction
D878UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AnytoneAutoRepeaterSettingsExtension::Direction)getUInt8(Offset::autoRepeaterDirB());
}
void
D878UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AnytoneAutoRepeaterSettingsExtension::Direction dir) {
  setUInt8(Offset::autoRepeaterDirB(), (unsigned)dir);
}

bool
D878UVCodeplug::GeneralSettingsElement::fmSendIDAndContact() const {
  return 0 != getUInt8(Offset::fmSendIDAndContact());
}
void
D878UVCodeplug::GeneralSettingsElement::enableFMSendIDAndContact(bool enable) {
  setUInt8(Offset::fmSendIDAndContact(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(Offset::defaultChannels());
}
void
D878UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(Offset::defaultChannels(), (enable ? 0x01 : 0x00));
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(Offset::defaultZoneA());
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(Offset::defaultZoneA(), idx);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(Offset::defaultZoneB());
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(Offset::defaultZoneB(), idx);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(Offset::defaultChannelA());
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelA(), idx);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

bool
D878UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned
D878UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(Offset::defaultChannelB());
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(Offset::defaultChannelB(), idx);
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::defaultRoamingZoneIndex() const {
  return getUInt8(Offset::defaultRoamingZone());
}
void
D878UVCodeplug::GeneralSettingsElement::setDefaultRoamingZoneIndex(unsigned idx) {
  setUInt8(Offset::defaultRoamingZone(), idx);
}

bool
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheck() const {
  return getUInt8(Offset::repRangeCheck());
}
void
D878UVCodeplug::GeneralSettingsElement::enableRepeaterRangeCheck(bool enable) {
  setUInt8(Offset::repRangeCheck(), (enable ? 0x01 : 0x00));
}

Interval
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheckInterval() const {
  return Interval::fromSeconds(((unsigned)getUInt8(Offset::rangeCheckInterval()))*5);
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckInterval(Interval intv) {
  setUInt8(Offset::rangeCheckInterval(), intv.seconds()/5);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::repeaterRangeCheckCount() const {
  return getUInt8(Offset::rangeCheckCount());
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterRangeCheckCount(unsigned n) {
  setUInt8(Offset::rangeCheckCount(), n);
}

AnytoneRoamingSettingsExtension::RoamStart
D878UVCodeplug::GeneralSettingsElement::roamingStartCondition() const {
  return (AnytoneRoamingSettingsExtension::RoamStart)getUInt8(Offset::roamStartCondition());
}
void
D878UVCodeplug::GeneralSettingsElement::setRoamingStartCondition(AnytoneRoamingSettingsExtension::RoamStart cond) {
  setUInt8(Offset::roamStartCondition(), (unsigned)cond);
}

Interval
D878UVCodeplug::GeneralSettingsElement::txBacklightDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::txBacklightDuration()));
}
void
D878UVCodeplug::GeneralSettingsElement::setTXBacklightDuration(Interval intv) {
  setUInt8(Offset::txBacklightDuration(), intv.seconds());
}

bool
D878UVCodeplug::GeneralSettingsElement::separateDisplay() const {
  return getUInt8(Offset::displaySeparator());
}
void
D878UVCodeplug::GeneralSettingsElement::enableSeparateDisplay(bool enable) {
  setUInt8(Offset::displaySeparator(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(Offset::keepLastCaller());
}
void
D878UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(Offset::keepLastCaller(), (enable ? 0x01 : 0x00));
}

AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::GeneralSettingsElement::channelNameColor() const {
  return NameColor::decode(getUInt8(Offset::channelNameColor()));
}
void
D878UVCodeplug::GeneralSettingsElement::setChannelNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::channelNameColor(), NameColor::encode(color));
}

bool
D878UVCodeplug::GeneralSettingsElement::repeaterCheckNotification() const {
  return getUInt8(Offset::repCheckNotify());
}
void
D878UVCodeplug::GeneralSettingsElement::enableRepeaterCheckNotification(bool enable) {
  setUInt8(Offset::repCheckNotify(), (enable ? 0x01 : 0x00));
}


Interval
D878UVCodeplug::GeneralSettingsElement::rxBacklightDuration() const {
  return Interval::fromSeconds(getUInt8(Offset::rxBacklightDuration()));
}
void
D878UVCodeplug::GeneralSettingsElement::setRXBacklightDuration(Interval intv) {
  setUInt8(Offset::rxBacklightDuration(), intv.seconds());
}

bool
D878UVCodeplug::GeneralSettingsElement::roaming() const {
  return getUInt8(Offset::roaming());
}
void
D878UVCodeplug::GeneralSettingsElement::enableRoaming(bool enable) {
  setUInt8(Offset::roaming(), (enable ? 0x01 : 0x00));
}

Interval
D878UVCodeplug::GeneralSettingsElement::muteDelay() const {
  return Interval::fromMinutes(getUInt8(Offset::muteDelay())+1);
}
void
D878UVCodeplug::GeneralSettingsElement::setMuteDelay(Interval min) {
  setUInt8(Offset::muteDelay(), std::max(1ULL, min.minutes())-1);
}

unsigned
D878UVCodeplug::GeneralSettingsElement::repeaterCheckNumNotifications() const {
  return getUInt8(Offset::repCheckNumNotify());
}
void
D878UVCodeplug::GeneralSettingsElement::setRepeaterCheckNumNotifications(unsigned num) {
  setUInt8(Offset::repCheckNumNotify(), num);
}

bool
D878UVCodeplug::GeneralSettingsElement::bootGPSCheck() const {
  return getUInt8(Offset::bootGPSCheck());
}
void
D878UVCodeplug::GeneralSettingsElement::enableBootGPSCheck(bool enable) {
  setUInt8(Offset::bootGPSCheck(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::bootReset() const {
  return getUInt8(Offset::bootReset());
}
void
D878UVCodeplug::GeneralSettingsElement::enableBootReset(bool enable) {
  setUInt8(Offset::bootReset(), (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::GeneralSettingsElement::btHoldTimeEnabled() const {
  return 0x00 != getUInt8(Offset::btHoldTime());
}
bool
D878UVCodeplug::GeneralSettingsElement::btHoldTimeInfinite() const {
  return 121U == getUInt8(Offset::btHoldTime());
}
Interval
D878UVCodeplug::GeneralSettingsElement::btHoldTime() const {
  return Interval::fromSeconds(getUInt8(Offset::btHoldTime()));
}
void
D878UVCodeplug::GeneralSettingsElement::setBTHoldTime(Interval interval) {
  unsigned int seconds = std::min(120ULL, std::max(1ULL, interval.seconds()));
  setUInt8(Offset::btHoldTime(), seconds);
}
void
D878UVCodeplug::GeneralSettingsElement::setBTHoldTimeInfinite() {
  setUInt8(Offset::btHoldTime(), 121);
}
void
D878UVCodeplug::GeneralSettingsElement::disableBTHoldTime() {
  setUInt8(Offset::btHoldTime(), 0);
}

Interval
D878UVCodeplug::GeneralSettingsElement::btRXDelay() const {
  if (0 == getUInt8(Offset::btRXDelay()))
    return Interval::fromMilliseconds(30);
  return Interval::fromMilliseconds(((unsigned int)getUInt8(Offset::btRXDelay())+1)*500);
}
void
D878UVCodeplug::GeneralSettingsElement::setBTRXDelay(Interval delay) {
  if (500 >= delay.milliseconds()) {
    setUInt8(Offset::btRXDelay(), 0);
  } else {
    unsigned int millis = std::min(5500ULL, std::max(500ULL, delay.milliseconds()));
    setUInt8(Offset::btRXDelay(), (millis-500)/500);
  }
}

bool
D878UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  // Set measurement system based on system locale (0x00==Metric)
  enableGPSUnitsImperial(QLocale::ImperialSystem == QLocale::system().measurementSystem());
  // Set transmit timeout
  setTransmitTimeout(ctx.config()->settings()->tot());

  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext)
    return true;

  // Encode boot settings
  if (ext->bootSettings()->priorityZoneA()->isNull())
    setPriorityZoneAIndex(0xff);
  else
    setPriorityZoneAIndex(ctx.index(ext->bootSettings()->priorityZoneA()->as<Zone>()));
  if (ext->bootSettings()->priorityZoneB()->isNull())
    setPriorityZoneBIndex(0xff);
  else
    setPriorityZoneBIndex(ctx.index(ext->bootSettings()->priorityZoneB()->as<Zone>()));
  if (! ext->roamingSettings()->defaultZone()->isNull())
    setDefaultRoamingZoneIndex(ctx.index(ext->roamingSettings()->defaultZone()->as<RoamingZone>()));
  enableBootGPSCheck(ext->bootSettings()->gpsCheckEnabled());
  enableBootReset(ext->bootSettings()->resetEnabled());

  // Encode key settings
  enableKnobLock(ext->keySettings()->knobLockEnabled());
  enableKeypadLock(ext->keySettings()->keypadLockEnabled());
  enableSidekeysLock(ext->keySettings()->sideKeysLockEnabled());
  enableKeyLockForced(ext->keySettings()->forcedKeyLockEnabled());

  // Encode tone settings
  setKeyToneLevel(ext->toneSettings()->keyToneLevel());

  // Encode audio settings
  setMuteDelay(ext->audioSettings()->muteDelay());

  // Encode display settings
  setCallDisplayColor(ext->displaySettings()->callColor());
  setLanguage(ext->displaySettings()->language());
  enableDisplayChannelNumber(ext->displaySettings()->showChannelNumberEnabled());
  enableShowCurrentContact(ext->displaySettings()->showContact());
  setStandbyTextColor(ext->displaySettings()->standbyTextColor());
  //setStandbyBackgroundImage(ext->displaySettings()->standbyBackgroundColor());
  enableShowLastHeard(ext->displaySettings()->showLastHeardEnabled());
  setChannelNameColor(ext->displaySettings()->channelNameColor());
  setRXBacklightDuration(ext->displaySettings()->backlightDurationRX());
  setTXBacklightDuration(ext->displaySettings()->backlightDurationTX());

  // Encode menu settings
  enableSeparateDisplay(ext->menuSettings()->separatorEnabled());

  // Encode auto-repeater settings
  setAutoRepeaterDirectionB(ext->autoRepeaterSettings()->directionB());
  setAutoRepeaterMinFrequencyVHF(ext->autoRepeaterSettings()->vhfMin());
  setAutoRepeaterMaxFrequencyVHF(ext->autoRepeaterSettings()->vhfMax());
  setAutoRepeaterMinFrequencyUHF(ext->autoRepeaterSettings()->uhfMin());
  setAutoRepeaterMaxFrequencyUHF(ext->autoRepeaterSettings()->uhfMax());

  // Encode DMR settings
  setGroupCallHangTime(ext->dmrSettings()->groupCallHangTime());
  setPrivateCallHangTime(ext->dmrSettings()->privateCallHangTime());
  setPreWaveDelay(ext->dmrSettings()->preWaveDelay());
  setWakeHeadPeriod(ext->dmrSettings()->wakeHeadPeriod());
  enableFilterOwnID(ext->dmrSettings()->filterOwnIDEnabled());
  setMonitorSlotMatch(ext->dmrSettings()->monitorSlotMatch());
  enableMonitorColorCodeMatch(ext->dmrSettings()->monitorColorCodeMatchEnabled());
  enableMonitorIDMatch(ext->dmrSettings()->monitorIDMatchEnabled());
  enableMonitorTimeSlotHold(ext->dmrSettings()->monitorTimeSlotHoldEnabled());
  setSMSFormat(ext->dmrSettings()->smsFormat());

  // Encode GPS settings
  enableGPSUnitsImperial(AnytoneGPSSettingsExtension::Units::Archaic == ext->gpsSettings()->units());
  setGPSTimeZone(ext->gpsSettings()->timeZone());
  enableGPSMessage(ext->gpsSettings()->positionReportingEnabled());
  setGPSUpdatePeriod(ext->gpsSettings()->updatePeriod());

  // Encode ranging/roaming settings.
  setAutoRoamPeriod(ext->roamingSettings()->autoRoamPeriod());
  setAutoRoamDelay(ext->roamingSettings()->autoRoamDelay());
  enableRepeaterRangeCheck(ext->roamingSettings()->repeaterRangeCheckEnabled());
  setRepeaterRangeCheckInterval(ext->roamingSettings()->repeaterCheckInterval());
  setRepeaterRangeCheckCount(ext->roamingSettings()->repeaterRangeCheckCount());
  setRoamingStartCondition(ext->roamingSettings()->roamingStartCondition());
  enableRepeaterCheckNotification(ext->roamingSettings()->notificationEnabled());
  setRepeaterCheckNumNotifications(ext->roamingSettings()->notificationCount());

  // Encode other settings
  enableKeepLastCaller(ext->keepLastCallerEnabled());
  setVFOFrequencyStep(ext->vfoStep());
  setSTEType(ext->steType());
  setSTEFrequency(ext->steFrequency());
  setTBSTFrequency(ext->tbstFrequency());
  enableProMode(ext->proModeEnabled());
  enableMaintainCallChannel(ext->maintainCallChannelEnabled());

  return true;
}

bool
D878UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;

  ctx.config()->settings()->setTOT(transmitTimeout());

  // Get or add settings extension
  AnytoneSettingsExtension *ext = nullptr;
  if (ctx.config()->settings()->anytoneExtension()) {
    ext = ctx.config()->settings()->anytoneExtension();
  } else {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Decode boot settings
  ext->bootSettings()->enableGPSCheck(this->bootGPSCheck());
  ext->bootSettings()->enableReset(this->bootReset());

  // Decode key settings
  ext->keySettings()->enableKnobLock(this->knobLock());
  ext->keySettings()->enableKeypadLock(this->keypadLock());
  ext->keySettings()->enableSideKeysLock(this->sidekeysLock());
  ext->keySettings()->enableForcedKeyLock(this->keyLockForced());

  // Decode tone settings
  ext->toneSettings()->setKeyToneLevel(keyToneLevel());

  // Store audio settings
  ext->audioSettings()->setMuteDelay(this->muteDelay());

  // Decode display settings
  ext->displaySettings()->setCallColor(this->callDisplayColor());
  ext->displaySettings()->setLanguage(this->language());
  ext->displaySettings()->enableShowChannelNumber(this->displayChannelNumber());
  ext->displaySettings()->enableShowContact(this->showCurrentContact());
  ext->displaySettings()->setStandbyTextColor(this->standbyTextColor());
  //ext->displaySettings()->setStandbyBackgroundColor(this->standbyBackgroundImage());
  ext->displaySettings()->enableShowLastHeard(this->showLastHeard());
  ext->displaySettings()->setBacklightDurationTX(this->txBacklightDuration());
  ext->displaySettings()->setChannelNameColor(this->channelNameColor());
  ext->displaySettings()->setBacklightDurationRX(this->rxBacklightDuration());

  // Decode menu settings
  ext->menuSettings()->enableSeparator(this->separateDisplay());

  // Decode auto-repeater settings
  ext->autoRepeaterSettings()->setDirectionB(autoRepeaterDirectionB());
  ext->autoRepeaterSettings()->setVHFMin(this->autoRepeaterMinFrequencyVHF());
  ext->autoRepeaterSettings()->setVHFMax(this->autoRepeaterMaxFrequencyVHF());
  ext->autoRepeaterSettings()->setUHFMin(this->autoRepeaterMinFrequencyUHF());
  ext->autoRepeaterSettings()->setUHFMax(this->autoRepeaterMaxFrequencyUHF());

  // Encode dmr settings
  ext->dmrSettings()->setGroupCallHangTime(this->groupCallHangTime());
  ext->dmrSettings()->setPrivateCallHangTime(this->privateCallHangTime());
  ext->dmrSettings()->setPreWaveDelay(this->preWaveDelay());
  ext->dmrSettings()->setWakeHeadPeriod(this->wakeHeadPeriod());
  ext->dmrSettings()->enableFilterOwnID(this->filterOwnID());
  ext->dmrSettings()->setMonitorSlotMatch(this->monitorSlotMatch());
  ext->dmrSettings()->enableMonitorColorCodeMatch(this->monitorColorCodeMatch());
  ext->dmrSettings()->enableMonitorIDMatch(this->monitorIDMatch());
  ext->dmrSettings()->enableMonitorTimeSlotHold(this->monitorTimeSlotHold());
  ext->dmrSettings()->setSMSFormat(this->smsFormat());

  // Encode GPS settings
  ext->gpsSettings()->setUnits(
        this->gpsUnitsImperial() ? AnytoneGPSSettingsExtension::Units::Archaic :
                                   AnytoneGPSSettingsExtension::Units::Metric);
  ext->gpsSettings()->enablePositionReporting(this->gpsMessageEnabled());
  ext->gpsSettings()->setUpdatePeriod(this->gpsUpdatePeriod());

  // Encode ranging/roaming settings
  ext->roamingSettings()->setAutoRoamPeriod(this->autoRoamPeriod());
  ext->roamingSettings()->setAutoRoamDelay(this->autoRoamDelay());
  ext->roamingSettings()->enableRepeaterRangeCheck(this->repeaterRangeCheck());
  ext->roamingSettings()->setRepeaterCheckInterval(this->repeaterRangeCheckInterval());
  ext->roamingSettings()->setRepeaterRangeCheckCount(this->repeaterRangeCheckCount());
  ext->roamingSettings()->setRoamingStartCondition(this->roamingStartCondition());
  ext->roamingSettings()->enableNotification(this->repeaterCheckNotification());
  ext->roamingSettings()->setNotificationCount(this->repeaterCheckNumNotifications());

  // Decode other settings
  ext->enableKeepLastCaller(this->keepLastCaller());
  ext->setVFOStep(this->vfoFrequencyStep());
  ext->setSTEType(this->steType());
  ext->setSTEFrequency(this->steFrequency());
  ext->setTBSTFrequency(this->tbstFrequency());
  ext->enableProMode(this->proMode());
  ext->enableMaintainCallChannel(this->maintainCallChannel());

  return true;
}

bool
D878UVCodeplug::GeneralSettingsElement::linkSettings(RadioSettings *settings, Context &ctx, const ErrorStack &err) {
  if (! AnytoneCodeplug::GeneralSettingsElement::linkSettings(settings, ctx, err))
    return false;

  AnytoneSettingsExtension *ext = settings->anytoneExtension();

  if (0xff != priorityZoneAIndex()) {
    if (! ctx.has<Zone>(priorityZoneAIndex())) {
      errMsg(err) << "Cannot link priority zone A index " << priorityZoneAIndex()
                  << ": Zone with that index not defined.";
      return false;
    }
    ext->bootSettings()->priorityZoneA()->set(ctx.get<Zone>(priorityZoneAIndex()));
  }
  if (0xff != priorityZoneBIndex()) {
    if (! ctx.has<Zone>(priorityZoneBIndex())) {
      errMsg(err) << "Cannot link priority zone B index " << priorityZoneBIndex()
                  << ": Zone with that index not defined.";
      return false;
    }
    ext->bootSettings()->priorityZoneB()->set(ctx.get<Zone>(priorityZoneBIndex()));
  }

  if (ctx.has<RoamingZone>(defaultRoamingZoneIndex())) {
    ext->roamingSettings()->defaultZone()->set(ctx.get<RoamingZone>(this->defaultRoamingZoneIndex()));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::GeneralSettingsExtensionElement
 * ******************************************************************************************** */
D878UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::ExtendedSettingsElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::ExtendedSettingsElement::ExtendedSettingsElement(uint8_t *ptr)
  : AnytoneCodeplug::ExtendedSettingsElement(ptr, ExtendedSettingsElement::size())
{
  // pass...
}

void
D878UVCodeplug::ExtendedSettingsElement::clear() {
  memset(_data, 0x00, _size);
  clearAutoRepeaterVHF2OffsetIndex();
  clearAutoRepeaterUHF2OffsetIndex();
}

bool
D878UVCodeplug::ExtendedSettingsElement::sendTalkerAlias() const {
  return getUInt8(Offset::sendTalkerAlias());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableSendTalkerAlias(bool enable) {
  setUInt8(Offset::sendTalkerAlias(), (enable ? 0x01 : 0x00));
}

AnytoneDMRSettingsExtension::TalkerAliasSource
D878UVCodeplug::ExtendedSettingsElement::talkerAliasSource() const {
  return (AnytoneDMRSettingsExtension::TalkerAliasSource)getUInt8(Offset::talkerAliasDisplay());
}
void
D878UVCodeplug::ExtendedSettingsElement::setTalkerAliasSource(AnytoneDMRSettingsExtension::TalkerAliasSource mode) {
  setUInt8(Offset::talkerAliasDisplay(), (unsigned)mode);
}

AnytoneDMRSettingsExtension::TalkerAliasEncoding
D878UVCodeplug::ExtendedSettingsElement::talkerAliasEncoding() const {
  return (AnytoneDMRSettingsExtension::TalkerAliasEncoding)getUInt8(Offset::talkerAliasEncoding());
}
void
D878UVCodeplug::ExtendedSettingsElement::setTalkerAliasEncoding(AnytoneDMRSettingsExtension::TalkerAliasEncoding enc) {
  setUInt8(Offset::talkerAliasEncoding(), (unsigned)enc);
}

bool
D878UVCodeplug::ExtendedSettingsElement::bluetoothPTTLatch() const {
  return getUInt8(Offset::btPTTLatch());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableBluetoothPTTLatch(bool enable) {
  setUInt8(Offset::btPTTLatch(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::ExtendedSettingsElement::infiniteBluetoothPTTSleepDelay() const {
  return  bluetoothPTTSleepDelay().isNull();
}
Interval
D878UVCodeplug::ExtendedSettingsElement::bluetoothPTTSleepDelay() const {
  return Interval::fromMinutes(getUInt8(Offset::btPTTSleepDelay()));
}
void
D878UVCodeplug::ExtendedSettingsElement::setBluetoothPTTSleepDelay(Interval delay) {
  unsigned int t = std::min(Limit::maxBluetoothPTTSleepDelay(), (unsigned int)delay.minutes());
  setUInt8(Offset::btPTTSleepDelay(), t);
}
void
D878UVCodeplug::ExtendedSettingsElement::setInfiniteBluetoothPTTSleepDelay() {
  setBluetoothPTTSleepDelay(Interval::fromMinutes(0));
}

bool
D878UVCodeplug::ExtendedSettingsElement::hasAutoRepeaterUHF2OffsetIndex() const {
  return 0xff != getUInt8(Offset::autoRepeaterUHF2OffsetIndex());
}
unsigned
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterUHF2OffsetIndex() const {
  return getUInt8(Offset::autoRepeaterUHF2OffsetIndex());
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterUHF2OffsetIndex(unsigned idx) {
  setUInt8(Offset::autoRepeaterUHF2OffsetIndex(), idx);
}
void
D878UVCodeplug::ExtendedSettingsElement::clearAutoRepeaterUHF2OffsetIndex() {
  setUInt8(Offset::autoRepeaterUHF2OffsetIndex(), 0xff);
}

bool
D878UVCodeplug::ExtendedSettingsElement::hasAutoRepeaterVHF2OffsetIndex() const {
  return 0xff != getUInt8(Offset::autoRepeaterVHF2OffsetIndex());
}
unsigned
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterVHF2OffsetIndex() const {
  return getUInt8(Offset::autoRepeaterVHF2OffsetIndex());
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterVHF2OffsetIndex(unsigned idx) {
  setUInt8(Offset::autoRepeaterVHF2OffsetIndex(), idx);
}
void
D878UVCodeplug::ExtendedSettingsElement::clearAutoRepeaterVHF2OffsetIndex() {
  setUInt8(Offset::autoRepeaterVHF2OffsetIndex(), 0xff);
}

Frequency
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterVHF2MinFrequency() const {
  return Frequency::fromHz(((unsigned long long)getUInt32_le(Offset::autoRepeaterVHF2MinFrequency()))*10);
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterVHF2MinFrequency(Frequency hz) {
  setUInt32_le(Offset::autoRepeaterVHF2MinFrequency(), hz.inHz()/10);
}
Frequency
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterVHF2MaxFrequency() const {
  return Frequency::fromHz(((unsigned long long)getUInt32_le(Offset::autoRepeaterVHF2MaxFrequency()))*10);
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterVHF2MaxFrequency(Frequency hz) {
  setBCD8_be(Offset::autoRepeaterVHF2MaxFrequency(), hz.inHz()/10);
}
Frequency
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterUHF2MinFrequency() const {
  return Frequency::fromHz(((unsigned long long)getUInt32_le(Offset::autoRepeaterUHF2MinFrequency()))*10);
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterUHF2MinFrequency(Frequency hz) {
  setUInt32_le(Offset::autoRepeaterUHF2MinFrequency(), hz.inHz()/10);
}
Frequency
D878UVCodeplug::ExtendedSettingsElement::autoRepeaterUHF2MaxFrequency() const {
  return Frequency::fromHz(((unsigned long long)getUInt32_le(Offset::autoRepeaterUHF2MaxFrequency()))*10);
}
void
D878UVCodeplug::ExtendedSettingsElement::setAutoRepeaterUHF2MaxFrequency(Frequency hz) {
  setUInt32_le(Offset::autoRepeaterUHF2MaxFrequency(), hz.inHz()/10);
}

AnytoneGPSSettingsExtension::GPSMode
D878UVCodeplug::ExtendedSettingsElement::gpsMode() const {
  return (AnytoneGPSSettingsExtension::GPSMode)getUInt8(Offset::gpsMode());
}
void
D878UVCodeplug::ExtendedSettingsElement::setGPSMode(AnytoneGPSSettingsExtension::GPSMode mode) {
  setUInt8(Offset::gpsMode(), (unsigned)mode);
}

Interval
D878UVCodeplug::ExtendedSettingsElement::steDuration() const {
  return Interval::fromMilliseconds((getUInt8(Offset::steDuration())+1)*10);
}
void
D878UVCodeplug::ExtendedSettingsElement::setSTEDuration(Interval dur) {
  unsigned int t = std::max(10U, std::min(1000U, (unsigned int)dur.milliseconds()));
  setUInt8(Offset::steDuration(), t/10-1);
}

bool
D878UVCodeplug::ExtendedSettingsElement::infiniteManDialGroupCallHangTime() const {
  return manDialGroupCallHangTime().isNull();
}
Interval
D878UVCodeplug::ExtendedSettingsElement::manDialGroupCallHangTime() const {
  unsigned int t = getUInt8(Offset::manGrpCallHangTime())+1;
  if (t<=30)
    return Interval::fromSeconds(t);
  else if (31==t)
    return Interval::fromMinutes(30);
  return Interval();
}
void
D878UVCodeplug::ExtendedSettingsElement::setManDialGroupCallHangTime(Interval dur) {
  unsigned int t = dur.seconds();
  if (t > 30)
    t = 31;  // = 30min
  else if (0 == t)
    t = 32;  // = infinite
  setUInt8(Offset::manGrpCallHangTime(), t-1);
}
void
D878UVCodeplug::ExtendedSettingsElement::setManDialGroupCallHangTimeInfinite() {
  setManDialGroupCallHangTime(Interval::fromSeconds(0));
}

bool
D878UVCodeplug::ExtendedSettingsElement::infiniteManDialPrivateCallHangTime() const {
  return manDialPrivateCallHangTime().isNull();
}
Interval
D878UVCodeplug::ExtendedSettingsElement::manDialPrivateCallHangTime() const {
  unsigned int t = getUInt8(Offset::manPrivCallHangTime())+1;
  if (t<=30)
    return Interval::fromSeconds(t);
  else if (31==t)
    return Interval::fromMinutes(30);
  return Interval();
}
void
D878UVCodeplug::ExtendedSettingsElement::setManDialPrivateCallHangTime(Interval dur) {
  unsigned int t = dur.seconds();
  if (t > 30)
    t = 31;  // = 30min
  else if (0 == t)
    t = 32;  // = infinite
  setUInt8(Offset::manPrivCallHangTime(), t-1);
}
void
D878UVCodeplug::ExtendedSettingsElement::setManDialPrivateCallHangTimeInfinite() {
  setManDialPrivateCallHangTime(Interval::fromSeconds(0));
}

AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::ExtendedSettingsElement::channelBNameColor() const {
  return NameColor::decode(getUInt8(Offset::channelBNameColor()));
}
void
D878UVCodeplug::ExtendedSettingsElement::setChannelBNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::channelBNameColor(), NameColor::encode(color));
}

AnytoneDMRSettingsExtension::EncryptionType
D878UVCodeplug::ExtendedSettingsElement::encryption() const {
  return (AnytoneDMRSettingsExtension::EncryptionType)getUInt8(Offset::encryptionType());
}
void
D878UVCodeplug::ExtendedSettingsElement::setEncryption(AnytoneDMRSettingsExtension::EncryptionType mode) {
  setUInt8(Offset::encryptionType(), (unsigned int)mode);
}

bool
D878UVCodeplug::ExtendedSettingsElement::totNotification() const {
  return 0x00 != getUInt8(Offset::totNotification());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableTOTNotification(bool enable) {
  setUInt8(Offset::totNotification(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::ExtendedSettingsElement::atpc() const {
  return 0x00 != getUInt8(Offset::atpc());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableATPC(bool enable) {
  setUInt8(Offset::atpc(), enable ? 0x01 : 0x00);
}

AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::ExtendedSettingsElement::zoneANameColor() const {
  return NameColor::decode(getUInt8(Offset::zoneANameColor()));
}
void
D878UVCodeplug::ExtendedSettingsElement::setZoneANameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::zoneANameColor(), NameColor::encode(color));
}
AnytoneDisplaySettingsExtension::Color
D878UVCodeplug::ExtendedSettingsElement::zoneBNameColor() const {
  return NameColor::decode(getUInt8(Offset::zoneBNameColor()));
}
void
D878UVCodeplug::ExtendedSettingsElement::setZoneBNameColor(AnytoneDisplaySettingsExtension::Color color) {
  setUInt8(Offset::zoneBNameColor(), NameColor::encode(color));
}

bool
D878UVCodeplug::ExtendedSettingsElement::resetAutoShutdownOnCall() const {
  return 0x00 != getUInt8(Offset::autoShutdownMode());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableResetAutoShutdownOnCall(bool enable) {
  setUInt8(Offset::autoShutdownMode(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::ExtendedSettingsElement::showColorCode() const {
  return getBit(Offset::displayColorCode(), 2);
}
void
D878UVCodeplug::ExtendedSettingsElement::enableShowColorCode(bool enable) {
  setBit(Offset::displayColorCode(), 2, enable);
}
bool
D878UVCodeplug::ExtendedSettingsElement::showTimeSlot() const {
  return getBit(Offset::displayTimeSlot(), 1);
}
void
D878UVCodeplug::ExtendedSettingsElement::enableShowTimeSlot(bool enable) {
  setBit(Offset::displayTimeSlot(), 1, enable);
}
bool
D878UVCodeplug::ExtendedSettingsElement::showChannelType() const {
  return getBit(Offset::displayChannelType(), 0);
}
void
D878UVCodeplug::ExtendedSettingsElement::enableShowChannelType(bool enable) {
  setBit(Offset::displayChannelType(), 0, enable);
}

bool
D878UVCodeplug::ExtendedSettingsElement::fmIdleTone() const {
  return 0x00 != getUInt8(Offset::fmIdleTone());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableFMIdleTone(bool enable) {
  setUInt8(Offset::fmIdleTone(), enable ? 0x01 : 0x00);
}

AnytoneDisplaySettingsExtension::DateFormat
D878UVCodeplug::ExtendedSettingsElement::dateFormat() const {
  return (AnytoneDisplaySettingsExtension::DateFormat)getUInt8(Offset::dateFormat());
}
void
D878UVCodeplug::ExtendedSettingsElement::setDateFormat(AnytoneDisplaySettingsExtension::DateFormat format) {
  setUInt8(Offset::dateFormat(), (unsigned int)format);
}

unsigned int
D878UVCodeplug::ExtendedSettingsElement::fmMicGain() const {
  return (getUInt8(Offset::analogMicGain())+1)*10/5;
}
void
D878UVCodeplug::ExtendedSettingsElement::setFMMicGain(unsigned int gain) {
  gain = std::min(10U, std::max(1U, gain));
  setUInt8(Offset::analogMicGain(), gain*4/10);
}

bool
D878UVCodeplug::ExtendedSettingsElement::gpsRoaming() const {
  return 0x00 != getUInt8(Offset::gpsRoaming());
}
void
D878UVCodeplug::ExtendedSettingsElement::enableGPSRoaming(bool enable) {
  setUInt8(Offset::gpsRoaming(), enable ? 0x01 : 0x00);
}

void
D878UVCodeplug::ExtendedSettingsElement::callEndToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double freq = getUInt16_le(Offset::callEndTones()+2*i);
    unsigned int duration = getUInt16_le(Offset::callEndDurations()+2*i);
    if (duration) tones.append({freq, duration});
  }
  melody.infer(tones);
}
void
D878UVCodeplug::ExtendedSettingsElement::setCallEndToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::callEndTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::callEndDurations()+2*i, tones.at(i).second);
  }
}

void
D878UVCodeplug::ExtendedSettingsElement::allCallToneMelody(Melody &melody) const {
  QVector<QPair<double, unsigned int>> tones; tones.reserve(5);
  for (int i=0; i<5; i++) {
    double freq = getUInt16_le(Offset::allCallTones()+2*i);
    unsigned int duration = getUInt16_le(Offset::allCallDurations()+2*i);
    if (duration) tones.append({freq, duration});
  }
  melody.infer(tones);
}
void
D878UVCodeplug::ExtendedSettingsElement::setAllCallToneMelody(const Melody &melody) {
  unsigned int n=std::min(5U, (unsigned int)melody.count());
  QVector<QPair<double, unsigned int>> tones = melody.toTones();
  for (unsigned int i=0; i<n; i++) {
    setUInt16_le(Offset::allCallTones()+2*i, tones.at(i).first);
    setUInt16_le(Offset::allCallDurations()+2*i, tones.at(i).second);
  }
}

bool
D878UVCodeplug::ExtendedSettingsElement::fromConfig(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (! flags.updateCodePlug)
    this->clear();

  if (! AnytoneCodeplug::ExtendedSettingsElement::fromConfig(flags, ctx, err))
    return false;

  if (nullptr == ctx.config()->settings()->anytoneExtension()) {
    // If there is no extension, reuse DMR mic gain setting
    setFMMicGain(ctx.config()->settings()->micLevel());
    return true;
  }

  // Get extension
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();

  // Some general settings
  setSTEDuration(ext->steDuration());

  // Power save settings
  enableATPC(ext->powerSaveSettings()->atpc());
  enableResetAutoShutdownOnCall(ext->powerSaveSettings()->resetAutoShutdownOnCall());

  // Encode tone settings
  enableTOTNotification(ext->toneSettings()->totNotification());
  enableFMIdleTone(ext->toneSettings()->fmIdleChannelToneEnabled());
  setCallEndToneMelody(*ext->toneSettings()->callEndMelody());

  // Encode audio settings
  if (ext->audioSettings()->fmMicGainEnabled())
    setFMMicGain(ext->audioSettings()->fmMicGain());
  else
    setFMMicGain(ctx.config()->settings()->micLevel());

  // Encode DMR settings
  setManDialGroupCallHangTime(ext->dmrSettings()->manualGroupCallHangTime());
  setManDialPrivateCallHangTime(ext->dmrSettings()->manualPrivateCallHangTime());
  setEncryption(ext->dmrSettings()->encryption());

  // Encode display settings
  enableShowColorCode(ext->displaySettings()->showColorCode());
  enableShowTimeSlot(ext->displaySettings()->showTimeSlot());
  enableShowChannelType(ext->displaySettings()->showChannelType());
  setDateFormat(ext->displaySettings()->dateFormat());

  // Encode auto-repeater frequency ranges
  setAutoRepeaterVHF2MinFrequency(ext->autoRepeaterSettings()->vhf2Min());
  setAutoRepeaterVHF2MaxFrequency(ext->autoRepeaterSettings()->vhf2Max());
  setAutoRepeaterUHF2MinFrequency(ext->autoRepeaterSettings()->uhf2Min());
  setAutoRepeaterUHF2MaxFrequency(ext->autoRepeaterSettings()->uhf2Max());

  // Encode auto-repeater offset indices
  clearAutoRepeaterVHF2OffsetIndex();
  if (! ext->autoRepeaterSettings()->vhf2Ref()->isNull()) {
    setAutoRepeaterVHF2OffsetIndex(
          ctx.index(
            ext->autoRepeaterSettings()->vhf2Ref()->as<AnytoneAutoRepeaterOffset>()));
  }
  clearAutoRepeaterUHF2OffsetIndex();
  if (! ext->autoRepeaterSettings()->uhf2Ref()->isNull()) {
    setAutoRepeaterUHF2OffsetIndex(
          ctx.index(
            ext->autoRepeaterSettings()->uhf2Ref()->as<AnytoneAutoRepeaterOffset>()));
  }

  // Encode GPS settings
  setGPSMode(ext->gpsSettings()->mode());

  // Encode roaming settings
  enableGPSRoaming(ext->roamingSettings()->gpsRoaming());

  // Encode bluetooth settings
  enableBluetoothPTTLatch(ext->bluetoothSettings()->pttLatch());
  setBluetoothPTTSleepDelay(ext->bluetoothSettings()->pttSleepTimer());

  return true;
}

bool
D878UVCodeplug::ExtendedSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);

  if (! AnytoneCodeplug::ExtendedSettingsElement::updateConfig(ctx, err))
    return false;

  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneSettingsExtension();
    ctx.config()->settings()->setAnytoneExtension(ext);
  }

  // Some general settings
  ext->setSTEDuration(this->steDuration());

  // Some power-save settings
  ext->powerSaveSettings()->enableATPC(this->atpc());
  ext->powerSaveSettings()->enableResetAutoShutdownOnCall(this->resetAutoShutdownOnCall());

  // Store tone settings
  ext->toneSettings()->enableTOTNotification(this->totNotification());
  ext->toneSettings()->enableFMIdleChannelTone(this->fmIdleTone());
  this->callEndToneMelody(*ext->toneSettings()->callEndMelody());

  // Store FM mic gain separately
  ext->audioSettings()->setFMMicGain(fmMicGain());
  // Enable separate mic gain, if it differs from the DMR mic gain:
  ext->audioSettings()->enableFMMicGain(
        ctx.config()->settings()->micLevel() != fmMicGain());

  // Store display settings
  ext->displaySettings()->enableShowColorCode(this->showColorCode());
  ext->displaySettings()->enableShowTimeSlot(this->showTimeSlot());
  ext->displaySettings()->enableShowChannelType(this->showChannelType());
  ext->displaySettings()->setDateFormat(this->dateFormat());

  // Store some DMR settings
  ext->dmrSettings()->setManualGroupCallHangTime(this->manDialGroupCallHangTime());
  ext->dmrSettings()->setManualPrivateCallHangTime(this->manDialPrivateCallHangTime());
  ext->dmrSettings()->setEncryption(this->encryption());

  // Store auto-repeater frequency ranges
  ext->autoRepeaterSettings()->setVHF2Min(this->autoRepeaterVHF2MinFrequency());
  ext->autoRepeaterSettings()->setVHF2Max(this->autoRepeaterVHF2MaxFrequency());
  ext->autoRepeaterSettings()->setUHF2Min(this->autoRepeaterUHF2MinFrequency());
  ext->autoRepeaterSettings()->setUHF2Max(this->autoRepeaterUHF2MaxFrequency());

  // Store GPS settings
  ext->gpsSettings()->setMode(this->gpsMode());

  // Store roaming settings
  ext->roamingSettings()->enableGPSRoaming(this->gpsRoaming());

  // Store bluetooth settings
  ext->bluetoothSettings()->enablePTTLatch(this->bluetoothPTTLatch());
  ext->bluetoothSettings()->setPTTSleepTimer(this->bluetoothPTTSleepDelay());

  return true;
}

bool
D878UVCodeplug::ExtendedSettingsElement::linkConfig(Context &ctx, const ErrorStack &err) {
  if (! AnytoneCodeplug::ExtendedSettingsElement::linkConfig(ctx, err))
    return false;

  // Get or add extension if not present
  AnytoneSettingsExtension *ext = ctx.config()->settings()->anytoneExtension();
  if (nullptr == ext) {
    errMsg(err) << "Cannot link config extension: not set.";
    return false;
  }

  if (hasAutoRepeaterVHF2OffsetIndex()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterVHF2OffsetIndex())) {
      errMsg(err) << "Cannot link AnyTone settings extension: "
                     "second auto-repeater VHF offset index "
                  << this->autoRepeaterVHF2OffsetIndex() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->vhf2Ref()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(
            this->autoRepeaterVHF2OffsetIndex()));
  }

  if (hasAutoRepeaterUHF2OffsetIndex()) {
    if (! ctx.has<AnytoneAutoRepeaterOffset>(this->autoRepeaterUHF2OffsetIndex())) {
      errMsg(err) << "Cannot link AnyTone settings extension: "
                     "second auto-repeater UHF offset index "
                  << this->autoRepeaterUHF2OffsetIndex() << " not defined.";
      return false;
    }
    ext->autoRepeaterSettings()->uhf2Ref()->set(
          ctx.get<AnytoneAutoRepeaterOffset>(
            this->autoRepeaterUHF2OffsetIndex()));
  }

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSSettingsElement
 * ******************************************************************************************** */
D878UVCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::APRSSettingsElement::APRSSettingsElement(uint8_t *ptr)
  : Element(ptr, APRSSettingsElement::size())
{
  // pass...
}

void
D878UVCodeplug::APRSSettingsElement::clear() {
  memset(_data, 0x00, _size);
  setUInt8(0x0000, 0xff);
  setFMTXDelay(Interval::fromMilliseconds(60));
  setUInt8(0x003d, 0x01); setUInt8(0x003e, 0x03); setUInt8(0x003f, 0xff);
}

bool
D878UVCodeplug::APRSSettingsElement::isValid() const {
  if (! Codeplug::Element::isValid())
    return false;
  return (! destination().simplified().isEmpty())
      && (! source().simplified().isEmpty());
}

Interval D878UVCodeplug::APRSSettingsElement::fmTXDelay() const {
  return Interval::fromMilliseconds( ((unsigned)getUInt8(Offset::fmTXDelay())) * 20);
}
void
D878UVCodeplug::APRSSettingsElement::setFMTXDelay(Interval ms) {
  setUInt8(Offset::fmTXDelay(), ms.milliseconds()/20);
}

Signaling::Code
D878UVCodeplug::APRSSettingsElement::txTone() const {
  if ((uint8_t)SignalingType::Off ==getUInt8(Offset::fmSigType())) { // none
    return Signaling::SIGNALING_NONE;
  } else if ((uint8_t)SignalingType::CTCSS == getUInt8(Offset::fmSigType())) { // CTCSS
    return CTCSS::decode(getUInt8(Offset::fmCTCSS()));
  } else if ((uint8_t)SignalingType::DCS == getUInt8(Offset::fmSigType())) { // DCS
    uint16_t code = getUInt16_le(Offset::fmDCS());
    if (512 < code)
      return Signaling::fromDCSNumber(dec_to_oct(code), false);
    return Signaling::fromDCSNumber(dec_to_oct(code-512), true);
  }

  return Signaling::SIGNALING_NONE;
}
void
D878UVCodeplug::APRSSettingsElement::setTXTone(Signaling::Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::Off);
  } else if (Signaling::isCTCSS(code)) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::CTCSS);
    setUInt8(Offset::fmCTCSS(), CTCSS::encode(code));
  } else if (Signaling::isDCSNormal(code)) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::DCS);
    setUInt16_le(Offset::fmDCS(), oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    setUInt8(Offset::fmSigType(), (uint8_t)SignalingType::DCS);
    setUInt16_le(Offset::fmDCS(), oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Interval
D878UVCodeplug::APRSSettingsElement::manualTXInterval() const {
  return Interval::fromSeconds(getUInt8(Offset::manualTXInterval()));
}
void
D878UVCodeplug::APRSSettingsElement::setManualTXInterval(Interval sec) {
  setUInt8(Offset::manualTXInterval(), sec.seconds());
}

bool
D878UVCodeplug::APRSSettingsElement::autoTX() const {
  return ! autoTXInterval().isNull();
}
Interval
D878UVCodeplug::APRSSettingsElement::autoTXInterval() const {
  return Interval::fromSeconds( ((unsigned)getUInt8(Offset::autoTXInterval()))*30 );
}
void
D878UVCodeplug::APRSSettingsElement::setAutoTXInterval(Interval sec) {
  setUInt8(Offset::autoTXInterval(), sec.seconds()/30);
}
void
D878UVCodeplug::APRSSettingsElement::disableAutoTX() {
  setAutoTXInterval(Interval::fromMilliseconds(0));
}

bool
D878UVCodeplug::APRSSettingsElement::fixedLocationEnabled() const {
  return getUInt8(Offset::fixedLocation());
}
QGeoCoordinate
D878UVCodeplug::APRSSettingsElement::fixedLocation() const {
  double latitude  = getUInt8(Offset::fixedLatDeg()) + double(getUInt8(Offset::fixedLatMin()))/60
      + double(getUInt8(Offset::fixedLatSec()))/3600;
  if (getUInt8(Offset::fixedLatSouth()))
    latitude *= -1;

  double longitude = getUInt8(Offset::fixedLonDeg()) + double(getUInt8(Offset::fixedLonMin()))/60
      + double(getUInt8(Offset::fixedLonSec()))/3600;
  if (getUInt8(Offset::fixedLonWest()))
    longitude *= -1;

  double height_ft = getUInt16_le(Offset::fixedHeight());
  return QGeoCoordinate(latitude, longitude, height_ft/3.281);
}
void
D878UVCodeplug::APRSSettingsElement::setFixedLocation(QGeoCoordinate &loc) {
  double latitude = loc.latitude();
  bool south = (0 > latitude); latitude = std::abs(latitude);
  unsigned lat_deg = int(latitude); latitude -= lat_deg; latitude *= 60;
  unsigned lat_min = int(latitude); latitude -= lat_min; latitude *= 60;
  unsigned lat_sec = int(latitude);
  double longitude = loc.longitude();
  bool west = (0 > longitude); longitude = std::abs(longitude);
  unsigned lon_deg = int(longitude); longitude -= lon_deg; longitude *= 60;
  unsigned lon_min = int(longitude); longitude -= lon_min; longitude *= 60;
  unsigned lon_sec = int(longitude);
  unsigned height_ft = int(loc.altitude()*3.281);
  setUInt8(Offset::fixedLatDeg(), lat_deg);
  setUInt8(Offset::fixedLatMin(), lat_min);
  setUInt8(Offset::fixedLatSec(), lat_sec);
  setUInt8(Offset::fixedLatSouth(), (south ? 0x01 : 0x00));
  setUInt8(Offset::fixedLonDeg(), lon_deg);
  setUInt8(Offset::fixedLonMin(), lon_min);
  setUInt8(Offset::fixedLonSec(), lon_sec);
  setUInt8(Offset::fixedLonWest(), (west ? 0x01 : 0x00));
  setUInt16_le(Offset::fixedHeight(), height_ft);
  // enable fixed location.
  setUInt8(Offset::fixedLocation(), 0x01);
}
void
D878UVCodeplug::APRSSettingsElement::disableFixedLocation() {
  setUInt8(Offset::fixedLocation(), 0x00);
}

QString
D878UVCodeplug::APRSSettingsElement::destination() const {
  // Terminated/padded with space
  return readASCII(Offset::destinationCall(), 6, ' ');
}
unsigned
D878UVCodeplug::APRSSettingsElement::destinationSSID() const {
  return getUInt8(Offset::destinationSSID());
}
void
D878UVCodeplug::APRSSettingsElement::setDestination(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(Offset::destinationCall(), call, 6, ' ');
  setUInt8(Offset::destinationSSID(), ssid);
}
QString
D878UVCodeplug::APRSSettingsElement::source() const {
  // Terminated/padded with space
  return readASCII(Offset::sourceCall(), 6, ' ');
}
unsigned
D878UVCodeplug::APRSSettingsElement::sourceSSID() const {
  return getUInt8(Offset::sourceSSID());
}
void
D878UVCodeplug::APRSSettingsElement::setSource(const QString &call, unsigned ssid) {
  // Terminated/padded with space
  writeASCII(Offset::sourceCall(), call, 6, ' ');
  setUInt8(Offset::sourceSSID(), ssid);
}

QString
D878UVCodeplug::APRSSettingsElement::path() const {
  return readASCII(Offset::path(), 20, 0x00);
}
void
D878UVCodeplug::APRSSettingsElement::setPath(const QString &path) {
  writeASCII(Offset::path(), path, 20, 0x00);
}

APRSSystem::Icon
D878UVCodeplug::APRSSettingsElement::icon() const {
  return code2aprsicon(getUInt8(Offset::symbolTable()), getUInt8(Offset::symbol()));
}
void
D878UVCodeplug::APRSSettingsElement::setIcon(APRSSystem::Icon icon) {
  setUInt8(Offset::symbolTable(), aprsicon2tablecode(icon));
  setUInt8(Offset::symbol(), aprsicon2iconcode(icon));
}

Channel::Power
D878UVCodeplug::APRSSettingsElement::power() const {
  switch (getUInt8(Offset::fmPower())) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::Mid;
  case 2: return Channel::Power::High;
  case 3: return Channel::Power::Max;
  }
  return Channel::Power::Low;
}
void
D878UVCodeplug::APRSSettingsElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:  setUInt8(Offset::fmPower(), 0x00); break;
  case Channel::Power::Mid:  setUInt8(Offset::fmPower(), 0x01); break;
  case Channel::Power::High: setUInt8(Offset::fmPower(), 0x02); break;
  case Channel::Power::Max:  setUInt8(Offset::fmPower(), 0x03); break;
  }
}

Interval
D878UVCodeplug::APRSSettingsElement::fmPreWaveDelay() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::fmPrewaveDelay()))*10);
}
void
D878UVCodeplug::APRSSettingsElement::setFMPreWaveDelay(Interval ms) {
  setUInt8(Offset::fmPrewaveDelay(), ms.milliseconds()/10);
}

bool
D878UVCodeplug::APRSSettingsElement::dmrChannelIsSelected(unsigned n) const {
  return 0xfa2 == dmrChannelIndex(n);
}
unsigned
D878UVCodeplug::APRSSettingsElement::dmrChannelIndex(unsigned n) const {
  return getUInt16_le(Offset::dmrChannelIndices() + n*Offset::betweenDMRChannelIndices());
}
void
D878UVCodeplug::APRSSettingsElement::setDMRChannelIndex(unsigned n, unsigned idx) {
  setUInt16_le(Offset::dmrChannelIndices() + n*Offset::betweenDMRChannelIndices(), idx);
}
void
D878UVCodeplug::APRSSettingsElement::setDMRChannelSelected(unsigned n) {
  setDMRChannelIndex(n, 0xfa2);
}

unsigned
D878UVCodeplug::APRSSettingsElement::dmrDestination(unsigned n) const {
  return getBCD8_be(Offset::dmrDestinations() + n*Offset::betweenDMRDestinations());
}
void
D878UVCodeplug::APRSSettingsElement::setDMRDestination(unsigned n, unsigned idx) {
  setBCD8_be(Offset::dmrDestinations() + n*Offset::betweenDMRDestinations(), idx);
}

DMRContact::Type
D878UVCodeplug::APRSSettingsElement::dmrCallType(unsigned n) const {
  switch(getUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes())) {
  case 0: return DMRContact::PrivateCall;
  case 1: return DMRContact::GroupCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
D878UVCodeplug::APRSSettingsElement::setDMRCallType(unsigned n, DMRContact::Type type) {
  switch(type) {
  case DMRContact::PrivateCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x00); break;
  case DMRContact::GroupCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x01); break;
  case DMRContact::AllCall: setUInt8(Offset::dmrCallTypes() + n*Offset::betweenDMRCallTypes(), 0x02); break;
  }
}

bool
D878UVCodeplug::APRSSettingsElement::dmrTimeSlotOverride(unsigned n) {
  return 0x00 != getUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots());
}
DMRChannel::TimeSlot
D878UVCodeplug::APRSSettingsElement::dmrTimeSlot(unsigned n) const {
  switch (getUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots())) {
  case 1: return DMRChannel::TimeSlot::TS1;
  case 2: return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
D878UVCodeplug::APRSSettingsElement::setDMRTimeSlot(unsigned n, DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0x01); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0x02); break;
  }
}
void
D878UVCodeplug::APRSSettingsElement::clearDMRTimeSlotOverride(unsigned n) {
  setUInt8(Offset::dmrTimeSlots() + n*Offset::betweenDMRTimeSlots(), 0);
}

bool
D878UVCodeplug::APRSSettingsElement::dmrRoaming() const {
  return getUInt8(Offset::roamingSupport());
}
void
D878UVCodeplug::APRSSettingsElement::enableDMRRoaming(bool enable) {
  setUInt8(Offset::roamingSupport(), (enable ? 0x01 : 0x00));
}

Interval
D878UVCodeplug::APRSSettingsElement::dmrPreWaveDelay() const {
  return Interval::fromMilliseconds(((unsigned)getUInt8(Offset::dmrPrewaveDelay()))*100);
}
void
D878UVCodeplug::APRSSettingsElement::setDMRPreWaveDelay(Interval ms) {
  setUInt8(Offset::dmrPrewaveDelay(), ms.milliseconds()/100);
}

bool
D878UVCodeplug::APRSSettingsElement::infiniteDisplayTime() const {
  return 13 == getUInt8(Offset::displayInterval());
}
Interval
D878UVCodeplug::APRSSettingsElement::displayTime() const {
  return Interval::fromSeconds(3 + getUInt8(Offset::displayInterval()));
}
void
D878UVCodeplug::APRSSettingsElement::setDisplayTime(Interval dur) {
  unsigned int sec = dur.seconds();
  sec = std::min(16U, std::max(3U, sec));
  setUInt8(Offset::displayInterval(), sec-3);
}
void
D878UVCodeplug::APRSSettingsElement::setDisplayTimeInifinite() {
  setUInt8(Offset::displayInterval(), 13);
}

bool
D878UVCodeplug::APRSSettingsElement::reportPosition() const {
  return getBit(Offset::reportPosition(), 0);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportPosition(bool enable) {
  setBit(Offset::reportPosition(), 0, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportMicE() const {
  return getBit(Offset::reportMicE(), 1);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportMicE(bool enable) {
  setBit(Offset::reportMicE(), 1, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportObject() const {
  return getBit(Offset::reportObject(), 2);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportObject(bool enable) {
  setBit(Offset::reportObject(), 2, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportItem() const {
  return getBit(Offset::reportItem(), 3);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportItem(bool enable) {
  setBit(Offset::reportItem(), 3, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportMessage() const {
  return getBit(Offset::reportMessage(), 4);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportMessage(bool enable) {
  setBit(Offset::reportMessage(), 4, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportWeather() const {
  return getBit(Offset::reportWeather(), 5);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportWeather(bool enable) {
  setBit(Offset::reportWeather(), 5, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportNMEA() const {
  return getBit(Offset::reportNMEA(), 6);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportNMEA(bool enable) {
  setBit(Offset::reportNMEA(), 6, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportStatus() const {
  return getBit(Offset::reportStatus(), 7);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportStatus(bool enable) {
  setBit(Offset::reportStatus(), 7, enable);
}

bool
D878UVCodeplug::APRSSettingsElement::reportOther() const {
  return getBit(Offset::reportOther(), 0);
}
void
D878UVCodeplug::APRSSettingsElement::enableReportOther(bool enable) {
  setBit(Offset::reportOther(), 0, enable);
}

AnytoneFMAPRSSettingsExtension::Bandwidth
D878UVCodeplug::APRSSettingsElement::fmChannelWidth() const {
  return (AnytoneFMAPRSSettingsExtension::Bandwidth)getUInt8(Offset::fmWidth());
}
void
D878UVCodeplug::APRSSettingsElement::setFMChannelWidth(AnytoneFMAPRSSettingsExtension::Bandwidth width) {
  setUInt8(Offset::fmWidth(), (uint8_t)width);
}

bool
D878UVCodeplug::APRSSettingsElement::fmPassAll() const {
  return 0x00 != getUInt8(Offset::passAll());
}
void
D878UVCodeplug::APRSSettingsElement::enableFMPassAll(bool enable) {
  setUInt8(Offset::passAll(), enable ? 0x01 : 0x00);
}

bool
D878UVCodeplug::APRSSettingsElement::fmFrequencySet(unsigned int n) const {
  n = std::min(Limit::fmFrequencies()-1, n);
  return 0 != getBCD8_be(Offset::fmFrequencies() + n*Offset::betweenFMFrequencies());
}
Frequency
D878UVCodeplug::APRSSettingsElement::fmFrequency(unsigned int n) const {
  n = std::min(Limit::fmFrequencies()-1, n);
  return Frequency::fromHz(getBCD8_be(Offset::fmFrequencies() + n*Offset::betweenFMFrequencies())*10);
}
void
D878UVCodeplug::APRSSettingsElement::setFMFrequency(unsigned int n, Frequency f) {
  n = std::min(Limit::fmFrequencies()-1, n);
  setBCD8_be(Offset::fmFrequencies() + n*Offset::betweenFMFrequencies(), f.inHz()/10);
}
void
D878UVCodeplug::APRSSettingsElement::clearFMFrequency(unsigned int n) {
  n = std::min(Limit::fmFrequencies()-1, n);
  setBCD8_be(Offset::fmFrequencies() + n*Offset::betweenFMFrequencies(), 0);
}

bool
D878UVCodeplug::APRSSettingsElement::fromFMAPRSSystem(
    const APRSSystem *sys, Context &ctx, FMAPRSFrequencyNamesElement &names, const ErrorStack &err)
{
  Q_UNUSED(ctx)
  clear();
  if (! sys->revertChannel()) {
    errMsg(err) << "Cannot encode APRS settings: "
                << "No revert channel defined for APRS system '" << sys->name() <<"'.";
    return false;
  }
  names.setName(0, sys->name());
  setFMFrequency(0, sys->revertChannel()->txFrequency());
  setTXTone(sys->revertChannel()->txTone());
  setPower(sys->revertChannel()->power());
  setFMChannelWidth(FMChannel::Bandwidth::Wide == sys->revertChannel()->bandwidth() ?
                      AnytoneFMAPRSSettingsExtension::Bandwidth::Wide :
                      AnytoneFMAPRSSettingsExtension::Bandwidth::Narrow);
  setManualTXInterval(Interval::fromSeconds(sys->period()));
  setAutoTXInterval(Interval::fromSeconds(sys->period()));
  setDestination(sys->destination(), sys->destSSID());
  setSource(sys->source(), sys->srcSSID());
  setPath(sys->path());
  setIcon(sys->icon());
  setFMPreWaveDelay(Interval());

  // Handle FM APRS Settings if set
  AnytoneFMAPRSSettingsExtension *ext = sys->anytoneExtension();
  if (nullptr == ext)
    return true;

  setFMTXDelay(ext->txDelay());
  setFMPreWaveDelay(ext->preWaveDelay());
  enableFMPassAll(ext->passAll());
  enableReportPosition(ext->reportPosition());
  enableReportMicE(ext->reportMicE());
  enableReportObject(ext->reportObject());
  enableReportItem(ext->reportItem());
  enableReportMessage(ext->reportMessage());
  enableReportWeather(ext->reportWeather());
  enableReportNMEA(ext->reportNMEA());
  enableReportStatus(ext->reportStatus());
  enableReportOther(ext->reportOther());

  // Encode additional FM APRS frequencies
  for (int i=0; i<ext->frequencies()->count(); i++) {
    setFMFrequency(ctx.index(ext->frequencies()->get(i)),
                   ext->frequencies()->get(i)->as<AnytoneAPRSFrequency>()->frequency());
    names.setName(ctx.index(ext->frequencies()->get(i)),
                  ext->frequencies()->get(i)->name());
  }

  return true;
}

APRSSystem *
D878UVCodeplug::APRSSettingsElement::toFMAPRSSystem(Context &ctx, const FMAPRSFrequencyNamesElement &names, const ErrorStack &err) {
  Q_UNUSED(err)
  QString name = QString("APRS %1").arg(destination());
  if (names.isValid() && (! names.name(0).isEmpty()))
    name = names.name(0);
  APRSSystem *sys = new APRSSystem(
        name, nullptr,
        destination(), destinationSSID(), source(), sourceSSID(),
        path(), icon(), "", autoTXInterval().seconds());

  // Decode extension
  AnytoneFMAPRSSettingsExtension *ext = new AnytoneFMAPRSSettingsExtension();
  sys->setAnytoneExtension(ext);

  ext->setTXDelay(fmTXDelay());
  ext->setPreWaveDelay(fmPreWaveDelay());
  ext->enablePassAll(fmPassAll());

  ext->enableReportPosition(reportPosition());
  ext->enableReportMicE(reportMicE());
  ext->enableReportObject(reportObject());
  ext->enableReportItem(reportItem());
  ext->enableReportMessage(reportMessage());
  ext->enableReportWeather(reportWeather());
  ext->enableReportNMEA(reportNMEA());
  ext->enableReportStatus(reportStatus());
  ext->enableReportOther(reportOther());

  for (unsigned int i=1; i<Limit::fmFrequencies(); i++) {
    if (! fmFrequencySet(i))
      continue;
    auto *f = new AnytoneAPRSFrequency();
    f->setFrequency(fmFrequency(i));
    QString name = QString("APRS %1").arg(i);
    if (names.isValid() && (! names.name(i).isEmpty()))
      name = names.name(i);
    f->setName(name);
    ext->frequencies()->add(f);
    ctx.add(f, i);
  }

  return sys;
}

bool
D878UVCodeplug::APRSSettingsElement::linkFMAPRSSystem(APRSSystem *sys, Context &ctx) {
  // First, try to find a matching analog channel in list
  FMChannel *ch = ctx.config()->channelList()->findFMChannelByTxFreq(fmFrequency(0));
  if (! ch) {
    // If no channel is found, create one with the settings from APRS channel:
    ch = new FMChannel();
    ch->setName("APRS Channel");
    ch->setRXFrequency(fmFrequency(0));
    ch->setTXFrequency(fmFrequency(0));
    ch->setPower(power());
    ch->setTXTone(txTone());
    ch->setBandwidth(AnytoneFMAPRSSettingsExtension::Bandwidth::Wide == fmChannelWidth() ?
                       FMChannel::Bandwidth::Wide : FMChannel::Bandwidth::Narrow);
    logInfo() << "No matching APRS channel found for TX frequency " << double(fmFrequency(0).inHz())/1e6
              << "MHz, create one as 'APRS Channel'";
    ctx.config()->channelList()->add(ch);
  }
  sys->setRevertChannel(ch);
  return true;
}

bool
D878UVCodeplug::APRSSettingsElement::fromDMRAPRSSystems(Context &ctx) {
  unsigned int n = std::min(ctx.count<GPSSystem>(), Limit::dmrSystems());
  for (unsigned int idx=0; idx<n; idx++)
    fromDMRAPRSSystemObj(idx, ctx.get<GPSSystem>(idx), ctx);
  return true;
}

bool
D878UVCodeplug::APRSSettingsElement::fromDMRAPRSSystemObj(unsigned int idx, GPSSystem *sys, Context &ctx) {
  if (sys->hasContact()) {
    setDMRDestination(idx, sys->contactObj()->number());
    setDMRCallType(idx, sys->contactObj()->type());
  }
  if (sys->hasRevertChannel() && (SelectedChannel::get() != (Channel *)sys->revertChannel())) {
    setDMRChannelIndex(idx, ctx.index(sys->revertChannel()));
    clearDMRTimeSlotOverride(idx);
  } else { // no revert channel specified or "selected channel":
    setDMRChannelSelected(idx);
  }
  return true;
}

GPSSystem *
D878UVCodeplug::APRSSettingsElement::toDMRAPRSSystemObj(int idx) const {
  if (0 == dmrDestination(idx))
    return nullptr;
  return new GPSSystem(tr("GPS Sys #%1").arg(idx+1));
}

bool
D878UVCodeplug::APRSSettingsElement::linkDMRAPRSSystem(int idx, GPSSystem *sys, Context &ctx) const {
  // Clear revert channel from GPS system
  sys->setRevertChannel(nullptr);

  // if a revert channel is defined -> link to it
  if (dmrChannelIsSelected(idx))
    sys->setRevertChannel(nullptr);
  else if (ctx.has<Channel>(dmrChannelIndex(idx)) && ctx.get<Channel>(dmrChannelIndex(idx))->is<DMRChannel>())
    sys->setRevertChannel(ctx.get<Channel>(dmrChannelIndex(idx))->as<DMRChannel>());

  // Search for a matching contact in contacts
  DMRContact *cont = ctx.config()->contacts()->findDigitalContact(dmrDestination(idx));
  // If no matching contact is found, create one
  if (nullptr == cont) {
    cont = new DMRContact(dmrCallType(idx), tr("GPS #%1 Contact").arg(idx+1),
                          dmrDestination(idx), false);
    ctx.config()->contacts()->add(cont);
  }
  // link contact to GPS system.
  sys->setContactObj(cont);

  return true;
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSMessageElement
 * ******************************************************************************************** */
D878UVCodeplug::AnalogAPRSMessageElement::AnalogAPRSMessageElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AnalogAPRSMessageElement::AnalogAPRSMessageElement(uint8_t *ptr)
  : Element(ptr, AnalogAPRSMessageElement::size())
{
  // pass...
}

void
D878UVCodeplug::AnalogAPRSMessageElement::clear() {
  memset(_data, 0x00, _size);
}

QString
D878UVCodeplug::AnalogAPRSMessageElement::message() const {
  return readASCII(0, Limit::length(), 0x00);
}

void
D878UVCodeplug::AnalogAPRSMessageElement::setMessage(const QString &msg) {
  writeASCII(0, msg, Limit::length(), 0x00);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AnalogAPRSRXEntryElement
 * ******************************************************************************************** */
D878UVCodeplug::AnalogAPRSRXEntryElement::AnalogAPRSRXEntryElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AnalogAPRSRXEntryElement::AnalogAPRSRXEntryElement(uint8_t *ptr)
  : Element(ptr, 0x0008)
{
  // pass...
}

void
D878UVCodeplug::AnalogAPRSRXEntryElement::clear() {
  memset(_data, 0x00, _size);
}

bool
D878UVCodeplug::AnalogAPRSRXEntryElement::isValid() const {
  return Element::isValid() && (0 != getUInt8(0x0000));
}

QString
D878UVCodeplug::AnalogAPRSRXEntryElement::call() const {
  return readASCII(0x0001, 6, 0x00);
}
unsigned
D878UVCodeplug::AnalogAPRSRXEntryElement::ssid() const {
  return getUInt8(0x0007);
}
void
D878UVCodeplug::AnalogAPRSRXEntryElement::setCall(const QString &call, unsigned ssid) {
  writeASCII(0x0001, call, 6, 0x00); // Store call
  setUInt8(0x0007, ssid);            // Store SSID
  setUInt8(0x0000, 0x01);            // Enable entry.
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::AESEncryptionKeyElement
 * ******************************************************************************************** */
D878UVCodeplug::AESEncryptionKeyElement::AESEncryptionKeyElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::AESEncryptionKeyElement::AESEncryptionKeyElement(uint8_t *ptr)
  : Element(ptr, AESEncryptionKeyElement::size())
{
  // pass...
}

void
D878UVCodeplug::AESEncryptionKeyElement::clear() {
  memset(_data, 0x00, _size);
  setIndex(0xff);
  setUInt8(0x0022, 0x40);
}

bool
D878UVCodeplug::AESEncryptionKeyElement::isValid() const {
  return Element::isValid() && (0xff != index());
}

unsigned
D878UVCodeplug::AESEncryptionKeyElement::index() const {
  return getUInt8(0x0000);
}
void
D878UVCodeplug::AESEncryptionKeyElement::setIndex(unsigned idx) {
  setUInt8(0x0000, idx);
}

QByteArray
D878UVCodeplug::AESEncryptionKeyElement::key() const {
  QByteArray ar(32, 0); memcpy(ar.data(), _data+0x0001, 32);
  return ar;
}

void
D878UVCodeplug::AESEncryptionKeyElement::setKey(const QByteArray &key) {
  if (32 != key.size())
    return;
  memcpy(_data+0x0001, key.constData(), 32);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::HiddenZoneBitmapElement
 * ******************************************************************************************** */
D878UVCodeplug::HiddenZoneBitmapElement::HiddenZoneBitmapElement(uint8_t *ptr, size_t size)
  : BitmapElement(ptr, size)
{
  // pass...
}

D878UVCodeplug::HiddenZoneBitmapElement::HiddenZoneBitmapElement(uint8_t *ptr)
  : BitmapElement(ptr, HiddenZoneBitmapElement::size())
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::RadioInfoElement
 * ******************************************************************************************** */
D878UVCodeplug::RadioInfoElement::RadioInfoElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

D878UVCodeplug::RadioInfoElement::RadioInfoElement(uint8_t *ptr)
  : Element(ptr, 0x0100)
{
  // pass...
}

void
D878UVCodeplug::RadioInfoElement::clear() {
  setBandSelectPassword("");
  setProgramPassword("");
}

bool
D878UVCodeplug::RadioInfoElement::fullTest() const {
  return getUInt8(0x0002);
}

D878UVCodeplug::RadioInfoElement::FrequencyRange
D878UVCodeplug::RadioInfoElement::frequencyRange() const {
  return (FrequencyRange)getUInt8(0x0003);
}
void
D878UVCodeplug::RadioInfoElement::setFrequencyRange(FrequencyRange range) {
  setUInt8(0x0003, (unsigned)range);
}

bool
D878UVCodeplug::RadioInfoElement::international() const {
  return getUInt8(0x0004);
}
void
D878UVCodeplug::RadioInfoElement::enableInternational(bool enable) {
  setUInt8(0x0004, (enable ? 0x01 : 0x00));
}

bool
D878UVCodeplug::RadioInfoElement::bandSelect() const {
  return getUInt8(0x0006);
}
void
D878UVCodeplug::RadioInfoElement::enableBandSelect(bool enable) {
  setUInt8(0x0006, (enable ? 0x01 : 0x00));
}

QString
D878UVCodeplug::RadioInfoElement::bandSelectPassword() const {
  return readASCII(0x000b, 4, 0x00);
}
void
D878UVCodeplug::RadioInfoElement::setBandSelectPassword(const QString &passwd) {
  writeASCII(0x000b, passwd, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::radioType() const {
  return readASCII(0x0010, 7, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::programPassword() const {
  return readASCII(0x0028, 4, 0x00);
}
void
D878UVCodeplug::RadioInfoElement::setProgramPassword(const QString &passwd) {
  writeASCII(0x0028, passwd, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::areaCode() const {
  return readASCII(0x002c, 4, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::serialNumber() const {
  return readASCII(0x0030, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::productionDate() const {
  return readASCII(0x0040, 10, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::manufacturerCode() const {
  return readASCII(0x0050, 8, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::maintainedDate() const {
  return readASCII(0x0060, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::dealerCode() const {
  return readASCII(0x0070, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::stockDate() const {
  return readASCII(0x0080, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::sellDate() const {
  return readASCII(0x0090, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::seller() const {
  return readASCII(0x00a0, 16, 0x00);
}

QString
D878UVCodeplug::RadioInfoElement::maintainerNote() const {
  return readASCII(0x00b0, 128, 0x00);
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(const QString &label, QObject *parent)
  : D868UVCodeplug(label, parent)
{
  // pass...
}

D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : D868UVCodeplug("Anytone AT-D878UV Codeplug", parent)
{
  // pass...
}

bool
D878UVCodeplug::allocateBitmaps() {
  if (! D868UVCodeplug::allocateBitmaps())
    return false;

  // Roaming channel bitmaps
  image(0).addElement(Offset::roamingChannelBitmap(), RoamingChannelBitmapElement::size());
  // Roaming zone bitmaps
  image(0).addElement(Offset::roamingZoneBitmap(), RoamingZoneBitmapElement::size());

  return true;
}

void
D878UVCodeplug::allocateUpdated() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateUpdated();

  // Encryption keys
  image(0).addElement(Offset::aesKeys(), Limit::aesKeys()*AESEncryptionKeyElement::size());

  // allocate APRS RX list
  image(0).addElement(Offset::analogAPRSRXEntries(),
                      Limit::analogAPRSRXEntries()*AnalogAPRSRXEntryElement::size());

  // allocate FM APRS frequency names
  image(0).addElement(Offset::fmAPRSFrequencyNames(), FMAPRSFrequencyNamesElement::size());
}

void
D878UVCodeplug::allocateForEncoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForEncoding();
  this->allocateRoaming();
}

void
D878UVCodeplug::allocateForDecoding() {
  // First allocate everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::allocateForDecoding();
  this->allocateRoaming();
  // allocate FM APRS frequency names
  image(0).addElement(Offset::fmAPRSFrequencyNames(), FMAPRSFrequencyNamesElement::size());
}

void
D878UVCodeplug::setBitmaps(Context& ctx)
{
  // First set everything common between D868UV and D878UV codeplugs.
  D868UVCodeplug::setBitmaps(ctx);

  // Mark roaming zones
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  unsigned int num_roaming_zones = std::min(Limit::roamingZones(), ctx.count<RoamingZone>());
  roaming_zone_bitmap.clear(); roaming_zone_bitmap.enableFirst(num_roaming_zones);

  // Mark roaming channels
  RoamingChannelBitmapElement roaming_ch_bitmap(data(Offset::roamingChannelBitmap()));
  unsigned int num_roaming_channel = std::min(Limit::roamingChannels(), ctx.count<RoamingChannel>());
  roaming_ch_bitmap.clear(); roaming_ch_bitmap.enableFirst(num_roaming_channel);
}

bool
D878UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  // Encode everything common between d868uv and d878uv radios.
  if (! D868UVCodeplug::encodeElements(flags, ctx, err))
    return false;

  if (! this->encodeRoaming(flags, ctx, err))
    return false;

  return true;
}


bool
D878UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  // Decode everything commong between d868uv and d878uv codeplugs.
  if (! D868UVCodeplug::decodeElements(ctx, err))
    return false;

  if (! this->createRoaming(ctx, err))
    return false;

  if (! this->linkRoaming(ctx, err))
    return false;

  return true;
}

void
D878UVCodeplug::allocateChannels() {
  /* Allocate channels */
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // if disabled -> skip
    if (! channel_bitmap.isEncoded(i))
      continue;
    // compute address for channel
    uint16_t bank = i/Limit::channelsPerBank(), idx=i%Limit::channelsPerBank();
    uint32_t addr = Offset::channelBanks() + bank*Offset::betweenChannelBanks()
        + idx * ChannelElement::size();
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, ChannelElement::size());
    }
    if (!isAllocated(addr+0x2000, 0)) {
      image(0).addElement(addr+0x2000, ChannelElement::size());
      memset(data(addr+0x2000), 0x00, ChannelElement::size());
    }
  }
}

bool
D878UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx);
  }
  return true;
}

bool
D878UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create channels
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D878UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  ChannelBitmapElement channel_bitmap(data(Offset::channelBitmap()));

  // Link channel objects
  for (uint16_t i=0; i<Limit::numChannels(); i++) {
    // Check if channel is enabled:
    uint16_t bank = i/Limit::channelsPerBank(), idx = i%Limit::channelsPerBank();
    if (! channel_bitmap.isEncoded(i))
      continue;
    ChannelElement ch(data(Offset::channelBanks() + bank*Offset::betweenChannelBanks()
                           + idx*ChannelElement::size()));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}


void
D878UVCodeplug::allocateZones() {
  D868UVCodeplug::allocateZones();
  // Hidden zone map
  image(0).addElement(Offset::hiddenZoneBitmap(), HiddenZoneBitmapElement::size());
}

bool
D878UVCodeplug::encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err) {
  if (! D868UVCodeplug::encodeZone(i, zone, isB, flags, ctx, err))
    return false;

  AnytoneZoneExtension *ext = zone->anytoneExtension();
  if (nullptr == ext)
      return true;

  HiddenZoneBitmapElement(data(Offset::hiddenZoneBitmap())).setEncoded(i, ext->hidden());

  return true;
}

bool
D878UVCodeplug::decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  if (! D868UVCodeplug::decodeZone(i, zone, isB, ctx, err))
    return false;
  AnytoneZoneExtension *ext = zone->anytoneExtension();
  if (nullptr == ext) {
    ext = new AnytoneZoneExtension();
    zone->setAnytoneExtension(ext);
  }

  HiddenZoneBitmapElement bitmap(data(Offset::hiddenZoneBitmap()));
  ext->enableHidden(bitmap.isEncoded(i) && (!isB));

  return true;
}


void
D878UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::dmrAPRSMessage(), DMRAPRSMessageElement::size());
  image(0).addElement(Offset::settingsExtension(), ExtendedSettingsElement::size());

}
bool
D878UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).fromConfig(flags, ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}
bool
D878UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
  DMRAPRSMessageElement(data(Offset::dmrAPRSMessage())).updateConfig(ctx);
  ExtendedSettingsElement(data(Offset::settingsExtension())).updateConfig(ctx);
  return true;
}
bool
D878UVCodeplug::linkGeneralSettings(Context &ctx, const ErrorStack &err) {
  if (! GeneralSettingsElement(data(Offset::settings())).linkSettings(ctx.config()->settings(), ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  if (! ExtendedSettingsElement(data(Offset::settingsExtension())).linkConfig(ctx, err)) {
    errMsg(err) << "Cannot link general settings extension.";
    return false;
  }

  return true;
}

void
D878UVCodeplug::allocateGPSSystems() {
  // replaces D868UVCodeplug::allocateGPSSystems

  // APRS settings
  image(0).addElement(Offset::aprsSettings(), APRSSettingsElement::size());
  image(0).addElement(Offset::analogAPRSMessage(), AnalogAPRSMessageElement::size());
}

bool
D878UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)
  // replaces D868UVCodeplug::encodeGPSSystems

  APRSSettingsElement aprs(data(Offset::aprsSettings()));
  FMAPRSFrequencyNamesElement aprsNames(data(Offset::fmAPRSFrequencyNames()));

  // Encode APRS system (there can only be one)
  if (0 < ctx.config()->posSystems()->aprsCount()) {
    aprs.fromFMAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx, aprsNames, err);
    AnalogAPRSMessageElement(data(Offset::analogAPRSMessage()))
        .setMessage(ctx.config()->posSystems()->aprsSystem(0)->message());
  }

  // Encode GPS systems
  if (! aprs.fromDMRAPRSSystems(ctx))
    return false;
  if (0 < ctx.config()->posSystems()->gpsCount()) {
    // If there is at least one GPS system defined -> set auto TX interval.
    //  This setting might be overridden by any analog APRS system below
    APRSSettingsElement aprs(data(Offset::aprsSettings()));
    aprs.setAutoTXInterval(Interval::fromSeconds(ctx.config()->posSystems()->gpsSystem(0)->period()));
    aprs.setManualTXInterval(Interval::fromSeconds(ctx.config()->posSystems()->gpsSystem(0)->period()));
  }
  return true;
}

bool
D878UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // replaces D868UVCodeplug::createGPSSystems

  // Before creating any GPS/APRS systems, get global auto TX interval
  APRSSettingsElement aprs(data(Offset::aprsSettings()));
  FMAPRSFrequencyNamesElement aprsNames(isAllocated(Offset::fmAPRSFrequencyNames()) ?
                                          data(Offset::fmAPRSFrequencyNames()):
                                          nullptr);
  AnalogAPRSMessageElement  aprsMessage(data(Offset::analogAPRSMessage()));
  unsigned pos_intervall = aprs.autoTXInterval().seconds();

  // Create APRS system (if enabled)
  if (aprs.isValid()) {
    APRSSystem *sys = aprs.toFMAPRSSystem(ctx, aprsNames, err);
    if (nullptr == sys) {
      errMsg(err) << "Cannot decode positioning systems.";
      return false;
    }
    sys->setPeriod(pos_intervall);
    sys->setMessage(aprsMessage.message());
    ctx.config()->posSystems()->add(sys); ctx.add(sys,0);
  }

  // Create GPS systems
  for (unsigned int i=0; i<Limit::dmrAPRSSystems(); i++) {
    if (0 == aprs.dmrDestination(i))
      continue;
    if (GPSSystem *sys = aprs.toDMRAPRSSystemObj(i)) {
      logDebug() << "Create GPS sys '" << sys->name() << "' at idx " << i << ".";
      sys->setPeriod(pos_intervall);
      ctx.config()->posSystems()->add(sys); ctx.add(sys, i);
    } else {
      return false;
    }
  }
  return true;
}

bool
D878UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  // replaces D868UVCodeplug::linkGPSSystems

  // Link APRS system
  APRSSettingsElement aprs(data(Offset::aprsSettings()));
  if (aprs.isValid()) {
    aprs.linkFMAPRSSystem(ctx.config()->posSystems()->aprsSystem(0), ctx);
  }

  // Link GPS systems
  for (unsigned int i=0; i<Limit::dmrAPRSSystems(); i++) {
    if (0 == aprs.dmrDestination(i))
      continue;
    aprs.linkDMRAPRSSystem(i, ctx.get<GPSSystem>(i), ctx);
  }

  return true;
}


void
D878UVCodeplug::allocateRoaming() {
  /* Allocate roaming channels */
  RoamingChannelBitmapElement roaming_channel_bitmap(data(Offset::roamingChannelBitmap()));
  for (uint8_t i=0; i<Limit::roamingChannels(); i++) {
    // Get byte and bit for roaming channel
    // if disabled -> skip
    if (! roaming_channel_bitmap.isEncoded(i))
      continue;
    // Allocate roaming channel
    uint32_t addr = Offset::roamingChannels() + i*RoamingChannelElement::size();
    if (!isAllocated(addr, 0)) {
      //logDebug() << "Allocate roaming channel at " << QString::number(addr, 16) << "h.";
      image(0).addElement(addr, RoamingChannelElement::size());
    }
  }

  /* Allocate roaming zones. */
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  for (uint8_t i=0; i<Limit::roamingZones(); i++) {
    // Get byte and bit for radio ID
    // if disabled -> skip
    if (! roaming_zone_bitmap.isEncoded(i))
      continue;
    // Allocate roaming zone
    uint32_t addr = Offset::roamingZones() + i*RoamingZoneElement::size();
    if (!isAllocated(addr, 0)) {
      logDebug() << "Allocate roaming zone at " << QString::number(addr, 16);
      image(0).addElement(addr, RoamingZoneElement::size());
    }
  }
}

bool
D878UVCodeplug::encodeRoaming(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err);

  // Encode roaming channels
  unsigned int num_roaming_channel = std::min(
        Limit::roamingChannels(), ctx.count<RoamingChannel>());
  for (uint8_t i=0; i<num_roaming_channel; i++) {
    // Encode roaming channel
    uint32_t addr = Offset::roamingChannels() + i*RoamingChannelElement::size();
    RoamingChannelElement rch_elm(data(addr));
    RoamingChannel *rch = ctx.config()->roamingChannels()->get(i)->as<RoamingChannel>();
    rch_elm.clear();
    rch_elm.fromChannel(rch);
    if (! ctx.add(rch, i)) {
      errMsg(err) << "Cannot add index " << i << " for roaming channel '"
                  << rch->name() << "' to codeplug context.";
      return false;
    }
  }

  // Encode roaming zones
  for (unsigned int i=0; i<ctx.count<RoamingZone>(); i++){
    uint32_t addr = Offset::roamingZones() + i*RoamingZoneElement::size();
    RoamingZoneElement zone(data(addr));
    logDebug() << "Encode roaming zone " << ctx.config()->roamingZones()->zone(i)->name()
               << " (" << (i+1) << ") at " << QString::number(addr, 16)
               << " with " << ctx.config()->roamingZones()->zone(i)->count() << " elements.";
    zone.fromRoamingZone(ctx.config()->roamingZones()->zone(i), ctx);
  }

  return true;
}

bool
D878UVCodeplug::createRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create or find roaming channels
  RoamingChannelBitmapElement roaming_channel_bitmap(data(Offset::roamingChannelBitmap()));
  for (unsigned int i=0; i<Limit::roamingChannels(); i++) {
    if (! roaming_channel_bitmap.isEncoded(i))
      continue;
    uint32_t addr = Offset::roamingChannels() + i*RoamingChannelElement::size();
    RoamingChannelElement ch(data(addr));
    RoamingChannel *digi = ch.toChannel(ctx);
    ctx.add(digi, i);
  }

  // Create and link roaming zones
  RoamingZoneBitmapElement roaming_zone_bitmap(data(Offset::roamingZoneBitmap()));
  for (unsigned int i=0; i<Limit::roamingZones(); i++) {
    if (! roaming_zone_bitmap.isEncoded(i))
      continue;
    uint32_t addr = Offset::roamingZones() + i*RoamingZoneElement::size();
    RoamingZoneElement z(data(addr));
    RoamingZone *zone = z.toRoamingZone(ctx, err);
    ctx.config()->roamingZones()->add(zone); ctx.add(zone, i);
    z.linkRoamingZone(zone, ctx, err);
  }

  return true;
}

bool
D878UVCodeplug::linkRoaming(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)
  // Pass, no need to link roaming channels.
  return true;
}

