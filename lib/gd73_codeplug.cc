#include "gd73_codeplug.hh"
#include "config.hh"

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
  ctx.config()->radioIDs()->setDefaultId(0);
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
  unsigned int count = std::min((unsigned int)getUInt8(Offset::contactCount()), Limit::contactCount());
  for (unsigned int i=0; i<count; i++) {
    ContactElement contact(_data + Offset::contacts() + i*Offset::betweenContacts());
    DMRContact *contactObj = contact.toContact(ctx, err);
    if (nullptr == contactObj) {
      errMsg(err) << "Cannot decode " << i+1 << "-th contact.";
      return false;
    }
    ctx.config()->contacts()->add(contactObj);
    ctx.add(contactObj, i+1);
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
      errMsg(err) << "Cannot decode " << i+1 << "-th group list.";
      return false;
    }
    ctx.config()->rxGroupLists()->add(glObj);
    ctx.add(glObj, i+1);
  }
  return true;
}

bool
GD73Codeplug::GroupListBankElement::linkGroupLists(Context &ctx, const ErrorStack &err) {
  unsigned int count = std::min((unsigned int)getUInt8(Offset::memberCount()), Limit::memberCount());
  for (unsigned int i=0; i<count; i++) {
    GroupListElement gl(_data+Offset::members() + i*Offset::betweenMembers());
    RXGroupList *glObj = ctx.get<RXGroupList>(i+1);
    if (! gl.linkGroupList(glObj, ctx, err)) {
      errMsg(err) << "Cannot link " << i+1 << "-th group list.";
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
    if (! ctx.has<DMRContact>(idx)) {
      errMsg(err) << "Cannot resolve contact at index " << idx << ".";
      return false;
    }
    lst->addContact(ctx.get<DMRContact>(idx));
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

bool
GD73Codeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}

bool
GD73Codeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
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

  if (! createContacts(ctx, err)) {
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
GD73Codeplug::createEncryptionKeys(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::createChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::linkChannels(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::createZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::linkZones(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::createScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}

bool
GD73Codeplug::linkScanLists(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return true;
}
