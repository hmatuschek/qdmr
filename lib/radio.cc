#include "radio.hh"

#include "anytone_interface.hh"
#include "radioddity_interface.hh"
#include "tyt_interface.hh"

#include "rd5r.hh"
#include "gd77.hh"
#include "md390.hh"
#include "uv390.hh"
#include "md2017.hh"
#include "dm1701.hh"
#include "opengd77.hh"
#include "d868uv.hh"
#include "d878uv.hh"
#include "d878uv2.hh"
#include "d578uv.hh"

#include "config.hh"
#include "logger.hh"

#include <QSet>


/* ******************************************************************************************** *
 * Implementation of Radio::Featrues::FrequencyRange
 * ******************************************************************************************** */
Radio::Features::FrequencyRange::FrequencyRange(double lower, double upper)
  : min(lower), max(upper)
{
  // pass..
}

Radio::Features::FrequencyRange::FrequencyRange(double limits[2])
  : min(limits[0]), max(limits[1])
{
  // pass..
}

bool
Radio::Features::FrequencyRange::contains(double f) const {
  return (min<=f) && (max>=f);
}


/* ******************************************************************************************** *
 * Implementation of Radio::Featrues::FrequencyLimits
 * ******************************************************************************************** */
Radio::Features::FrequencyLimits::FrequencyLimits(const QVector<FrequencyRange> &frequency_ranges)
  : ranges(frequency_ranges)
{
  // pass...
}

bool
Radio::Features::FrequencyLimits::contains(double f) const {
  for (int i=0; i<ranges.size(); i++) {
    if (ranges[i].contains(f))
      return true;
  }
  return false;
}


/* ******************************************************************************************** *
 * Implementation of Radio
 * ******************************************************************************************** */
Radio::Radio(QObject *parent)
  : QThread(parent), _task(StatusIdle)
{
  // pass...
}

Radio::~Radio() {
  // pass...
}

const CallsignDB *
Radio::callsignDB() const {
  return nullptr;
}

CallsignDB *
Radio::callsignDB() {
  return nullptr;
}

VerifyIssue::Type
Radio::verifyConfig(Config *config, QList<VerifyIssue> &issues, const VerifyFlags &flags)
{
  // Is still beta?
  if (features().betaWarning)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Support for this radio is still new and not well tested. "
                       "The code-plug might be incomplete, non-functional or even harmful.")));

  // Check radio IDs
  if (config->radioIDs()->count() > features().maxRadioIDs) {
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("This radio only supports %1 DMR IDs, %2 are set.")
                    .arg(features().maxRadioIDs).arg(config->radioIDs()->count())));
  }

  /*
   *  Check general config
   */
  if ((nullptr == config->radioIDs()->defaultId()) && features().needsDefaultRadioID)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Radio needs a default radio ID but none is set.")));

  if (config->settings()->introLine1().size() > features().maxIntroLineLength)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Intro line 1 of length %1 exceeds limit of %2 characters.")
                    .arg(config->settings()->introLine1().size()).arg(features().maxIntroLineLength)));

  if (config->settings()->introLine2().size() > features().maxIntroLineLength)
    issues.append(VerifyIssue(
                    VerifyIssue::WARNING,
                    tr("Intro line 2 of length %1 exceeds limit of %2 characters.")
                    .arg(config->settings()->introLine2().size()).arg(features().maxIntroLineLength)));

  /*
   *  Check contact list
   */
  if (config->contacts()->count() > features().maxContacts)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of contacts %1 exceeds limit of %2.")
                    .arg(config->contacts()->count()).arg(features().maxContacts)));

  for (int i=0; i<config->contacts()->count(); i++) {
    QSet<QString> names;
    Contact *contact = config->contacts()->contact(i);

    if (names.contains(contact->name()))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Duplicate contact name '%1'.").arg(contact->name())));
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

  /*
   *  Check RX group lists.
   */
  if (config->rxGroupLists()->count() > features().maxGrouplists)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of Rx group lists %1 exceeds limit of %2")
                    .arg(config->rxGroupLists()->count()).arg(features().maxGrouplists)));

  for (int i=0; i<config->rxGroupLists()->count(); i++) {
    QSet<QString> names;
    RXGroupList *list = config->rxGroupLists()->list(i);

    if (names.contains(list->name()))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Duplicate Rx group list name '%1'.").arg(list->name())));
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

  /*
   * Check channel list
   */
  if (config->channelList()->count() > features().maxChannels)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of channels %1 exceeds limit %2.")
                      .arg(config->channelList()->count()).arg(features().maxChannels)));

  for (int i=0; i<config->channelList()->count(); i++) {
    QSet<QString> names;
    Channel *channel = config->channelList()->channel(i);

    if (names.contains(channel->name()))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Duplicate channel name '%1'.").arg(channel->name())));
    names.insert(channel->name());

    /*if (!features().frequencyLimits.contains(channel->rxFrequency())) {
      VerifyIssue::Type type = flags.ignoreFrequencyLimits ? VerifyIssue::WARNING : VerifyIssue::ERROR;
      issues.append(VerifyIssue(type,tr("RX frequency %1 of channel '%2' is out of range.")
                                .arg(channel->rxFrequency()).arg(channel->name())));
    }*/

    if (!features().frequencyLimits.contains(channel->txFrequency())) {
      VerifyIssue::Type type = flags.ignoreFrequencyLimits ? VerifyIssue::WARNING : VerifyIssue::ERROR;
      issues.append(VerifyIssue(type,tr("TX frequency %1 of channel '%2' is out of range.")
                                .arg(channel->txFrequency()).arg(channel->name())));
    }

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

    if (channel->is<DigitalChannel>() && (nullptr == channel->as<DigitalChannel>()->txContactObj())
        && (! features().allowChannelNoDefaultContact))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Radio requires TX contact for channel '%1'!")
                      .arg(channel->name())));
  }

  /*
   * Check zone list
   */
  if (config->zones()->count() > features().maxZones)
    issues.append(VerifyIssue(
                    VerifyIssue::ERROR,
                    tr("Number of zones %1 exceeds limit %2")
                    .arg(config->zones()->count()).arg(features().maxZones)));

  for (int i=0; i<config->zones()->count(); i++) {
    QSet<QString> names;
    Zone *zone = config->zones()->zone(i);

    if (names.contains(zone->name()))
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Duplicate zone name '%1'.").arg(zone->name())));
    names.insert(zone->name());

    if (zone->name().size()>features().maxZoneNameLength)
      issues.append(VerifyIssue(
                      VerifyIssue::WARNING,
                      tr("Zone name '%1' length %2 exceeds limit of %3 characters.")
                      .arg(zone->name()).arg(zone->name().size()).arg(features().maxZoneNameLength)));

    if (zone->A()->count() > features().maxChannelsInZone)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of channels %2 in zone '%1' A exceeds limit %3.")
                      .arg(zone->name()).arg(zone->A()->count()).arg(features().maxChannelsInZone)));

    if (zone->B()->count() > features().maxChannelsInZone)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of channels %2 in zone '%1' B exceeds limit %3.")
                      .arg(zone->name()).arg(zone->A()->count()).arg(features().maxChannelsInZone)));

    if ((0 < zone->B()->count()) && (! features().hasABZone)) {
      issues.append(VerifyIssue(
                      VerifyIssue::NOTIFICATION,
                      tr("Radio does not support dual-zones. Zone '%1' will be split into two.")
                      .arg(zone->name())));
      if ((zone->name().size()+2) > features().maxZoneNameLength) {
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Zone '%1' will be split into two but its name is too long to "
                           "differentiate the created zones (exceeding limit of %2 chars). "
                           "Consider using a shorter zone name.")
                        .arg(zone->name()).arg(features().maxZoneNameLength)));
      }
    }
  }

  /*
   * Check scan lists
   */
  if (features().hasScanlists) {
    if (config->scanlists()->count() > features().maxScanlists)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of scanlists %1 exceeds limit %2")
                      .arg(config->scanlists()->count()).arg(features().maxScanlists)));

    for (int i=0; i<config->scanlists()->count(); i++) {
      QSet<QString> names;
      ScanList *list = config->scanlists()->scanlist(i);

      if (names.contains(list->name()))
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Duplicate scan list name '%1'.").arg(list->name())));
      names.insert(list->name());

      if (list->name().size() > features().maxScanlistNameLength)
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Scan list name '%1' length %2 exceeds limit of %3 characters.")
                        .arg(list->name()).arg(list->name().size()).arg(features().maxScanlistNameLength)));

      if (0 == list->primaryChannel())
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Scan list '%1' has no priority channel set.")
                        .arg(list->name())));

      else if (! list->contains(list->primaryChannel()))
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Scan list '%1' does not contain priority channel '%2'.")
                        .arg(list->name()).arg(list->primaryChannel()->name())));
    }
  }

  /*
   * Check roaming zones and channels
   */
  if (features().hasRoaming) {
    // Check total numer of unique roaming channels
    QSet<DigitalChannel *> roamingChannels; config->roaming()->uniqueChannels(roamingChannels);
    if (roamingChannels.size() > features().maxRoamingChannels)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Total number of roaming channels %1 exceeds limit %2")
                      .arg(roamingChannels.size()).arg(features().maxRoamingChannels)));
    // Check number of roaming zones
    if (config->roaming()->count() > features().maxRoamingZones)
      issues.append(VerifyIssue(
                      VerifyIssue::ERROR,
                      tr("Number of roaming zones %1 exceeds limit %2")
                      .arg(config->roaming()->count()).arg(features().maxRoamingZones)));
    for (int i=0; i<config->roaming()->count(); i++) {
      RoamingZone *zone = config->roaming()->zone(i);
      if (zone->count() > features().maxChannelsInRoamingZone)
        issues.append(VerifyIssue(
                        VerifyIssue::WARNING,
                        tr("Number of channels (%1) in roaming zone '%2' exceeds limit %3")
                        .arg(roamingChannels.size()).arg(zone->name()).arg(features().maxRoamingChannels)));
    }
  }

  // Get max issue severity
  VerifyIssue::Type maxType = VerifyIssue::NONE;
  foreach (VerifyIssue issue, issues) {
    if (issue.type() > maxType)
      maxType = issue.type();
  }
  return maxType;
}


Radio *
Radio::detect(const ErrorStack &err) {
  return Radio::detect(RadioInfo(), err);
}

Radio *
Radio::detect(const RadioInfo &force, const ErrorStack &err) {
  RadioInfo id;

  // Try TYT UV Family
  {
    TyTInterface *dfu = new TyTInterface(err);
    if (dfu->isOpen()) {
      id = dfu->identifier();
      if ((id.isValid() && (RadioInfo::MD390 == id.id())) || (force.isValid() && (RadioInfo::MD390 == force.id()))) {
        return new MD390(dfu);
      } else if ((id.isValid() && (RadioInfo::UV390 == id.id())) || (force.isValid() && (RadioInfo::UV390 == force.id()))) {
        return new UV390(dfu);
      } else if ((id.isValid() && (RadioInfo::MD2017 == id.id())) || (force.isValid() && (RadioInfo::MD2017 == force.id()))) {
        return new MD2017(dfu);
      } else if ((id.isValid() && (RadioInfo::DM1701 == id.id())) || (force.isValid() && (RadioInfo::DM1701 == force.id()))) {
        return new DM1701(dfu);
      } else {
        dfu->close();
        dfu->deleteLater();
        return nullptr;
      }
    } else {
      dfu->deleteLater();
    }
  }

  // Try Radioddity/Baofeng RD5R & GD-77
  {
    RadioddityInterface *hid = new RadioddityInterface(err);
    if (hid->isOpen()) {
      id = hid->identifier();
      if ((id.isValid() && (RadioInfo::RD5R == id.id())) || (force.isValid() && (RadioInfo::RD5R == force.id()))) {
        return new RD5R(hid);
      } else if ((id.isValid() && (RadioInfo::GD77 == id.id())) || (force.isValid() && (RadioInfo::GD77 == force.id()))) {
        return new GD77(hid);
      } else {
        hid->close();
        hid->deleteLater();
        return nullptr;
      }
    } else {
      hid->deleteLater();
    }
  }

  // Try Open GD77 firmware
  {
    OpenGD77Interface *ogd77 = new OpenGD77Interface(err);
    if (ogd77->isOpen()) {
      id = ogd77->identifier();
      if ((id.isValid() && (RadioInfo::OpenGD77 == id.id())) || (force.isValid() && (RadioInfo::OpenGD77 == force.id()))) {
        return new OpenGD77(ogd77);
      } else {
        ogd77->close();
        ogd77->deleteLater();
        return nullptr;
      }
    } else {
      ogd77->deleteLater();
    }
  }

  // Try Anytone USB-serial devices
  {
    AnytoneInterface *anytone = new AnytoneInterface(err);
    if (anytone->isOpen()) {
      id = anytone->identifier();
      if ((id.isValid() && (RadioInfo::D868UVE == id.id())) || (force.isValid() && (RadioInfo::D868UVE == force.id()))) {
        return new D868UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UV == id.id())) || (force.isValid() && (RadioInfo::D878UV == force.id()))) {
        return new D878UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UVII == id.id())) || (force.isValid() && (RadioInfo::D878UVII == force.id()))) {
        return new D878UV2(anytone);
      } else if ((id.isValid() && (RadioInfo::D578UV == id.id())) || (force.isValid() && (RadioInfo::D578UV == force.id()))) {
        return new D578UV(anytone);
      } else {
        anytone->close();
        anytone->deleteLater();
        return nullptr;
      }
    } else {
      anytone->deleteLater();
    }
  }

  errMsg(err) << "Cannot connect to radio.";
  return nullptr;
}


Radio *
Radio::detect(const RadioInterface::Descriptor &descr, const RadioInfo &force, const ErrorStack &err) {
  if (! descr.isValid()) {
    errMsg(err) << "Cannot detect radio: Invalid interface descriptor.";
  }
  logDebug() << "Try to detect radio at " << descr.description() << ".";

  if (AnytoneInterface::interfaceInfo() == descr) {
    AnytoneInterface *anytone = new AnytoneInterface(descr, err);
    if (anytone->isOpen()) {
      RadioInfo id = anytone->identifier();
      if ((id.isValid() && (RadioInfo::D868UVE == id.id())) || (force.isValid() && (RadioInfo::D868UVE == force.id()))) {
        return new D868UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UV == id.id())) || (force.isValid() && (RadioInfo::D878UV == force.id()))) {
        return new D878UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UVII == id.id())) || (force.isValid() && (RadioInfo::D878UVII == force.id()))) {
        return new D878UV2(anytone);
      } else if ((id.isValid() && (RadioInfo::D578UV == id.id())) || (force.isValid() && (RadioInfo::D578UV == force.id()))) {
        return new D578UV(anytone);
      }
      anytone->close();
      anytone->deleteLater();
      return nullptr;
    }
    anytone->deleteLater();
    return nullptr;
  }else if (OpenGD77Interface::interfaceInfo() == descr) {
    OpenGD77Interface *ogd77 = new OpenGD77Interface(descr, err);
    if (ogd77->isOpen()) {
      RadioInfo id = ogd77->identifier();
      if ((id.isValid() && (RadioInfo::OpenGD77 == id.id())) || (force.isValid() && (RadioInfo::OpenGD77 == force.id()))) {
        return new OpenGD77(ogd77);
      }
      ogd77->close();
      ogd77->deleteLater();
      return nullptr;
    }
    ogd77->deleteLater();
    return nullptr;
  } else if (TyTInterface::interfaceInfo() == descr) {
    TyTInterface *dfu = new TyTInterface(descr, err);
    if (dfu->isOpen()) {
      RadioInfo id = dfu->identifier();
      if ((id.isValid() && (RadioInfo::MD390 == id.id())) || (force.isValid() && (RadioInfo::MD390 == force.id()))) {
        return new MD390(dfu);
      } else if ((id.isValid() && (RadioInfo::UV390 == id.id())) || (force.isValid() && (RadioInfo::UV390 == force.id()))) {
        return new UV390(dfu);
      } else if ((id.isValid() && (RadioInfo::MD2017 == id.id())) || (force.isValid() && (RadioInfo::MD2017 == force.id()))) {
        return new MD2017(dfu);
      } else if ((id.isValid() && (RadioInfo::DM1701 == id.id())) || (force.isValid() && (RadioInfo::DM1701 == force.id()))) {
        return new DM1701(dfu);
      }
      dfu->close();
      dfu->deleteLater();
      return nullptr;
    }
    dfu->deleteLater();
    return nullptr;
  } else if (RadioddityInterface::interfaceInfo() == descr) {
    RadioddityInterface *hid = new RadioddityInterface(descr, err);
    if (hid->isOpen()) {
      RadioInfo id = hid->identifier();
      if ((id.isValid() && (RadioInfo::RD5R == id.id())) || (force.isValid() && (RadioInfo::RD5R == force.id()))) {
        return new RD5R(hid);
      } else if ((id.isValid() && (RadioInfo::GD77 == id.id())) || (force.isValid() && (RadioInfo::GD77 == force.id()))) {
        return new GD77(hid);
      } else {
        hid->close();
        hid->deleteLater();
        return nullptr;
      }
    }
    hid->deleteLater();
    return nullptr;
  }
  return nullptr;
}

Radio::Status
Radio::status() const {
  return _task;
}

const ErrorStack &
Radio::errorStack() const {
  return _errorStack;
}
