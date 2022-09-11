#include "openrtx_codeplug.hh"
#include "utils.hh"
#include "logger.hh"
#include "scanlist.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ChannelElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, 0x38)
{
  // pass...
}

OpenRTXCodeplug::ChannelElement::~ChannelElement() {
  // pass...
}

void
OpenRTXCodeplug::ChannelElement::clear() {
}

Channel *
OpenRTXCodeplug::ChannelElement::toChannelObj(Codeplug::Context &ctx) const {
  return nullptr;
}

bool
OpenRTXCodeplug::ChannelElement::linkChannelObj(Channel *c, Context &ctx) const {
  return false;
}

bool
OpenRTXCodeplug::ChannelElement::fromChannelObj(const Channel *c, Context &ctx) {
  clear();
  return false;
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ContactElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ContactElement::ContactElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, 0x0018)
{
  // pass...
}

OpenRTXCodeplug::ContactElement::~ContactElement() {
  // pass...
}

void
OpenRTXCodeplug::ContactElement::clear() {
}

bool
OpenRTXCodeplug::ContactElement::isValid() const {
  return false;
}

DigitalContact *
OpenRTXCodeplug::ContactElement::toContactObj(Context &ctx) const {
  return nullptr;
}

void
OpenRTXCodeplug::ContactElement::fromContactObj(const DigitalContact *cont, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ZoneElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ZoneElement::ZoneElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

OpenRTXCodeplug::ZoneElement::~ZoneElement() {
  // pass...
}

void
OpenRTXCodeplug::ZoneElement::clear() {
}
bool
OpenRTXCodeplug::ZoneElement::isValid() const {
  return false;
}

Zone *
OpenRTXCodeplug::ZoneElement::toZoneObj(Context &ctx) const {
  return nullptr;
}

bool
OpenRTXCodeplug::ZoneElement::linkZoneObj(Zone *zone, Context &ctx, bool putInB) const {
  return false;
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjA(const Zone *zone, Context &ctx) {
}

void
OpenRTXCodeplug::ZoneElement::fromZoneObjB(const Zone *zone, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::GroupListElement
 * ********************************************************************************************* */
OpenRTXCodeplug::GroupListElement::GroupListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, 0x0030)
{
  // pass...
}

OpenRTXCodeplug::GroupListElement::~GroupListElement() {
  // pass...
}

void
OpenRTXCodeplug::GroupListElement::clear() {
}

RXGroupList *
OpenRTXCodeplug::GroupListElement::toRXGroupListObj(Context &ctx) {
  return nullptr;
}

bool
OpenRTXCodeplug::GroupListElement::linkRXGroupListObj(int ncnt, RXGroupList *lst, Context &ctx) const {
  return false;
}

void
OpenRTXCodeplug::GroupListElement::fromRXGroupListObj(const RXGroupList *lst, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::ScanListElement
 * ********************************************************************************************* */
OpenRTXCodeplug::ScanListElement::ScanListElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pass...
}

OpenRTXCodeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, 0x0058)
{
  // pass...
}

OpenRTXCodeplug::ScanListElement::~ScanListElement() {
  // pass...
}

void
OpenRTXCodeplug::ScanListElement::clear() {
  // pass...
}

ScanList *
OpenRTXCodeplug::ScanListElement::toScanListObj(Context &ctx) const {
  return nullptr;
}

bool
OpenRTXCodeplug::ScanListElement::linkScanListObj(ScanList *lst, Context &ctx) const {
  return false;
}

void
OpenRTXCodeplug::ScanListElement::fromScanListObj(const ScanList *lst, Context &ctx) {
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug::GeneralSettingsElement
 * ********************************************************************************************* */
OpenRTXCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : Element(ptr, size)
{
  // pas...
}

OpenRTXCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : Element(ptr, 0x0028)
{
  // pass...
}

OpenRTXCodeplug::GeneralSettingsElement::~GeneralSettingsElement() {
  // pass...
}

void
OpenRTXCodeplug::GeneralSettingsElement::clear() {
}

bool
OpenRTXCodeplug::GeneralSettingsElement::fromConfig(const Config *conf, Context &ctx) {
  return false;
}

bool
OpenRTXCodeplug::GeneralSettingsElement::updateConfig(Config *conf, Context &ctx) {
  return false;
}


/* ********************************************************************************************* *
 * Implementation of OpenRTXCodeplug
 * ********************************************************************************************* */
OpenRTXCodeplug::OpenRTXCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // pass...
}

OpenRTXCodeplug::~OpenRTXCodeplug() {
  // pass...
}

void
OpenRTXCodeplug::clear() {
  // Clear general config
  clearGeneralSettings();
  // Clear contacts
  clearContacts();
  // clear zones
  clearZones();
  // clear scan lists
  clearScanLists();
  // clear group lists
  clearGroupLists();
}

bool
OpenRTXCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  // All indices as 1-based. That is, the first channel gets index 1.

  // Map radio IDs
  for (int i=0; i<config->radioIDs()->count(); i++)
    ctx.add(config->radioIDs()->getId(i), i+1);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (config->contacts()->contact(i)->is<DigitalContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DigitalContact>(), d+1); d++;
    } else if (config->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(config->contacts()->contact(i)->as<DTMFContact>(), a+1); a++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i+1);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i+1);

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i+1);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i+1);

  // Map DMR APRS systems
  for (int i=0,a=0,d=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<GPSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<GPSSystem>(), d+1); d++;
    } else if (config->posSystems()->system(i)->is<APRSSystem>()) {
      ctx.add(config->posSystems()->system(i)->as<APRSSystem>(), a+1); a++;
    }
  }

  // Map roaming
  for (int i=0; i<config->roaming()->count(); i++)
    ctx.add(config->roaming()->zone(i), i+1);

  return true;
}

bool
OpenRTXCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  // Check if default DMR id is set.
  if (nullptr == config->radioIDs()->defaultId()) {
    errMsg(err) << "Cannot encode TyT codeplug: No default radio ID specified.";
    return false;
  }

  // Create index<->object table.
  Context ctx(config);
  if (! index(config, ctx, err))
    return false;

  return this->encodeElements(flags, ctx, err);
}

bool
OpenRTXCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err) {
  // General config
  if (! this->encodeGeneralSettings(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! this->encodeChannels(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }

  if (! this->encodeZones(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! this->encodeScanLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode scan lists.";
    return false;
  }

  if (! this->encodeGroupLists(ctx.config(), flags, ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  return true;
}

bool
OpenRTXCodeplug::decode(Config *config, const ErrorStack &err) {
  // Clear config object
  config->clear();

  // Create index<->object table.
  Context ctx(config);

  return this->decodeElements(ctx, err);
}

bool
OpenRTXCodeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! this->decodeGeneralSettings(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  if (! this->createContacts(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create contacts.";
    return false;
  }

  if (! this->createChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create channels.";
    return false;
  }

  if (! this->createZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create zones.";
    return false;
  }

  if (! this->createScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create scan lists.";
    return false;
  }

  if (! this->createGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot create group lists.";
    return false;
  }

  if (! this->linkChannels(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! this->linkZones(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  if (! this->linkScanLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link scan lists.";
    return false;
  }

  if (! this->linkGroupLists(ctx.config(), ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  return true;
}


void
OpenRTXCodeplug::clearGeneralSettings(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeGeneralSettings(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::decodeGeneralSettings(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearContacts(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeContacts(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createContacts(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearChannels(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeChannels(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createChannels(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkChannels(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearZones(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeZones(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createZones(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkZones(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearGroupLists(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeGroupLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkGroupLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}


void
OpenRTXCodeplug::clearScanLists(const ErrorStack &err) {
}

bool
OpenRTXCodeplug::encodeScanLists(Config *config, const Flags &flags, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::createScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}

bool
OpenRTXCodeplug::linkScanLists(Config *config, Context &ctx, const ErrorStack &err) {
  return false;
}
