#include "radioddity_codeplug.hh"


/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug::ChannelElement
 * ********************************************************************************************* */
RadioddityCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

RadioddityCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x38)
{
  // pass...
}

RadioddityCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of RadioddityCodeplug
 * ********************************************************************************************* */
RadioddityCodeplug::RadioddityCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

RadioddityCodeplug::~RadioddityCodeplug() {
  // pass...
}

void
RadioddityCodeplug::clear() {
  // pass...
}

bool
RadioddityCodeplug::encode(Config *config, const Flags &flags) {
  return false;
}

bool
RadioddityCodeplug::decode(Config *config) {
  return false;
}
