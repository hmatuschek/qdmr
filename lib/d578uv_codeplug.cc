#include "d578uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>

#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define CHANNEL_BANK_0            0x00800000
#define CHANNEL_BANK_SIZE         0x00002000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000800
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000200
#define CHANNEL_SIZE              0x00000040

#define NUM_CONTACTS              10000      // Total number of contacts
#define CONTACTS_PER_BLOCK        4
#define CONTACT_BLOCK_0           0x02680000 // First bank of 4 contacts
#define CONTACT_BLOCK_SIZE        0x00000190 // Size of 4 contacts
#define CONTACT_BANK_SIZE         0x00040000 // Size of one contact bank
#define CONTACTS_PER_BANK         1000       // Number of contacts per bank
#define CONTACT_INDEX_LIST        0x02600000 // Address of contact index list
#define CONTACTS_BITMAP           0x02640000 // Address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // Size of contact bitmap
#define CONTACT_SIZE              0x00000064 // Size of contact element
#define CONTACT_ID_MAP            0x04800000 // Address of ID->Contact index map
#define CONTACT_ID_ENTRY_SIZE     0x00000008 // Size of each map entry

#define ADDR_HOTKEY               0x025C0000 // Same address as D868UV::hotkey_settings_t
#define HOTKEY_SIZE               0x00000970 // Different size.

#define ADDR_UNKNOWN_SETTING_1    0x02BC0000 // Address of unknown settings
#define UNKNOWN_SETTING_1_SIZE    0x00000020 // Size of unknown settings.
#define ADDR_UNKNOWN_SETTING_2    0x02BC0C60 // Address of unknown settings
#define UNKNOWN_SETTING_2_SIZE    0x00000020 // Size of unknown settings.
#define ADDR_UNKNOWN_SETTING_3    0x02BC1000 // Address of unknown settings
#define UNKNOWN_SETTING_3_SIZE    0x00000060 // Size of unknown settings.


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::ChannelElement
 * ******************************************************************************************** */
D578UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::ChannelElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : D878UVCodeplug::ChannelElement(ptr)
{
  // pass...
}

bool
D578UVCodeplug::ChannelElement::handsFree() const {
  return getBit(0x0034, 2);
}
void
D578UVCodeplug::ChannelElement::enableHandsFree(bool enable) {
  setBit(0x0034, 2, enable);
}

bool
D578UVCodeplug::ChannelElement::roamingEnabled() const {
  // inverted!
  return !getBit(0x0034,3);
}
void
D578UVCodeplug::ChannelElement::enableRoaming(bool enable) {
  // inverted!
  setBit(0x0034, 3, !enable);
}

bool
D578UVCodeplug::ChannelElement::dataACK() const {
  // inverted!
  return !getBit(0x003d,3);
}
void
D578UVCodeplug::ChannelElement::enableDataACK(bool enable) {
  // inverted!
  setBit(0x003d, 3, !enable);
}

unsigned
D578UVCodeplug::ChannelElement::dmrEncryptionKeyIndex() const {
  return 0;
}
void
D578UVCodeplug::ChannelElement::setDMREncryptionKeyIndex(unsigned idx) {
  Q_UNUSED(idx)
}

bool
D578UVCodeplug::ChannelElement::analogScambler() const {
  return getUInt8(0x003a);
}
void
D578UVCodeplug::ChannelElement::enableAnalogScamber(bool enable) {
  setUInt8(0x003a, (enable ? 0x01 : 0x00));
}

Channel *
D578UVCodeplug::ChannelElement::toChannelObj(Context &ctx) const {
  Channel *ch = D878UVCodeplug::ChannelElement::toChannelObj(ctx);
  if (nullptr == ch)
    return nullptr;

  // Apply extensions
  if (FMChannel *fch = ch->as<FMChannel>()) {
    if (AnytoneFMChannelExtension *ext = fch->anytoneChannelExtension()) {
      // Common settings
      ext->enableHandsFree(handsFree());
      // FM specific settings
      ext->enableScrambler(analogScambler());
    }
  } else if (DMRChannel *dch = ch->as<DMRChannel>()) {
    if (AnytoneDMRChannelExtension *ext = dch->anytoneChannelExtension()) {
      // Common settings
      ext->enableHandsFree(handsFree());
      // DMR specific extensions
    }
  }

  // Done.
  return ch;
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D578UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : D878UVCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass...
}

D578UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D878UVCodeplug::GeneralSettingsElement(ptr, GeneralSettingsElement::size())
{
  // pass...
}

uint8_t
D578UVCodeplug::GeneralSettingsElement::mapKeyFunctionToCode(AnytoneKeySettingsExtension::KeyFunction func) const {
  switch (func) {
  case AnytoneKeySettingsExtension::KeyFunction::Off:               return (uint8_t)KeyFunction::Off;
  case AnytoneKeySettingsExtension::KeyFunction::Voltage:           return (uint8_t)KeyFunction::Voltage;
  case AnytoneKeySettingsExtension::KeyFunction::Power:             return (uint8_t)KeyFunction::Power;
  case AnytoneKeySettingsExtension::KeyFunction::Repeater:          return (uint8_t)KeyFunction::Repeater;
  case AnytoneKeySettingsExtension::KeyFunction::Reverse:           return (uint8_t)KeyFunction::Reverse;
  case AnytoneKeySettingsExtension::KeyFunction::Encryption:        return (uint8_t)KeyFunction::Encryption;
  case AnytoneKeySettingsExtension::KeyFunction::Call:              return (uint8_t)KeyFunction::Call;
  case AnytoneKeySettingsExtension::KeyFunction::ToggleVFO:         return (uint8_t)KeyFunction::ToggleVFO;
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
  case AnytoneKeySettingsExtension::KeyFunction::MaxVolume:         return (uint8_t)KeyFunction::MaxVolume;
  case AnytoneKeySettingsExtension::KeyFunction::Slot:              return (uint8_t)KeyFunction::Slot;
  case AnytoneKeySettingsExtension::KeyFunction::Zone:              return (uint8_t)KeyFunction::Zone;
  case AnytoneKeySettingsExtension::KeyFunction::MuteA:             return (uint8_t)KeyFunction::MuteA;
  case AnytoneKeySettingsExtension::KeyFunction::MuteB:             return (uint8_t)KeyFunction::MuteB;
  case AnytoneKeySettingsExtension::KeyFunction::RoamingSet:        return (uint8_t)KeyFunction::RoamingSet;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSet:           return (uint8_t)KeyFunction::APRSSet;
  case AnytoneKeySettingsExtension::KeyFunction::ZoneUp:            return (uint8_t)KeyFunction::ZoneUp;
  case AnytoneKeySettingsExtension::KeyFunction::ZoneDown:          return (uint8_t)KeyFunction::ZoneDown;
  case AnytoneKeySettingsExtension::KeyFunction::XBandRepeater:     return (uint8_t)KeyFunction::XBandRepeater;
  case AnytoneKeySettingsExtension::KeyFunction::Speaker:           return (uint8_t)KeyFunction::Speaker;
  case AnytoneKeySettingsExtension::KeyFunction::ChannelName:       return (uint8_t)KeyFunction::ChannelName;
  case AnytoneKeySettingsExtension::KeyFunction::Bluetooth:         return (uint8_t)KeyFunction::Bluetooth;
  case AnytoneKeySettingsExtension::KeyFunction::GPS:               return (uint8_t)KeyFunction::GPS;
  case AnytoneKeySettingsExtension::KeyFunction::CDTScan:           return (uint8_t)KeyFunction::CDTScan;
  case AnytoneKeySettingsExtension::KeyFunction::TBSTSend:          return (uint8_t)KeyFunction::TBSTSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSSend:          return (uint8_t)KeyFunction::APRSSend;
  case AnytoneKeySettingsExtension::KeyFunction::APRSInfo:          return (uint8_t)KeyFunction::APRSInfo;
  case AnytoneKeySettingsExtension::KeyFunction::GPSRoaming:        return (uint8_t)KeyFunction::GPSRoaming;
  default:                                                          return (uint8_t)KeyFunction::Off;
  }
}

AnytoneKeySettingsExtension::KeyFunction
D578UVCodeplug::GeneralSettingsElement::mapCodeToKeyFunction(uint8_t code) const {
  switch ((KeyFunction)code) {
  case KeyFunction::Off:               return AnytoneKeySettingsExtension::KeyFunction::Off;
  case KeyFunction::Voltage:           return AnytoneKeySettingsExtension::KeyFunction::Voltage;
  case KeyFunction::Power:             return AnytoneKeySettingsExtension::KeyFunction::Power;
  case KeyFunction::Repeater:          return AnytoneKeySettingsExtension::KeyFunction::Repeater;
  case KeyFunction::Reverse:           return AnytoneKeySettingsExtension::KeyFunction::Reverse;
  case KeyFunction::Encryption:        return AnytoneKeySettingsExtension::KeyFunction::Encryption;
  case KeyFunction::Call:              return AnytoneKeySettingsExtension::KeyFunction::Call;
  case KeyFunction::ToggleVFO:         return AnytoneKeySettingsExtension::KeyFunction::ToggleVFO;
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
  case KeyFunction::MaxVolume:         return AnytoneKeySettingsExtension::KeyFunction::MaxVolume;
  case KeyFunction::Slot:              return AnytoneKeySettingsExtension::KeyFunction::Slot;
  case KeyFunction::Zone:              return AnytoneKeySettingsExtension::KeyFunction::Zone;
  case KeyFunction::MuteA:             return AnytoneKeySettingsExtension::KeyFunction::MuteA;
  case KeyFunction::MuteB:             return AnytoneKeySettingsExtension::KeyFunction::MuteB;
  case KeyFunction::RoamingSet:        return AnytoneKeySettingsExtension::KeyFunction::RoamingSet;
  case KeyFunction::APRSSet:           return AnytoneKeySettingsExtension::KeyFunction::APRSSet;
  case KeyFunction::ZoneUp:            return AnytoneKeySettingsExtension::KeyFunction::ZoneUp;
  case KeyFunction::ZoneDown:          return AnytoneKeySettingsExtension::KeyFunction::ZoneDown;
  case KeyFunction::XBandRepeater:     return AnytoneKeySettingsExtension::KeyFunction::XBandRepeater;
  case KeyFunction::Speaker:           return AnytoneKeySettingsExtension::KeyFunction::Speaker;
  case KeyFunction::ChannelName:       return AnytoneKeySettingsExtension::KeyFunction::ChannelName;
  case KeyFunction::Bluetooth:         return AnytoneKeySettingsExtension::KeyFunction::Bluetooth;
  case KeyFunction::GPS:               return AnytoneKeySettingsExtension::KeyFunction::GPS;
  case KeyFunction::CDTScan:           return AnytoneKeySettingsExtension::KeyFunction::CDTScan;
  case KeyFunction::TBSTSend:          return AnytoneKeySettingsExtension::KeyFunction::TBSTSend;
  case KeyFunction::APRSSend:          return AnytoneKeySettingsExtension::KeyFunction::APRSSend;
  case KeyFunction::APRSInfo:          return AnytoneKeySettingsExtension::KeyFunction::APRSInfo;
  case KeyFunction::GPSRoaming:        return AnytoneKeySettingsExtension::KeyFunction::GPSRoaming;
  default:                             return AnytoneKeySettingsExtension::KeyFunction::Off;
  }
}


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug
 * ******************************************************************************************** */
D578UVCodeplug::D578UVCodeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D578UVCodeplug::D578UVCodeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D578UV Codeplug", parent)
{
  // pass...
}

void
D578UVCodeplug::allocateUpdated() {
  D878UVCodeplug::allocateUpdated();

  image(0).addElement(ADDR_UNKNOWN_SETTING_1, UNKNOWN_SETTING_1_SIZE);
  image(0).addElement(ADDR_UNKNOWN_SETTING_2, UNKNOWN_SETTING_2_SIZE);
  image(0).addElement(ADDR_UNKNOWN_SETTING_3, UNKNOWN_SETTING_3_SIZE);
}

void
D578UVCodeplug::allocateHotKeySettings() {
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
}

bool
D578UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (int i=0; i<ctx.config()->channelList()->count(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    ch.fromChannelObj(ctx.config()->channelList()->channel(i), ctx);
  }
  return true;
}

bool
D578UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D578UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}


void
D578UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  unsigned contactCount=0;
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // enabled if false (ass hole)
    if (1 == ((contact_bitmap[i/8]>>(i%8)) & 0x01))
      continue;
    contactCount++;
    uint32_t bank_addr = CONTACT_BLOCK_0 + (i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + (i%CONTACTS_PER_BANK)*CONTACT_SIZE;
    if (!isAllocated(addr, 0)) {
      image(0).addElement(addr, CONTACT_BANK_SIZE);
      memset(data(addr), 0x00, CONTACT_BANK_SIZE);
    }
  }
  if (contactCount) {
    image(0).addElement(CONTACT_INDEX_LIST, align_size(4*contactCount, 16));
    memset(data(CONTACT_INDEX_LIST), 0xff, align_size(4*contactCount, 16));
    image(0).addElement(CONTACT_ID_MAP, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
    memset(data(CONTACT_ID_MAP), 0xff, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
  }
}


bool
D578UVCodeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  QVector<DMRContact*> contacts;
  // Encode contacts and also collect id<->index map
  for (int i=0; i<ctx.config()->contacts()->digitalCount(); i++) {
    uint32_t bank_addr = CONTACT_BLOCK_0 + (i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + (i%CONTACTS_PER_BANK)*CONTACT_SIZE;
    ContactElement con(data(addr));
    DMRContact *contact = ctx.config()->contacts()->digitalContact(i);
    if(! con.fromContactObj(contact, ctx))
      return false;
    ((uint32_t *)data(CONTACT_INDEX_LIST))[i] = qToLittleEndian(i);
    contacts.append(contact);
  }
  // encode index map for contacts
  std::sort(contacts.begin(), contacts.end(),
            [](DMRContact *a, DMRContact *b) {
    return a->number() < b->number();
  });
  for (int i=0; i<contacts.size(); i++) {
    ContactMapElement el(data(CONTACT_ID_MAP + i*CONTACT_ID_ENTRY_SIZE));
    el.setID(contacts[i]->number(), (DMRContact::GroupCall==contacts[i]->type()));
    el.setIndex(ctx.index(contacts[i]));
  }
  return true;
}


void
D578UVCodeplug::allocateGeneralSettings() {
  // override allocation of general settings for D878UV code-plug. General settings are larger!
  image(0).addElement(Offset::settings(), GeneralSettingsElement::size());
  image(0).addElement(Offset::gpsMessages(), GPSMessageElement::size());
  image(0).addElement(Offset::settingsExtension(), GeneralSettingsExtensionElement::size());

}
bool
D578UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).fromConfig(flags, ctx);
  GPSMessageElement(data(Offset::gpsMessages())).fromConfig(flags, ctx);
  GeneralSettingsExtensionElement(data(Offset::settingsExtension())).fromConfig(flags, ctx);
  return true;
}
bool
D578UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  GeneralSettingsElement(data(Offset::settings())).updateConfig(ctx);
  GPSMessageElement(data(Offset::gpsMessages())).updateConfig(ctx);
  GeneralSettingsExtensionElement(data(Offset::settingsExtension())).updateConfig(ctx);
  return true;
}



