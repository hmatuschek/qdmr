#include "rd5r_limits.hh"
#include "radioid.hh"
#include "channel.hh"
#include "scanlist.hh"
#include "zone.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "roaming.hh"


RD5RLimits::RD5RLimits(QObject *parent)
  : RadioLimits(false, parent)
{  
  // Define limits for call-sign DB
  _hasCallSignDB          = false;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 0;

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
        { "tot", new RadioLimitUInt(0, 3825) }
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
        { DigitalContact::staticMetaObject, 1, 256, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum{
                (unsigned)DigitalContact::PrivateCall,
                    (unsigned)DigitalContact::GroupCall,
                    (unsigned)DigitalContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } },
        { DTMFContact::staticMetaObject, 0, 8, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "number", new RadioLimitStringRegEx("^[0-9A-Fa-f]+$") }
          } }
      });

  /* Define limits for group lists. */
  add("groupLists",
      new RadioLimitList(
            RXGroupList::staticMetaObject, 1, 64, new RadioLimitObject {
              { "name", new RadioLimitString(1,16, RadioLimitString::ASCII) },
              { "contacts", new RadioLimitGroupCallRefList(1,16) }
        }));

  /* Define limits for channel list. */
  add("channels",
      new RadioLimitList(
        Channel::staticMetaObject, 1, 1024, // < up to 1024 channels
        new RadioLimitObjects {
          { AnalogChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}}, true)},
              {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, 3825, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"admit", new RadioLimitEnum{
                 (unsigned)AnalogChannel::Admit::Always,
                 (unsigned)AnalogChannel::Admit::Free,
                 (unsigned)AnalogChannel::Admit::Tone
               } },
              {"squelch", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"bandwidth", new RadioLimitEnum{
                 (unsigned)AnalogChannel::Bandwidth::Narrow,
                 (unsigned)AnalogChannel::Bandwidth::Wide
               }},
              {"aprs", new RadioLimitObjRefIgnored()}
            } },
          { DigitalChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1,16, RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}}, true)},
              {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, 3825, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum {
                 unsigned(DigitalChannel::Admit::Always),
                 unsigned(DigitalChannel::Admit::Free),
                 unsigned(DigitalChannel::Admit::ColorCode) } },
              {"colorCode", new RadioLimitUInt(0,16)},
              {"timeSlot", new RadioLimitEnum {
                 unsigned(DigitalChannel::TimeSlot::TS1),
                 unsigned(DigitalChannel::TimeSlot::TS2) } },
              {"radioID", new RadioLimitObjRef(RadioID::staticMetaObject, true)},
              {"groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, false)},
              {"contact", new RadioLimitObjRef(DigitalContact::staticMetaObject, true)},
              {"aprs", new RadioLimitObjRefIgnored() },
              {"roaming", new RadioLimitObjRefIgnored(DefaultRoamingZone::get()) },
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ));

  /* Define limits for zone list. */
  add("zones",
      new RadioLimitList(
        Zone::staticMetaObject, 1, 250, new RadioLimitSingleZone(
          16, {
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

  /* Ignore positioning systems. */
  add("positioning",
      new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );

  /* Ignore roaming zones. */
  add("roaming",
      new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );
}
