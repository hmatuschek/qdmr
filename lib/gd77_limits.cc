#include "gd77_limits.hh"
#include "gd77_codeplug.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "zone.hh"
#include "scanlist.hh"
#include "roamingzone.hh"


GD77Limits::GD77Limits(QObject *parent)
  : RadioLimits(true, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = true;
  _callSignDBImplemented  = true;
  _numCallSignDBEntries   = 10920;

  // Define limits for satellite config
  _hasSatelliteConfig          = false;
  _satelliteConfigImplemented  = false;
  _numSatellites               = 0;

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
            RXGroupList::staticMetaObject, 1, 76, new RadioLimitObject {
              { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII) },
              { "contacts", new RadioLimitGroupCallRefList(1, 32) }
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

  /* Ignore scan lists. */
  add("scanlists", new RadioLimitList(
        ScanList::staticMetaObject, 0, 64, new RadioLimitObject{
          { "name", new RadioLimitString(1, 15, RadioLimitString::ASCII) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, false) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "channels", new RadioLimitRefList(0, 32, Channel::staticMetaObject) }
        }));

  /* Check encryption keys. */
  add("commercial", new RadioLimitItem {
        {"encryptionKeys", new RadioLimitList(
         BasicEncryptionKey::staticMetaObject,
         0, RadioddityCodeplug::EncryptionElement::Limit::keyCount(),
         new RadioLimitObject {
           {"name", new RadioLimitIgnored()},
           {"key", new RadioLimitStringRegEx("[0-9a-fA-F]{8}")}
         })}
      });

  /* Ignore positioning systems. */
  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );

  /* Ignore roaming zones. */
  add("roaming",
      new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored()) );
}
