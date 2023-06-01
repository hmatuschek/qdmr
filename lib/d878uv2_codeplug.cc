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
  ContactBitmapElement contact_bitmap(data(Offset::contactBitmap()));
  unsigned contactCount=0;
  for (uint16_t i=0; i<Limit::numContacts(); i++) {
    // enabled if false (ass hole)
    if (! contact_bitmap.isEncoded(i))
      continue;
    contactCount++;
    uint32_t bank_addr = Offset::contactBanks() + (contactCount/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + ((i%Limit::contactsPerBank())/Limit::contactsPerBlock())*Offset::betweenContactBlocks();
    if (! isAllocated(addr, 0)) {
      image(0).addElement(addr, Offset::betweenContactBlocks());
      memset(data(addr), 0x00, Offset::betweenContactBlocks());
    }
  }
  if (contactCount) {
    image(0).addElement(Offset::contactIndex(), align_size(4*contactCount, 16));
    memset(data(Offset::contactIndex()), 0xff, align_size(4*contactCount, 16));
    image(0).addElement(Offset::contactIdTable(), align_size(ContactMapElement::size()*(1+contactCount), 16));
    memset(data(Offset::contactIdTable()), 0xff, align_size(ContactMapElement::size()*(1+contactCount), 16));
  }
}

bool
D878UV2Codeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  QVector<DMRContact*> contacts;
  // Encode contacts and also collect id<->index map
  for (unsigned int i=0; i<ctx.count<DMRContact>(); i++) {
    uint32_t bank_addr = Offset::contactBanks() + (i/Limit::contactsPerBank())*Offset::betweenContactBanks();
    uint32_t addr = bank_addr + (i%Limit::contactsPerBank())*ContactElement::size();
    ContactElement con(data(addr));
    DMRContact *contact = ctx.get<DMRContact>(i);
    if(! con.fromContactObj(contact, ctx))
      return false;
    ((uint32_t *)data(Offset::contactIndex()))[i] = qToLittleEndian(i);
    contacts.append(contact);
  }
  // encode index map for contacts
  std::sort(contacts.begin(), contacts.end(),
            [](DMRContact *a, DMRContact *b) {
    return a->number() < b->number();
  });
  for (int i=0; i<contacts.size(); i++) {
    ContactMapElement el(data(Offset::contactIdTable() + i*ContactMapElement::size()));
    el.setID(contacts[i]->number(), (DMRContact::GroupCall==contacts[i]->type()));
    el.setIndex(ctx.index(contacts[i]));
  }
  return true;
}


