#include "gd73_codeplug.hh"
#include "config.hh"
#include "intermediaterepresentation.hh"


QVector<Signaling::Code> _ctcss_codes = {
  Signaling::SIGNALING_NONE, Signaling::CTCSS_67_0Hz, Signaling::SIGNALING_NONE,
  Signaling::CTCSS_71_9Hz, Signaling::CTCSS_74_4Hz, Signaling::CTCSS_77_0Hz,
  Signaling::CTCSS_79_7Hz, Signaling::CTCSS_82_5Hz, Signaling::CTCSS_85_4Hz,
  Signaling::CTCSS_88_5Hz, Signaling::CTCSS_91_5Hz, Signaling::CTCSS_94_8Hz,
  Signaling::CTCSS_97_4Hz, Signaling::CTCSS_100_0Hz, Signaling::CTCSS_103_5Hz,
  Signaling::CTCSS_107_2Hz, Signaling::CTCSS_110_9Hz, Signaling::CTCSS_114_8Hz,
  Signaling::CTCSS_118_8Hz, Signaling::CTCSS_123_0Hz, Signaling::CTCSS_127_3Hz,
  Signaling::CTCSS_131_8Hz, Signaling::CTCSS_136_5Hz, Signaling::CTCSS_141_3Hz,
  Signaling::CTCSS_146_2Hz, Signaling::CTCSS_151_4Hz, Signaling::CTCSS_156_7Hz,
  Signaling::SIGNALING_NONE, Signaling::CTCSS_162_2Hz, Signaling::SIGNALING_NONE,
  Signaling::CTCSS_167_9Hz, Signaling::SIGNALING_NONE, Signaling::CTCSS_173_8Hz,
  Signaling::SIGNALING_NONE, Signaling::CTCSS_179_9Hz, Signaling::SIGNALING_NONE,
  Signaling::CTCSS_186_2Hz, Signaling::SIGNALING_NONE, Signaling::CTCSS_192_8Hz,
  Signaling::SIGNALING_NONE, Signaling::SIGNALING_NONE, Signaling::CTCSS_203_5Hz,
  Signaling::SIGNALING_NONE, Signaling::CTCSS_210_7Hz, Signaling::CTCSS_218_1Hz,
  Signaling::CTCSS_225_7Hz, Signaling::SIGNALING_NONE, Signaling::CTCSS_233_6Hz,
  Signaling::CTCSS_241_8Hz, Signaling::CTCSS_250_3Hz, Signaling::SIGNALING_NONE
};

QVector<unsigned int> _dcs_codes = {
  23,  25,  26,  31,  32,  36,  43,  47,  51,  53,  54,  65,  71,  72,  73,  74,
 114, 115, 116, 122, 125, 131, 132, 134, 143, 145, 152, 155, 156, 162, 165, 172,
 174, 205, 212, 223, 225, 226, 243, 244, 245, 246, 251, 252, 255, 261, 263, 265,
 266, 271, 274, 306, 311, 315, 325, 331, 332, 343, 346, 351, 356, 364, 365, 371,
 411, 412, 413, 423, 431, 432, 445, 446, 452, 454, 455, 462, 464, 465, 466, 503,
 506, 516, 523, 526, 532, 546, 565, 606, 612, 624, 627, 631, 632, 645, 654, 662,
 703, 712, 723, 731, 732, 734, 743, 754
};


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::InformationElement
 * ********************************************************************************************* */
GD73Codeplug::InformationElement::InformationElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::InformationElement::InformationElement(uint8_t *ptr)
  : Element(ptr, InformationElement::size())
{
  // pass...
}

FrequencyRange
GD73Codeplug::InformationElement::frequencyRange() const {
  switch (getUInt8(Offset::frequencyRange())) {
  case 0x00: return FrequencyRange {Frequency::fromMHz(406.1), Frequency::fromMHz(470.0)}; break;
  case 0x01: return FrequencyRange {Frequency::fromMHz(446.0), Frequency::fromMHz(446.995)}; break;
  case 0x02: return FrequencyRange {Frequency::fromMHz(400.0), Frequency::fromMHz(470.0)}; break;
  }
  return FrequencyRange();
}

void
GD73Codeplug::InformationElement::setFrequencyRange(const FrequencyRange &range) {
  if ((Frequency::fromMHz(446.0)<=range.lower) && (range.upper <=Frequency::fromMHz(446.995)))
    setUInt8(Offset::frequencyRange(), 0x01);
  else if ((range.lower>=Frequency::fromMHz(406.1)) && (range.upper <=Frequency::fromMHz(470.0)))
    setUInt8(Offset::frequencyRange(), 0x00);
  else
    setUInt8(Offset::frequencyRange(), 0x02);
}


QDateTime
GD73Codeplug::InformationElement::timestamp() const {
  int year = ((int)getUInt8(Offset::dateCentury()))*100 +
      getUInt8(Offset::dateYear());
  return QDateTime(QDate(year, getUInt8(Offset::dateMonth()), getUInt8(Offset::dateDay())),
                   QTime(getUInt8(Offset::dateHour()), getUInt8(Offset::dateMinute()),
                         getUInt8(Offset::dateSecond())));
}

void
GD73Codeplug::InformationElement::setTimestamp(const QDateTime &timestamp) {
  setUInt8(Offset::dateCentury(), timestamp.date().year()/100);
  setUInt8(Offset::dateYear(), timestamp.date().year()%100);
  setUInt8(Offset::dateMonth(), timestamp.date().month());
  setUInt8(Offset::dateDay(), timestamp.date().day());
  setUInt8(Offset::dateHour(), timestamp.time().hour());
  setUInt8(Offset::dateMinute(), timestamp.time().minute());
  setUInt8(Offset::dateSecond(), timestamp.time().second());
}

QString
GD73Codeplug::InformationElement::serial() const {
  return readASCII(Offset::serial(), Limit::serial(), 0x00);
}

QString
GD73Codeplug::InformationElement::modelName() const {
  return readASCII(Offset::modelName(), Limit::modelName(), 0x00);
}

QString
GD73Codeplug::InformationElement::deviceID() const {
  return readASCII(Offset::deviceID(), Limit::deviceID(), 0x00);
}

QString
GD73Codeplug::InformationElement::modelNumber() const {
  return readASCII(Offset::modelNumber(), Limit::modelNumber(), 0x00);
}

QString
GD73Codeplug::InformationElement::softwareVersion() const {
  return readASCII(Offset::softwareVersion(), Limit::softwareVersion(), 0x00);
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::SettingsElement
 * ********************************************************************************************* */
GD73Codeplug::SettingsElement::SettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::SettingsElement::SettingsElement(uint8_t *ptr)
  : Element(ptr, SettingsElement::size())
{
  // pass...
}

QString
GD73Codeplug::SettingsElement::name() const {
  return readUnicode(Offset::name(), Limit::name(), 0x0000);
}
void
GD73Codeplug::SettingsElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::name(), 0x0000);
}

unsigned int
GD73Codeplug::SettingsElement::dmrID() const {
  return getUInt32_le(Offset::dmrId());
}
void
GD73Codeplug::SettingsElement::setDMRID(unsigned int id) {
  setUInt32_le(Offset::dmrId(), id);
}

unsigned int
GD73Codeplug::SettingsElement::vox() const {
  return getUInt8(Offset::voxLevel())*10/4;
}
void
GD73Codeplug::SettingsElement::setVOX(unsigned int level) {
  setUInt8(Offset::voxLevel(), level*4/10);
}

unsigned int
GD73Codeplug::SettingsElement::squelch() const {
  return getUInt8(Offset::squelchLevel())*10/9;
}
void
GD73Codeplug::SettingsElement::setSquelch(unsigned int level) {
  setUInt8(Offset::squelchLevel(), level*9/10);
}

bool
GD73Codeplug::SettingsElement::totIsSet() const {
  return 0 != getUInt8(Offset::tot());
}
Interval
GD73Codeplug::SettingsElement::tot() const  {
  unsigned int n = getUInt8(Offset::tot());
  if (0 == n)
    return Interval::fromSeconds(0);
  return Interval::fromSeconds(n*10+20);
}
void
GD73Codeplug::SettingsElement::setTOT(const Interval &interval) {
  Interval intv = Limit::tot().map(interval);
  if (intv.seconds()<20)
    setUInt8(Offset::tot(), 0);
  setUInt8(Offset::tot(), (intv.seconds()-20)/10);
}
void
GD73Codeplug::SettingsElement::clearTOT() {
  setUInt8(Offset::tot(), 0);
}

bool
GD73Codeplug::SettingsElement::txInterruptedEnabled() const {
  return 0 != getUInt8(Offset::txInterrupt());
}
void
GD73Codeplug::SettingsElement::enableTXInterrupt(bool enable) {
  setUInt8(Offset::txInterrupt(), enable ? 0x01 : 0x00);
}

bool
GD73Codeplug::SettingsElement::powerSaveEnabled() const {
  return 0x00 != getUInt8(Offset::powerSave());
}
void
GD73Codeplug::SettingsElement::enablePowerSave(bool enable) {
  setUInt8(Offset::powerSave(), enable ? 0x01 : 0x00);
}
Interval
GD73Codeplug::SettingsElement::powerSaveTimeout() const {
  return Interval::fromSeconds(getUInt8(Offset::powerSaveTimeout()));
}
void
GD73Codeplug::SettingsElement::setPowerSaveTimeout(const Interval &interval) {
  Interval intv = Limit::powerSaveTimeout().map(interval);
  setUInt8(Offset::powerSaveTimeout(), intv.seconds());
}

bool
GD73Codeplug::SettingsElement::readLockEnabled() const {
  return 0x00 != getUInt8(Offset::readLockEnable());
}
void
GD73Codeplug::SettingsElement::enableReadLock(bool enable) {
  setUInt8(Offset::readLockEnable(), enable ? 0x01 : 0x00);
}
QString
GD73Codeplug::SettingsElement::readLockPin() const {
  return readASCII(Offset::readLockPin(), Limit::pin(), 0x00);
}
void
GD73Codeplug::SettingsElement::setReadLockPin(const QString &pin) {
  writeASCII(Offset::readLockPin(), pin, Limit::pin(), 0x00);
}

bool
GD73Codeplug::SettingsElement::writeLockEnabled() const {
  return 0x00 != getUInt8(Offset::writeLockEnable());
}
void
GD73Codeplug::SettingsElement::enableWriteLock(bool enable) {
  setUInt8(Offset::writeLockEnable(), enable ? 0x01 : 0x00);
}
QString
GD73Codeplug::SettingsElement::writeLockPin() const {
  return readASCII(Offset::writeLockPin(), Limit::pin(), 0x00);
}
void
GD73Codeplug::SettingsElement::setWriteLockPin(const QString &pin) {
  writeASCII(Offset::writeLockPin(), pin, Limit::pin(), 0x00);
}

GD73Codeplug::SettingsElement::ChannelDisplayMode
GD73Codeplug::SettingsElement::channelDisplayMode() const {
  return (ChannelDisplayMode)getUInt8(Offset::channelDisplayMode());
}
void
GD73Codeplug::SettingsElement::setChannelDisplayMode(ChannelDisplayMode mode) {
  setUInt8(Offset::channelDisplayMode(), (unsigned int)mode);
}

unsigned int
GD73Codeplug::SettingsElement::dmrMicGain() const {
  return getUInt8(Offset::dmrMicGain())*9/5+1;
}
void
GD73Codeplug::SettingsElement::setDMRMicGain(unsigned int gain) {
  setUInt8(Offset::dmrMicGain(), (gain-1)*5/9);
}
unsigned int
GD73Codeplug::SettingsElement::fmMicGain() const {
  return getUInt8(Offset::fmMicGain())*9/5+1;
}
void
GD73Codeplug::SettingsElement::setFMMicGain(unsigned int gain) {
  setUInt8(Offset::fmMicGain(), (gain-1)*5/9);
}

Interval
GD73Codeplug::SettingsElement::loneWorkerResponseTimeout() const {
  return Interval::fromMinutes(getUInt16_le(Offset::loneWorkerResponseTimeout()));
}
void
GD73Codeplug::SettingsElement::setLoneWorkerResponseTimeout(const Interval &interval) {
  Interval intv = Limit::loneWorkerResponse().map(interval);
  setUInt16_le(Offset::loneWorkerResponseTimeout(), intv.minutes());
}
Interval
GD73Codeplug::SettingsElement::loneWorkerRemindPeriod() const {
  return Interval::fromSeconds(getUInt16_le(Offset::loneWorkerReminderPeriod()));
}
void
GD73Codeplug::SettingsElement::setLoneWorkerRemindPeriod(const Interval &interval) {
  Interval intv = Limit::loneWorkerRemindPeriod().map(interval);
  setUInt16_le(Offset::loneWorkerReminderPeriod(), intv.seconds());
}

GD73Codeplug::SettingsElement::BootDisplayMode
GD73Codeplug::SettingsElement::bootDisplayMode() const {
  return (BootDisplayMode)getUInt8(Offset::bootDisplayMode());
}
void
GD73Codeplug::SettingsElement::setBootDisplayMode(BootDisplayMode mode) {
  setUInt8(Offset::bootDisplayMode(), (unsigned int)mode);
}
QString
GD73Codeplug::SettingsElement::bootTextLine1() const {
  return readUnicode(Offset::bootTextLine1(), Limit::bootTextLine(), 0x0000);
}
void
GD73Codeplug::SettingsElement::setBootTextLine1(const QString &line) {
  writeUnicode(Offset::bootTextLine1(), line, Limit::bootTextLine(), 0x0000);
}
QString
GD73Codeplug::SettingsElement::bootTextLine2() const {
  return readUnicode(Offset::bootTextLine2(), Limit::bootTextLine(), 0x0000);
}
void
GD73Codeplug::SettingsElement::setBootTextLine2(const QString &line) {
  writeUnicode(Offset::bootTextLine2(), line, Limit::bootTextLine(), 0x0000);
}

bool
GD73Codeplug::SettingsElement::keyToneEnabled() const {
  return 0x00 != getUInt8(Offset::keyToneEnable());
}
void
GD73Codeplug::SettingsElement::enableKeyTone(bool enable) {
  setUInt8(Offset::keyToneEnable(), enable ? 0x01 : 0x00);
}
unsigned int
GD73Codeplug::SettingsElement::keyToneVolume() const {
  return getUInt8(Offset::keyToneVolume());
}
void
GD73Codeplug::SettingsElement::setKeyToneVolume(unsigned int vol) {
  setUInt8(Offset::keyToneVolume(), Limit::toneVolume().map(vol));
}

bool
GD73Codeplug::SettingsElement::lowBatteryToneEnabled() const {
  return 0x00 != getUInt8(Offset::lowBatToneEnable());
}
void
GD73Codeplug::SettingsElement::enableLowBatteryTone(bool enable) {
  setUInt8(Offset::lowBatToneEnable(), enable ? 0x01 : 0x00);
}
unsigned int
GD73Codeplug::SettingsElement::lowBatteryToneVolume() const {
  return getUInt8(Offset::lowBatToneVolume());
}
void
GD73Codeplug::SettingsElement::setLowBatteryToneVolume(unsigned int vol) {
  setUInt8(Offset::lowBatToneVolume(), Limit::toneVolume().map(vol));
}

Interval
GD73Codeplug::SettingsElement::longPressDuration() const {
  return Interval::fromMilliseconds(500*((unsigned int)getUInt8(Offset::longPressDuration())));
}
void
GD73Codeplug::SettingsElement::setLongPressDuration(const Interval &interval) {
  Interval intv = Limit::longPressDuration().map(interval);
  setUInt8(Offset::longPressDuration(), intv.milliseconds()/500);
}
GD73Codeplug::SettingsElement::KeyFunction
GD73Codeplug::SettingsElement::keyFunctionLongPressP1() const {
  return (KeyFunction)getUInt8(Offset::progFuncKey1LongPress());
}
void
GD73Codeplug::SettingsElement::setKeyFunctionLongPressP1(KeyFunction function) {
  setUInt8(Offset::progFuncKey1LongPress(), (unsigned int)function);
}
GD73Codeplug::SettingsElement::KeyFunction
GD73Codeplug::SettingsElement::keyFunctionShortPressP1() const {
  return (KeyFunction)getUInt8(Offset::progFuncKey1ShortPress());
}
void
GD73Codeplug::SettingsElement::setKeyFunctionShortPressP1(KeyFunction function) {
  setUInt8(Offset::progFuncKey1ShortPress(), (unsigned int)function);
}
GD73Codeplug::SettingsElement::KeyFunction
GD73Codeplug::SettingsElement::keyFunctionLongPressP2() const {
  return (KeyFunction)getUInt8(Offset::progFuncKey2LongPress());
}
void
GD73Codeplug::SettingsElement::setKeyFunctionLongPressP2(KeyFunction function) {
  setUInt8(Offset::progFuncKey2LongPress(), (unsigned int)function);
}
GD73Codeplug::SettingsElement::KeyFunction
GD73Codeplug::SettingsElement::keyFunctionShortPressP2() const {
  return (KeyFunction)getUInt8(Offset::progFuncKey2ShortPress());
}
void
GD73Codeplug::SettingsElement::setKeyFunctionShortPressP2(KeyFunction function) {
  setUInt8(Offset::progFuncKey2ShortPress(), (unsigned int)function);
}
GD73Codeplug::OneTouchSettingElement
GD73Codeplug::SettingsElement::oneTouch(unsigned int n) {
  return OneTouchSettingElement(
        _data+Offset::oneTouchSettings() + n*Offset::betweenOneTouchSettings());
}

bool
GD73Codeplug::SettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Create radio ID
  DMRRadioID *radioID = new DMRRadioID(name(), dmrID());
  ctx.config()->radioIDs()->add(radioID);
  ctx.config()->settings()->setDefaultId(radioID);
  ctx.add(radioID, 0);

  // Apply settings
  ctx.config()->settings()->setIntroLine1(bootTextLine1());
  ctx.config()->settings()->setIntroLine2(bootTextLine2());
  ctx.config()->settings()->setMicLevel(dmrMicGain());
  ctx.config()->settings()->setSquelch(squelch());
  ctx.config()->settings()->setVOX(vox());
  if (! totIsSet())
    ctx.config()->settings()->setTOT(0);
  else
    ctx.config()->settings()->setTOT(tot().seconds());

  // Get/add radioddity settings extension
  RadiodditySettingsExtension *ext = ctx.config()->settings()->radioddityExtension();
  if (! ext)
    ctx.config()->settings()->setRadioddityExtension(ext = new RadiodditySettingsExtension());

  ext->setLoneWorkerResponseTime(loneWorkerResponseTimeout());
  ext->setLoneWorkerReminderPeriod(loneWorkerRemindPeriod());

  return true;
}

bool
GD73Codeplug::SettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Get default radio ID
  if (nullptr == ctx.config()->settings()->defaultId()) {
    errMsg(err) << "Cannot encode default radio DMR ID, none is set.";
    return false;
  }

  setDMRID(ctx.config()->settings()->defaultId()->number());
  setName(ctx.config()->settings()->defaultId()->name());

  // Apply settings
  setBootTextLine1(ctx.config()->settings()->introLine1());
  setBootTextLine2(ctx.config()->settings()->introLine2());
  setDMRMicGain(ctx.config()->settings()->micLevel());
  setSquelch(ctx.config()->settings()->squelch());
  setVOX(ctx.config()->settings()->vox());
  if (ctx.config()->settings()->totDisabled())
    clearTOT();
  else
    setTOT(Interval::fromSeconds(ctx.config()->settings()->tot()));

  setUInt8(0x0024, 0x01);   // < Unknown byte at 0x0024
  //setUInt8(0x003c, 0x01);
  //setUInt8(0x003e, 0x01);

  // Get/add radioddity settings extension
  RadiodditySettingsExtension *ext = ctx.config()->settings()->radioddityExtension();
  if (nullptr == ext)
    return true;
  setLoneWorkerResponseTimeout(ext->loneWorkerResponseTime());
  setLoneWorkerRemindPeriod(ext->loneWorkerReminderPeriod());

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::OneTouchSettingElement
 * ********************************************************************************************* */
GD73Codeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::OneTouchSettingElement::OneTouchSettingElement(uint8_t *ptr)
  : Element(ptr, OneTouchSettingElement::size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::DMRSettingsElement
 * ********************************************************************************************* */
GD73Codeplug::DMRSettingsElement::DMRSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::DMRSettingsElement::DMRSettingsElement(uint8_t *ptr)
  : Element(ptr, DMRSettingsElement::size())
{
  // pass...
}

Interval
GD73Codeplug::DMRSettingsElement::callHangTime() const {
  return Interval::fromSeconds(getUInt8(Offset::callHangTime())+1);
}
void
GD73Codeplug::DMRSettingsElement::setCallHangTime(const Interval &intv) {
  setUInt8(Offset::callHangTime(), Limit::callHangTime().map(intv).seconds()-1);
}
Interval
GD73Codeplug::DMRSettingsElement::activeWaitTime() const {
  return Interval::fromMilliseconds(120 + ((unsigned int)getUInt8(Offset::activeWaitTime()))*5);
}
void
GD73Codeplug::DMRSettingsElement::setActiveWaitTime(const Interval &intv) {
  setUInt8(Offset::activeWaitTime(), (Limit::activeWaitTime().map(intv).milliseconds()-120)/5);
}

unsigned int
GD73Codeplug::DMRSettingsElement::activeRetries() const {
  return getUInt8(Offset::activeRetries());
}
void
GD73Codeplug::DMRSettingsElement::setActiveRetries(unsigned int count) {
  setUInt8(Offset::activeRetries(), Limit::activeRetires().map(count));
}

unsigned int
GD73Codeplug::DMRSettingsElement::txPreambles() const {
  return getUInt8(Offset::txPreambles());
}
void
GD73Codeplug::DMRSettingsElement::setTXPreambles(unsigned int count) {
  setUInt8(Offset::txPreambles(), Limit::txPreambles().map(count));
}

bool
GD73Codeplug::DMRSettingsElement::decodeDisableRadioEnabled() const {
  return 0x00 != getUInt8(Offset::decodeDisableRadio());
}
void
GD73Codeplug::DMRSettingsElement::enableDecodeDisableRadio(bool enable) {
  setUInt8(Offset::decodeDisableRadio(), enable ? 0x01 : 0x00);
}
bool
GD73Codeplug::DMRSettingsElement::decodeRadioCheckEnabled() const {
  return 0x00 != getUInt8(Offset::decodeCheckRadio());
}
void
GD73Codeplug::DMRSettingsElement::enableDecodeRadioCheck(bool enable) {
  setUInt8(Offset::decodeCheckRadio(), enable ? 0x01 : 0x00);
}
bool
GD73Codeplug::DMRSettingsElement::decodeEnableRadioEnabled() const {
  return 0x00 != getUInt8(Offset::decodeEnableRadio());
}
void
GD73Codeplug::DMRSettingsElement::enableDecodeEnableRadio(bool enable) {
  setUInt8(Offset::decodeEnableRadio(), enable ? 0x01 : 0x00);
}

bool
GD73Codeplug::DMRSettingsElement::updateConfig(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (nullptr == ctx.config()->settings()->radioddityExtension()) {
    ctx.config()->settings()->setRadioddityExtension(new RadiodditySettingsExtension());
  }
  auto ext = ctx.config()->settings()->radioddityExtension();

  ext->setPrivateCallHangTime(callHangTime());
  ext->setGroupCallHangTime(callHangTime());

  return true;
}

bool
GD73Codeplug::DMRSettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);

  if (nullptr == ctx.config()->settings()->radioddityExtension())
    return true;
  auto ext = ctx.config()->settings()->radioddityExtension();

  setCallHangTime(std::max(ext->privateCallHangTime(), ext->groupCallHangTime()));

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ContactBankElement
 * ********************************************************************************************* */
GD73Codeplug::ContactBankElement::ContactBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ContactBankElement::ContactBankElement(uint8_t *ptr)
  : Element(ptr, ContactBankElement::size())
{
  // pass...
}

bool
GD73Codeplug::ContactBankElement::createContacts(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt16_le(Offset::contactCount()), Limit::contactCount());
  for (unsigned int i=0; i<count; i++) {
    ContactElement contact(_data + Offset::contacts() + i*Offset::betweenContacts());
    DMRContact *contactObj = contact.toContact(ctx, err);
    if (nullptr == contactObj) {
      errMsg(err) << "Cannot decode " << i << "-th contact.";
      return false;
    }
    ctx.config()->contacts()->add(contactObj);
    ctx.add(contactObj, i);
  }
  return true;
}

bool
GD73Codeplug::ContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<DMRContact>(), Limit::contactCount());
  setUInt16_le(Offset::contactCount(), count);

  for (unsigned int i=0; i<count; i++) {
    ContactElement contact(_data + Offset::contacts() + i*Offset::betweenContacts());
    if (! ctx.has<DMRContact>(i)) {
      errMsg(err) << "Contact at index " << i << " not indexed.";
      errMsg(err) << "Cannot encode contact bank.";
      return false;
    }
    DMRContact *contactObj = ctx.get<DMRContact>(i);
    if (! contact.encode(contactObj, ctx, err)) {
      errMsg(err) << "Cannot encode contact at index " << i << ".";
      errMsg(err) << "Cannot encode contact bank.";
      return false;
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ContactElement
 * ********************************************************************************************* */
GD73Codeplug::ContactElement::ContactElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ContactElement::ContactElement(uint8_t *ptr)
  : Element(ptr, ContactElement::size())
{
  // pass...
}

QString
GD73Codeplug::ContactElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
GD73Codeplug::ContactElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

DMRContact::Type
GD73Codeplug::ContactElement::type() const {
  switch (getUInt8(Offset::type())) {
  case 0: return DMRContact::GroupCall;
  case 1: return DMRContact::PrivateCall;
  case 2: return DMRContact::AllCall;
  }
  return DMRContact::PrivateCall;
}
void
GD73Codeplug::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::GroupCall: setUInt8(Offset::type(), 0); break;
  case DMRContact::PrivateCall: setUInt8(Offset::type(), 1); break;
  case DMRContact::AllCall: setUInt8(Offset::type(), 2); break;
  }
}

unsigned int
GD73Codeplug::ContactElement::id() const {
  return getUInt32_le(Offset::id());
}
void
GD73Codeplug::ContactElement::setID(unsigned int id) {
  setUInt32_le(Offset::id(), id);
}

DMRContact *
GD73Codeplug::ContactElement::toContact(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new DMRContact(type(), name(), id());
}

bool
GD73Codeplug::ContactElement::encode(const DMRContact *contact, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  setType(contact->type());
  setName(contact->name());
  setID(contact->number());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::GroupListBankElement
 * ********************************************************************************************* */
GD73Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : Element(ptr, GroupListBankElement::size())
{
  // pass...
}

bool
GD73Codeplug::GroupListBankElement::createGroupLists(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::memberCount()), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    GroupListElement gl(_data+Offset::members() + i*Offset::betweenMembers());
    RXGroupList *glObj = gl.toGroupList(ctx, err);
    if (nullptr == glObj) {
      errMsg(err) << "Cannot decode " << i << "-th group list.";
      return false;
    }
    ctx.config()->rxGroupLists()->add(glObj);
    ctx.add(glObj, i);
  }
  return true;
}

bool
GD73Codeplug::GroupListBankElement::linkGroupLists(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<RXGroupList>(), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    GroupListElement gl(_data+Offset::members() + i*Offset::betweenMembers());
    RXGroupList *glObj = ctx.get<RXGroupList>(i);
    if (! gl.linkGroupList(glObj, ctx, err)) {
      errMsg(err) << "Cannot link " << i << "-th group list.";
      return false;
    }
  }
  return true;
}

bool
GD73Codeplug::GroupListBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<RXGroupList>(), Limit::memberCount());
  setUInt8(Offset::memberCount(), count);
  for (unsigned int i=0; i<count; i++) {
    GroupListElement gl(_data+Offset::members() + i*Offset::betweenMembers());
    if (! ctx.has<RXGroupList>(i)) {
      errMsg(err) << "Group list at index " << i << " is not indexed.";
      return false;
    }
    if (!gl.encode(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot encode group list at index " << i << ".";
      return false;
    }
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::GroupListElement
 * ********************************************************************************************* */
GD73Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : Element(ptr, GroupListElement::size())
{
  // pass...
}

QString
GD73Codeplug::GroupListElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
GD73Codeplug::GroupListElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

RXGroupList *
GD73Codeplug::GroupListElement::toGroupList(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new RXGroupList(name());
}

bool
GD73Codeplug::GroupListElement::linkGroupList(RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::memberCount()), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    unsigned int idx = getUInt16_le(Offset::members() + i*Offset::betweenMembers());
    if (0 == idx)
      continue;
    if (! ctx.has<DMRContact>(idx-1)) {
      errMsg(err) << "Cannot resolve contact at index " << idx-1 << ".";
      return false;
    }
    lst->addContact(ctx.get<DMRContact>(idx-1));
  }
  return true;
}

bool
GD73Codeplug::GroupListElement::encode(RXGroupList *lst, Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)lst->count(), Limit::memberCount());
  setName(lst->name());
  setUInt8(Offset::memberCount(), count);

  for (unsigned int i=0; i<count; i++) {
    if (0 > ctx.index(lst->contact(i))) {
      errMsg(err) << "Cannot resolve index of contact '" << lst->contact(i)->name() << "'.";
      return false;
    }
    setUInt16_le(Offset::members() + i*Offset::betweenMembers(), ctx.index(lst->contact(i))+1);
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ChannelBankElement
 * ********************************************************************************************* */
GD73Codeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : Element(ptr, ChannelBankElement::size())
{
  // pass...
}

bool
GD73Codeplug::ChannelBankElement::createChannels(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::channelCount()), Limit::channelCount());
  for (unsigned int i=0; i<count; i++) {
    ChannelElement ch(_data + Offset::channels() + i*Offset::betweenChannels());
    Channel *chObj = ch.toChannel(ctx, err);
    if (nullptr == chObj) {
      errMsg(err) << "Cannot decode channel at index " << i << ".";
      return false;
    }
    ctx.config()->channelList()->add(chObj);
    ctx.add(chObj, i);
  }
  return true;
}

bool
GD73Codeplug::ChannelBankElement::linkChannels(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<Channel>(), Limit::channelCount());
  for (unsigned int i=0; i<count; i++) {
    ChannelElement ch(_data + Offset::channels() + i*Offset::betweenChannels());
    Channel *chObj = ctx.get<Channel>(i);
    if (! ch.linkChannel(chObj, ctx, err)) {
      errMsg(err) << "Cannot link channel at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
GD73Codeplug::ChannelBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<Channel>(), Limit::channelCount());
  setUInt16_le(Offset::channelCount(), count);
  for (unsigned int i=0; i<count; i++) {
    ChannelElement ch(_data + Offset::channels() + i*Offset::betweenChannels());
    Channel *chObj = ctx.get<Channel>(i);
    if (! ch.encode(chObj, ctx, err)) {
      errMsg(err) << "Cannot encode channel at index " << i << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ChannelElement
 * ********************************************************************************************* */
GD73Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : Element(ptr, ChannelElement::size())
{
  // pass...
}

QString
GD73Codeplug::ChannelElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
GD73Codeplug::ChannelElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

FMChannel::Bandwidth
GD73Codeplug::ChannelElement::bandwidth() const {
  switch (getUInt8(Offset::bandwidth())) {
  case 0: return FMChannel::Bandwidth::Narrow;
  case 1: return FMChannel::Bandwidth::Wide;
  }
  return FMChannel::Bandwidth::Narrow;
}
void
GD73Codeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bandwidth) {
  switch (bandwidth) {
  case FMChannel::Bandwidth::Narrow: setUInt8(Offset::bandwidth(), 0); break;
  case FMChannel::Bandwidth::Wide: setUInt8(Offset::bandwidth(), 1); break;
  }
}

bool
GD73Codeplug::ChannelElement::hasScanListIndex() const {
  return 0 != getUInt8(Offset::scanList());
}
unsigned int
GD73Codeplug::ChannelElement::scanListIndex() const {
  return getUInt8(Offset::scanList())-1;
}
void
GD73Codeplug::ChannelElement::setScanListIndex(unsigned int idx) {
  setUInt8(Offset::scanList(), idx+1);
}
void
GD73Codeplug::ChannelElement::clearScanListIndex() {
  setUInt8(Offset::scanList(), 0);
}

GD73Codeplug::ChannelElement::Type
GD73Codeplug::ChannelElement::type() const {
  return (Type) getUInt8(Offset::channelType());
}
void
GD73Codeplug::ChannelElement::setType(Type type) {
  setUInt8(Offset::channelType(), (unsigned int)type);
}

bool
GD73Codeplug::ChannelElement::talkaroundEnabled() const {
  return 0x00 != getUInt8(Offset::talkaround());
}
void
GD73Codeplug::ChannelElement::enableTalkaround(bool enable) {
  setUInt8(Offset::talkaround(), enable ? 0x01 : 0x00);
}

bool
GD73Codeplug::ChannelElement::rxOnly() const {
  return 0x00 != getUInt8(Offset::rxOnly());
}
void
GD73Codeplug::ChannelElement::enableRXOnly(bool enable) {
  setUInt8(Offset::rxOnly(), enable ? 0x01 : 0x00);
}

bool
GD73Codeplug::ChannelElement::scanAutoStartEnabled() const {
  return 0x00 != getUInt8(Offset::scanAutoStart());
}
void
GD73Codeplug::ChannelElement::enableScanAutoStart(bool enable) {
  setUInt8(Offset::scanAutoStart(), enable ? 0x01 : 0x00);
}

Frequency
GD73Codeplug::ChannelElement::rxFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::rxFrequency()));
}
void
GD73Codeplug::ChannelElement::setRXFrequency(const Frequency &f) {
  setUInt32_le(Offset::rxFrequency(), f.inHz());
}

Frequency
GD73Codeplug::ChannelElement::txFrequency() const {
  return Frequency::fromHz(getUInt32_le(Offset::txFrequency()));
}
void
GD73Codeplug::ChannelElement::setTXFrequency(const Frequency &f) {
  setUInt32_le(Offset::txFrequency(), f.inHz());
}

bool
GD73Codeplug::ChannelElement::hasDTMFPTTSettingsIndex() const {
  return 0x00 != getUInt8(Offset::dtmfPTTSettingsIndex());
}
unsigned int
GD73Codeplug::ChannelElement::dtmfPTTSettingsIndex() const {
  return getUInt8(Offset::dtmfPTTSettingsIndex())-1;
}
void
GD73Codeplug::ChannelElement::setDTMFPTTSettingsIndex(unsigned int idx) {
  setUInt8(Offset::dtmfPTTSettingsIndex(), idx+1);
}
void
GD73Codeplug::ChannelElement::clearDTMFPTTSettingsIndex() {
  setUInt8(Offset::dtmfPTTSettingsIndex(), 0);
}

Channel::Power
GD73Codeplug::ChannelElement::power() const {
  switch (getUInt8(Offset::power())) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::High;
  }
  return Channel::Power::Low;
}
void
GD73Codeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low:
  case Channel::Power::Mid:
    setUInt8(Offset::power(), 0);
    break;
  case Channel::Power::High:
  case Channel::Power::Max:
    setUInt8(Offset::power(), 1);
    break;
  }
}

GD73Codeplug::ChannelElement::Admit
GD73Codeplug::ChannelElement::admit() const {
  return (Admit) getUInt8(Offset::admid());
}
void
GD73Codeplug::ChannelElement::setAdmit(Admit admit) {
  setUInt8(Offset::admid(), (unsigned int) admit);
}

Signaling::Code
GD73Codeplug::ChannelElement::rxTone() const {
  int mode = getUInt8(Offset::rxToneMode());
  int ctcss_code = getUInt8(Offset::rxCTCSS());
  int dcs_code = getUInt8(Offset::rxDCS());
  if (0 == mode)
    return Signaling::SIGNALING_NONE;
  if (1 == mode) {
    if (ctcss_code >= _ctcss_codes.size())
      return Signaling::SIGNALING_NONE;
    return _ctcss_codes[ctcss_code];
  }
  if (dcs_code >= _dcs_codes.size())
    return Signaling::SIGNALING_NONE;
  return Signaling::fromDCSNumber(_dcs_codes[dcs_code], 3 == mode);
}
void
GD73Codeplug::ChannelElement::setRXTone(Signaling::Code code) {
  int mode = 0, ctcss_code = 0, dcs_code = 0;
  if (Signaling::isCTCSS(code)) {
    mode = 1;
    ctcss_code = _ctcss_codes.indexOf(code);
  } else if (Signaling::isDCSNormal(code)) {
    mode = 2;
    dcs_code = _dcs_codes.indexOf(Signaling::toDCSNumber(code));
  } else if (Signaling::isDCSInverted(code)) {
    mode = 3;
    dcs_code = _dcs_codes.indexOf(Signaling::toDCSNumber(code));
  }
  setUInt8(Offset::rxToneMode(), mode);
  setUInt8(Offset::rxCTCSS(), ctcss_code);
  setUInt8(Offset::rxDCS(), dcs_code);
}

Signaling::Code
GD73Codeplug::ChannelElement::txTone() const {
  int mode = getUInt8(Offset::txToneMode());
  int ctcss_code = getUInt8(Offset::txCTCSS());
  int dcs_code = getUInt8(Offset::txDCS());
  if (0 == mode)
    return Signaling::SIGNALING_NONE;
  if (1 == mode) {
    if (ctcss_code >= _ctcss_codes.size())
      return Signaling::SIGNALING_NONE;
    return _ctcss_codes[ctcss_code];
  }
  if (dcs_code >= _dcs_codes.size())
    return Signaling::SIGNALING_NONE;
  return Signaling::fromDCSNumber(_dcs_codes[dcs_code], 3 == mode);
}
void
GD73Codeplug::ChannelElement::setTXTone(Signaling::Code code) {
  int mode = 0, ctcss_code = 0, dcs_code = 0;
  if (Signaling::isCTCSS(code)) {
    mode = 1;
    ctcss_code = _ctcss_codes.indexOf(code);
  } else if (Signaling::isDCSNormal(code)) {
    mode = 2;
    dcs_code = _dcs_codes.indexOf(Signaling::toDCSNumber(code));
  } else if (Signaling::isDCSInverted(code)) {
    mode = 3;
    dcs_code = _dcs_codes.indexOf(Signaling::toDCSNumber(code));
  }
  setUInt8(Offset::txToneMode(), mode);
  setUInt8(Offset::txCTCSS(), ctcss_code);
  setUInt8(Offset::txDCS(), dcs_code);
}

DMRChannel::TimeSlot
GD73Codeplug::ChannelElement::timeSlot() const {
  switch (getUInt8(Offset::timeslot())) {
  case 0:
  case 2:
    return DMRChannel::TimeSlot::TS1;
  case 1:
  case 3:
    return DMRChannel::TimeSlot::TS2;
  }
  return DMRChannel::TimeSlot::TS1;
}
void
GD73Codeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  switch (ts) {
  case DMRChannel::TimeSlot::TS1: setUInt8(Offset::timeslot(), 0); break;
  case DMRChannel::TimeSlot::TS2: setUInt8(Offset::timeslot(), 1); break;
  }
}

unsigned int
GD73Codeplug::ChannelElement::colorCode() const {
  return getUInt8(Offset::colorcode());
}
void
GD73Codeplug::ChannelElement::setColorCode(unsigned int cc) {
  setUInt8(Offset::colorcode(), std::min(15u, cc));
}

bool
GD73Codeplug::ChannelElement::groupListMatchesContact() const {
  return 0 == getUInt8(Offset::groupListIndex());
}
bool
GD73Codeplug::ChannelElement::groupListAllMatch() const {
  return 1 == getUInt8(Offset::groupListIndex());
}
unsigned int
GD73Codeplug::ChannelElement::groupListIndex() const {
  return getUInt8(Offset::groupListIndex())-2;
}
void
GD73Codeplug::ChannelElement::setGroupListIndex(unsigned int idx) {
  setUInt8(Offset::groupListIndex(), idx+2);
}
void
GD73Codeplug::ChannelElement::setGroupListAllMatch() {
  setUInt8(Offset::groupListIndex(), 1);
}
void
GD73Codeplug::ChannelElement::setGroupListMatchesContact() {
  setUInt8(Offset::groupListIndex(), 0);
}

bool
GD73Codeplug::ChannelElement::hasTXContact() const {
  return 0 != getUInt16_le(Offset::contactIndex());
}
unsigned int
GD73Codeplug::ChannelElement::txContactIndex() const {
  return getUInt16_le(Offset::contactIndex())-1;
}
void
GD73Codeplug::ChannelElement::setTXContactIndex(unsigned int idx) {
  setUInt16_le(Offset::contactIndex(), idx+1);
}
void
GD73Codeplug::ChannelElement::clearTXContactIndex() {
  setUInt16_le(Offset::contactIndex(), 0);
}

bool
GD73Codeplug::ChannelElement::hasEmergencySystemIndex() const {
  return 0 != getUInt8(Offset::emergencySystemIndex());
}
unsigned int
GD73Codeplug::ChannelElement::emergencySystemIndex() const {
  return getUInt8(Offset::emergencySystemIndex())-1;
}
void
GD73Codeplug::ChannelElement::setEmergencySystemIndex(unsigned int idx) {
  setUInt8(Offset::emergencySystemIndex(), idx+1);
}
void
GD73Codeplug::ChannelElement::clearEmergencySystemIndex() {
  setUInt8(Offset::emergencySystemIndex(), 0);
}

bool
GD73Codeplug::ChannelElement::hasEncryptionKeyIndex() const {
  return 0 != getUInt8(Offset::encryptionKeyIndex());
}
unsigned int
GD73Codeplug::ChannelElement::encryptionKeyIndex() const {
  return getUInt8(Offset::encryptionKeyIndex())-1;
}
void
GD73Codeplug::ChannelElement::setEncryptionKeyIndex(unsigned int idx) {
  setUInt8(Offset::encryptionKeyIndex(), idx+1);
}
void
GD73Codeplug::ChannelElement::clearEncryptionKeyIndex() {
  setUInt8(Offset::encryptionKeyIndex(), 0);
}

Channel *
GD73Codeplug::ChannelElement::toChannel(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)

  Channel *ch = nullptr;

  if (Type::FM == type()) {
    FMChannel *fm = new FMChannel(); ch = fm;
    switch (admit()) {
    case Admit::Always: fm->setAdmit(FMChannel::Admit::Always); break;
    case Admit::CC_CTCSS: fm->setAdmit(FMChannel::Admit::Tone); break;
    case Admit::Free: fm->setAdmit(FMChannel::Admit::Free); break;
    }
    fm->setBandwidth(bandwidth());
    fm->setSquelchDefault();
  } else if (Type::DMR == type()) {
    DMRChannel *dmr = new DMRChannel(); ch = dmr;
    switch (admit()) {
    case Admit::Always: dmr->setAdmit(DMRChannel::Admit::Always); break;
    case Admit::CC_CTCSS: dmr->setAdmit(DMRChannel::Admit::ColorCode); break;
    case Admit::Free: dmr->setAdmit(DMRChannel::Admit::Free); break;
    }
    dmr->setColorCode(colorCode());
    dmr->setTimeSlot(timeSlot());
    dmr->setRadioIdObj(DefaultRadioID::get());
  }

  ch->setName(name());
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());
  ch->setRXOnly(rxOnly());

  return ch;
}


bool
GD73Codeplug::ChannelElement::linkChannel(Channel *ch, Context &ctx, const ErrorStack &err) {

  if (Type::DMR == type()) {
    DMRChannel *dmr = ch->as<DMRChannel>();
    if (hasTXContact()) {
      if (! ctx.has<DMRContact>(txContactIndex())) {
        errMsg(err) << "Cannot link channel '" << name() << "', cannot resolve contact index "
                    << txContactIndex() << ".";
        return false;
      }
      dmr->setTXContactObj(ctx.get<DMRContact>(txContactIndex()));
    }
    if ((! groupListAllMatch()) && (! groupListMatchesContact())) {
      if (! ctx.has<RXGroupList>(groupListIndex())) {
        errMsg(err) << "Cannot link channel '" << name() << "', cannot resolve group list index "
                    << groupListIndex() << ".";
        return false;
      }
      dmr->setGroupListObj(ctx.get<RXGroupList>(groupListIndex()));
    }
  }

  if (hasScanListIndex()) {
    if (! ctx.has<ScanList>(scanListIndex())) {
      errMsg(err) << "Cannot link channel '" << name() << "', cannot resolve scanlist index "
                  << scanListIndex() << ".";
      return false;
    }
    ch->setScanList(ctx.get<ScanList>(scanListIndex()));
  }

  return true;
}


bool
GD73Codeplug::ChannelElement::encode(Channel *ch, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err)

  // Encode common stuff
  setName(ch->name());
  setRXFrequency(ch->rxFrequency());
  setTXFrequency(ch->txFrequency());
  enableRXOnly(ch->rxOnly());
  if (! ch->scanListRef()->isNull())
    setScanListIndex(ctx.index(ch->scanList()));

  // Dispatch by type
  if (ch->is<DMRChannel>()) {
    DMRChannel *dmr = ch->as<DMRChannel>();
    setType(ChannelElement::Type::DMR);
    if (! dmr->contact()->isNull())
      setTXContactIndex(ctx.index(dmr->txContactObj()));
    if (dmr->groupList()->isNull())
      setGroupListAllMatch();
    else
      setGroupListIndex(ctx.index(dmr->groupListObj()));
    switch (dmr->admit()) {
    case DMRChannel::Admit::Always: setAdmit(Admit::Always); break;
    case DMRChannel::Admit::ColorCode: setAdmit(Admit::CC_CTCSS); break;
    case DMRChannel::Admit::Free: setAdmit(Admit::Free); break;
    }
    setColorCode(dmr->colorCode());
    setTimeSlot(dmr->timeSlot());
  } else if (ch->is<FMChannel>()) {
    FMChannel *fm = ch->as<FMChannel>();
    switch (fm->admit()) {
    case FMChannel::Admit::Always: setAdmit(Admit::Always); break;
    case FMChannel::Admit::Tone: setAdmit(Admit::CC_CTCSS); break;
    case FMChannel::Admit::Free: setAdmit(Admit::Free); break;
    }
    setBandwidth(fm->bandwidth());
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ZoneBankElement
 * ********************************************************************************************* */
GD73Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : Element(ptr, ZoneBankElement::size())
{
  // pass...
}

bool
GD73Codeplug::ZoneBankElement::createZones(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::zoneCount()), Limit::zoneCount());
  for (unsigned int i=0; i<count; i++) {
    ZoneElement zone(_data + Offset::zones() + i*Offset::betweenZones());
    Zone *zoneObj = zone.toZone(ctx, err);
    if (nullptr == zoneObj) {
      errMsg(err) << "Cannot decode zone at index " << i << ".";
      return false;
    }
    ctx.config()->zones()->add(zoneObj);
    ctx.add(zoneObj, i);
  }
  return true;
}

bool
GD73Codeplug::ZoneBankElement::linkZones(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<Zone>(), Limit::zoneCount());
  for (unsigned int i=0; i<count; i++) {
    ZoneElement zone(_data+Offset::zones() + i*Offset::betweenZones());
    Zone *zoneObj = ctx.get<Zone>(i);
    if (! zone.linkZone(zoneObj, ctx, err)) {
      errMsg(err) << "Cannot link zone at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
GD73Codeplug::ZoneBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<Zone>(), Limit::zoneCount());
  setUInt8(Offset::zoneCount(), count);
  for (unsigned int i=0; i<count; i++) {
    ZoneElement zone(_data+Offset::zones() + i*Offset::betweenZones());
    Zone *zoneObj = ctx.get<Zone>(i);
    if (! zone.encode(zoneObj, ctx, err)) {
      errMsg(err) << "Cannot encode zone at index " << i << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ZoneElement
 * ********************************************************************************************* */
GD73Codeplug::ZoneElement::ZoneElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : Element(ptr, ZoneElement::size())
{
  // pass...
}

QString
GD73Codeplug::ZoneElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
GD73Codeplug::ZoneElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

Zone *
GD73Codeplug::ZoneElement::toZone(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new Zone(name());
}

bool
GD73Codeplug::ZoneElement::linkZone(Zone *zone, Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::channeCount()), Limit::channelCount());
  for (unsigned int i=0; i<count; i++) {
    unsigned int index = getUInt16_le(Offset::channelIndices() + i*Offset::betweenChannelIndices());
    if (0 == index)
      continue;
    if (! ctx.has<Channel>(index-1)) {
      errMsg(err) << "Cannot link zone '" << zone->name() << "': Channel at index " << (index-1)
                  << " not known.";
      return false;
    }
    zone->A()->add(ctx.get<Channel>(index-1));
  }
  return true;
}

bool
GD73Codeplug::ZoneElement::encode(Zone *zone, Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)zone->A()->count(), Limit::channelCount());
  setUInt8(Offset::channeCount(), count);
  setName(zone->name());

  for (unsigned int i=0; i<count; i++) {
    if (0 > ctx.index(zone->A()->get(i)->as<Channel>())) {
      errMsg(err) << "Cannot find index for channel " << zone->A()->get(i)->name()
                  << " in zone " << zone->name() << ".";
      return false;
    }
    setUInt16_le(Offset::channelIndices() + i*Offset::betweenChannelIndices(),
                 ctx.index(zone->A()->get(i)->as<Channel>())+1);
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ScanListBankElement
 * ********************************************************************************************* */
GD73Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ScanListBankElement::ScanListBankElement(uint8_t *ptr)
  : Element(ptr, ScanListBankElement::size())
{
  // pass...
}

bool
GD73Codeplug::ScanListBankElement::createScanLists(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::memberCount()), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    ScanListElement lst(_data + Offset::members() + i*Offset::betweenMembers());
    ScanList *lstObj = lst.toScanList(ctx, err);
    if (nullptr == lstObj) {
      errMsg(err) << "Cannot decode scan list at index " << i << ".";
      return false;
    }
    ctx.config()->scanlists()->add(lstObj);
    ctx.add(lstObj, i);
  }
  return true;
}

bool
GD73Codeplug::ScanListBankElement::linkScanLists(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<ScanList>(), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    ScanListElement lst(_data + Offset::members() + i*Offset::betweenMembers());
    ScanList *lstObj = ctx.get<ScanList>(i);
    if (! lst.linkScanList(lstObj, ctx, err)) {
      errMsg(err) << "Cannot link scan list at index " << i << ".";
      return false;
    }
  }
  return true;
}

bool
GD73Codeplug::ScanListBankElement::encode(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min(ctx.count<ScanList>(), Limit::memberCount());
  setUInt8(Offset::memberCount(), count);
  for (unsigned int i=0; i<count; i++) {
    ScanListElement lst(_data + Offset::members() + i*Offset::betweenMembers());
    ScanList *lstObj = ctx.get<ScanList>(i);
    if (! lst.encode(lstObj, ctx, err)) {
      errMsg(err) << "Cannot encode scan list at index " << i << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of GD73Codeplug::ScanListElement
 * ********************************************************************************************* */
GD73Codeplug::ScanListElement::ScanListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

GD73Codeplug::ScanListElement::ScanListElement(uint8_t *ptr)
  : Element(ptr, ScanListElement::size())
{
  // pass...
}

QString
GD73Codeplug::ScanListElement::name() const {
  return readUnicode(Offset::name(), Limit::nameLength(), 0x0000);
}
void
GD73Codeplug::ScanListElement::setName(const QString &name) {
  writeUnicode(Offset::name(), name, Limit::nameLength(), 0x0000);
}

GD73Codeplug::ScanListElement::ChannelMode
GD73Codeplug::ScanListElement::primaryChannelMode() const {
  return (ChannelMode)getUInt8(Offset::priChannel1Mode());
}
void
GD73Codeplug::ScanListElement::setPrimaryChannelMode(ChannelMode mode) {
  setUInt8(Offset::priChannel1Mode(), (unsigned int)mode);
}
bool
GD73Codeplug::ScanListElement::hasPrimaryZoneIndex() const {
  return 0 != getUInt8(Offset::priChannel1Zone());
}
unsigned int
GD73Codeplug::ScanListElement::primaryZoneIndex() const {
  return getUInt8(Offset::priChannel1Zone())-1;
}
void
GD73Codeplug::ScanListElement::setPrimaryZoneIndex(unsigned int idx) {
  setUInt8(Offset::priChannel1Zone(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearPrimaryZoneIndex() {
  setUInt8(Offset::priChannel1Zone(), 0);
}
bool
GD73Codeplug::ScanListElement::hasPrimaryChannelIndex() const {
  return 0 != getUInt8(Offset::priChannel1Channel());
}
unsigned int
GD73Codeplug::ScanListElement::primaryChannelIndex() const {
  return getUInt8(Offset::priChannel1Channel())-1;
}
void
GD73Codeplug::ScanListElement::setPrimaryChannelIndex(unsigned int idx) {
  setUInt8(Offset::priChannel1Channel(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearPrimaryChannelIndex() {
  setUInt8(Offset::priChannel1Channel(), 0);
}

GD73Codeplug::ScanListElement::ChannelMode
GD73Codeplug::ScanListElement::secondaryChannelMode() const {
  return (ChannelMode)getUInt8(Offset::priChannel2Mode());
}
void
GD73Codeplug::ScanListElement::setSecondaryChannelMode(ChannelMode mode) {
  setUInt8(Offset::priChannel2Mode(), (unsigned int)mode);
}
bool
GD73Codeplug::ScanListElement::hasSecondaryZoneIndex() const {
  return 0 != getUInt8(Offset::priChannel2Zone());
}
unsigned int
GD73Codeplug::ScanListElement::secondaryZoneIndex() const {
  return getUInt8(Offset::priChannel2Zone())-1;
}
void
GD73Codeplug::ScanListElement::setSecondaryZoneIndex(unsigned int idx) {
  setUInt8(Offset::priChannel2Zone(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearSecondaryZoneIndex() {
  setUInt8(Offset::priChannel2Zone(), 0);
}
bool
GD73Codeplug::ScanListElement::hasSecondaryChannelIndex() const {
  return 0 != getUInt8(Offset::priChannel2Channel());
}
unsigned int
GD73Codeplug::ScanListElement::secondaryChannelIndex() const {
  return getUInt8(Offset::priChannel2Channel())-1;
}
void
GD73Codeplug::ScanListElement::setSecondaryChannelIndex(unsigned int idx) {
  setUInt8(Offset::priChannel2Channel(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearSecondaryChannelIndex() {
  setUInt8(Offset::priChannel2Channel(), 0);
}

GD73Codeplug::ScanListElement::ChannelMode
GD73Codeplug::ScanListElement::revertChannelMode() const {
  return (ChannelMode)getUInt8(Offset::txChannelMode());
}
void
GD73Codeplug::ScanListElement::setRevertChannelMode(ChannelMode mode) {
  setUInt8(Offset::txChannelMode(), (unsigned int)mode);
}
bool
GD73Codeplug::ScanListElement::hasRevertZoneIndex() const {
  return 0 != getUInt8(Offset::txChannelZone());
}
unsigned int
GD73Codeplug::ScanListElement::revertZoneIndex() const {
  return getUInt8(Offset::txChannelZone())-1;
}
void
GD73Codeplug::ScanListElement::setRevertZoneIndex(unsigned int idx) {
  setUInt8(Offset::txChannelZone(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearRevertZoneIndex() {
  setUInt8(Offset::txChannelZone(), 0);
}
bool
GD73Codeplug::ScanListElement::hasRevertChannelIndex() const {
  return 0 != getUInt8(Offset::txChannelChannel());
}
unsigned
GD73Codeplug::ScanListElement::revertChannelIndex() const {
  return getUInt8(Offset::txChannelChannel())-1;
}
void
GD73Codeplug::ScanListElement::setRevertChannelIndex(unsigned int idx) {
  setUInt8(Offset::txChannelChannel(), idx+1);
}
void
GD73Codeplug::ScanListElement::clearRevertChannelIndex() {
  setUInt8(Offset::txChannelChannel(), 0);
}

Interval
GD73Codeplug::ScanListElement::rxHoldTime() const {
  return Interval::fromMilliseconds(500*getUInt8(Offset::holdTime()));
}
void
GD73Codeplug::ScanListElement::setRXHoldTime(const Interval &interval) {
  setUInt8(Offset::holdTime(), Limit::holdTime().map(interval).milliseconds()/500);
}

Interval
GD73Codeplug::ScanListElement::txHoldTime() const {
  return Interval::fromMilliseconds(500*getUInt8(Offset::txHoldTime()));
}
void
GD73Codeplug::ScanListElement::setTXHoldTime(const Interval &interval) {
  setUInt8(Offset::txHoldTime(), Limit::holdTime().map(interval).milliseconds()/500);
}

ScanList *
GD73Codeplug::ScanListElement::toScanList(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new ScanList(name());
}

bool
GD73Codeplug::ScanListElement::linkScanList(ScanList *lst, Context &ctx, const ErrorStack &err) {
  if ((ChannelMode::Fixed == primaryChannelMode()) && hasPrimaryChannelIndex()) {
    if (! ctx.has<Channel>(primaryChannelIndex())) {
      errMsg(err) << "Cannot link scan list '" << lst->name()
                  << "': Cannot resolve primary channel index " << primaryChannelIndex() << ".";
      return false;
    }
    lst->setPrimaryChannel(ctx.get<Channel>(primaryChannelIndex()));
  } else if (ChannelMode::Selected == primaryChannelMode()) {
    lst->setPrimaryChannel(SelectedChannel::get());
  }

  if ((ChannelMode::Fixed == secondaryChannelMode()) && hasSecondaryChannelIndex()) {
    if (! ctx.has<Channel>(secondaryChannelIndex())) {
      errMsg(err) << "Cannot link scan list '" << lst->name()
                  << "': Cannot resolve secondary channel index " << secondaryChannelIndex() << ".";
      return false;
    }
    lst->setSecondaryChannel(ctx.get<Channel>(secondaryChannelIndex()));
  } else if (ChannelMode::Selected == secondaryChannelMode()) {
    lst->setSecondaryChannel(SelectedChannel::get());
  }

  if ((ChannelMode::Fixed == revertChannelMode()) && hasRevertChannelIndex()) {
    if (! ctx.has<Channel>(revertChannelIndex())) {
      errMsg(err) << "Cannot link scan list '" << lst->name()
                  << "': Cannot resolve revert channel index " << revertChannelIndex() << ".";
      return false;
    }
    lst->setRevertChannel(ctx.get<Channel>(revertChannelIndex()));
  } else if (ChannelMode::Selected == revertChannelMode()) {
    lst->setRevertChannel(SelectedChannel::get());
  }

  unsigned int count = std::min((unsigned int)getUInt8(Offset::memberCount()), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    unsigned int index = getUInt16_le(Offset::members() + i*Offset::betweenMembers());
    if (0 == index)
      continue;
    if (! ctx.has<Channel>(index-1)) {
      errMsg(err) << "Cannot link scan list '" << lst->name()
                  << "': Cannot resolve member index" << index-1 << ".";
      return false;
    }
    lst->addChannel(ctx.get<Channel>(index-1));
  }

  return true;
}

bool
GD73Codeplug::ScanListElement::encode(ScanList *lst, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  setName(lst->name());

  if (! lst->primary()->isNull()) {
    if (SelectedChannel::get() == lst->primaryChannel()) {
      setPrimaryChannelMode(ChannelMode::Selected);
    } else {
      setPrimaryChannelMode(ChannelMode::Fixed);
      setPrimaryChannelIndex(ctx.index(lst->primaryChannel()));
    }
  }

  if (! lst->secondary()->isNull()) {
    if (SelectedChannel::get() == lst->secondaryChannel()) {
      setSecondaryChannelMode(ChannelMode::Selected);
    } else {
      setSecondaryChannelMode(ChannelMode::Fixed);
      setSecondaryChannelIndex(ctx.index(lst->secondaryChannel()));
    }
  }

  if (! lst->revert()->isNull()) {
    if (SelectedChannel::get() == lst->revertChannel()) {
      setRevertChannelMode(ChannelMode::Selected);
    } else {
      setRevertChannelMode(ChannelMode::Fixed);
      setRevertChannelIndex(ctx.index(lst->revertChannel()));
    }
  }

  unsigned int count = std::min((unsigned int)lst->count(), Limit::memberCount());
  setUInt8(Offset::memberCount(), count);

  for (unsigned int i=0; i<count; i++) {
    setUInt16_le(Offset::members() + i*Offset::betweenMembers(), ctx.index(lst->channel(i))+1);
  }

  return true;
}

/* ********************************************************************************************* *
 * Implementation of GD73Codeplug
 * ********************************************************************************************* */
GD73Codeplug::GD73Codeplug(QObject *parent)
  : Codeplug{parent}
{
  addImage("Radioddity GD-73A/E codeplug");
  image(0).addElement(0x000000, 0x22014);
}

Config *
GD73Codeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *copy = Codeplug::preprocess(config, err);
  if (nullptr == copy) {
    errMsg(err) << "Cannot pre-process codeplug for GD73A/E.";
    return nullptr;
  }

  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err)) {
    errMsg(err) << "Cannot pre-process codeplug for GD73A/E.";
    return nullptr;
  }

  return copy;
}

bool
GD73Codeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for GD73A/E.";
    return false;
  }

  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for GD73A/E.";
    return false;
  }

  return true;
}


bool
GD73Codeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  // There must be a default DMR radio ID.
  if (nullptr == ctx.config()->settings()->defaultId()) {
    errMsg(err) << "No default DMR radio ID specified.";
    errMsg(err) << "Cannot index codplug for encoding for the Radioddity GD73.";
    return false;
  }

  // Map radio IDs
  for (int i=0; i<ctx.config()->radioIDs()->count(); i++)
    ctx.add(ctx.config()->radioIDs()->getId(i), i);

  // Map digital and DTMF contacts
  for (int i=0, d=0, a=0; i<config->contacts()->count(); i++) {
    if (ctx.config()->contacts()->contact(i)->is<DMRContact>()) {
      ctx.add(ctx.config()->contacts()->contact(i)->as<DMRContact>(), d); d++;
    } else if (ctx.config()->contacts()->contact(i)->is<DTMFContact>()) {
      ctx.add(ctx.config()->contacts()->contact(i)->as<DTMFContact>(), a); a++;
    }
  }

  // Map rx group lists
  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(ctx.config()->rxGroupLists()->list(i), i);

  // Map channels
  for (int i=0; i<config->channelList()->count(); i++) {
    if (ctx.config()->channelList()->get(i)->is<DMRChannel>() ||
        ctx.config()->channelList()->get(i)->is<FMChannel>())
      ctx.add(ctx.config()->channelList()->channel(i), i);
  }

  // Map zones
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  // Map scan lists
  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  return true;

}

bool
GD73Codeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags);

  Context ctx(config);

  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeTimestamp(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeSettings(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeContacts(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeGroupLists(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeChannels(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeZones(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  if (! encodeScanLists(ctx, err)) {
    errMsg(err) << "Cannot encode codeplug for Radioddity GD73.";
    return false;
  }

  return true;
}

bool
GD73Codeplug::decode(Config *config, const ErrorStack &err) {
  Context ctx(config);

  if (! decodeTimestamp(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createMessages(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! decodeSettings(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createContacts(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createDTMFContacts(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createGroupLists(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createEncryptionKeys(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createChannels(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createZones(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! createScanLists(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! linkGroupLists(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! linkChannels(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! linkZones(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  if (! linkScanLists(ctx, err)) {
    errMsg(err) << "Cannot decode codeplug.";
    return false;
  }

  return true;
}

bool
GD73Codeplug::decodeTimestamp(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  InformationElement info(data(Offset::timestamp()));
  if (! info.isValid()) {
    errMsg(err) << "Cannot parse info element @" << Qt::hex << Offset::timestamp() << ".";
    return false;
  }

  // Nothing to do here.
  return true;
}

bool
GD73Codeplug::encodeTimestamp(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err)

  InformationElement info(data(Offset::timestamp()));
  info.setTimestamp(QDateTime::currentDateTimeUtc());
  info.setFrequencyRange(FrequencyRange {Frequency::fromMHz(400.0), Frequency::fromMHz(470.0)});
  return true;
}


bool
GD73Codeplug::createMessages(Context &ctx, const ErrorStack &err) {
  // Not implemented yet.
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::decodeSettings(Context &ctx, const ErrorStack &err) {
  SettingsElement settings(data(Offset::settings()));
  if (! settings.updateConfig(ctx, err)) {
    errMsg(err) << "Cannot decode settings element.";
    return false;
  }
  DMRSettingsElement dmrSettings(data(Offset::dmrSettings()));
  if (! dmrSettings.updateConfig(ctx, err)) {
    errMsg(err) << "Cannot decode DMR settings element.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeSettings(Context &ctx, const ErrorStack &err) {
  SettingsElement settings(data(Offset::settings()));
  if (! settings.encode(ctx, err)) {
    errMsg(err) << "Cannot encode settings element.";
    return false;
  }
  DMRSettingsElement dmrSettings(data(Offset::dmrSettings()));
  if (! dmrSettings.encode(ctx, err)) {
    errMsg(err) << "Cannot encode DMR settings element.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::createContacts(Context &ctx, const ErrorStack &err) {
  ContactBankElement bank(data(Offset::contacts()));
  if (! bank.createContacts(ctx, err)) {
    errMsg(err) << "Cannot create contacts.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeContacts(Context &ctx, const ErrorStack &err) {
  ContactBankElement bank(data(Offset::contacts()));
  if (! bank.encode(ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }
  return true;
}


bool
GD73Codeplug::createDTMFContacts(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::createGroupLists(Context &ctx, const ErrorStack &err) {
  if (! GroupListBankElement(data(Offset::groupLists())).createGroupLists(ctx, err)) {
    errMsg(err) << "Cannot decode group lists.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::linkGroupLists(Context &ctx, const ErrorStack &err) {
  if (! GroupListBankElement(data(Offset::groupLists())).linkGroupLists(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeGroupLists(Context &ctx, const ErrorStack &err) {
  if (! GroupListBankElement(data(Offset::groupLists())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::createEncryptionKeys(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::createChannels(Context &ctx, const ErrorStack &err) {
  if (! ChannelBankElement(data(Offset::channels())).createChannels(ctx, err)) {
    errMsg(err) << "Cannot create channels.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  if (! ChannelBankElement(data(Offset::channels())).linkChannels(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeChannels(Context &ctx, const ErrorStack &err) {
  if (! ChannelBankElement(data(Offset::channels())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::createZones(Context &ctx, const ErrorStack &err) {
  if (! ZoneBankElement(data(Offset::zones())).createZones(ctx, err)) {
    errMsg(err) << "Cannot create zones.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  if (! ZoneBankElement(data(Offset::zones())).linkZones(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeZones(Context &ctx, const ErrorStack &err) {
  if (! ZoneBankElement(data(Offset::zones())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::createScanLists(Context &ctx, const ErrorStack &err) {
  if (! ScanListBankElement(data(Offset::scanLists())).createScanLists(ctx, err)) {
    errMsg(err) << "Cannot create scan-lists.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  if (! ScanListBankElement(data(Offset::scanLists())).linkScanLists(ctx, err)) {
    errMsg(err) << "Cannot link scan-lists.";
    return false;
  }
  return true;
}

bool
GD73Codeplug::encodeScanLists(Context &ctx, const ErrorStack &err) {
  if (! ScanListBankElement(data(Offset::scanLists())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode scan-lists.";
    return false;
  }
  return true;
}
