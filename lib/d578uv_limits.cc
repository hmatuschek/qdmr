#include "d578uv_limits.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "gpssystem.hh"
#include "roamingzone.hh"


D578UVLimits::D578UVLimits(const std::initializer_list<std::pair<Frequency, Frequency> > &rxFreqRanges,
                           const std::initializer_list<std::pair<Frequency, Frequency> > &txFreqRanges,
                           const QString &hardwareRevision, QObject *parent)
  : AnytoneLimits(hardwareRevision, "V110", true, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = true;
  _callSignDBImplemented  = true;
  _numCallSignDBEntries   = 500000;

  /* Define limits for the general settings. */
  add("settings",
      new RadioLimitItem{
        { "introLine1", new RadioLimitString(-1, 14, RadioLimitString::ASCII) },
        { "introLine2", new RadioLimitString(-1, 14, RadioLimitString::ASCII) },
        { "micLevel", new RadioLimitUInt(1, 10) },
        { "speech", new RadioLimitIgnoredBool() },
        { "power", new RadioLimitEnum({unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}) },
        { "squlech", new RadioLimitUInt(0, 10) },
        { "vox", new RadioLimitUInt(0, 10) },
        { "tot", new RadioLimitUInt(0, -1) }
      });

  /* Define limits for radio IDs. */
  add("radioIDs",
      new RadioLimitList {
        { DMRRadioID::staticMetaObject, 1, 250, new RadioLimitObject {
            {"name", new RadioLimitString(1,8, RadioLimitString::ASCII) },
            {"id", new RadioLimitUInt(0, 16777215)}
          } }
      });

  /* Define limits for contacts. */
  add("contacts",
      new RadioLimitList{
        { DMRContact::staticMetaObject, 1, 10000, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum{
                (unsigned)DMRContact::PrivateCall,
                    (unsigned)DMRContact::GroupCall,
                    (unsigned)DMRContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } },
        { DTMFContact::staticMetaObject, -1, -1, new RadioLimitIgnored() }
      });

  /* Define limits for group lists. */
  add("groupLists",
      new RadioLimitList(
            RXGroupList::staticMetaObject, 1, 250, new RadioLimitObject {
              { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
              { "contacts", new RadioLimitGroupCallRefList(1, 64) }
        }));

  /* Define limits for channel list. */
  add("channels",
      new RadioLimitList(
        Channel::staticMetaObject, 1, 4000,
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies(rxFreqRanges, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies(txFreqRanges)},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, -1, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum{
                 (unsigned)FMChannel::Admit::Always,
                 (unsigned)FMChannel::Admit::Free,
                 (unsigned)FMChannel::Admit::Tone
               } },
              {"squelch", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"bandwidth", new RadioLimitEnum{
                 (unsigned)FMChannel::Bandwidth::Narrow,
                 (unsigned)FMChannel::Bandwidth::Wide
               }},
              {"aprs", new RadioLimitObjRef(APRSSystem::staticMetaObject)},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1,16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies(rxFreqRanges, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies(txFreqRanges)},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, -1, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum {
                 unsigned(DMRChannel::Admit::Always),
                 unsigned(DMRChannel::Admit::Free),
                 unsigned(DMRChannel::Admit::ColorCode) } },
              {"colorCode", new RadioLimitUInt(0,16)},
              {"timeSlot", new RadioLimitEnum {
                 unsigned(DMRChannel::TimeSlot::TS1),
                 unsigned(DMRChannel::TimeSlot::TS2) } },
              {"radioID", new RadioLimitObjRef(RadioID::staticMetaObject, true)},
              {"groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, false)},
              {"contact", new RadioLimitObjRef(DMRContact::staticMetaObject, false)},
              {"aprs", new RadioLimitObjRef(PositioningSystem::staticMetaObject, true)},
              {"roaming", new RadioLimitObjRef(RoamingZone::staticMetaObject, true) },
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ));

  /* Define limits for zone list. */
  add("zones",
      new RadioLimitList(
        Zone::staticMetaObject, 1, 250, new RadioLimitSingleZone(
          250, {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) }, // 16 ASCII chars in name
            { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
          })
        ) );

  /* Define limits for scan lists. */
  add("scanlists",
      new RadioLimitList(
        ScanList::staticMetaObject, 1, 250, new RadioLimitObject{
          { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, false) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "channels", new RadioLimitRefList(0, 31, Channel::staticMetaObject) }
        }));

  /* Handle positioning systems. */
  add("positioning", new RadioLimitList{
        { GPSSystem::staticMetaObject, 0, 8, new RadioLimitObject {
            { "name", new RadioLimitStringIgnored() },
            { "period", new RadioLimitUInt(0, 7650) },
            { "contact", new RadioLimitObjRef(DMRContact::staticMetaObject, false) },
            { "revert", new RadioLimitObjRef(DMRChannel::staticMetaObject, true) } } },
        { APRSSystem::staticMetaObject, 0, 1, new RadioLimitObject {
            { "name", new RadioLimitStringIgnored() },
            { "period", new RadioLimitUInt(0, 7650) },
            { "revert", new RadioLimitObjRef(FMChannel::staticMetaObject, false) },
            { "icon", new RadioLimitEnum{} },
            { "message", new RadioLimitString(0, 60, RadioLimitString::ASCII) }
            ///@todo extend APRSSystem to expose other settings as properties.
          }} } );

  /* Handle roaming zones. */
  add("roaming",
      new RadioLimitList(RoamingZone::staticMetaObject, 0, 64,
                         new RadioLimitObject {
                           { "name", new RadioLimitStringIgnored() },
                           { "channels", new RadioLimitRefList(0, 64, DMRChannel::staticMetaObject) }
                         } ) );
}
