#include "tyt_codeplug.hh"
#include "codeplugcontext.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "config.h"
#include "logger.hh"
#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of TyTCodeplug
 * ******************************************************************************************** */
TyTCodeplug::TyTCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

TyTCodeplug::~TyTCodeplug() {
  // pass...
}

void
TyTCodeplug::clear()
{
  // Clear timestamp
  this->clearTimestamp();
  // Clear general config
  this->clearGeneralSettings();
  // Clear menu settings
  this->clearMenuSettings();
  // Clear button settings
  this->clearButtonSettings();
  // Clear text messages
  this->clearTextMessages();
  // Clear privacy keys
  this->clearPrivacyKeys();
  // Clear emergency systems
  this->clearEmergencySystems();
  // Clear RX group lists
  this->clearGroupLists();
  // Clear zones & zone extensions
  this->clearZones();
  // Clear scan lists
  this->clearScanLists();
  // Clear VFO A & B settings.
  this->clearVFOSettings();
  // Clear GPS systems
  this->clearPositioningSystems();
  // Clear channels
  this->clearChannels();
  // Clear contacts
  this->clearContacts();
}

bool
TyTCodeplug::encode(Config *config, const Flags &flags) {
  // Set timestamp
  if (! this->encodeTimestamp(config, flags)) {
    _errorMessage = tr("Cannot encode time-stamp: %1").arg(_errorMessage);
    return false;
  }
  // General config
  if (! this->encodeGeneralSettings(config, flags)) {
    _errorMessage = tr("Cannot encode general settings: %1").arg(_errorMessage);
    return false;
  }

  // Boot time settings
  if (! this->encodeBootSettings(config, flags)) {
    _errorMessage = tr("Cannot encode boot settings: %1").arg(_errorMessage);
    return false;
  }

  // Define Contacts
  if (! this->encodeContacts(config, flags)) {
    _errorMessage = tr("Cannot encode contacts: %1").arg(_errorMessage);
    return false;
  }

  // Define RX GroupLists
  if (! this->encodeGroupLists(config, flags)) {
    _errorMessage = tr("Cannot encode group lists: %1").arg(_errorMessage);
    return false;
  }

  // Define Channels
  if (! this->encodeChannels(config, flags)) {
    _errorMessage = tr("Cannot encode channels: %1").arg(_errorMessage);
    return false;
  }

  // Define Zones
  if (! this->encodeZones(config, flags)) {
    _errorMessage = tr("Cannot encode zones: %1").arg(_errorMessage);
    return false;
  }

  // Define Scanlists
  if (! this->encodeScanLists(config, flags)) {
    _errorMessage = tr("Cannot encode scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Define GPS systems
  if (! this->encodePositioningSystems(config, flags)) {
    _errorMessage = tr("Cannot encode positioning systems: %1").arg(_errorMessage);
    return false;
  }

  return true;
}

bool
TyTCodeplug::decode(Config *config) {
  // Create index<->object table.
  CodeplugContext ctx(config);

  // Clear config object
  config->reset();

  // General config
  if (! this->decodeGeneralSettings(config)) {
    _errorMessage = tr("Cannot decode general settings: %1").arg(_errorMessage);
    return false;
  }

  // Define Contacts
  if (! this->createContacts(config, ctx)) {
    _errorMessage = tr("Cannot create contacts: %1").arg(_errorMessage);
    return false;
  }

  // Define RX GroupLists
  if (! this->createGroupLists(config, ctx)) {
    _errorMessage = tr("Cannot create group lists: %1").arg(_errorMessage);
    return false;
  }

  // Define Channels
  if (! this->createChannels(config, ctx)) {
    _errorMessage = tr("Cannot create channels: %1").arg(_errorMessage);
    return false;
  }

  // Define Zones
  if (! this->createZones(config, ctx)) {
    _errorMessage = tr("Cannot create zones: %1").arg(_errorMessage);
    return false;
  }

  // Define Scanlists
  if (! this->createScanLists(config, ctx)) {
    _errorMessage = tr("Cannot create scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Define GPS systems
  if (! this->createPositioningSystems(config, ctx)) {
    _errorMessage = tr("Cannot create positioning systems: %1").arg(_errorMessage);
    return false;
  }

  // Link RX GroupLists
  if (! this->linkGroupLists(config, ctx)) {
    _errorMessage = tr("Cannot link group lists: %1").arg(_errorMessage);
    return false;
  }

  // Link Channels
  if (! this->linkChannels(config, ctx)) {
    _errorMessage = tr("Cannot link channels: %1").arg(_errorMessage);
    return false;
  }

  // Link Zones
  if (! this->linkZones(config, ctx)) {
    _errorMessage = tr("Cannot link zones: %1").arg(_errorMessage);
    return false;
  }

  // Link Scanlists
  if (! this->linkScanLists(config, ctx)) {
    _errorMessage = tr("Cannot link scan lists: %1").arg(_errorMessage);
    return false;
  }

  // Link GPS systems
  if (! this->linkPositioningSystems(config, ctx)) {
    _errorMessage = tr("Cannot link positioning systems: %1").arg(_errorMessage);
    return false;
  }

  return true;
}


void
TyTCodeplug::clearTimestamp() {
  // pass...
}

bool
TyTCodeplug::encodeTimestamp(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeTimestamp(Config *config) {
  return true;
}


void
TyTCodeplug::clearGeneralSettings() {
  // pass...
}

bool
TyTCodeplug::encodeGeneralSettings(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeGeneralSettings(Config *config) {
  return true;
}


void
TyTCodeplug::clearBootSettings() {
  // pass...
}

bool
TyTCodeplug::encodeBootSettings(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::decodeBootSettings(Config *config) {
  return true;
}


void
TyTCodeplug::clearContacts() {
  // pass...
}

bool
TyTCodeplug::encodeContacts(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createContacts(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearGroupLists() {
  // pass...
}

bool
TyTCodeplug::encodeGroupLists(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createGroupLists(Config *config, CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkGroupLists(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearChannels() {
  // pass...
}

bool
TyTCodeplug::encodeChannels(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createChannels(Config *config, CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkChannels(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearZones() {
  // pass...
}

bool
TyTCodeplug::encodeZones(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createZones(Config *config, CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkZones(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearScanLists() {
  // pass...
}

bool
TyTCodeplug::encodeScanLists(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createScanLists(Config *config, CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkScanLists(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearPositioningSystems() {
  // pass...
}

bool
TyTCodeplug::encodePositioningSystems(Config *config, const Flags &flags) {
  return true;
}

bool
TyTCodeplug::createPositioningSystems(Config *config, CodeplugContext &ctx) {
  return true;
}

bool
TyTCodeplug::linkPositioningSystems(Config *config, CodeplugContext &ctx) {
  return true;
}


void
TyTCodeplug::clearMenuSettings() {
  // pass...
}

void
TyTCodeplug::clearButtonSettings() {
  // pass...
}

void
TyTCodeplug::clearTextMessages() {
  // pass...
}

void
TyTCodeplug::clearPrivacyKeys() {
  // pass...
}

void
TyTCodeplug::clearEmergencySystems() {
  // pass...
}

void
TyTCodeplug::clearVFOSettings() {
  // pass...
}
