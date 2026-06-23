//
// Created by hannes on 23.06.26.
//

#include "rt4d_codeplug.hh"

/* ********************************************************************************************* *
 * Implementation of RT4DCodeplug
 * ********************************************************************************************* */
RT4DCodeplug::RT4DCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // Preallocate entrie codeplug
  addImage("Radtel RT4D Codeplug, FW 3.25");
  image(0).addElement(Offset::firstSettings(), FirstSettingsElement::size());
  image(0).element(0).data().fill(0xff);
  image(0).addElement(Offset::channels(), ChannelBankElement::size());
  image(0).addElement(Offset::secondSettings(), SecondSettingsElement::size());
  image(0).addElement(Offset::zones(), ZoneBankElement::size());
  image(0).addElement(Offset::contacts(), ContactBankElement::size());
  image(0).addElement(Offset::groupLists(), GroupListBankElement::size());
  image(0).addElement(Offset::keys(), EncryptionKeyBankElement::size());
  image(0).addElement(Offset::messages(), MessageBankElement::size());
  image(0).addElement(Offset::fmBroadcast(), FMBroadcastChannelBankElement::size());
}


bool
RT4DCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}


bool
RT4DCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
}


bool
RT4DCodeplug::decode(Config *config, const ErrorStack &err) {
  return false;
}