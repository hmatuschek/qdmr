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
#define NUM_CONTACT_BANKS         2500       // Number of contact banks
#define CONTACTS_PER_BANK         4
#define CONTACT_BANK_0            0x02680000 // First bank of 4 contacts
#define CONTACT_BANK_SIZE         0x00000190 // Size of 4 contacts
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


/* ******************************************************************************************** *
 * Implementation of D578UVCodeplug
 * ******************************************************************************************** */
D578UVCodeplug::D578UVCodeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
  // pass...
}

void
D578UVCodeplug::allocateUpdated() {
  D868UVCodeplug::allocateUpdated();

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
    uint32_t addr = CONTACT_BANK_0+(i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
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

  QVector<DigitalContact*> contacts;
  // Encode contacts and also collect id<->index map
  for (int i=0; i<ctx.config()->contacts()->digitalCount(); i++) {
    ContactElement con(data(CONTACT_BANK_0+i*CONTACT_SIZE));
    DigitalContact *contact = ctx.config()->contacts()->digitalContact(i);
    if(! con.fromContactObj(contact, ctx))
      return false;
    ((uint32_t *)data(CONTACT_INDEX_LIST))[i] = qToLittleEndian(i);
    contacts.append(contact);
  }
  // encode index map for contacts
  std::sort(contacts.begin(), contacts.end(),
            [](DigitalContact *a, DigitalContact *b) {
    return a->number() < b->number();
  });
  for (int i=0; i<contacts.size(); i++) {
    ContactMapElement el(data(CONTACT_ID_MAP + i*CONTACT_ID_ENTRY_SIZE));
    el.setID(contacts[i]->number(), (DigitalContact::GroupCall==contacts[i]->type()));
    el.setIndex(ctx.index(contacts[i]));
  }
  return true;
}


