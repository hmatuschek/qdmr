#include "rt4d_limits.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"
#include "gpssystem.hh"
#include "roamingzone.hh"
#include "rt4d_codeplug.hh"

RT4DLimits::RT4DLimits(QObject *parent)
  : RadioLimits(true, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = true;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 0;

  // Define limits for satellite config
  _hasSatelliteConfig          = false;
  _satelliteConfigImplemented  = false;
  _numSatellites               = 0;

  add("settings", new RadioLimitItem {
        { "introLine1", new RadioLimitString(
          -1, RT4DCodeplug::FirstSettingsElement::Limit::bootTextLength(), RadioLimitString::ASCII) },
        { "introLine2", new RadioLimitIgnored() },
        { "micLevel", new RadioLimitLevel({1, 5}, false) },
        { "speech", new RadioLimitIgnoredBool() },
        { "power", new RadioLimitEnum {
            unsigned(Channel::Power::Low),
            unsigned(Channel::Power::High) } },
        { "squelch", new RadioLimitIgnored(RadioLimitIssue::Silent) },
        { "vox", new RadioLimitIgnored(RadioLimitIssue::Silent) },
        { "tot", new RadioLimitInterval() },
        { "boot", new RadioLimitItem {
            {"passwordEnabled", new RadioLimitIgnored(RadioLimitIssue::Silent) },
            {"password", new RadioLimitPin(RT4DCodeplug::FirstSettingsElement::Limit::bootPasswordLength(), RadioLimitIssue::Critical) } } }
        /// @todo check default radio ID.
      } );

  /* Define limits for radio IDs. */
  add("radioIDs", new RadioLimitList{
        { DMRRadioID::staticMetaObject, 1, 1, new RadioLimitObject {
            {"name", new RadioLimitString(
             1, RT4DCodeplug::FirstSettingsElement::Limit::radioNameLength(), RadioLimitString::ASCII) },
            {"number", new RadioLimitDMRId(RadioLimitIssue::Severity::Critical)}
          } }
      } );

  /* Define limits for contacts. */
  add("contacts", new RadioLimitList{
        { DMRContact::staticMetaObject, 1, RT4DCodeplug::ContactBankElement::Limit::contacts(),
          new RadioLimitObject {
            { "name", new RadioLimitString(
              1, RT4DCodeplug::ContactElement::Limit::name(), RadioLimitString::ASCII) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum {
                (unsigned) DMRContact::PrivateCall,
                (unsigned) DMRContact::GroupCall,
                (unsigned) DMRContact::AllCall
              }},
            { "number", new RadioLimitDMRId(RadioLimitIssue::Severity::Hint) }
          } },
        { DTMFContact::staticMetaObject, -1, -1, new RadioLimitIgnored() }
      } );

  /* Define limits for group lists. */
  add("groupLists", new RadioLimitList(
        RXGroupList::staticMetaObject, 1, RT4DCodeplug::GroupListBankElement::Limit::groupLists(),
        new RadioLimitObject {
          { "name", new RadioLimitString(1, -1, RadioLimitString::ASCII) }, // Name is not encoded.
          { "contacts", new RadioLimitGroupCallRefList(1, 32) }
        }) );

  /* Define limits for channel list. */
  add("channels", new RadioLimitList(
        Channel::staticMetaObject, 1, RT4DCodeplug::ChannelBankElement::Limit::channels(),
        new RadioLimitObjects {
          { FMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(
               1, RT4DCodeplug::ChannelElement::Limit::name(),
               RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                         {Frequency::fromMHz(400.), Frequency::fromMHz(480.)}}, RadioLimitIssue::Severity::Critical)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitInterval()},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitIgnored(RadioLimitIssue::Silent)},
              {"rxOnly", new RadioLimitBool()},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"admit", new RadioLimitEnum{
                 (unsigned)FMChannel::Admit::Always,
                 (unsigned)FMChannel::Admit::Free,
                 (unsigned)FMChannel::Admit::Tone
               } },
              {"squelch", new RadioLimitIgnored(RadioLimitIssue::Silent)},
              {"bandwidth", new RadioLimitEnum{
                 (unsigned)FMChannel::Bandwidth::Narrow,
                 (unsigned)FMChannel::Bandwidth::Wide
               }},
              {"aprs", new RadioLimitObjRefIgnored(nullptr, RadioLimitIssue::Hint)}
            } },
          { DMRChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(
               1, RT4DCodeplug::ChannelElement::Limit::name(),
               RadioLimitString::ASCII)},
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                         {Frequency::fromMHz(400.), Frequency::fromMHz(480.)}}, RadioLimitIssue::Severity::Critical)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(136.), Frequency::fromMHz(174.)},
                                                                 {Frequency::fromMHz(400.), Frequency::fromMHz(480.)}})},
              {"power", new RadioLimitEnum {
                 unsigned(Channel::Power::Low),
                 unsigned(Channel::Power::High),
               }},
              {"timeout", new RadioLimitInterval()},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitIgnored(RadioLimitIssue::Silent)},
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
            } },
          { AMChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(
                1, RT4DCodeplug::ChannelElement::Limit::name(), RadioLimitString::ASCII) },
              {"rxFrequency", new RadioLimitFrequencies({{Frequency::fromMHz(118.), Frequency::fromMHz(137.)}}, RadioLimitIssue::Severity::Critical)},
              {"txFrequency", new RadioLimitTransmitFrequencies({{Frequency::fromMHz(118.), Frequency::fromMHz(137.)}})},
              {"power", new RadioLimitEnum {unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"squelch", new RadioLimitIgnored(RadioLimitIssue::Silent)},
              {"rxOnly", new RadioLimitBool()}
            } }
        } ) );

  /* Define limits for zone list. */
  add("zones", new RadioLimitList(
        Zone::staticMetaObject, 1, RT4DCodeplug::ZoneBankElement::Limit::zones(), new RadioLimitObject {
          { "name", new RadioLimitString(
            1, RT4DCodeplug::ZoneElement::Limit::name(), RadioLimitString::Unicode) },
          { "A", new RadioLimitRefList(
            0, RT4DCodeplug::ZoneElement::Limit::channels(), Channel::staticMetaObject) },
          { "B", new RadioLimitRefList(0, 0, Channel::staticMetaObject) },
          { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
        } ) );

  /* Define limits for scan lists. */
  add("scanlists", new RadioLimitIgnored());

  /* Define limits for positioning systems. */
  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );

  /* Ignore roaming zones. */
  add("roaming", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );
}
