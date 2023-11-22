#include "gd73_limits.hh"
#include "gd73_codeplug.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "roamingzone.hh"


GD73Limits::GD73Limits(QObject *parent)
  : RadioLimits(true, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = false;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 0;

  /* Define limits for the general settings. */
  add("settings",
      new RadioLimitItem{
        { "introLine1", new RadioLimitString(-1, GD73Codeplug::SettingsElement::Limit::bootTextLine(), RadioLimitString::Unicode) },
        { "introLine2", new RadioLimitString(-1, GD73Codeplug::SettingsElement::Limit::bootTextLine(), RadioLimitString::Unicode) },
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
            {"name", new RadioLimitString(1, GD73Codeplug::SettingsElement::Limit::name(), RadioLimitString::Unicode) },
            {"id", new RadioLimitUInt(0, 16777215)}
          } }
        /// @todo check default radio ID.
      });

  /* Define limits for contacts. */
  add("contacts",
      new RadioLimitList{
        { DMRContact::staticMetaObject, 1, GD73Codeplug::ContactBankElement::Limit::contactCount(), new RadioLimitObject {
            { "name", new RadioLimitString(1, GD73Codeplug::ContactElement::Limit::nameLength(), RadioLimitString::Unicode) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum{
                (unsigned)DMRContact::PrivateCall,
                (unsigned)DMRContact::GroupCall,
                (unsigned)DMRContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } }
      });

  /* Define limits for group lists. */
  add("groupLists",
      new RadioLimitList(
            RXGroupList::staticMetaObject, 1, GD73Codeplug::GroupListBankElement::Limit::memberCount(), new RadioLimitObject {
              { "name", new RadioLimitString(1, GD73Codeplug::GroupListElement::Limit::nameLength(), RadioLimitString::Unicode) },
              { "contacts", new RadioLimitGroupCallRefList(1, GD73Codeplug::GroupListElement::Limit::memberCount()) }
        }));

  /* Define limits for channel list. */
  add("channels",
      new RadioLimitList(
        Channel::staticMetaObject, 1, GD73Codeplug::ChannelBankElement::Limit::channelCount(),
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, GD73Codeplug::ChannelElement::Limit::nameLength(), RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(400.), Frequency::fromMHz(470.)}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(400.), Frequency::fromMHz(470.)}})},
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
              {"openGD77", new RadioLimitIgnored()},
              {"tyt", new RadioLimitIgnored()}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, GD73Codeplug::ChannelElement::Limit::nameLength(), RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(400.), Frequency::fromMHz(470.)}}, true)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(400.), Frequency::fromMHz(470.)}})},
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
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ));

  /* Define limits for zone list. */
  add("zones",
      new RadioLimitList(
        Zone::staticMetaObject, 1, GD73Codeplug::ZoneBankElement::Limit::zoneCount(), new RadioLimitSingleZone(
          GD73Codeplug::ZoneElement::Limit::channelCount(), {
            { "name", new RadioLimitString(1, GD73Codeplug::ZoneElement::Limit::nameLength(), RadioLimitString::Unicode) },
            { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
          })
        ) );

  /* Ignore scan lists. */
  add("scanlists", new RadioLimitList(
        ScanList::staticMetaObject, 1, GD73Codeplug::ScanListBankElement::Limit::memberCount(), new RadioLimitObject{
          { "name", new RadioLimitString(1, GD73Codeplug::ScanListElement::Limit::nameLength(), RadioLimitString::Unicode) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, false) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "channels", new RadioLimitRefList(0, GD73Codeplug::ScanListElement::Limit::memberCount(), Channel::staticMetaObject) }
        }));

  /* Ignore positioning systems. */
  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );

  /* Ignore roaming zones. */
  add("roaming",
      new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );
}
