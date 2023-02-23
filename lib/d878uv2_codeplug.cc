#include "d878uv2_codeplug.hh"
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



/* ******************************************************************************************** *
 * Implementation of D878UV2Codeplug
 * ******************************************************************************************** */
D878UV2Codeplug::D878UV2Codeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D878UV2Codeplug::D878UV2Codeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D868UVII Codeplug", parent)
{
  // pass...
}

/* The address of the contact ID<->Index table has changed hence allocation and encoding must
 * be reimplemented. Otherwise, everything remains the same. */
void
D878UV2Codeplug::allocateContacts() {
  /* Allocate contacts */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  unsigned contactCount=0;
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // enabled if false (ass hole)
    if (1 == ((contact_bitmap[i/8]>>(i%8)) & 0x01))
      continue;
    contactCount++;
    uint32_t bank_addr = CONTACT_BLOCK_0 + (contactCount/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + ((i%CONTACTS_PER_BANK)/CONTACTS_PER_BLOCK)*CONTACT_BLOCK_SIZE;
    if (! isAllocated(addr, 0)) {
      image(0).addElement(addr, CONTACT_BLOCK_SIZE);
      memset(data(addr), 0x00, CONTACT_BLOCK_SIZE);
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
D878UV2Codeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
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


