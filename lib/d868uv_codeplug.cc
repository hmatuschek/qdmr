#include "d868uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"
#include "utils.hh"
#include <cmath>

#include <QTimeZone>
#include <QtEndian>
#include <QSet>


#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define CHANNEL_BANK_0            0x00800000
#define CHANNEL_BANK_SIZE         0x00002000
#define CHANNEL_BANK_31           0x00fc0000
#define CHANNEL_BANK_31_SIZE      0x00000800
#define CHANNEL_BANK_OFFSET       0x00040000
#define CHANNEL_BITMAP            0x024c1500
#define CHANNEL_BITMAP_SIZE       0x00000200
#define CHANNEL_SIZE              0x00000040

#define VFO_A_ADDR                0x00fc0800 // Address of VFO A settings (channel_t)
#define VFO_B_ADDR                0x00fc0840 // Address of VFO B settings (channel_t)
#define VFO_SIZE                  0x00000040 // Size of each VFO settings.

#define NUM_CONTACTS              10000      // Total number of contacts
#define CONTACTS_PER_BLOCK        4
#define CONTACT_BLOCK_0           0x02680000 // First bank of 4 contacts
#define CONTACT_BLOCK_SIZE        0x00000190 // Size of 4 contacts
#define CONTACT_BANK_SIZE         0x00040000 // Size of one contact bank
#define CONTACTS_PER_BANK         1000       // Number of contacts per bank
#define CONTACT_INDEX_LIST        0x02600000 // Address of contact index list
#define CONTACTS_BITMAP           0x02640000 // Address of contact bitmap
#define CONTACTS_BITMAP_SIZE      0x00000500 // Size of contact bitmap
#define CONTACT_SIZE              0x00000064 // Size of contact element
#define CONTACT_ID_MAP            0x04340000 // Address of ID->Contact index map
#define CONTACT_ID_ENTRY_SIZE     0x00000008 // Size of each map entry


#define NUM_ANALOGCONTACTS        128
#define NUM_ANALOGCONTACT_BANKS   64
#define ANALOGCONTACTS_PER_BANK   2
#define ANALOGCONTACT_BANK_0      0x02940000
#define ANALOGCONTACT_BANK_SIZE   0x00000030
#define ANALOGCONTACT_INDEX_LIST  0x02900000 // Address of analog contact index list
#define ANALOGCONTACT_LIST_SIZE   0x00000080 // Size of analog contact index list
#define ANALOGCONTACT_BYTEMAP     0x02900100 // Address of contact bytemap
#define ANALOGCONTACT_BYTEMAP_SIZE 0x00000080 // Size of contact bytemap
#define ANALOGCONTACT_SIZE        0x00000018 // Size of analog contact

#define NUM_RXGRP                 250        // Total number of RX group lists
#define ADDR_RXGRP_0              0x02980000 // Address of the first RX group list.
#define RXGRP_SIZE                0x00000120 // Size of each RX group list.
#define RXGRP_OFFSET              0x00000200 // Offset between group lists.
#define RXGRP_BITMAP              0x025C0B10 // Address of RX group list bitmap.
#define RXGRP_BITMAP_SIZE         0x00000020 // Storage size of RX group list bitmap.

#define NUM_ZONES                 250        // Maximum number of zones
#define NUM_CH_PER_ZONE           250        // Maximum number of channels per zone
#define ADDR_ZONE                 0x01000000 // Address of zone-channel lists, see zone_t
#define ZONE_SIZE                 0x00000200 // Size of each zone-channel list
#define ZONE_OFFSET               0x00000200 // Offset between zone-channel lists
#define ADDR_ZONE_NAME            0x02540000 // Address of zone names.
#define ZONE_NAME_SIZE            0x00000010 // Size of zone names
#define ZONE_NAME_OFFSET          0x00000020 // Offset between zone names.
#define ZONE_BITMAPS              0x024c1300 // Bitmap of all enabled zones
#define ZONE_BITMAPS_SIZE         0x00000020 // Size of the zone bitmap

#define NUM_RADIOIDS              250
#define ADDR_RADIOIDS             0x02580000
#define RADIOID_SIZE              0x00000020
#define RADIOID_BITMAP            0x024c1320
#define RADIOID_BITMAP_SIZE       0x00000020

#define NUM_SCAN_LISTS            250
#define NUM_SCANLISTS_PER_BANK    16
#define SCAN_LIST_BANK_0          0x01080000 // First scanlist bank
#define SCAN_LIST_OFFSET          0x00000200 // Offset to next list.
#define SCAN_LIST_SIZE            0x00000090 // Size of scan-list.
#define SCAN_LIST_BANK_OFFSET     0x00040000 // Offset to next bank
#define SCAN_BITMAP               0x024c1340 // Address of scan-list bitmap.
#define SCAN_BITMAP_SIZE          0x00000020 // Size of scan-list bitmap.

#define ADDR_GENERAL_CONFIG       0x02500000
#define GENERAL_CONFIG_SIZE       0x000000d0

#define ADDR_ZONE_CHANNELS        0x02500100
#define ZONE_CHANNELS_SIZE        0x00000400

#define ADDR_DTMF_NUMBERS         0x02500500
#define DTMF_NUMBERS_SIZE         0x00000100

#define ADDR_BOOT_SETTINGS        0x02500600
#define BOOT_SETTINGS_SIZE        0x00000030

#define ADDR_GPS_SETTINGS         0x02501000
#define GPS_SETTINGS_SIZE         0x00000030

#define ADDR_GPS_MESSAGE          0x02501100
#define GPS_MESSAGE_SIZE          0x00000030

#define NUM_MESSAGES              100
#define NUM_MESSAGES_PER_BANK     8
#define MESSAGE_SIZE              0x00000100
#define MESSAGE_BANK_0            0x02140000
#define MESSAGE_BANK_SIZE         0x00000800
#define MESSAGE_BANK_OFFSET       0x00040000
#define MESSAGE_INDEX_LIST        0x01640000
#define MESSAGE_BYTEMAP           0x01640800
#define MESSAGE_BYTEMAP_SIZE      0x00000090

#define ADDR_HOTKEY               0x025C0000
#define HOTKEY_SIZE               0x00000860
#define STATUSMESSAGE_BITMAP      0x025C0B00
#define STATUSMESSAGE_BITMAP_SIZE 0x00000010

#define ADDR_OFFSET_FREQ          0x024C2000
#define OFFSET_FREQ_SIZE          0x000003F0

#define ADDR_ALARM_SETTING        0x024C1400
#define ALARM_SETTING_SIZE        0x00000020

#define ADDR_ALARM_SETTING_EXT    0x024c1440
#define ALARM_SETTING_EXT_SIZE    0x00000030

#define FMBC_BITMAP               0x02480210
#define FMBC_BITMAP_SIZE          0x00000020
#define ADDR_FMBC                 0x02480000
#define FMBC_SIZE                 0x00000200
#define ADDR_FMBC_VFO             0x02480200
#define FMBC_VFO_SIZE             0x00000010

#define FIVE_TONE_ID_BITMAP       0x024C0C80
#define FIVE_TONE_ID_BITMAP_SIZE  0x00000010
#define NUM_FIVE_TONE_IDS         100
#define ADDR_FIVE_TONE_ID_LIST    0x024C0000
#define FIVE_TONE_ID_SIZE         0x00000020
#define FIVE_TONE_ID_LIST_SIZE    0x00000c80
#define NUM_FIVE_TONE_FUNCTIONS   16
#define ADDR_FIVE_TONE_FUNCTIONS  0x024C0D00
#define FIVE_TONE_FUNCTION_SIZE   0x00000020
#define FIVE_TONE_FUNCTIONS_SIZE  0x00000200
#define ADDR_FIVE_TONE_SETTINGS   0x024C1000
#define FIVE_TONE_SETTINGS_SIZE   0x00000080

#define ADDR_DTMF_SETTINGS        0x024C1080
#define DTMF_SETTINGS_SIZE        0x00000050

#define NUM_TWO_TONE_IDS          24
#define TWO_TONE_IDS_BITMAP       0x024C1280
#define TWO_TONE_IDS_BITMAP_SIZE  0x00000010
#define ADDR_TWO_TONE_IDS         0x024C1100
#define TWO_TONE_ID_SIZE          0x00000010

#define NUM_TWO_TONE_FUNCTIONS    16
#define TWO_TONE_FUNCTIONS_BITMAP 0x024c2600
#define TWO_TONE_FUNC_BITMAP_SIZE 0x00000010
#define ADDR_TWO_TONE_FUNCTIONS   0x024c2400
#define TWO_TONE_FUNCTION_SIZE    0x00000020

#define ADDR_TWO_TONE_SETTINGS    0x024C1290
#define TWO_TONE_SETTINGS_SIZE    0x00000010

#define ADDR_DMR_ENCRYPTION_LIST  0x024C1700
#define DMR_ENCRYPTION_LIST_SIZE  0x00000040
#define ADDR_DMR_ENCRYPTION_KEYS  0x024C1800
#define DMR_ENCRYPTION_KEYS_SIZE  0x00000500

using namespace Signaling;

Code _ctcss_num2code[52] = {
  SIGNALING_NONE, // 62.5 not supported
  CTCSS_67_0Hz,  SIGNALING_NONE, // 69.3 not supported
  CTCSS_71_9Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_7Hz,  CTCSS_82_5Hz,
  CTCSS_85_4Hz,  CTCSS_88_5Hz,  CTCSS_91_5Hz,  CTCSS_94_8Hz,  CTCSS_97_4Hz,  CTCSS_100_0Hz,
  CTCSS_103_5Hz, CTCSS_107_2Hz, CTCSS_110_9Hz, CTCSS_114_8Hz, CTCSS_118_8Hz, CTCSS_123_0Hz,
  CTCSS_127_3Hz, CTCSS_131_8Hz, CTCSS_136_5Hz, CTCSS_141_3Hz, CTCSS_146_2Hz, CTCSS_151_4Hz,
  CTCSS_156_7Hz,
  SIGNALING_NONE, // 159.8 not supported
  CTCSS_162_2Hz,
  SIGNALING_NONE, // 165.5 not supported
  CTCSS_167_9Hz,
  SIGNALING_NONE, // 171.3 not supported
  CTCSS_173_8Hz,
  SIGNALING_NONE, // 177.3 not supported
  CTCSS_179_9Hz,
  SIGNALING_NONE, // 183.5 not supported
  CTCSS_186_2Hz,
  SIGNALING_NONE, // 189.9 not supported
  CTCSS_192_8Hz,
  SIGNALING_NONE, SIGNALING_NONE, // 196.6 & 199.5 not supported
  CTCSS_203_5Hz,
  SIGNALING_NONE, // 206.5 not supported
  CTCSS_210_7Hz, CTCSS_218_1Hz, CTCSS_225_7Hz,
  SIGNALING_NONE, // 229.1 not supported
  CTCSS_233_6Hz, CTCSS_241_8Hz, CTCSS_250_3Hz,
  SIGNALING_NONE, SIGNALING_NONE // 254.1 and custom CTCSS not supported.
};

uint8_t
D868UVCodeplug::ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == _ctcss_num2code[i])
      return i;
  }
  return 0;
}

Signaling::Code
D868UVCodeplug::ctcss_num2code(uint8_t num) {
  if (num >= 52)
    return Signaling::SIGNALING_NONE;
  return _ctcss_num2code[num];
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug::GeneralSettingsElement
 * ******************************************************************************************** */
D868UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, unsigned size)
  : AnytoneCodeplug::GeneralSettingsElement(ptr, size)
{
  // pass....
}

D868UVCodeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : D868UVCodeplug::GeneralSettingsElement(ptr, 0x00d0)
{
  // pass...
}

void
D868UVCodeplug::GeneralSettingsElement::clear() {
  AnytoneCodeplug::GeneralSettingsElement::clear();
}

D868UVCodeplug::GeneralSettingsElement::Color
D868UVCodeplug::GeneralSettingsElement::callDisplayColor() const {
  return (Color)getUInt8(0x00b0);
}
void
D868UVCodeplug::GeneralSettingsElement::setCallDisplayColor(Color color) {
  setUInt8(0x00b0, (unsigned)color);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::gpsUpdatePeriod() const {
  return getUInt8(0x00b1);
}
void
D868UVCodeplug::GeneralSettingsElement::setGPSUpdatePeriod(unsigned sec) {
  setUInt8(0x00b1, sec);
}

bool
D868UVCodeplug::GeneralSettingsElement::showZoneAndContact() const {
  return getUInt8(0x00b2);
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowZoneAndContact(bool enable) {
  setUInt8(0x00b2, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::keyToneLevelAdjustable() const {
  return 0 == keyToneLevel();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::keyToneLevel() const {
  return ((unsigned)getUInt8(0x00b3))*10/15;
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevel(unsigned level) {
  setUInt8(0x00b3, level*10/15);
}
void
D868UVCodeplug::GeneralSettingsElement::setKeyToneLevelAdjustable() {
  setUInt8(0x00b3, 0);
}

bool
D868UVCodeplug::GeneralSettingsElement::gpsUnitsImperial() const {
  return getUInt8(0x00b4);
}
void
D868UVCodeplug::GeneralSettingsElement::enableGPSUnitsImperial(bool enable) {
  setUInt8(0x00b4, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::knobLock() const {
  return getBit(0x00b5, 0);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKnobLock(bool enable) {
  setBit(0x00b5, 0, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keypadLock() const {
  return getBit(0x00b5, 1);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeypadLock(bool enable) {
  setBit(0x00b5, 1, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::sidekeysLock() const {
  return getBit(0x00b5, 3);
}
void
D868UVCodeplug::GeneralSettingsElement::enableSidekeysLock(bool enable) {
  setBit(0x00b5, 3, enable);
}
bool
D868UVCodeplug::GeneralSettingsElement::keyLockForced() const {
  return getBit(0x00b5, 4);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeyLockForced(bool enable) {
  setBit(0x00b5, 4, enable);
}

bool
D868UVCodeplug::GeneralSettingsElement::showLastHeard() const {
  return getUInt8(0x00b6);
}
void
D868UVCodeplug::GeneralSettingsElement::enableShowLastHeard(bool enable) {
  setUInt8(0x00b6, (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyVHF() const {
  return getBCD8_be(0x00b8)*10;
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyVHF(unsigned Hz) {
  setBCD8_be(0x00b8, Hz/10);
}
unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyVHF() const {
  return getBCD8_be(0x00bc)*10;
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyVHF(unsigned Hz) {
  setBCD8_be(0x00bc, Hz/10);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMinFrequencyUHF() const {
  return getBCD8_be(0x00c0)*10;
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMinFrequencyUHF(unsigned Hz) {
  setBCD8_be(0x00c0, Hz/10);
}
unsigned
D868UVCodeplug::GeneralSettingsElement::autoRepeaterMaxFrequencyUHF() const {
  return getBCD8_be(0x00c4)*10;
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterMaxFrequencyUHF(unsigned Hz) {
  setBCD8_be(0x00c4, Hz/10);
}

D868UVCodeplug::GeneralSettingsElement::AutoRepDir
D868UVCodeplug::GeneralSettingsElement::autoRepeaterDirectionB() const {
  return (AutoRepDir)getUInt8(0x00c8);
}
void
D868UVCodeplug::GeneralSettingsElement::setAutoRepeaterDirectionB(AutoRepDir dir) {
  setUInt8(0x00c8, (unsigned)dir);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannel() const {
  return getUInt8(0x00ca);
}
void
D868UVCodeplug::GeneralSettingsElement::enableDefaultChannel(bool enable) {
  setUInt8(0x00ca, (enable ? 0x01 : 0x00));
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexA() const {
  return getUInt8(0x00cb);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexA(unsigned idx) {
  setUInt8(0x00cb, idx);
}

unsigned
D868UVCodeplug::GeneralSettingsElement::defaultZoneIndexB() const {
  return getUInt8(0x00cc);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultZoneIndexB(unsigned idx) {
  setUInt8(0x00cc, idx);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIsVFO() const {
  return 0xff == defaultChannelAIndex();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::defaultChannelAIndex() const {
  return getUInt8(0x00cd);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelAIndex(unsigned idx) {
  setUInt8(0x00cd, idx);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelAToVFO() {
  setDefaultChannelAIndex(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::defaultChannelBIsVFO() const {
  return 0xff == defaultChannelBIndex();
}
unsigned
D868UVCodeplug::GeneralSettingsElement::defaultChannelBIndex() const {
  return getUInt8(0x00ce);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBIndex(unsigned idx) {
  setUInt8(0x00ce, idx);
}
void
D868UVCodeplug::GeneralSettingsElement::setDefaultChannelBToVFO() {
  setDefaultChannelBIndex(0xff);
}

bool
D868UVCodeplug::GeneralSettingsElement::keepLastCaller() const {
  return getUInt8(0x00cf);
}
void
D868UVCodeplug::GeneralSettingsElement::enableKeepLastCaller(bool enable) {
  setUInt8(0x00cf, (enable ? 0x01 : 0x00));
}

bool
D868UVCodeplug::GeneralSettingsElement::fromConfig(const Flags &flags, Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::fromConfig(flags, ctx))
    return false;

  setGPSUpdatePeriod(0x05);
  // Set measurement system based on system locale (0x00==Metric)
  enableGPSUnitsImperial(QLocale::ImperialSystem == QLocale::system().measurementSystem());

  return true;
}

bool
D868UVCodeplug::GeneralSettingsElement::updateConfig(Context &ctx) {
  if (! AnytoneCodeplug::GeneralSettingsElement::updateConfig(ctx))
    return false;
  return true;
}


/* ******************************************************************************************** *
 * Implementation of D868UVCodeplug
 * ******************************************************************************************** */
D868UVCodeplug::D868UVCodeplug(QObject *parent)
  : AnytoneCodeplug(parent)
{
}

void
D868UVCodeplug::clear() {
  while (this->numImages())
    remImage(0);

  addImage("Anytone AT-D868UV Codeplug");

  // Channel bitmap
  image(0).addElement(CHANNEL_BITMAP, CHANNEL_BITMAP_SIZE);
  // Zone bitmap
  image(0).addElement(ZONE_BITMAPS, ZONE_BITMAPS_SIZE);
  // Contacts bitmap
  image(0).addElement(CONTACTS_BITMAP, CONTACTS_BITMAP_SIZE);
  // Analog contacts bytemap
  image(0).addElement(ANALOGCONTACT_BYTEMAP, ANALOGCONTACT_BYTEMAP_SIZE);
  // RX group list bitmaps
  image(0).addElement(RXGRP_BITMAP, RXGRP_BITMAP_SIZE);
  // Scan list bitmaps
  image(0).addElement(SCAN_BITMAP, SCAN_BITMAP_SIZE);
  // Radio IDs bitmaps
  image(0).addElement(RADIOID_BITMAP, RADIOID_BITMAP_SIZE);
  // Message bitmaps
  image(0).addElement(MESSAGE_BYTEMAP, MESSAGE_BYTEMAP_SIZE);
  // Status messages
  image(0).addElement(STATUSMESSAGE_BITMAP, STATUSMESSAGE_BITMAP_SIZE);
  // FM Broadcast bitmaps
  image(0).addElement(FMBC_BITMAP, FMBC_BITMAP_SIZE);
  // 5-Tone function bitmaps
  image(0).addElement(FIVE_TONE_ID_BITMAP, FIVE_TONE_ID_BITMAP_SIZE);
  // 2-Tone function bitmaps
  image(0).addElement(TWO_TONE_IDS_BITMAP, TWO_TONE_IDS_BITMAP_SIZE);
  image(0).addElement(TWO_TONE_FUNCTIONS_BITMAP, TWO_TONE_FUNC_BITMAP_SIZE);
}

void
D868UVCodeplug::allocateUpdated() {
  this->allocateVFOSettings();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateDTMFNumbers();
  this->allocateBootSettings();

  this->allocateGPSSystems();

  this->allocateSMSMessages();
  this->allocateHotKeySettings();
  this->allocateRepeaterOffsetSettings();
  this->allocateAlarmSettings();
  this->allocateFMBroadcastSettings();

  this->allocate5ToneIDs();
  this->allocate5ToneFunctions();
  this->allocate5ToneSettings();

  this->allocate2ToneIDs();
  this->allocate2ToneFunctions();
  this->allocate2ToneSettings();

  this->allocateDTMFSettings();

  image(0).addElement(ADDR_DMR_ENCRYPTION_LIST, DMR_ENCRYPTION_LIST_SIZE);
  image(0).addElement(ADDR_DMR_ENCRYPTION_KEYS, DMR_ENCRYPTION_KEYS_SIZE);
}

void
D868UVCodeplug::allocateForEncoding() {
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();
  this->allocateRadioIDs();
}

void
D868UVCodeplug::allocateForDecoding() {
  this->allocateRadioIDs();
  this->allocateChannels();
  this->allocateZones();
  this->allocateContacts();
  this->allocateAnalogContacts();
  this->allocateRXGroupLists();
  this->allocateScanLists();

  // General config
  this->allocateGeneralSettings();
  this->allocateZoneChannelList();
  this->allocateBootSettings();

  // GPS settings
  this->allocateGPSSystems();
}


void
D868UVCodeplug::setBitmaps(Config *config)
{
  // Mark first radio ID as valid
  uint8_t *radioid_bitmap = data(RADIOID_BITMAP);
  memset(radioid_bitmap, 0, RADIOID_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_RADIOIDS, config->radioIDs()->count()); i++)
    radioid_bitmap[i/8] |= (1 << (i%8));

  // Mark valid channels (set bit)
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  memset(channel_bitmap, 0, CHANNEL_BITMAP_SIZE);
  for (int i=0,c=0; i<std::min(NUM_CHANNELS, config->channelList()->count()); i++) {
    Channel *channel = config->channelList()->channel(i);
    if ((! channel->is<DMRChannel>()) && (! channel->is<FMChannel>()))
      continue;
    channel_bitmap[c/8] |= (1 << (c%8)); c++;
  }

  // Mark valid contacts (clear bit)
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  memset(contact_bitmap, 0x00, CONTACTS_BITMAP_SIZE);
  memset(contact_bitmap, 0xff, NUM_CONTACTS/8+1);
  for (int i=0, c=0; i<std::min(NUM_CONTACTS, config->contacts()->count()); i++) {
    Contact *contact = config->contacts()->contact(i);
    if (! contact->is<DMRContact>())
      continue;
    contact_bitmap[c/8] &= ~(1 << (c%8)); c++;
  }

  // Mark valid analog contacts (clear bytes)
  uint8_t *analog_contact_bitmap = data(ANALOGCONTACT_BYTEMAP);
  memset(analog_contact_bitmap, 0xff, ANALOGCONTACT_BYTEMAP_SIZE);
  for (int i=0, c=0; i<std::min(NUM_ANALOGCONTACTS, config->contacts()->count()); i++) {
    Contact *contact = config->contacts()->contact(i);
    if (! contact->is<DTMFContact>())
      continue;
    analog_contact_bitmap[i] = 0x00; c++;
  }

  // Mark valid zones (set bits)
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  memset(zone_bitmap, 0x00, ZONE_BITMAPS_SIZE);
  for (int i=0,z=0; i<std::min(NUM_ZONES, config->zones()->count()); i++) {
    zone_bitmap[z/8] |= (1 << (z%8)); z++;
    if (config->zones()->zone(i)->B()->count()) {
      zone_bitmap[z/8] |= (1 << (z%8)); z++;
    }
  }

  // Mark group lists
  uint8_t *group_bitmap = data(RXGRP_BITMAP);
  memset(group_bitmap, 0x00, RXGRP_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_RXGRP, config->rxGroupLists()->count()); i++)
    group_bitmap[i/8] |= (1 << (i%8));

  // Mark scan lists
  uint8_t *scan_bitmap = data(SCAN_BITMAP);
  memset(scan_bitmap, 0x00, SCAN_BITMAP_SIZE);
  for (int i=0; i<std::min(NUM_SCAN_LISTS, config->scanlists()->count()); i++) {
    scan_bitmap[i/8] |= (1<<(i%8));
  }
}

bool
D868UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Context ctx(config);
  if (! index(config, ctx, err))
    return false;

  return encodeElements(flags, ctx, err);
}

bool D868UVCodeplug::decode(Config *config, const ErrorStack &err) {
  // Maps code-plug indices to objects
  Context ctx(config);
  return decodeElements(ctx, err);
}

bool
D868UVCodeplug::encodeElements(const Flags &flags, Context &ctx, const ErrorStack &err)
{
  if (! this->encodeRadioID(flags, ctx, err))
    return false;

  if (! this->encodeGeneralSettings(flags, ctx, err))
    return false;

  if (! this->encodeBootSettings(flags, ctx, err))
    return false;

  if (! this->encodeChannels(flags, ctx, err))
    return false;

  if (! this->encodeContacts(flags, ctx, err))
    return false;

  if (! this->encodeAnalogContacts(flags, ctx, err))
    return false;

  if (! this->encodeRXGroupLists(flags, ctx, err))
    return false;

  if (! this->encodeZones(flags, ctx, err))
    return false;

  if (! this->encodeScanLists(flags, ctx, err))
    return false;

  if (! this->encodeGPSSystems(flags, ctx, err))
    return false;

  return true;
}

bool
D868UVCodeplug::decodeElements(Context &ctx, const ErrorStack &err)
{
  if (! this->setRadioID(ctx, err))
    return false;

  if (! this->decodeGeneralSettings(ctx, err))
    return false;

  if (! this->decodeBootSettings(ctx, err))
    return false;

  if (! this->createChannels(ctx, err))
    return false;

  if (! this->createContacts(ctx, err))
    return false;

  if (! this->createAnalogContacts(ctx, err))
    return false;

  if (! this->createRXGroupLists(ctx, err))
    return false;

  if (! this->linkRXGroupLists(ctx, err))
    return false;

  if (! this->createZones(ctx, err))
    return false;

  if (! this->linkZones(ctx, err))
    return false;

  if (! this->createScanLists(ctx, err))
    return false;

  if (! this->linkScanLists(ctx, err))
    return false;

  if (! this->createGPSSystems(ctx, err))
    return false;

  if (! this->linkChannels(ctx, err))
    return false;

  if (! this->linkGPSSystems(ctx, err))
    return false;

  return true;
}


void
D868UVCodeplug::allocateChannels() {
  /* Allocate channels */
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Get byte and bit for channel, as well as bank of channel
    uint16_t bit = i%8, byte = i/8, bank = i/128, idx=i%128;
    // if disabled -> skip
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    // compute address for channel
    uint32_t addr = CHANNEL_BANK_0
        + bank*CHANNEL_BANK_OFFSET
        + idx*CHANNEL_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, CHANNEL_SIZE);
    }
  }
}

bool
D868UVCodeplug::encodeChannels(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode channels
  for (unsigned int i=0; i<ctx.count<Channel>(); i++) {
    // enable channel
    uint16_t bank = i/128, idx = i%128;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (! ch.fromChannelObj(ctx.get<Channel>(i), ctx))
      return false;
  }
  return true;
}

bool
D868UVCodeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)
  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (Channel *obj = ch.toChannelObj(ctx)) {
      ctx.config()->channelList()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D868UVCodeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (ctx.has<Channel>(i))
      ch.linkChannelObj(ctx.get<Channel>(i), ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateVFOSettings() {
  // Allocate VFO channels
  image(0).addElement(VFO_A_ADDR, CHANNEL_SIZE);
  image(0).addElement(VFO_A_ADDR+0x2000, CHANNEL_SIZE);
  image(0).addElement(VFO_B_ADDR, CHANNEL_SIZE);
  image(0).addElement(VFO_B_ADDR+0x2000, CHANNEL_SIZE);
}

void
D868UVCodeplug::allocateContacts() {
  /* Allocate contacts */
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  unsigned contactCount=0;
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // enabled if false (ass hole)
    if (1 == ((contact_bitmap[i/8]>>(i%8)) & 0x01))
      continue;
    contactCount++;
    uint32_t bank_addr = CONTACT_BLOCK_0 + (i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + ((i%CONTACTS_PER_BANK)/CONTACTS_PER_BLOCK)*CONTACT_BLOCK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, CONTACT_BLOCK_SIZE);
      memset(data(addr), 0x00, CONTACT_BLOCK_SIZE);
    }
  }

  if (contactCount) {
    image(0).addElement(CONTACT_INDEX_LIST, align_size(4*contactCount, 16));
    memset(data(CONTACT_INDEX_LIST), 0xff, align_size(4*contactCount, 16));
    image(0).addElement(CONTACT_ID_MAP, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
    memset(data(CONTACT_ID_MAP), 0xff, align_size(CONTACT_ID_ENTRY_SIZE*(1+contactCount), 16));
  }
}

bool
D868UVCodeplug::encodeContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  QVector<DMRContact*> contacts;
  // Encode contacts and also collect id<->index map
  for (unsigned int i=0; i<ctx.count<DMRContact>(); i++) {
    uint32_t bank_addr = CONTACT_BLOCK_0 + (i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + (i%CONTACTS_PER_BANK)*CONTACT_SIZE;
    ContactElement con(data(addr));
    DMRContact *contact = ctx.get<DMRContact>(i);
    if(! con.fromContactObj(contact, ctx))
      return false;
    ((uint32_t *)data(CONTACT_INDEX_LIST))[i] = qToLittleEndian(i);
    contacts.append(contact);
  }
  // encode index map for contacts
  std::sort(contacts.begin(), contacts.end(),
            [](DMRContact *a, DMRContact *b) {
    return a->number() < b->number();
  });
  for (int i=0; i<contacts.size(); i++) {
    ContactMapElement el(data(CONTACT_ID_MAP + i*CONTACT_ID_ENTRY_SIZE));
    el.setID(contacts[i]->number(), (DMRContact::GroupCall==contacts[i]->type()));
    el.setIndex(ctx.index(contacts[i]));
  }
  return true;
}

bool
D868UVCodeplug::createContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create digital contacts
  uint8_t *contact_bitmap = data(CONTACTS_BITMAP);
  for (uint16_t i=0; i<NUM_CONTACTS; i++) {
    // Check if contact is enabled:
    uint16_t  bit = i%8, byte = i/8;
    if (1 == ((contact_bitmap[byte]>>bit) & 0x01))
      continue;
    uint32_t bank_addr = CONTACT_BLOCK_0 + (i/CONTACTS_PER_BANK)*CONTACT_BANK_SIZE;
    uint32_t addr = bank_addr + (i%CONTACTS_PER_BANK)*CONTACT_SIZE;
    ContactElement con(data(addr));
    if (DMRContact *obj = con.toContactObj(ctx)) {
      ctx.config()->contacts()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}


void
D868UVCodeplug::allocateAnalogContacts() {
  /* Allocate analog contacts */
  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BYTEMAP);
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i+=2) {
    // if disabled -> skip
    if (0xff == analog_contact_bytemap[i])
      continue;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, ANALOGCONTACT_BANK_SIZE);
    }
  }
  image(0).addElement(ANALOGCONTACT_INDEX_LIST, ANALOGCONTACT_LIST_SIZE);
}

bool
D868UVCodeplug::encodeAnalogContacts(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  uint8_t *idxlst = data(ANALOGCONTACT_INDEX_LIST);
  memset(idxlst, 0xff, ANALOGCONTACT_LIST_SIZE);
  for (unsigned int i=0; i<ctx.count<DTMFContact>(); i++) {
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE
        + (i%ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_SIZE;
    DTMFContactElement cont(data(addr));
    cont.fromContact(ctx.get<DTMFContact>(i));
    idxlst[i] = i;
  }
  return true;
}

bool
D868UVCodeplug::createAnalogContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  uint8_t *analog_contact_bytemap = data(ANALOGCONTACT_BYTEMAP);
  for (uint8_t i=0; i<NUM_ANALOGCONTACTS; i++) {
    // if disabled -> skip
    if (0xff == analog_contact_bytemap[i])
      continue;
    uint32_t addr = ANALOGCONTACT_BANK_0 + (i/ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_BANK_SIZE
        + (i%ANALOGCONTACTS_PER_BANK)*ANALOGCONTACT_SIZE;
    DTMFContactElement cont(data(addr));
    if (DTMFContact *dtmf = cont.toContact()) {
      ctx.config()->contacts()->add(dtmf);
      ctx.add(dtmf, i);
    }
  }
  return true;
}


void
D868UVCodeplug::allocateRadioIDs() {
  /* Allocate radio IDs */
  uint8_t *radioid_bitmap = data(RADIOID_BITMAP);
  for (uint8_t i=0; i<NUM_RADIOIDS; i++) {
    // Get byte and bit for radio ID
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((radioid_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate radio IDs individually
    uint32_t addr = ADDR_RADIOIDS + i*RADIOID_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, RADIOID_SIZE);
    }
  }
}

bool
D868UVCodeplug::encodeRadioID(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode radio IDs
  for (int i=0; i<ctx.config()->radioIDs()->count(); i++) {
    RadioIDElement(data(ADDR_RADIOIDS + i*RADIOID_SIZE)).fromRadioID(
          ctx.config()->radioIDs()->getId(i));
  }
  return true;
}

bool
D868UVCodeplug::setRadioID(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Find a valid RadioID
  uint8_t *radio_id_bitmap = data(RADIOID_BITMAP);
  for (uint16_t i=0; i<NUM_RADIOIDS; i++) {
    if (0 == (radio_id_bitmap[i/8] & (1 << (i%8))))
      continue;
    RadioIDElement id(data(ADDR_RADIOIDS + i*RADIOID_SIZE));
    logDebug() << "Store id " << id.number() << " at idx " << i << ".";
    if (DMRRadioID *rid = id.toRadioID()) {
      ctx.config()->radioIDs()->add(rid);  ctx.add(rid, i);
    }
  }
  return true;
}


void
D868UVCodeplug::allocateRXGroupLists() {
  /*
   * Allocate group lists
   */
  uint8_t *grouplist_bitmap = data(RXGRP_BITMAP);
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    // Get byte and bit for group list
    uint16_t bit = i%8, byte = i/8;
    // if disabled -> skip
    if (0 == ((grouplist_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate RX group lists indivitually
    uint32_t addr = ADDR_RXGRP_0 + i*RXGRP_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, RXGRP_SIZE);
      memset(data(addr), 0xff, RXGRP_SIZE);
    }
  }

}

bool
D868UVCodeplug::encodeRXGroupLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode RX group-lists
  for (int i=0; i<ctx.config()->rxGroupLists()->count(); i++) {
    GroupListElement grp(data(ADDR_RXGRP_0 + i*RXGRP_OFFSET));
    grp.fromGroupListObj(ctx.config()->rxGroupLists()->list(i), ctx);
  }
  return true;
}

bool
D868UVCodeplug::createRXGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create RX group lists
  uint8_t *grouplist_bitmap = data(RXGRP_BITMAP);
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    // check if group list is enabled
    uint16_t  bit = i%8, byte = i/8;
    if (0 == ((grouplist_bitmap[byte]>>bit) & 0x01))
      continue;
    // construct RXGroupList from definition
    GroupListElement grp(data(ADDR_RXGRP_0+i*RXGRP_OFFSET));
    if (RXGroupList *obj = grp.toGroupListObj()) {
      ctx.config()->rxGroupLists()->add(obj); ctx.add(obj, i);
    }
  }
  return true;
}

bool
D868UVCodeplug::linkRXGroupLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  uint8_t *grouplist_bitmap = data(RXGRP_BITMAP);
  for (uint16_t i=0; i<NUM_RXGRP; i++) {
    // check if group list is enabled
    uint16_t  bit = i%8, byte = i/8;
    if (0 == ((grouplist_bitmap[byte]>>bit) & 0x01))
      continue;
    // link group list
    GroupListElement grp(data(ADDR_RXGRP_0+i*RXGRP_OFFSET));
    if (! grp.linkGroupList(ctx.get<RXGroupList>(i), ctx)) {
      logError() << "Cannot link RX group list idx=" << i;
      return false;
    }
  }
  return true;
}


void
D868UVCodeplug::allocateZones() {
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Get byte and bit for zone
    uint16_t bit = i%8, byte = i/8;
    // if invalid -> skip
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate zone itself
    image(0).addElement(ADDR_ZONE+i*ZONE_OFFSET, ZONE_SIZE);
    image(0).addElement(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET, ZONE_NAME_SIZE);
  }
}

bool
D868UVCodeplug::encodeZones(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode zones
  unsigned zidx = 0;
  for (int i=0; i<ctx.config()->zones()->count(); i++) {
    // Clear name and channel list
    uint8_t  *name     = (uint8_t *)data(ADDR_ZONE_NAME + zidx*ZONE_NAME_OFFSET);
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE + zidx*ZONE_OFFSET);
    memset(name, 0, ZONE_NAME_SIZE);
    memset(channels, 0xff, ZONE_SIZE);
    if (ctx.config()->zones()->zone(i)->B()->count())
      encode_ascii(name, ctx.config()->zones()->zone(i)->name()+" A", 16, 0);
    else
      encode_ascii(name, ctx.config()->zones()->zone(i)->name(), 16, 0);
    // Handle list A
    for (int j=0; j<ctx.config()->zones()->zone(i)->A()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(ctx.config()->zones()->zone(i)->A()->get(j)));
    }

    if (! encodeZone(zidx, ctx.config()->zones()->zone(i), false, flags, ctx, err))
      return false;
    zidx++;

    if (! ctx.config()->zones()->zone(i)->B()->count())
      continue;

    // Process list B if present
    name     = (uint8_t *)data(ADDR_ZONE_NAME+zidx*ZONE_NAME_OFFSET);
    channels = (uint16_t *)data(ADDR_ZONE+zidx*ZONE_OFFSET);
    memset(name, 0, ZONE_NAME_SIZE);
    memset(channels, 0xff, ZONE_SIZE);
    encode_ascii(name, ctx.config()->zones()->zone(i)->name()+" B", 16, 0);
    // Handle list B
    for (int j=0; j<ctx.config()->zones()->zone(i)->B()->count(); j++) {
      channels[j] = qToLittleEndian(ctx.index(ctx.config()->zones()->zone(i)->B()->get(j)));
    }

    if (! encodeZone(zidx, ctx.config()->zones()->zone(i), true, flags, ctx, err))
      return false;
    zidx++;
  }
  return true;
}

bool
D868UVCodeplug::encodeZone(int i, Zone *zone, bool isB, const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(flags); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

bool
D868UVCodeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create zones
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Check if zone is enabled:
    uint16_t bit = i%8, byte = i/8;
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Determine whether this zone should be combined with the previous one
    QString zonename = decode_ascii(data(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET), 16, 0);
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // If enabled, create zone with name
    if (! extend_last_zone) {
      last_zone = new Zone(zonename);
      if (! decodeZone(i, last_zone, false, ctx, err)) {
        last_zone->deleteLater();
        return false;
      }
      // add to config
      ctx.config()->zones()->add(last_zone); ctx.add(last_zone, i);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
      if (! decodeZone(i, last_zone, true, ctx, err)) {
        last_zone->deleteLater();
        return false;
      }
    }
  }
  return true;
}

bool
D868UVCodeplug::decodeZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

bool
D868UVCodeplug::linkZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create zones
  uint8_t *zone_bitmap = data(ZONE_BITMAPS);
  QString last_zonename, last_zonebasename; Zone *last_zone = nullptr;
  bool extend_last_zone = false;
  for (uint16_t i=0; i<NUM_ZONES; i++) {
    // Check if zone is enabled:
    uint16_t bit = i%8, byte = i/8;
    if (0 == ((zone_bitmap[byte]>>bit) & 0x01))
      continue;
    // Determine whether this zone should be combined with the previous one
    QString zonename = decode_ascii(data(ADDR_ZONE_NAME+i*ZONE_NAME_OFFSET), 16, 0);
    QString zonebasename = zonename; zonebasename.chop(2);
    extend_last_zone = ( zonename.endsWith(" B") && last_zonename.endsWith(" A")
                         && (zonebasename == last_zonebasename)
                         && (nullptr != last_zone) && (0 == last_zone->B()->count()) );
    last_zonename = zonename;
    last_zonebasename = zonebasename;

    // If enabled, get zone
    if (! extend_last_zone) {
      last_zone = ctx.get<Zone>(i);
    } else {
      // when extending the last zone, chop its name to remove the "... A" part.
      last_zone->setName(last_zonebasename);
    }

    // link zone
    uint16_t *channels = (uint16_t *)data(ADDR_ZONE+i*ZONE_OFFSET);
    for (uint8_t j=0; j<NUM_CH_PER_ZONE; j++, channels++) {
      // If not enabled -> continue
      if (0xffff == *channels)
        continue;
      // Get channel index and check if defined
      uint16_t cidx = qFromLittleEndian(*channels);
      if (! ctx.has<Channel>(cidx))
        continue;
      // If defined -> add channel to zone obj
      if (extend_last_zone)
        last_zone->B()->add(ctx.get<Channel>(cidx));
      else
        last_zone->A()->add(ctx.get<Channel>(cidx));
    }

    if (! linkZone(i, last_zone, extend_last_zone, ctx, err))
      return false;
  }
  return true;
}

bool
D868UVCodeplug::linkZone(int i, Zone *zone, bool isB, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(i); Q_UNUSED(zone); Q_UNUSED(isB); Q_UNUSED(ctx); Q_UNUSED(err)
  return true;
}

void
D868UVCodeplug::allocateScanLists() {
  /*
   * Allocate scan lists
   */
  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (uint8_t i=0; i<NUM_SCAN_LISTS; i++) {
    // Get byte and bit for scan list, bank and bank_idx
    uint16_t bit = i%8, byte = i/8;
    uint8_t bank = (i/NUM_SCANLISTS_PER_BANK), bank_idx = (i%NUM_SCANLISTS_PER_BANK);
    // if disabled -> skip
    if (0 == ((scanlist_bitmap[byte]>>bit) & 0x01))
      continue;
    // Allocate scan lists indivitually
    uint32_t addr = SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + bank_idx*SCAN_LIST_OFFSET;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, SCAN_LIST_SIZE);
      memset(data(addr), 0xff, SCAN_LIST_SIZE);
    }
  }
}

bool
D868UVCodeplug::encodeScanLists(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(flags); Q_UNUSED(err)

  // Encode scan lists
  for (int i=0; i<ctx.config()->scanlists()->count(); i++) {
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, idx = i%NUM_SCANLISTS_PER_BANK;
    ScanListElement scan(data(SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + idx*SCAN_LIST_OFFSET));
    scan.fromScanListObj(ctx.config()->scanlists()->scanlist(i), ctx);
  }
  return true;
}

bool
D868UVCodeplug::createScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create scan lists
  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (unsigned i=0; i<NUM_SCAN_LISTS; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((scanlist_bitmap[byte]>>bit) & 0x01))
      continue;
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, bank_idx = i%NUM_SCANLISTS_PER_BANK;
    uint32_t addr = SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + bank_idx*SCAN_LIST_OFFSET;
    ScanListElement scanl(data(addr));
    // Create scanlist
    ScanList *obj = scanl.toScanListObj();
    ctx.config()->scanlists()->add(obj); ctx.add(obj, i);
  }
  return true;
}

bool
D868UVCodeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  uint8_t *scanlist_bitmap = data(SCAN_BITMAP);
  for (unsigned i=0; i<NUM_SCAN_LISTS; i++) {
    uint8_t byte=i/8, bit=i%8;
    if (0 == ((scanlist_bitmap[byte]>>bit) & 0x01))
      continue;
    uint8_t bank = i/NUM_SCANLISTS_PER_BANK, bank_idx = i%NUM_SCANLISTS_PER_BANK;
    uint32_t addr = SCAN_LIST_BANK_0 + bank*SCAN_LIST_BANK_OFFSET + bank_idx*SCAN_LIST_OFFSET;
    ScanListElement scanl(data(addr));
    // Create scanlist
    ScanList *obj = ctx.get<ScanList>(i);
    // Link scanlists immediately, all channels are defined already
    ctx.config()->scanlists()->add(obj); scanl.linkScanListObj(obj, ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateGeneralSettings() {
  image(0).addElement(ADDR_GENERAL_CONFIG, GENERAL_CONFIG_SIZE);
}

bool
D868UVCodeplug::encodeGeneralSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return GeneralSettingsElement(data(ADDR_GENERAL_CONFIG)).fromConfig(flags, ctx);
}

bool
D868UVCodeplug::decodeGeneralSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return GeneralSettingsElement(data(ADDR_GENERAL_CONFIG)).updateConfig(ctx);
}


void
D868UVCodeplug::allocateZoneChannelList() {
  image(0).addElement(ADDR_ZONE_CHANNELS, ZONE_CHANNELS_SIZE);
}


void
D868UVCodeplug::allocateDTMFNumbers() {
  image(0).addElement(ADDR_DTMF_NUMBERS, DTMF_NUMBERS_SIZE);
}


void
D868UVCodeplug::allocateBootSettings() {
  image(0).addElement(ADDR_BOOT_SETTINGS, BOOT_SETTINGS_SIZE);
}

bool
D868UVCodeplug::encodeBootSettings(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return BootSettingsElement(data(ADDR_BOOT_SETTINGS)).fromConfig(flags, ctx);
}

bool
D868UVCodeplug::decodeBootSettings(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  return BootSettingsElement(data(ADDR_BOOT_SETTINGS)).updateConfig(ctx);
}


void
D868UVCodeplug::allocateGPSSystems() {
  image(0).addElement(ADDR_GPS_SETTINGS, GPS_SETTINGS_SIZE);
  image(0).addElement(ADDR_GPS_MESSAGE, GPS_MESSAGE_SIZE);
}

bool
D868UVCodeplug::encodeGPSSystems(const Flags &flags, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DMRAPRSSettingsElement gps(data(ADDR_GPS_SETTINGS));
  return gps.fromConfig(flags, ctx);
}

bool
D868UVCodeplug::createGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  QSet<uint8_t> systems;
  // First find all GPS systems linked, that is referenced by any channel
  // Create channels
  uint8_t *channel_bitmap = data(CHANNEL_BITMAP);
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == ((channel_bitmap[byte]>>bit) & 0x01))
      continue;
    if (ctx.get<Channel>(i)->is<FMChannel>())
      continue;
    ChannelElement ch(data(CHANNEL_BANK_0 + bank*CHANNEL_BANK_OFFSET + idx*CHANNEL_SIZE));
    if (ch.txDigitalAPRS())
      systems.insert(ch.digitalAPRSSystemIndex());
  }
  // Then create all referenced GPS systems
  DMRAPRSSettingsElement gps(data(ADDR_GPS_SETTINGS));
  for (QSet<uint8_t>::iterator idx=systems.begin(); idx!=systems.end(); idx++)
    gps.createGPSSystem(*idx, ctx);
  return true;
}

bool
D868UVCodeplug::linkGPSSystems(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  DMRAPRSSettingsElement gps(data(ADDR_GPS_SETTINGS));
  // Then link all referenced GPS systems
  for (uint8_t i=0; i<8; i++) {
    if (! ctx.has<GPSSystem>(i))
      continue;
    gps.linkGPSSystem(i, ctx);
  }
  return true;
}


void
D868UVCodeplug::allocateSMSMessages() {
  // Prefab. SMS messages
  uint8_t *messages_bytemap = data(MESSAGE_BYTEMAP);
  unsigned message_count = 0;
  for (uint8_t i=0; i<NUM_MESSAGES; i++) {
    uint8_t bank = i/NUM_MESSAGES_PER_BANK;
    if (0xff == messages_bytemap[i])
      continue;
    message_count++;
    uint32_t addr = MESSAGE_BANK_0 + bank*MESSAGE_BANK_SIZE;
    if (nullptr == data(addr, 0)) {
      image(0).addElement(addr, MESSAGE_BANK_SIZE);
    }
  }
  if (message_count) {
    image(0).addElement(MESSAGE_INDEX_LIST, 0x10*message_count);
  }
}

void
D868UVCodeplug::allocateHotKeySettings() {
  // Allocate Hot Keys
  image(0).addElement(ADDR_HOTKEY, HOTKEY_SIZE);
}

void
D868UVCodeplug::allocateRepeaterOffsetSettings() {
  // Offset frequencies
  image(0).addElement(ADDR_OFFSET_FREQ, OFFSET_FREQ_SIZE);
}

void
D868UVCodeplug::allocateAlarmSettings() {
  // Alarm settings
  image(0).addElement(ADDR_ALARM_SETTING, ALARM_SETTING_SIZE);
  image(0).addElement(ADDR_ALARM_SETTING_EXT, ALARM_SETTING_EXT_SIZE);
}

void
D868UVCodeplug::allocateFMBroadcastSettings() {
  // FM broad-cast settings
  image(0).addElement(ADDR_FMBC, FMBC_SIZE+FMBC_VFO_SIZE);
}

void
D868UVCodeplug::allocate5ToneIDs() {
  // Allocate 5-tone functions
  uint8_t *bitmap = data(FIVE_TONE_ID_BITMAP);
  for (uint8_t i=0; i<NUM_FIVE_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_FIVE_TONE_ID_LIST + i*FIVE_TONE_ID_SIZE, FIVE_TONE_ID_SIZE);
  }
}

void
D868UVCodeplug::allocate5ToneFunctions() {
  image(0).addElement(ADDR_FIVE_TONE_FUNCTIONS, FIVE_TONE_FUNCTIONS_SIZE);
}

void
D868UVCodeplug::allocate5ToneSettings() {
  image(0).addElement(ADDR_FIVE_TONE_SETTINGS, FIVE_TONE_SETTINGS_SIZE);
}

void
D868UVCodeplug::allocate2ToneIDs() {
  // Allocate 2-tone encoding
  uint8_t *enc_bitmap = data(TWO_TONE_IDS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_IDS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (enc_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_IDS + i*TWO_TONE_ID_SIZE, TWO_TONE_ID_SIZE);
  }
}


void
D868UVCodeplug::allocate2ToneFunctions() {
  // Allocate 2-tone decoding
  uint8_t *dec_bitmap = data(TWO_TONE_FUNCTIONS_BITMAP);
  for (uint8_t i=0; i<NUM_TWO_TONE_FUNCTIONS; i++) {
    uint16_t  bit = i%8, byte = i/8;
    if (0 == (dec_bitmap[byte] & (1<<bit)))
      continue;
    image(0).addElement(ADDR_TWO_TONE_FUNCTIONS + i*TWO_TONE_FUNCTION_SIZE, TWO_TONE_FUNCTION_SIZE);
  }
}

void
D868UVCodeplug::allocate2ToneSettings() {
  image(0).addElement(ADDR_TWO_TONE_SETTINGS, TWO_TONE_SETTINGS_SIZE);
}


void
D868UVCodeplug::allocateDTMFSettings() {
  image(0).addElement(ADDR_DTMF_SETTINGS, DTMF_SETTINGS_SIZE);
}
