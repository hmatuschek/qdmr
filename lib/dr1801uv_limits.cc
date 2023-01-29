#include "dr1801uv_limits.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"
#include "gpssystem.hh"
#include "roamingzone.hh"
#include "dr1801uv_codeplug.hh"

DR1801UVLimits::DR1801UVLimits(QObject *parent)
  : RadioLimits(true, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = false;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 0;

  add("settings", new RadioLimitItem {
        { "introLine1", new RadioLimitString(
          -1, DR1801UVCodeplug::SettingsElement::Limit::bootLineLength(), RadioLimitString::ASCII) },
        { "introLine2", new RadioLimitString(
          -1, DR1801UVCodeplug::SettingsElement::Limit::bootLineLength(), RadioLimitString::ASCII) },
        { "micLevel", new RadioLimitUInt(1, 10) },
        { "speech", new RadioLimitIgnoredBool() },
        { "power", new RadioLimitEnum {
            unsigned(Channel::Power::Low),
            unsigned(Channel::Power::High) } },
        { "squlech", new RadioLimitUInt(0, 10) },
        { "vox", new RadioLimitUInt(0, 10) },
        { "tot", new RadioLimitUInt(0, -1) }
        /// @todo check default radio ID.
      } );

  /* Define limits for radio IDs. */
  add("radioIDs", new RadioLimitList{
        { DMRRadioID::staticMetaObject, 1, 1, new RadioLimitObject {
            {"name", new RadioLimitString(
             1, DR1801UVCodeplug::SettingsElement::Limit::radioNameLength(), RadioLimitString::ASCII) },
            {"id", new RadioLimitUInt(0, 16777215)}
          } }
      } );

  /* Define limits for contacts. */
  add("contacts", new RadioLimitList{
        { DMRContact::staticMetaObject, 1, DR1801UVCodeplug::ContactBankElement::Limit::contactCount(),
          new RadioLimitObject {
            { "name", new RadioLimitString(
              1, DR1801UVCodeplug::ContactElement::Limit::nameLength(), RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum {
                (unsigned) DMRContact::PrivateCall,
                (unsigned) DMRContact::GroupCall,
                (unsigned) DMRContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } },
        { DTMFContact::staticMetaObject, -1, -1, new RadioLimitIgnored() }
      } );

  /* Define limits for group lists. */
  add("groupLists", new RadioLimitList(
        RXGroupList::staticMetaObject, 1, DR1801UVCodeplug::GroupListBankElement::Limit::groupListCount(),
        new RadioLimitObject {
          { "name", new RadioLimitString(1, -1, RadioLimitString::ASCII) }, // Name is not encoded.
          { "contacts", new RadioLimitGroupCallRefList(1, 32) }
        }) );

  /* Define limits for channel list. */
  add("channels", new RadioLimitList(
        Channel::staticMetaObject, 1, DR1801UVCodeplug::ChannelBankElement::Limit::channelCount(),
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(
               1, DR1801UVCodeplug::ChannelBankElement::Limit::channelNameLength(),
               RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{136., 174.}, {400., 480.}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, -1, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)},
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
              {"aprs", new RadioLimitObjRefIgnored(nullptr, RadioLimitIssue::Hint)}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(
               1, DR1801UVCodeplug::ChannelBankElement::Limit::channelNameLength(),
               RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{136., 174.}, {400., 480.}})},
              {"power", new RadioLimitEnum {
                 unsigned(Channel::Power::Low),
                 unsigned(Channel::Power::High),
               }},
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
              {"groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, true)},
              {"contact", new RadioLimitObjRef(DMRContact::staticMetaObject, true)},
              {"aprs", new RadioLimitObjRefIgnored()},
              {"roaming", new RadioLimitObjRefIgnored(DefaultRoamingZone::get())},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ) );

  /* Define limits for zone list. */
  add("zones", new RadioLimitList(
        Zone::staticMetaObject, 1, DR1801UVCodeplug::ZoneBankElement::Limit::zoneCount(), new RadioLimitObject {
          { "name", new RadioLimitString(
            1, DR1801UVCodeplug::ZoneElement::Limit::nameLength(), RadioLimitString::Unicode) },
          { "A", new RadioLimitRefList(
            0, DR1801UVCodeplug::ZoneElement::Limit::memberCount(), Channel::staticMetaObject) },
          { "B", new RadioLimitRefList(0, 0, Channel::staticMetaObject) },
          { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
        } ) );

  /* Define limits for scan lists. */
  add("scanlists", new RadioLimitList(
        ScanList::staticMetaObject, 1, DR1801UVCodeplug::ScanListBankElement::Limit::scanListCount(),
        new RadioLimitObject{
          { "name", new RadioLimitString(
            1, DR1801UVCodeplug::ScanListElement::Limit::nameLength(), RadioLimitString::Unicode) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "channels", new RadioLimitRefList(
            0, DR1801UVCodeplug::ScanListBankElement::Limit::scanListCount(), Channel::staticMetaObject) }
        }) );

  /* Define limits for positioning systems. */
  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );

  /* Ignore roaming zones. */
  add("roaming", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );
}
