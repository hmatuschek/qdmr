#include "radio.hh"
#include "hidinterface.hh"
#include "dfu_libusb.hh"
#include <QDebug>
#include "rd5r.hh"
#include "uv390.hh"
#include "config.hh"



/* ******************************************************************************************** *
 * Implementation of Radio
 * ******************************************************************************************** */
Radio::Radio(QObject *parent)
  : QThread(parent), _task(StatusIdle)
{
  // pass...
}

bool
Radio::verifyConfig(Config *config, QList<VerifyIssue> &issues)
{
  // Check general config
  if (config->name().size() > features().maxNameLength)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Radio name of length %1 exceeds limit of %2 characters.")
                    .arg(config->name().size()).arg(features().maxNameLength)));
  if (config->introLine1().size() > features().maxIntroLineLength)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Intro line 1 of length %1 exceeds limit of %2 characters.")
                    .arg(config->introLine1().size()).arg(features().maxNameLength)));
  if (config->introLine2().size() > features().maxIntroLineLength)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Intro line 2 of length %1 exceeds limit of %2 characters.")
                    .arg(config->introLine2().size()).arg(features().maxNameLength)));

  // Check contact list
  if (config->contacts()->count() > features().maxContacts)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of contacts %1 exceeds limit of %2.")
                    .arg(config->contacts()->count()).arg(features().maxContacts)));
  for (int i=0; i<config->contacts()->count(); i++) {
    QSet<QString> names;
    Contact *contact = config->contacts()->contact(i);
    if (names.contains(contact->name()))
      issues.append(VerifyIssue(VerifyIssue::ERROR,tr("Duplicate contact name '%1'.").arg(contact->name())));
    names.insert(contact->name());

    if (contact->name().size() > features().maxContactNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Contact name '%1' length %2 exceeds limit of %3 characters.")
                      .arg(contact->name()).arg(contact->name().size()).arg(features().maxContactNameLength)));

    if (contact->is<DigitalContact>() && (! features().hasDigital))
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Radio does not support digital contact '%1'")
                      .arg(contact->name())));

    if (contact->is<DTMFContact>() && (! features().hasAnalog))
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Radio does not support DTMF contact '%1'")
                      .arg(contact->name())));
  }

  // Check RX group lists.
  if (config->rxGroupLists()->count() > features().maxGrouplists)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of Rx group lists %1 exceeds limit of %2")
                    .arg(config->rxGroupLists()->count()).arg(features().maxGrouplists)));
  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    QSet<QString> names;
    RXGroupList *list = config->rxGroupLists()->list(i);
    if (names.contains(list->name()))
      issues.append(VerifyIssue(VerifyIssue::ERROR,tr("Duplicate Rx group list name '%1'.").arg(list->name())));
    names.insert(list->name());
    if (list->name().size() > features().maxGrouplistNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Group list name '%1' of length %2 exceeds limit of %2 characters.")
                      .arg(list->name()).arg(list->name().size()).arg(features().maxGrouplistNameLength)));
    if (list->count() > features().maxContactsInGrouplist)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of contacts (%2) in group list '%1' exceeds limit %3.")
                      .arg(list->name()).arg(list->count()).arg(features().maxContactsInGrouplist)));
  }

  // Check channel list
  if (config->channelList()->count() > features().maxChannels)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of channels %1 exceeds limit %2.")
                      .arg(config->channelList()->count()).arg(features().maxChannels)));
  for (int i=0; i<config->channelList()->count(); i++) {
    QSet<QString> names;
    Channel *channel = config->channelList()->channel(i);
    if (names.contains(channel->name()))
      issues.append(VerifyIssue(VerifyIssue::ERROR,tr("Duplicate channel name '%1'.").arg(channel->name())));
    names.insert(channel->name());
    if (channel->name().size() > features().maxChannelNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Channel name '%1' length %2 exceeds limit of %3 characters.")
                      .arg(channel->name()).arg(channel->name().length()).arg(features().maxChannelNameLength)));
    if (channel->is<DigitalChannel>() && (! features().hasDigital))
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Radio does not support digital channel'%1'")
                      .arg(channel->name())));
    if (channel->is<AnalogChannel>() && (! features().hasAnalog))
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Radio does not support analog channel'%1'")
                      .arg(channel->name())));
  }

  // Check zone list
  if (config->zones()->count() > features().maxZones)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of zones %1 exceeds limit %2")
                    .arg(config->zones()->count()).arg(features().maxZones)));
  for (int i=0; i<config->zones()->count(); i++) {
    QSet<QString> names;
    Zone *zone = config->zones()->zone(i);
    if (names.contains(zone->name()))
      issues.append(VerifyIssue(VerifyIssue::ERROR,tr("Duplicate zone name '%1'.").arg(zone->name())));
    names.insert(zone->name());
    if (zone->name().size()>features().maxZoneNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Zone name '%1' length %2 exceeds limit of %3 characters.")
                      .arg(zone->name()).arg(zone->name().size()).arg(features().maxZoneNameLength)));
    if (zone->count() > features().maxChannelsInZone)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of channels %2 in zone '%1' exceeds limit %3.")
                      .arg(zone->name()).arg(zone->count()).arg(features().maxChannelsInZone)));
  }

  // Check scan lists
  if (config->scanlists()->count() > features().maxScanlists)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of scanlists %1 exceeds limit %2")
                    .arg(config->scanlists()->count()).arg(features().maxScanlists)));
  for (int i=0; i<config->scanlists()->count(); i++) {
    QSet<QString> names;
    ScanList *list = config->scanlists()->scanlist(i);
    if (names.contains(list->name()))
      issues.append(VerifyIssue(VerifyIssue::ERROR,tr("Duplicate scan list name '%1'.").arg(list->name())));
    names.insert(list->name());
    if (list->name().size() > features().maxScanlistNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Scan list name '%1' length %2 exceeds limit of %3 characters.")
                      .arg(list->name()).arg(list->name().size()).arg(features().maxScanlistNameLength)));
    if (0 == list->priorityChannel())
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Scan list '%1' does not contain a priority channel.")
                      .arg(list->name())));
    else if (! list->contains(list->priorityChannel()))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Scan list '%1' does not contain priority channel '%2'.")
                      .arg(list->name()).arg(list->priorityChannel()->name())));
  }

  return 0 == issues.size();
}


Radio *
Radio::detect() {
  QString id;

  // Try TYT MD Family
  DFUDevice dfu(0x0483, 0xdf11);
  if (dfu.isOpen()) {
    id = dfu.identifier();
  } else {
    // Try Radioddity/Baofeng RD-5R
    HID hid(0x15a2, 0x0073);
    if (hid.isOpen()) {
      id = hid.identify();
      hid.close();
    } else {
      qDebug() << "No radio found.";
      return nullptr;
    }
  }

  if (id.isEmpty()) {
    qDebug() << "Readio returned no idetifier!";
    return nullptr;
  }

  qDebug() << "Found Radio:" << id;

  if ("BF-5R" == id) {
    return new RD5R();
  } else if ("MD-UV390") {
    return new UV390();
  }

  return nullptr;
}

Radio::Status
Radio::status() const {
  return _task;
}

const QString &
Radio::errorMessage() const {
  return _errorMessage;
}

void
Radio::clearError() {
  if (StatusError == _task) {
    _task = StatusIdle;
    _errorMessage.clear();
  }
}
