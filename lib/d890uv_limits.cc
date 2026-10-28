#include "d890uv_limits.hh"
#include "config.hh"
#include "channel.hh"
#include "contact.hh"
#include "radioid.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"


D890UVLimits::D890UVLimits(const std::initializer_list<std::pair<Frequency, Frequency> > &rxFreqRanges,
                           const std::initializer_list<std::pair<Frequency, Frequency> > &txFreqRanges,
                           const QString &hardwareRevision, QObject *parent)
  : AnytoneLimits(hardwareRevision, "V100", true, parent)
{
  // None of the general settings are written.
  add("settings", new RadioLimitIgnored(RadioLimitIssue::Hint));

  add("radioIDs",
      new RadioLimitList {
        { DMRRadioID::staticMetaObject, 1, 250, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
            { "number", new RadioLimitDMRId(RadioLimitIssue::Severity::Critical) }
          } }
      });

  add("contacts",
      new RadioLimitList{
        { DMRContact::staticMetaObject, 1, 10000, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
            { "ring", new RadioLimitIgnoredBool() },
            { "type", new RadioLimitEnum{
                (unsigned)DMRContact::PrivateCall,
                (unsigned)DMRContact::GroupCall,
                (unsigned)DMRContact::AllCall
              }},
            { "number", new RadioLimitDMRId(RadioLimitIssue::Severity::Hint) },
            { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }
          } }
      });

  add("groupLists",
      new RadioLimitList(
        RXGroupList::staticMetaObject, 0, 250, new RadioLimitObject {
          { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
          { "contacts", new RadioLimitGroupCallRefList(1, 64) }
        }));

  add("channels",
      new RadioLimitList(
        Channel::staticMetaObject, 1, 4000,
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies(rxFreqRanges, RadioLimitIssue::Severity::Critical)},
              {"txFrequency", new RadioLimitTransmitFrequencies(txFreqRanges)},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::Mid),
                                           unsigned(Channel::Power::High), unsigned(Channel::Power::Max)}},
              {"timeout", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"scanListRef", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum{
                 (unsigned)FMChannel::Admit::Always,
                 (unsigned)FMChannel::Admit::Free,
                 (unsigned)FMChannel::Admit::Tone
               } },
              {"squelch", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"bandwidth", new RadioLimitEnum{
                 (unsigned)FMChannel::Bandwidth::Narrow,
                 (unsigned)FMChannel::Bandwidth::Wide
               }},
              {"aprs", new RadioLimitIgnored(RadioLimitIssue::Warning)},
              {"extended", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"anytone", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies(rxFreqRanges, RadioLimitIssue::Severity::Critical)},
              {"txFrequency", new RadioLimitTransmitFrequencies(txFreqRanges)},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::Mid),
                                           unsigned(Channel::Power::High), unsigned(Channel::Power::Max)}},
              {"timeout", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"scanListRef", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum {
                 unsigned(DMRChannel::Admit::Always),
                 unsigned(DMRChannel::Admit::Free),
                 unsigned(DMRChannel::Admit::ColorCode) } },
              {"colorCode", new RadioLimitUInt(0, 15)},
              {"timeSlot", new RadioLimitEnum {
                 unsigned(DMRChannel::TimeSlot::TS1),
                 unsigned(DMRChannel::TimeSlot::TS2) } },
              {"radioId", new RadioLimitObjRef(RadioID::staticMetaObject, true)},
              {"groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, true)},
              {"contact", new RadioLimitObjRef(DMRContact::staticMetaObject, true)},
              {"aprs", new RadioLimitIgnored(RadioLimitIssue::Warning)},
              {"roaming", new RadioLimitIgnored(RadioLimitIssue::Warning)},
              {"extended", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"anytone", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"commercial", new RadioLimitIgnored(RadioLimitIssue::Warning)}
            } }
        } ));

  add("zones",
      new RadioLimitList(
        Zone::staticMetaObject, 1, 250, new RadioLimitSingleZone(
          160, {
            { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
            { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }
          })
        ));

  add("scanLists",
      new RadioLimitList(
        ScanList::staticMetaObject, 0, 100, new RadioLimitObject {
          { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitIgnored(RadioLimitIssue::Warning) },
          { "channels", new RadioLimitRefList(0, 50, Channel::staticMetaObject) }
        }));
}

bool
D890UVLimits::verifyConfig(const Config *config, RadioLimitContext &context) const {
  if (! AnytoneLimits::verifyConfig(config, context))
    return false;

  // Parts of the config that are not written at all.
  struct { int count; const char *what; } unsupported[] = {
    { config->posSystems()->count(), "positioning/APRS systems" },
    { config->roamingChannels()->count(), "roaming channels" },
    { config->roamingZones()->count(), "roaming zones" },
    { config->smsExtension()->smsTemplates()->count(), "SMS templates" }
  };
  for (auto &u : unsupported) {
    if (u.count) {
      auto &msg = context.newMessage(RadioLimitIssue::Warning);
      msg << "The AT-D890UV support does not write " << u.what << " yet, they are ignored.";
    }
  }

  return true;
}
