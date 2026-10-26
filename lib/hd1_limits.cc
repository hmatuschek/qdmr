#include "hd1_limits.hh"
#include "hd1_codeplug.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"
#include "roamingzone.hh"

// The bands of the info block (136–174 and 400–480 MHz).
#define HD1_BANDS {{Frequency::fromMHz(136.), Frequency::fromMHz(174.)}, \
                   {Frequency::fromMHz(400.), Frequency::fromMHz(480.)}}

HD1Limits::HD1Limits(QObject *parent)
  : RadioLimits(true, parent)
{
  // The address book is a separate session; not supported yet.
  _hasCallSignDB          = false;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 0;

  _hasSatelliteConfig          = false;
  _satelliteConfigImplemented  = false;
  _numSatellites               = 0;

  add("settings", new RadioLimitItem {
        { "power", new RadioLimitEnum {
            unsigned(Channel::Power::Low), unsigned(Channel::Power::Mid),
            unsigned(Channel::Power::High) } },
        { "tot", new RadioLimitInterval() },  // used by channels with the default timeout
        { "audio", new RadioLimitItem {
            { "squelch", new RadioLimitLevel({0, 9}, false) },
            { "speech", new RadioLimitBool() },
            { "micGain", new RadioLimitIgnored(RadioLimitIssue::Silent) },
            { "vox", new RadioLimitIgnored(RadioLimitIssue::Silent) } } },
        { "boot", new RadioLimitIgnored(RadioLimitIssue::Silent) },
        { "tone", new RadioLimitIgnored(RadioLimitIssue::Silent) },
        { "dmr", new RadioLimitIgnored(RadioLimitIssue::Silent) },
        { "gnss", new RadioLimitIgnored(RadioLimitIssue::Silent) }
      } );

  add("radioIDs", new RadioLimitList{
        { DMRRadioID::staticMetaObject, 1, HD1Codeplug::RadioIdBankElement::Limit::ids(), new RadioLimitObject {
            { "name", new RadioLimitString(
              1, HD1Codeplug::RadioIdElement::Limit::nameLength(), RadioLimitString::ASCII) },
            { "number", new RadioLimitDMRId(RadioLimitIssue::Severity::Critical) }
          } }
      } );

  add("contacts", new RadioLimitList{
        { DMRContact::staticMetaObject, 1, HD1Codeplug::ContactBankElement::Limit::contacts(), new RadioLimitObject {
            { "name", new RadioLimitString(
              1, HD1Codeplug::ContactElement::Limit::nameLength(), RadioLimitString::ASCII) },
            { "ring", new RadioLimitIgnoredBool() },
            { "type", new RadioLimitEnum {
                unsigned(DMRContact::PrivateCall), unsigned(DMRContact::GroupCall),
                unsigned(DMRContact::AllCall) } },
            { "number", new RadioLimitDMRId(RadioLimitIssue::Severity::Hint) }
          } },
        { DTMFContact::staticMetaObject, -1, -1, new RadioLimitIgnored() }
      } );

  // More lists than table slots are fine: channels copy their list's members.
  add("groupLists", new RadioLimitList(
        RXGroupList::staticMetaObject, 0, -1, new RadioLimitObject {
          { "name", new RadioLimitString(
            1, HD1Codeplug::GroupListElement::Limit::nameLength(), RadioLimitString::ASCII) },
          { "contacts", new RadioLimitGroupCallRefList(1, HD1Codeplug::GroupListElement::Limit::members()) }
        }) );

  add("channels", new RadioLimitList(
        Channel::staticMetaObject, 1, HD1Codeplug::ChannelBankElement::Limit::channels(), new RadioLimitObjects {
          { FMChannel::staticMetaObject, new RadioLimitObject {
              { "name", new RadioLimitString(
                1, HD1Codeplug::ChannelElement::Limit::nameLength(), RadioLimitString::ASCII) },
              { "rxFrequency", new RadioLimitFrequencies(HD1_BANDS, RadioLimitIssue::Severity::Critical) },
              { "txFrequency", new RadioLimitTransmitFrequencies(HD1_BANDS) },
              { "power", new RadioLimitEnum {
                  unsigned(Channel::Power::Low), unsigned(Channel::Power::Mid),
                  unsigned(Channel::Power::High) } },
              { "timeout", new RadioLimitInterval() },  // 15 s steps up to 600 s, or endless
              { "scanlist", new RadioLimitObjRef(ScanList::staticMetaObject) },
              { "vox", new RadioLimitIgnored(RadioLimitIssue::Silent) },
              { "rxOnly", new RadioLimitIgnoredBool() },
              { "admit", new RadioLimitIgnored(RadioLimitIssue::Silent) },
              { "squelch", new RadioLimitIgnored(RadioLimitIssue::Silent) },
              { "bandwidth", new RadioLimitEnum {
                  unsigned(FMChannel::Bandwidth::Narrow), unsigned(FMChannel::Bandwidth::Wide) } },
              { "aprs", new RadioLimitObjRefIgnored(nullptr, RadioLimitIssue::Hint) },
              { "openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint) },
              { "tyt", new RadioLimitIgnored(RadioLimitIssue::Hint) }
            } },
          { DMRChannel::staticMetaObject, new RadioLimitObject {
              { "name", new RadioLimitString(
                1, HD1Codeplug::ChannelElement::Limit::nameLength(), RadioLimitString::ASCII) },
              { "rxFrequency", new RadioLimitFrequencies(HD1_BANDS, RadioLimitIssue::Severity::Critical) },
              { "txFrequency", new RadioLimitTransmitFrequencies(HD1_BANDS) },
              { "power", new RadioLimitEnum {
                  unsigned(Channel::Power::Low), unsigned(Channel::Power::Mid),
                  unsigned(Channel::Power::High) } },
              { "timeout", new RadioLimitInterval() },  // 15 s steps up to 600 s, or endless
              { "scanlist", new RadioLimitObjRef(ScanList::staticMetaObject) },
              { "vox", new RadioLimitIgnored(RadioLimitIssue::Silent) },
              { "rxOnly", new RadioLimitBool() },
              { "admit", new RadioLimitEnum {
                  unsigned(DMRChannel::Admit::Always), unsigned(DMRChannel::Admit::Free),
                  unsigned(DMRChannel::Admit::ColorCode) } },
              { "colorCode", new RadioLimitUInt(0, 15) },
              { "timeSlot", new RadioLimitEnum {
                  unsigned(DMRChannel::TimeSlot::TS1), unsigned(DMRChannel::TimeSlot::TS2) } },
              { "radioID", new RadioLimitObjRef(RadioID::staticMetaObject, true) },
              { "groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, true) },
              { "contact", new RadioLimitObjRef(DMRContact::staticMetaObject, true) },
              { "aprs", new RadioLimitObjRefIgnored() },
              { "roaming", new RadioLimitObjRefIgnored(DefaultRoamingZone::get()) },
              { "openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint) },
              { "tyt", new RadioLimitIgnored(RadioLimitIssue::Hint) }
            } }
        } ) );

  add("zones", new RadioLimitList(
        Zone::staticMetaObject, 1, HD1Codeplug::ZoneBankElement::Limit::zones(), new RadioLimitObject {
          { "name", new RadioLimitString(
            1, HD1Codeplug::ZoneElement::Limit::nameLength(), RadioLimitString::ASCII) },
          { "A", new RadioLimitRefList(0, HD1Codeplug::ZoneElement::Limit::channels(),
                                       Channel::staticMetaObject) },
          { "B", new RadioLimitRefList(0, 0, Channel::staticMetaObject) },
          { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }
        } ) );

  // The HD1 only has a per-channel "scan add" flag: one list holds those channels.
  add("scanlists", new RadioLimitList(
        ScanList::staticMetaObject, 0, 1, new RadioLimitObject {
          { "name", new RadioLimitStringIgnored() },
          { "primary", new RadioLimitObjRefIgnored() },
          { "secondary", new RadioLimitObjRefIgnored() },
          { "revert", new RadioLimitObjRefIgnored() },
          { "channels", new RadioLimitRefList(0, HD1Codeplug::ChannelBankElement::Limit::channels(), Channel::staticMetaObject) }
        }) );

  add("positioning", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)));
  add("roaming", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)));
}
