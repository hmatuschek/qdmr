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

#define NUM_CONTACTS              10000      // Total number of contacts
#define NUM_CONTACT_BANKS         2500       // Number of contact banks
#define CONTACTS_PER_BANK         4
#define CONTACT_BANK_0            0x02680000 // First bank of 4 contacts
#define CONTACT_BANK_SIZE         0x00000190 // Size of 4 contacts
#define CONTACT_INDEX_LIST        0x02600000 // Address of contact index list
#define CONTACTS_BITMAP           0x02640000 // Address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // Size of contact bitmap
#define CONTACT_ID_MAP            0x04800000 // Address of ID->Contact index map
#define CONTACT_ID_ENTRY_SIZE     sizeof(contact_map_t) // Size of each map entry

#define NUM_APRS_RX_ENTRY         32
#define ADDR_APRS_RX_ENTRY        0x02501800 // Address of APRS RX list
#define APRS_RX_ENTRY_SIZE        0x00000008 // Size of each APRS RX entry
static_assert(
  APRS_RX_ENTRY_SIZE == sizeof(D578UVCodeplug::aprs_rx_entry_t),
  "D578UVCodeplug::aprs_rx_entry_t size check failed.");

#define ADDR_APRS_SET_EXT         0x025010A0 // Address of APRS settings extension
#define APRS_SET_EXT_SIZE         0x00000060 // Size of APRS settings extension
static_assert(
  APRS_SET_EXT_SIZE == sizeof(D578UVCodeplug::aprs_setting_ext_t),
  "D578UVCodeplug::aprs_setting_ext_t size check failed.");

#define ADDR_UNKNOWN_SETTING      0x02500600 // Address of unknown settings
#define UNKNOWN_SETTING_SIZE      0x00000030 // Size of unknown settings.



/* ******************************************************************************************** *
 * Implementation of D878UV2Codeplug
 * ******************************************************************************************** */
D578UVCodeplug::D578UVCodeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
  // pass...
}

void
D578UVCodeplug::allocateUpdated() {
  // allocate everything from D878UV codeplug
  D878UVCodeplug::allocateUpdated();

  // allocate unknown settings
  image(0).addElement(ADDR_UNKNOWN_SETTING, UNKNOWN_SETTING_SIZE);

  // allocate APRS RX list
  image(0).addElement(ADDR_APRS_RX_ENTRY, NUM_APRS_RX_ENTRY*APRS_RX_ENTRY_SIZE);
  // allocate APRS settings extension
  image(0).addElement(ADDR_APRS_SET_EXT, APRS_SET_EXT_SIZE);
}


void
D578UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  uint contactCount=0;
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
D578UVCodeplug::encodeContacts(Config *config, const Flags &flags) {
  // Encode contacts
  QVector<contact_map_t> contact_id_map;
  contact_id_map.reserve(config->contacts()->digitalCount());
  for (int i=0; i<config->contacts()->digitalCount(); i++) {
    contact_t *con = (contact_t *)data(CONTACT_BANK_0+i*sizeof(contact_t));
    con->fromContactObj(config->contacts()->digitalContact(i));
    ((uint32_t *)data(CONTACT_INDEX_LIST))[i] = qToLittleEndian(i);
    contact_map_t entry;
    entry.setID(config->contacts()->digitalContact(i)->number(),
                DigitalContact::GroupCall == config->contacts()->digitalContact(i)->type());
    entry.setIndex(i);
    contact_id_map.append(entry);
  }
  // encode index map for contacts
  std::sort(contact_id_map.begin(), contact_id_map.end(),
            [](const contact_map_t &a, const contact_map_t &b) {
    return a.ID() < b.ID();
  });
  for (int i=0; i<contact_id_map.size(); i++) {
    ((contact_map_t *)data(CONTACT_ID_MAP))[i] = contact_id_map[i];
  }
  return true;
}


