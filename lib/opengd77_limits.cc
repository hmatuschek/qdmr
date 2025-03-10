#include "opengd77_limits.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "roamingzone.hh"
#include "opengd77_satelliteconfig.hh"



OpenGD77Limits::OpenGD77Limits(QObject *parent)
  : RadioLimits(false, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = true;
  _callSignDBImplemented  = true;
  _numCallSignDBEntries   = 15796;

  // Define limits for satellite config
  _hasSatelliteConfig          = true;
  _satelliteConfigImplemented  = true;
  _numSatellites               = OpenGD77SatelliteConfig::Limit::satellites();

  /* Define limits for the general settings. */
  add("settings",
      new RadioLimitItem{
        { "introLine1", new RadioLimitString(-1, 16, RadioLimitString::ASCII) },
        { "introLine2", new RadioLimitString(-1, 16, RadioLimitString::ASCII) },
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
        { DMRRadioID::staticMetaObject, 1, 1, new RadioLimitObject {
            {"name", new RadioLimitString(1,8, RadioLimitString::ASCII) },
            {"id", new RadioLimitUInt(0, 16777215)}
          } }
        /// @todo check default radio ID.
      });

  /* Define limits for contacts. */
  add("contacts",
      new RadioLimitList{
        { DMRContact::staticMetaObject, 1, 1024, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum{
                (unsigned)DMRContact::PrivateCall,
                (unsigned)DMRContact::GroupCall,
                (unsigned)DMRContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } },
        { DTMFContact::staticMetaObject, 0, 32, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "number", new RadioLimitStringRegEx("^[0-9A-Fa-f]+$") }
          } }
      });

  /* Define limits for group lists. */
  add("groupLists",
      new RadioLimitList(
            RXGroupList::staticMetaObject, 0, 76, new RadioLimitObject {
              { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
              // allow for any digital contact reference
              { "contacts", new RadioLimitRefList(1, 32, DMRContact::staticMetaObject) }
        }));

  /* Define limits for channel list. */
  add("channels",
      new RadioLimitList(
        Channel::staticMetaObject, 1, 1024, // < up to 1024 channels
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                         {Frequency::fromMHz(400.), Frequency::fromMHz(470.)}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                                 {Frequency::fromMHz(400.), Frequency::fromMHz(470.)}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, 3825, std::numeric_limits<unsigned>::max())},
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
              {"aprs", new RadioLimitObjRefIgnored()},
              /// @todo handle OpenGD77 extension
              {"openGD77", new RadioLimitIgnored()},
              {"tyt", new RadioLimitIgnored()}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1,16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                         {Frequency::fromMHz(400.), Frequency::fromMHz(470.)}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                                 {Frequency::fromMHz(400.), Frequency::fromMHz(470.)}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, 3825, std::numeric_limits<unsigned>::max())},
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
              {"contact", new RadioLimitObjRef(DMRContact::staticMetaObject, true)},
              {"aprs", new RadioLimitObjRefIgnored()},
              {"roaming", new RadioLimitObjRefIgnored(DefaultRoamingZone::get())},
              /// @todo handle OpenGD77 extension
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ));

  /* Define limits for zone list. */
  add("zones",
      new RadioLimitList(
        Zone::staticMetaObject, 0, 68, new RadioLimitSingleZone(
          80, {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) }, // 16 ASCII chars in name
            { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
          })
        ) );

  /* Ignore scan lists. */
  add("scanlists", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );

  /* Ignore positioning systems. */
  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );

  /* Ignore roaming zones. */
  add("roaming",
      new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );
}
