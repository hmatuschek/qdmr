#include "hd1_codeplug.hh"
#include "config.hh"
#include "intermediaterepresentation.hh"
#include "logger.hh"
#include "radioid.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"
#include <QtEndian>
#include <algorithm>
#include <QSet>


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::Region
 * ********************************************************************************************* */
QList<HD1Codeplug::Region>
HD1Codeplug::Region::elements() {
  const uint32_t page = HD1Interface::pageSize();
  return {
    {HD1Interface::Small, 0x0000, 0x180},
    {HD1Interface::Small, 0x2000, 0x300},
    {HD1Interface::Small, 0x2900, 0x1980},
    {HD1Interface::Small, 0x5000, 0xa00},
    {HD1Interface::Small, 0xe000, 0x40},
    {HD1Interface::Small, 0xe800, 0x07},
    {HD1Interface::Bulk,  0x1b84*page, 0x24c*page},
    {HD1Interface::Bulk,  0x1df8*page, 0x00b*page},
    {HD1Interface::Bulk,  0x21d8*page, 0x050*page},
    {HD1Interface::Info,  0x0000, 0x10},
  };
}

QList<HD1Codeplug::Region>
HD1Codeplug::Region::reads() {
  const uint32_t page = HD1Interface::pageSize();
  return {
    {HD1Interface::Info,  0x0000, 0x10},
    {HD1Interface::Small, 0xe800, 0x07},       // model, "IHD8580"
    {HD1Interface::Small, 0x2950, 0x40},       // also part of 0x2900 below
    {HD1Interface::Small, 0x0000, 0x180},
    {HD1Interface::Small, 0x2000, 0x300},
    {HD1Interface::Small, 0x2900, 0x1980},
    {HD1Interface::Small, 0x5000, 0xa00},
    {HD1Interface::Bulk,  0x1b84*page, 0x24c*page},
    {HD1Interface::Bulk,  0x21d8*page, 0x050*page},
    {HD1Interface::Bulk,  0x1df8*page, 0x00b*page},
    {HD1Interface::Small, 0xe000, 0x40},
  };
}

QList<HD1Codeplug::Region>
HD1Codeplug::Region::writes() {
  const uint32_t page = HD1Interface::pageSize();
  return {
    {HD1Interface::Small, 0x0000, 0x180},
    {HD1Interface::Small, 0x2000, 0x300},
    {HD1Interface::Small, 0x2900, 0x1980},
    {HD1Interface::Small, 0x5000, 0xa00},
    {HD1Interface::Small, 0x59c0, 0x80},       // again, plus 0x5a00..0x5a3f as 0xff
    {HD1Interface::Bulk,  0x1b84*page, 0x24c*page},
    {HD1Interface::Bulk,  0x21d8*page, 0x050*page},
    {HD1Interface::Bulk,  0x1df8*page, 0x00c*page},  // the last page (0x1e03) as 0xff
  };
}

uint32_t
HD1Codeplug::Region::writeFrameSize(HD1Interface::Bank bank) {
  return (HD1Interface::Bulk == bank) ? 4*HD1Interface::pageSize() : 0x80;
}


/* ********************************************************************************************* *
 * Helpers
 * ********************************************************************************************* */
static bool
allBytes(const uint8_t *ptr, unsigned int n, uint8_t value) {
  for (unsigned int i=0; i<n; i++)
    if (value != ptr[i])
      return false;
  return true;
}

static QList<uint32_t>
readRefs(const uint8_t *ptr, unsigned int count) {
  QList<uint32_t> refs;
  for (unsigned int i=0; i<count; i++) {
    uint32_t ref = qFromLittleEndian<uint32_t>(ptr+4*i);
    if (HD1Codeplug::ContactRef::none() != ref)
      refs.append(ref);
  }
  return refs;
}

static void
writeRefs(uint8_t *ptr, unsigned int count, const QList<uint32_t> &refs) {
  for (unsigned int i=0; i<count; i++)
    qToLittleEndian<uint32_t>((i < unsigned(refs.size())) ? refs[i] : HD1Codeplug::ContactRef::none(),
                              ptr+4*i);
}

/** BCD value of @c n bytes, little endian; -1 if a nibble is not a decimal digit. */
static long long
bcdLE(const uint8_t *ptr, unsigned int n) {
  long long v = 0;
  for (int i=n-1; i>=0; i--) {
    uint8_t hi = ptr[i] >> 4, lo = ptr[i] & 0x0f;
    if ((hi > 9) || (lo > 9))
      return -1;
    v = v*100 + hi*10 + lo;
  }
  return v;
}

static SelectiveCall
decodeTone(uint16_t v) {
  if (0xffff == v)
    return SelectiveCall();
  if (v & 0x8000) {
    // DCS: 0x8000 | 0x4000 (inverted) | three octal digits as nibbles.
    unsigned int d0 = (v >> 8) & 0xf, d1 = (v >> 4) & 0xf, d2 = v & 0xf;
    if ((v & 0x3000) || (d0 > 7) || (d1 > 7) || (d2 > 7))
      return SelectiveCall();
    return SelectiveCall(d0*100 + d1*10 + d2, bool(v & 0x4000));
  }
  uint8_t bytes[2] = {uint8_t(v & 0xff), uint8_t(v >> 8)};
  long long tenths = bcdLE(bytes, 2);
  if (tenths <= 0)
    return SelectiveCall();
  return SelectiveCall(double(tenths)/10);
}

static uint16_t
encodeTone(const SelectiveCall &tone) {
  if (tone.isCTCSS()) {
    unsigned int v = (tone.mHz()+50)/100;
    return ((v/1000)%10)<<12 | ((v/100)%10)<<8 | ((v/10)%10)<<4 | (v%10);
  }
  if (tone.isDCS()) {
    unsigned int code = tone.octalCode();
    return 0x8000 | (tone.isInverted() ? 0x4000 : 0)
        | ((code/100)%10)<<8 | ((code/10)%10)<<4 | (code%10);
  }
  return 0xffff;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ChannelElement
 * ********************************************************************************************* */
HD1Codeplug::ChannelElement::ChannelElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ChannelElement::ChannelElement(uint8_t *ptr)
  : ChannelElement(ptr, size())
{
  // pass...
}

bool
HD1Codeplug::ChannelElement::isValid() const {
  return Element::isValid() && (! allBytes(_data, size(), 0xff));
}

void
HD1Codeplug::ChannelElement::clear() {
  // A new channel as the manufacturer's default codeplug has them: no members, TOT 75 s,
  // "receive GPS" on, everything else off.
  memset(_data, 0x00, size());
  setMembers({});
  setUInt32_le(Offset::unknown80(), 0xffffffff);
  setTXContact(ContactRef::noTXContact());
  setUInt8(Offset::timeout(), 5);
  setUInt16_le(Offset::rxTone(), 0xffff);
  setUInt16_le(Offset::txTone(), 0xffff);
  setBit(Offset::receiveGPS(), true);
  setPower(Channel::Power::High);
  setScanAdd(true);
  setColorCode(1);
  setTXAuthority(TXAuthority::Always);
}

QString
HD1Codeplug::ChannelElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
HD1Codeplug::ChannelElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

Frequency
HD1Codeplug::ChannelElement::rxFrequency() const {
  long long v = bcdLE(_data+Offset::rxFrequency(), 4);
  return Frequency::fromHz((v < 0) ? 0 : v*10);
}
void
HD1Codeplug::ChannelElement::setRXFrequency(const Frequency &f) {
  setBCD8_le(Offset::rxFrequency(), f.inHz()/10);
}

Frequency
HD1Codeplug::ChannelElement::txFrequency() const {
  long long v = bcdLE(_data+Offset::txFrequency(), 4);
  return Frequency::fromHz((v < 0) ? 0 : v*10);
}
void
HD1Codeplug::ChannelElement::setTXFrequency(const Frequency &f) {
  setBCD8_le(Offset::txFrequency(), f.inHz()/10);
  // The manufacturer's software sets bit 4 when TX is above RX and bit 5 when it is below.
  long long rx = bcdLE(_data+Offset::rxFrequency(), 4), tx = bcdLE(_data+Offset::txFrequency(), 4);
  setBit(Offset::offsetUp(), tx > rx);
  setBit(Offset::offsetDown(), tx < rx);
}

bool
HD1Codeplug::ChannelElement::isDigital() const {
  return getBit(Offset::digital());
}
void
HD1Codeplug::ChannelElement::setDigital(bool digital) {
  // Bit 7 of 0xa9 is usually set on digital channels, but not always; only touch it when the
  // channel type actually changes.
  if (getBit(Offset::digital()) != digital)
    setBit(Offset::digitalA9(), digital);
  setBit(Offset::digital(), digital);
}

Channel::Power
HD1Codeplug::ChannelElement::power() const {
  switch (getUInt2(Offset::power())) {
  case 0: return Channel::Power::Low;
  case 1: return Channel::Power::Mid;
  default: break;
  }
  return Channel::Power::High;
}
void
HD1Codeplug::ChannelElement::setPower(Channel::Power power) {
  switch (power) {
  case Channel::Power::Min:
  case Channel::Power::Low: setUInt2(Offset::power(), 0); break;
  case Channel::Power::Mid: setUInt2(Offset::power(), 1); break;
  case Channel::Power::High:
  case Channel::Power::Max: setUInt2(Offset::power(), 2); break;
  }
}

bool
HD1Codeplug::ChannelElement::scanAdd() const {
  return getBit(Offset::scanAdd());
}
void
HD1Codeplug::ChannelElement::setScanAdd(bool add) {
  setBit(Offset::scanAdd(), add);
}

bool
HD1Codeplug::ChannelElement::talkaround() const {
  return getBit(Offset::talkaround());
}
void
HD1Codeplug::ChannelElement::enableTalkaround(bool enable) {
  setBit(Offset::talkaround(), enable);
}

Interval
HD1Codeplug::ChannelElement::timeout() const {
  uint8_t steps = getUInt8(Offset::timeout());
  if (0 == steps)
    return Interval::infinity();
  return Interval::fromSeconds(15*steps);
}
void
HD1Codeplug::ChannelElement::setTimeout(const Interval &tot) {
  if (tot.isInfinite() || tot.isNull()) {
    setUInt8(Offset::timeout(), 0);
    return;
  }
  unsigned long long s = std::min<unsigned long long>(tot.seconds(), Limit::maxTimeout());
  setUInt8(Offset::timeout(), std::max<unsigned long long>(1, (s+7)/15));
}

FMChannel::Bandwidth
HD1Codeplug::ChannelElement::bandwidth() const {
  return getBit(Offset::bandwidth()) ? FMChannel::Bandwidth::Wide : FMChannel::Bandwidth::Narrow;
}
void
HD1Codeplug::ChannelElement::setBandwidth(FMChannel::Bandwidth bw) {
  setBit(Offset::bandwidth(), FMChannel::Bandwidth::Wide == bw);
}

SelectiveCall
HD1Codeplug::ChannelElement::rxTone() const {
  return decodeTone(getUInt16_le(Offset::rxTone()));
}
void
HD1Codeplug::ChannelElement::setRXTone(const SelectiveCall &tone) {
  setUInt16_le(Offset::rxTone(), encodeTone(tone));
}

SelectiveCall
HD1Codeplug::ChannelElement::txTone() const {
  return decodeTone(getUInt16_le(Offset::txTone()));
}
void
HD1Codeplug::ChannelElement::setTXTone(const SelectiveCall &tone) {
  setUInt16_le(Offset::txTone(), encodeTone(tone));
}

unsigned int
HD1Codeplug::ChannelElement::colorCode() const {
  return getUInt4(Offset::colorCode());
}
void
HD1Codeplug::ChannelElement::setColorCode(unsigned int cc) {
  setUInt4(Offset::colorCode(), std::min(15U, cc));
}

DMRChannel::TimeSlot
HD1Codeplug::ChannelElement::timeSlot() const {
  return getBit(Offset::timeSlot()) ? DMRChannel::TimeSlot::TS2 : DMRChannel::TimeSlot::TS1;
}
void
HD1Codeplug::ChannelElement::setTimeSlot(DMRChannel::TimeSlot ts) {
  setBit(Offset::timeSlot(), DMRChannel::TimeSlot::TS2 == ts);
}

HD1Codeplug::ChannelElement::TXAuthority
HD1Codeplug::ChannelElement::txAuthority() const {
  return TXAuthority(getUInt2(Offset::txAuthority()));
}
void
HD1Codeplug::ChannelElement::setTXAuthority(TXAuthority auth) {
  setUInt2(Offset::txAuthority(), uint8_t(auth));
}

unsigned int
HD1Codeplug::ChannelElement::radioIdIndex() const {
  return getUInt5(Offset::radioId());
}
void
HD1Codeplug::ChannelElement::setRadioIdIndex(unsigned int idx) {
  setUInt5(Offset::radioId(), std::min(31U, idx));
}

uint32_t
HD1Codeplug::ChannelElement::txContact() const {
  return getUInt32_le(Offset::txContact());
}
void
HD1Codeplug::ChannelElement::setTXContact(uint32_t ref) {
  setUInt32_le(Offset::txContact(), ref);
}

QList<uint32_t>
HD1Codeplug::ChannelElement::members() const {
  return readRefs(_data+Offset::members(), Limit::members());
}
void
HD1Codeplug::ChannelElement::setMembers(const QList<uint32_t> &refs) {
  writeRefs(_data+Offset::members(), Limit::members(), refs);
}

Channel *
HD1Codeplug::ChannelElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  Channel *ch = nullptr;
  if (isDigital()) {
    DMRChannel *dmr = new DMRChannel(); ch = dmr;
    dmr->setColorCode(colorCode());
    dmr->setTimeSlot(timeSlot());
    switch (txAuthority()) {
    case TXAuthority::Prohibit:
      dmr->setRXOnly(true);
      dmr->setAdmit(DMRChannel::Admit::Always);
      break;
    case TXAuthority::Always: dmr->setAdmit(DMRChannel::Admit::Always); break;
    case TXAuthority::ColorCode: dmr->setAdmit(DMRChannel::Admit::ColorCode); break;
    case TXAuthority::ChannelFree: dmr->setAdmit(DMRChannel::Admit::Free); break;
    }
    dmr->extended()->enableTalkaround(talkaround());
  } else {
    FMChannel *fm = new FMChannel(); ch = fm;
    fm->setBandwidth(bandwidth());
    fm->setRXTone(rxTone());
    fm->setTXTone(txTone());
    fm->setAdmit(FMChannel::Admit::Always);
    fm->extended()->enableTalkaround(talkaround());
  }
  ch->setName(name());
  ch->setRXFrequency(rxFrequency());
  ch->setTXFrequency(txFrequency());
  ch->setPower(power());
  ch->setTimeout(timeout());
  return ch;
}

bool
HD1Codeplug::ChannelElement::link(Channel *channel, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);
  DMRChannel *dmr = channel->as<DMRChannel>();
  if (nullptr == dmr)
    return true;
  uint32_t ref = txContact();
  if ((ref >= ContactRef::base()) && ctx.has<DMRContact>(ref - ContactRef::base()))
    dmr->setContact(ctx.get<DMRContact>(ref - ContactRef::base()));
  else if ((ContactRef::none() != ref) && (ContactRef::noTXContact() != ref))
    logInfo() << "Channel '" << name() << "': TX contact " << ref
              << " is not a priority contact (address book?), dropped.";
  DMRRadioID *defaultId = ctx.config()->settings()->defaultId();
  if (ctx.has<DMRRadioID>(radioIdIndex()) && (ctx.get<DMRRadioID>(radioIdIndex()) != defaultId))
    dmr->setRadioId(ctx.get<DMRRadioID>(radioIdIndex()));
  else
    dmr->setRadioId(DefaultRadioID::get());
  return true;
}

bool
HD1Codeplug::ChannelElement::encode(const Channel *channel, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  setName(channel->name());
  setRXFrequency(channel->rxFrequency());
  setTXFrequency(channel->txFrequency());
  setPower(channel->defaultPower() ? ctx.config()->settings()->power() : channel->power());
  if (channel->defaultTimeout())
    setTimeout(ctx.config()->settings()->totDisabled() ? Interval::infinity()
                                                         : ctx.config()->settings()->tot());
  else
    setTimeout(channel->timeout());
  setScanAdd(nullptr != channel->scanList());

  if (const FMChannel *fm = channel->as<FMChannel>()) {
    setDigital(false);
    setBandwidth(fm->bandwidth());
    setRXTone(fm->rxTone());
    setTXTone(fm->txTone());
    enableTalkaround(fm->extended()->talkaround());
    // Member list and TX contact of analog channels are not modelled; they stay as they are.
  } else if (const DMRChannel *dmr = channel->as<DMRChannel>()) {
    setDigital(true);
    setBandwidth(FMChannel::Bandwidth::Narrow);
    setRXTone(SelectiveCall());
    setTXTone(SelectiveCall());
    setColorCode(dmr->colorCode());
    setTimeSlot(dmr->timeSlot());
    if (dmr->rxOnly()) {
      setTXAuthority(TXAuthority::Prohibit);
    } else {
      switch (dmr->admit()) {
      case DMRChannel::Admit::Always: setTXAuthority(TXAuthority::Always); break;
      case DMRChannel::Admit::Free: setTXAuthority(TXAuthority::ChannelFree); break;
      case DMRChannel::Admit::ColorCode: setTXAuthority(TXAuthority::ColorCode); break;
      }
    }
    if (dmr->contact()) {
      setTXContact(ContactRef::base() + ctx.index(dmr->contact()));
    } else {
      // A reference qdmr cannot represent (address book number, empty contact slot) is kept;
      // a reference to an existing contact is cleared.
      uint32_t ref = txContact();
      if ((ref >= ContactRef::base()) && (ref - ContactRef::base() < ctx.count<DMRContact>()))
        setTXContact(ContactRef::noTXContact());
    }
    setMembers(dmr->groupList() ? GroupListElement::memberRefs(dmr->groupList(), ctx)
                                : QList<uint32_t>());
    if (dmr->radioIdRef()->is<DefaultRadioID>())
      setRadioIdIndex(ctx.index(ctx.config()->settings()->defaultId()));
    else
      setRadioIdIndex(ctx.index(dmr->radioId()));
    enableTalkaround(dmr->extended()->talkaround());
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ChannelBankElement
 * ********************************************************************************************* */
HD1Codeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ChannelBankElement::ChannelBankElement(uint8_t *ptr)
  : ChannelBankElement(ptr, size())
{
  // pass...
}

HD1Codeplug::ChannelElement
HD1Codeplug::ChannelBankElement::channel(unsigned int n) const {
  return ChannelElement(_data + n*ChannelElement::size());
}

bool
HD1Codeplug::ChannelBankElement::decode(Context &ctx, const ErrorStack &err) {
  // The HD1 only has a per-channel "scan add" flag: those channels form one scan list.
  ScanList *scan = nullptr;
  for (unsigned int i=0; i<Limit::channels(); i++) {
    ChannelElement el = channel(i);
    if (! el.isValid())
      continue;
    Channel *ch = el.decode(ctx, err);
    if (nullptr == ch) {
      errMsg(err) << "Cannot decode channel " << (i+1) << ".";
      return false;
    }
    ctx.config()->channelList()->add(ch);
    ctx.add(ch, i);
    if (el.scanAdd()) {
      if (nullptr == scan) {
        scan = new ScanList("Scan");
        ctx.config()->scanlists()->add(scan);
      }
      scan->addChannel(ch);
      ch->setScanList(scan);
    }
  }
  return true;
}

bool
HD1Codeplug::ChannelBankElement::link(Context &ctx, const ErrorStack &err) {
  // Group lists by their member references: a channel's member list is a copy of one.
  QHash<QList<uint32_t>, RXGroupList *> listsByMembers;
  for (int i=0; i<ctx.config()->rxGroupLists()->count(); i++) {
    RXGroupList *list = ctx.config()->rxGroupLists()->list(i);
    QList<uint32_t> refs;
    for (int j=0; j<list->count(); j++)
      refs.append(ContactRef::base() + ctx.index(list->contact(j)));
    if (! listsByMembers.contains(refs))
      listsByMembers.insert(refs, list);
  }

  for (unsigned int i=0; i<Limit::channels(); i++) {
    if (! ctx.has<Channel>(i))
      continue;
    ChannelElement el = channel(i);
    Channel *ch = ctx.get<Channel>(i);
    if (! el.link(ch, ctx, err)) {
      errMsg(err) << "Cannot link channel " << (i+1) << ".";
      return false;
    }
    DMRChannel *dmr = ch->as<DMRChannel>();
    QList<uint32_t> refs = el.members();
    if ((nullptr == dmr) || refs.isEmpty())
      continue;
    // A custom member set gets a group list of its own, named after the first channel using it.
    if (! listsByMembers.contains(refs)) {
      RXGroupList *list = new RXGroupList(el.name());
      for (uint32_t ref: refs) {
        if ((ref >= ContactRef::base()) && ctx.has<DMRContact>(ref - ContactRef::base()))
          list->addContact(ctx.get<DMRContact>(ref - ContactRef::base()));
      }
      ctx.config()->rxGroupLists()->add(list);
      listsByMembers.insert(refs, list);
    }
    dmr->setGroupList(listsByMembers.value(refs));
  }
  return true;
}

/** Key for finding a channel's old record: name and both frequencies. */
static QString
channelKey(const QString &name, const Frequency &rx, const Frequency &tx) {
  return QString("%1|%2|%3").arg(name.left(HD1Codeplug::ChannelElement::Limit::nameLength()))
      .arg(rx.inHz()/10).arg(tx.inHz()/10);
}

bool
HD1Codeplug::ChannelBankElement::encode(Context &ctx, const ErrorStack &err) {
  // Unmodelled bytes move with their channel: start each channel from the old record with the
  // same name and frequencies, or from a fresh default record.
  QByteArray old(reinterpret_cast<const char *>(_data), size());
  QMultiHash<QString, unsigned int> oldByKey;
  for (int i=Limit::channels()-1; i>=0; i--) {
    ChannelElement el(reinterpret_cast<uint8_t *>(old.data()) + i*ChannelElement::size());
    if (el.isValid())
      oldByKey.insert(channelKey(el.name(), el.rxFrequency(), el.txFrequency()), i);
  }

  // First pass: channels with an unchanged name and frequencies claim their old record. Equal
  // channels keep their order: each takes the first old record left.
  int count = ctx.config()->channelList()->count();
  QVector<int> source(count, -1);
  QSet<unsigned int> claimed;
  for (int i=0; i<count; i++) {
    Channel *ch = ctx.config()->channelList()->channel(i);
    QString key = channelKey(ch->name(), ch->rxFrequency(), ch->txFrequency());
    if (! oldByKey.contains(key))
      continue;
    QList<unsigned int> candidates = oldByKey.values(key);
    unsigned int j = *std::min_element(candidates.begin(), candidates.end());
    oldByKey.remove(key, j);
    source[i] = j;
    claimed.insert(j);
  }
  // Second pass: an edited channel (renamed, retuned) keeps the old record of its slot, unless
  // another channel claimed it.
  for (int i=0; i<count; i++) {
    if ((source[i] < 0) && (! claimed.contains(i))) {
      ChannelElement el(reinterpret_cast<uint8_t *>(old.data()) + i*ChannelElement::size());
      if (el.isValid()) {
        source[i] = i;
        claimed.insert(i);
      }
    }
  }

  for (unsigned int i=0; i<Limit::channels(); i++) {
    ChannelElement el = channel(i);
    if (int(i) >= count) {
      memset(_data + i*ChannelElement::size(), 0xff, ChannelElement::size());
      continue;
    }
    Channel *ch = ctx.config()->channelList()->channel(i);
    if (source[i] >= 0)
      memcpy(_data + i*ChannelElement::size(), old.constData() + source[i]*ChannelElement::size(),
             ChannelElement::size());
    else
      el.clear();
    if (! el.encode(ch, ctx, err)) {
      errMsg(err) << "Cannot encode channel " << (i+1) << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ContactElement
 * ********************************************************************************************* */
HD1Codeplug::ContactElement::ContactElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ContactElement::ContactElement(uint8_t *ptr)
  : ContactElement(ptr, size())
{
  // pass...
}

bool
HD1Codeplug::ContactElement::isValid() const {
  return Element::isValid() && (! allBytes(_data, size(), 0xff));
}

void
HD1Codeplug::ContactElement::clear() {
  memset(_data, 0x00, size());
  memset(_data+Offset::unknown45(), 0xff, 3);
}

unsigned int
HD1Codeplug::ContactElement::number() const {
  return getUInt32_le(Offset::number());
}
void
HD1Codeplug::ContactElement::setNumber(unsigned int number) {
  setUInt32_le(Offset::number(), number);
}

DMRContact::Type
HD1Codeplug::ContactElement::type() const {
  switch (getUInt8(Offset::type())) {
  case 4: return DMRContact::GroupCall;
  case 6: return DMRContact::AllCall;
  default: break;
  }
  return DMRContact::PrivateCall;
}
void
HD1Codeplug::ContactElement::setType(DMRContact::Type type) {
  switch (type) {
  case DMRContact::GroupCall: setUInt8(Offset::type(), 4); break;
  case DMRContact::PrivateCall: setUInt8(Offset::type(), 5); break;
  case DMRContact::AllCall: setUInt8(Offset::type(), 6); break;
  }
}

QString
HD1Codeplug::ContactElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
HD1Codeplug::ContactElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

DMRContact *
HD1Codeplug::ContactElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx);
  uint8_t t = getUInt8(Offset::type());
  if ((t < 4) || (t > 6)) {
    errMsg(err) << "Unknown call type " << t << ".";
    return nullptr;
  }
  return new DMRContact(type(), name(), number());
}

bool
HD1Codeplug::ContactElement::encode(const DMRContact *contact, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  setNumber(contact->number());
  setType(contact->type());
  setName(contact->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ContactBankElement
 * ********************************************************************************************* */
HD1Codeplug::ContactBankElement::ContactBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ContactBankElement::ContactBankElement(uint8_t *ptr)
  : ContactBankElement(ptr, size())
{
  // pass...
}

HD1Codeplug::ContactElement
HD1Codeplug::ContactBankElement::contact(unsigned int n) const {
  return ContactElement(_data + n*ContactElement::size());
}

bool
HD1Codeplug::ContactBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::contacts(); i++) {
    ContactElement el = contact(i);
    if (! el.isValid())
      continue;
    DMRContact *c = el.decode(ctx, err);
    if (nullptr == c) {
      errMsg(err) << "Cannot decode contact " << (i+1) << ".";
      return false;
    }
    ctx.config()->contacts()->add(c);
    ctx.add(c, i);
  }
  return true;
}

bool
HD1Codeplug::ContactBankElement::encode(Context &ctx, const ErrorStack &err) {
  // City, province and country are not modelled: keep them with the contact (same ID and type).
  QByteArray old(reinterpret_cast<const char *>(_data), size());
  QMultiHash<QPair<unsigned int, int>, unsigned int> oldByKey;
  for (int i=Limit::contacts()-1; i>=0; i--) {
    ContactElement el(reinterpret_cast<uint8_t *>(old.data()) + i*ContactElement::size());
    if (el.isValid())
      oldByKey.insert({el.number(), int(el.type())}, i);
  }

  for (unsigned int i=0; i<Limit::contacts(); i++) {
    ContactElement el = contact(i);
    if (! ctx.has<DMRContact>(i)) {
      memset(_data + i*ContactElement::size(), 0xff, ContactElement::size());
      continue;
    }
    DMRContact *c = ctx.get<DMRContact>(i);
    QPair<unsigned int, int> key(c->number(), int(c->type()));
    if (oldByKey.contains(key)) {
      QList<unsigned int> candidates = oldByKey.values(key);
      unsigned int j = *std::min_element(candidates.begin(), candidates.end());
      oldByKey.remove(key, j);
      memcpy(_data + i*ContactElement::size(), old.constData() + j*ContactElement::size(),
             ContactElement::size());
    } else {
      el.clear();
    }
    if (! el.encode(c, ctx, err)) {
      errMsg(err) << "Cannot encode contact " << (i+1) << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::GroupListElement
 * ********************************************************************************************* */
HD1Codeplug::GroupListElement::GroupListElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::GroupListElement::GroupListElement(uint8_t *ptr)
  : GroupListElement(ptr, size())
{
  // pass...
}

bool
HD1Codeplug::GroupListElement::isValid() const {
  return Element::isValid() && (! allBytes(_data, size(), 0xff));
}

void
HD1Codeplug::GroupListElement::clear() {
  memset(_data, 0xff, size());
}

QString
HD1Codeplug::GroupListElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0xff);
}
void
HD1Codeplug::GroupListElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0xff);
}

QList<uint32_t>
HD1Codeplug::GroupListElement::members() const {
  return readRefs(_data+Offset::members(), Limit::members());
}
void
HD1Codeplug::GroupListElement::setMembers(const QList<uint32_t> &refs) {
  writeRefs(_data+Offset::members(), Limit::members(), refs);
}

QList<uint32_t>
HD1Codeplug::GroupListElement::memberRefs(const RXGroupList *list, Context &ctx) {
  QList<uint32_t> refs;
  for (int i=0; (i<list->count()) && (refs.size() < int(Limit::members())); i++) {
    DMRContact *c = list->contact(i);
    int idx = ctx.index(c);
    if ((DMRContact::GroupCall == c->type()) && (idx >= 0))
      refs.append(ContactRef::base() + idx);
  }
  return refs;
}

RXGroupList *
HD1Codeplug::GroupListElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new RXGroupList(name());
}

bool
HD1Codeplug::GroupListElement::link(RXGroupList *list, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);
  for (uint32_t ref: members()) {
    if ((ref >= ContactRef::base()) && ctx.has<DMRContact>(ref - ContactRef::base()))
      list->addContact(ctx.get<DMRContact>(ref - ContactRef::base()));
    else
      logInfo() << "RX group list '" << name() << "': contact reference " << ref << " dropped.";
  }
  return true;
}

bool
HD1Codeplug::GroupListElement::encode(const RXGroupList *list, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  clear();
  setName(list->name());
  setMembers(memberRefs(list, ctx));
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::GroupListBankElement
 * ********************************************************************************************* */
HD1Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::GroupListBankElement::GroupListBankElement(uint8_t *ptr)
  : GroupListBankElement(ptr, size())
{
  // pass...
}

HD1Codeplug::GroupListElement
HD1Codeplug::GroupListBankElement::groupList(unsigned int n) const {
  return GroupListElement(_data + n*GroupListElement::size());
}

bool
HD1Codeplug::GroupListBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::groupLists(); i++) {
    GroupListElement el = groupList(i);
    if (! el.isValid())
      continue;
    RXGroupList *list = el.decode(ctx, err);
    ctx.config()->rxGroupLists()->add(list);
    ctx.add(list, i);
  }
  return true;
}

bool
HD1Codeplug::GroupListBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::groupLists(); i++) {
    if (! ctx.has<RXGroupList>(i))
      continue;
    if (! groupList(i).link(ctx.get<RXGroupList>(i), ctx, err)) {
      errMsg(err) << "Cannot link RX group list " << (i+1) << ".";
      return false;
    }
  }
  return true;
}

bool
HD1Codeplug::GroupListBankElement::encode(Context &ctx, const ErrorStack &err) {
  // More lists than slots are fine: channels copy their list's members anyway.
  if (ctx.count<RXGroupList>() > Limit::groupLists())
    logInfo() << "Only the first " << Limit::groupLists() << " of " << ctx.count<RXGroupList>()
              << " RX group lists fit the HD1's group list table; channels keep all members.";
  for (unsigned int i=0; i<Limit::groupLists(); i++) {
    GroupListElement el = groupList(i);
    el.clear();
    if (ctx.has<RXGroupList>(i) && (! el.encode(ctx.get<RXGroupList>(i), ctx, err))) {
      errMsg(err) << "Cannot encode RX group list " << (i+1) << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ZoneElement
 * ********************************************************************************************* */
HD1Codeplug::ZoneElement::ZoneElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ZoneElement::ZoneElement(uint8_t *ptr)
  : ZoneElement(ptr, size())
{
  // pass...
}

bool
HD1Codeplug::ZoneElement::isValid() const {
  return Element::isValid() && (! allBytes(_data, size(), 0xff));
}

void
HD1Codeplug::ZoneElement::clear() {
  memset(_data, 0xff, size());
}

QString
HD1Codeplug::ZoneElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
HD1Codeplug::ZoneElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned int
HD1Codeplug::ZoneElement::channelIndex(unsigned int n) const {
  return getUInt16_le(Offset::channels() + 2*n);
}
void
HD1Codeplug::ZoneElement::setChannelIndex(unsigned int n, unsigned int idx) {
  setUInt16_le(Offset::channels() + 2*n, idx);
}
void
HD1Codeplug::ZoneElement::clearChannelIndex(unsigned int n) {
  setUInt16_le(Offset::channels() + 2*n, 0xffff);
}

Zone *
HD1Codeplug::ZoneElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new Zone(name());
}

bool
HD1Codeplug::ZoneElement::link(Zone *zone, Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);
  for (unsigned int n=0; n<Limit::channels(); n++) {
    unsigned int idx = channelIndex(n);
    if ((0xffff != idx) && ctx.has<Channel>(idx))
      zone->A()->add(ctx.get<Channel>(idx));
  }
  return true;
}

bool
HD1Codeplug::ZoneElement::encode(const Zone *zone, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  unsigned int n = 0;
  for (int i=0; (i<zone->A()->count()) && (n < Limit::channels()); i++) {
    int idx = ctx.index(zone->A()->get(i));
    if (idx >= 0)
      setChannelIndex(n++, idx);
  }
  for (unsigned int i=n; i<Limit::channels(); i++)
    clearChannelIndex(i);
  setUInt8(Offset::count(), n);
  setName(zone->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ZoneBankElement
 * ********************************************************************************************* */
HD1Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::ZoneBankElement::ZoneBankElement(uint8_t *ptr)
  : ZoneBankElement(ptr, size())
{
  // pass...
}

HD1Codeplug::ZoneElement
HD1Codeplug::ZoneBankElement::zone(unsigned int n) const {
  return ZoneElement(_data + n*ZoneElement::size());
}

bool
HD1Codeplug::ZoneBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zones(); i++) {
    ZoneElement el = zone(i);
    if (! el.isValid())
      continue;
    Zone *z = el.decode(ctx, err);
    ctx.config()->zones()->add(z);
    ctx.add(z, i);
  }
  return true;
}

bool
HD1Codeplug::ZoneBankElement::link(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zones(); i++) {
    if (ctx.has<Zone>(i) && (! zone(i).link(ctx.get<Zone>(i), ctx, err))) {
      errMsg(err) << "Cannot link zone " << (i+1) << ".";
      return false;
    }
  }
  return true;
}

bool
HD1Codeplug::ZoneBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::zones(); i++) {
    ZoneElement el = zone(i);
    el.clear();
    if (ctx.has<Zone>(i) && (! el.encode(ctx.get<Zone>(i), ctx, err))) {
      errMsg(err) << "Cannot encode zone " << (i+1) << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::RadioIdElement
 * ********************************************************************************************* */
HD1Codeplug::RadioIdElement::RadioIdElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::RadioIdElement::RadioIdElement(uint8_t *ptr)
  : RadioIdElement(ptr, size())
{
  // pass...
}

bool
HD1Codeplug::RadioIdElement::isValid() const {
  return Element::isValid() && (! allBytes(_data, size(), 0xff));
}

void
HD1Codeplug::RadioIdElement::clear() {
  memset(_data, 0xff, size());
}

QString
HD1Codeplug::RadioIdElement::name() const {
  return readASCII(Offset::name(), Limit::nameLength(), 0x00);
}
void
HD1Codeplug::RadioIdElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

unsigned int
HD1Codeplug::RadioIdElement::number() const {
  return getUInt32_le(Offset::number());
}
void
HD1Codeplug::RadioIdElement::setNumber(unsigned int number) {
  setUInt32_le(Offset::number(), number);
}

DMRRadioID *
HD1Codeplug::RadioIdElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(ctx); Q_UNUSED(err);
  return new DMRRadioID(name(), number());
}

bool
HD1Codeplug::RadioIdElement::encode(const DMRRadioID *id, Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx); Q_UNUSED(err);
  setNumber(id->number());
  setName(id->name());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::RadioIdBankElement
 * ********************************************************************************************* */
HD1Codeplug::RadioIdBankElement::RadioIdBankElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::RadioIdBankElement::RadioIdBankElement(uint8_t *ptr)
  : RadioIdBankElement(ptr, size())
{
  // pass...
}

HD1Codeplug::RadioIdElement
HD1Codeplug::RadioIdBankElement::id(unsigned int n) const {
  return RadioIdElement(_data + n*RadioIdElement::size());
}

bool
HD1Codeplug::RadioIdBankElement::decode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::ids(); i++) {
    RadioIdElement el = id(i);
    if (! el.isValid())
      continue;
    DMRRadioID *rid = el.decode(ctx, err);
    ctx.config()->radioIDs()->add(rid);
    ctx.add(rid, i);
    if (nullptr == ctx.config()->settings()->defaultId())
      ctx.config()->settings()->setDefaultId(rid);
  }
  return true;
}

bool
HD1Codeplug::RadioIdBankElement::encode(Context &ctx, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::ids(); i++) {
    RadioIdElement el = id(i);
    el.clear();
    if (ctx.has<DMRRadioID>(i) && (! el.encode(ctx.get<DMRRadioID>(i), ctx, err))) {
      errMsg(err) << "Cannot encode radio ID " << (i+1) << ".";
      return false;
    }
  }
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ChannelBitmapElement
 * ********************************************************************************************* */
HD1Codeplug::ChannelBitmapElement::ChannelBitmapElement(uint8_t *ptr)
  : InvertedBitmapElement(ptr, size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::ScanBitmapElement
 * ********************************************************************************************* */
HD1Codeplug::ScanBitmapElement::ScanBitmapElement(uint8_t *ptr)
  : InvertedBitmapElement(ptr, size())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug::GeneralSettingsElement
 * ********************************************************************************************* */
HD1Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

HD1Codeplug::GeneralSettingsElement::GeneralSettingsElement(uint8_t *ptr)
  : GeneralSettingsElement(ptr, size())
{
  // pass...
}

unsigned int
HD1Codeplug::GeneralSettingsElement::squelch() const {
  return std::min(9U, unsigned(getUInt8(Offset::squelch())));
}
void
HD1Codeplug::GeneralSettingsElement::setSquelch(unsigned int level) {
  setUInt8(Offset::squelch(), std::min(9U, level));
}

bool
HD1Codeplug::GeneralSettingsElement::voicePrompts() const {
  return getBit(Offset::voicePrompts());
}
void
HD1Codeplug::GeneralSettingsElement::enableVoicePrompts(bool enable) {
  setBit(Offset::voicePrompts(), enable);
}

bool
HD1Codeplug::GeneralSettingsElement::decode(Context &ctx, const ErrorStack &err) const {
  Q_UNUSED(err);
  ctx.config()->settings()->audio()->setSquelch(Level::fromValue(squelch(), {0, 9}));
  ctx.config()->settings()->audio()->enableSpeechSynthesis(voicePrompts());
  return true;
}

bool
HD1Codeplug::GeneralSettingsElement::encode(Context &ctx, const ErrorStack &err) {
  Q_UNUSED(err);
  setSquelch(ctx.config()->settings()->audio()->squelch().mapTo({0, 9}));
  enableVoicePrompts(ctx.config()->settings()->audio()->speechSynthesisEnabled());
  return true;
}


/* ********************************************************************************************* *
 * Implementation of HD1Codeplug
 * ********************************************************************************************* */
HD1Codeplug::HD1Codeplug(QObject *parent)
  : Codeplug(parent)
{
  addImage("Ailunce HD1 settings (0x0f)");
  addImage("Ailunce HD1 memory (0x31)");
  addImage("Ailunce HD1 info (0x0d)");
  for (const Region &r: Region::elements())
    image(r.bank).addElement(r.address, r.size);
}

uint8_t *
HD1Codeplug::bankData(HD1Interface::Bank bank, uint32_t address) {
  if (! image(bank).isAllocated(address))
    return nullptr;
  return data(address, bank);
}

Config *
HD1Codeplug::preprocess(Config *config, const ErrorStack &err) const {
  Config *copy = Codeplug::preprocess(config, err);
  if (nullptr == copy) {
    errMsg(err) << "Cannot pre-process HD1 codeplug.";
    return nullptr;
  }

  // Remove AM and M17 channels, the HD1 has neither.
  ObjectFilterVisitor filter{AMChannel::staticMetaObject, M17Channel::staticMetaObject};
  if (! filter.process(copy, err)) {
    errMsg(err) << "Cannot remove AM and M17 channels.";
    errMsg(err) << "Cannot pre-process HD1 codeplug.";
    delete copy;
    return nullptr;
  }

  // Split dual-zones into two.
  ZoneSplitVisitor splitter;
  if (! splitter.process(copy, err)) {
    errMsg(err) << "Cannot split dual VFO zones.";
    errMsg(err) << "Cannot pre-process HD1 codeplug.";
    delete copy;
    return nullptr;
  }

  return copy;
}

bool
HD1Codeplug::postprocess(Config *config, const ErrorStack &err) const {
  if (! Codeplug::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process HD1 codeplug.";
    return false;
  }

  // Merge split zones into one.
  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process HD1 codeplug.";
    return false;
  }

  return true;
}

bool
HD1Codeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  // All indices are 0-based, i.e. the slot number minus one.

  // There must be a default DMR radio ID.
  if (nullptr == config->settings()->defaultId()) {
    errMsg(err) << "No default DMR radio ID specified.";
    errMsg(err) << "Cannot index codeplug for encoding for the HD1.";
    return false;
  }

  for (int i=0, d=0; i<config->radioIDs()->count(); i++) {
    if (DMRRadioID *id = config->radioIDs()->get(i)->as<DMRRadioID>()) {
      if (d >= int(RadioIdBankElement::Limit::ids())) {
        errMsg(err) << "The HD1 holds at most " << RadioIdBankElement::Limit::ids() << " radio IDs.";
        return false;
      }
      ctx.add(id, d++);
    }
  }

  for (int i=0, d=0; i<config->contacts()->count(); i++) {
    if (DMRContact *c = config->contacts()->contact(i)->as<DMRContact>()) {
      if (d >= int(ContactBankElement::Limit::contacts())) {
        errMsg(err) << "The HD1 holds at most " << ContactBankElement::Limit::contacts() << " contacts.";
        return false;
      }
      ctx.add(c, d++);
    }
  }

  for (int i=0; i<config->rxGroupLists()->count(); i++)
    ctx.add(config->rxGroupLists()->list(i), i);

  if (config->channelList()->count() > int(ChannelBankElement::Limit::channels())) {
    errMsg(err) << "The HD1 holds at most " << ChannelBankElement::Limit::channels() << " channels.";
    return false;
  }
  for (int i=0; i<config->channelList()->count(); i++)
    ctx.add(config->channelList()->channel(i), i);

  if (config->zones()->count() > int(ZoneBankElement::Limit::zones())) {
    errMsg(err) << "The HD1 holds at most " << ZoneBankElement::Limit::zones() << " zones.";
    return false;
  }
  for (int i=0; i<config->zones()->count(); i++)
    ctx.add(config->zones()->zone(i), i);

  for (int i=0; i<config->scanlists()->count(); i++)
    ctx.add(config->scanlists()->scanlist(i), i);

  return true;
}

bool
HD1Codeplug::decode(Config *config, const ErrorStack &err) {
  Context ctx(config);

  if (! decodeElements(ctx, err)) {
    errMsg(err) << "Cannot decode HD1 codeplug.";
    return false;
  }

  if (! linkElements(ctx, err)) {
    errMsg(err) << "Cannot decode HD1 codeplug.";
    return false;
  }

  return true;
}

bool
HD1Codeplug::decodeElements(Context &ctx, const ErrorStack &err) {
  if (! RadioIdBankElement(bankData(HD1Interface::Small, Offset::radioIds())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode radio IDs.";
    return false;
  }

  if (! ContactBankElement(bankData(HD1Interface::Bulk, Offset::contacts())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode contacts.";
    return false;
  }

  if (! GroupListBankElement(bankData(HD1Interface::Bulk, Offset::groupLists())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode group lists.";
    return false;
  }

  if (! ChannelBankElement(bankData(HD1Interface::Bulk, Offset::channels())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode channels.";
    return false;
  }

  if (! ZoneBankElement(bankData(HD1Interface::Bulk, Offset::zones())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode zones.";
    return false;
  }

  if (! GeneralSettingsElement(bankData(HD1Interface::Small, Offset::generalSettings())).decode(ctx, err)) {
    errMsg(err) << "Cannot decode general settings.";
    return false;
  }

  return true;
}

bool
HD1Codeplug::linkElements(Context &ctx, const ErrorStack &err) {
  if (! GroupListBankElement(bankData(HD1Interface::Bulk, Offset::groupLists())).link(ctx, err)) {
    errMsg(err) << "Cannot link group lists.";
    return false;
  }

  // After the group lists: channels are linked to the list with their members.
  if (! ChannelBankElement(bankData(HD1Interface::Bulk, Offset::channels())).link(ctx, err)) {
    errMsg(err) << "Cannot link channels.";
    return false;
  }

  if (! ZoneBankElement(bankData(HD1Interface::Bulk, Offset::zones())).link(ctx, err)) {
    errMsg(err) << "Cannot link zones.";
    return false;
  }

  return true;
}

bool
HD1Codeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  Q_UNUSED(flags);

  Context ctx(config);
  if (! index(config, ctx, err)) {
    errMsg(err) << "Cannot encode HD1 codeplug.";
    return false;
  }

  if (! encodeElements(ctx, err)) {
    errMsg(err) << "Cannot encode HD1 codeplug.";
    return false;
  }

  return true;
}

bool
HD1Codeplug::encodeElements(Context &ctx, const ErrorStack &err) {
  if (! RadioIdBankElement(bankData(HD1Interface::Small, Offset::radioIds())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode radio IDs.";
    return false;
  }

  if (! ContactBankElement(bankData(HD1Interface::Bulk, Offset::contacts())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode contacts.";
    return false;
  }

  if (! GroupListBankElement(bankData(HD1Interface::Bulk, Offset::groupLists())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode group lists.";
    return false;
  }

  ChannelBankElement channels(bankData(HD1Interface::Bulk, Offset::channels()));
  if (! channels.encode(ctx, err)) {
    errMsg(err) << "Cannot encode channels.";
    return false;
  }

  // The bitmaps duplicate which channels exist and their scan-add flags.
  ChannelBitmapElement used(bankData(HD1Interface::Small, Offset::channelBitmap()));
  ScanBitmapElement scan(bankData(HD1Interface::Small, Offset::scanBitmap()));
  for (unsigned int i=0; i<ChannelBankElement::Limit::channels(); i++) {
    used.setEncoded(i, channels.channel(i).isValid());
    scan.setEncoded(i, channels.channel(i).isValid() && channels.channel(i).scanAdd());
  }

  if (! ZoneBankElement(bankData(HD1Interface::Bulk, Offset::zones())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode zones.";
    return false;
  }

  if (! GeneralSettingsElement(bankData(HD1Interface::Small, Offset::generalSettings())).encode(ctx, err)) {
    errMsg(err) << "Cannot encode general settings.";
    return false;
  }

  return true;
}
