#include "anytone_satelliteconfig.hh"
#include "anytone_codeplug.hh"
#include "satellitedatabase.hh"


/* ********************************************************************************************* *
 * Implementation of AnytoneSatelliteConfig::Satellite
 * ********************************************************************************************* */
AnytoneSatelliteConfig::SatelliteElement::SatelliteElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

AnytoneSatelliteConfig::SatelliteElement::SatelliteElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}

void
AnytoneSatelliteConfig::SatelliteElement::clear() {
  memset(_data, 0, size());
  memset(_data, 0x20, 0x50);
}

void
AnytoneSatelliteConfig::SatelliteElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::name(), 0x20);
}

void
AnytoneSatelliteConfig::SatelliteElement::setEpoch(const OrbitalElement::Epoch &epoch) {
  double tmp = epoch.toEpoch();
  int year = epoch.year;
  int day  = tmp; tmp -= day; tmp *= 100000000;
  int fday = tmp;

  QString fmt_year = QString("%1").arg(year%100, 2, 10, QChar('0'));
  QString fmt_day = QString("%1.%2")
      .arg(day, 3, 10, QChar('0'))
      .arg(fday, 8, 10, QChar('0'));

  writeASCII(Offset::epochYear(), fmt_year, 2);
  writeASCII(Offset::epochDay(), fmt_day, 12);
}

void
AnytoneSatelliteConfig::SatelliteElement::setMeanMotionDerivative(double dmm) {
  QString fmt = QString("%1").arg((int)(dmm * 100000000), 8, 10, QChar('0'));
  writeASCII(Offset::meanMotionDerivative(), fmt, 8);
}

void
AnytoneSatelliteConfig::SatelliteElement::setInclination(double incl) {
  int ddd = incl; incl -= ddd;
  int ffff = incl*10000;

  QString fmt = QString("%1.%2")
      .arg(ddd, 3, 10, QChar(' '))
      .arg(ffff, 4, 10, QChar('0'));

  writeASCII(Offset::inclination(), fmt, 8);
}

void
AnytoneSatelliteConfig::SatelliteElement::setAscension(double asc) {
  int ddd = asc; asc -= ddd;
  int ffff = asc*10000;

  QString fmt = QString("%1.%2")
      .arg(ddd, 3, 10, QChar(' '))
      .arg(ffff, 4, 10, QChar('0'));

  writeASCII(Offset::ascension(), fmt, 8);
}

void
AnytoneSatelliteConfig::SatelliteElement::setEccentricity(double ecc) {
  QString fmt = QString("%1").arg((int)(ecc * 10000000), 7, 10, QChar('0'));
  writeASCII(Offset::meanMotionDerivative(), fmt, 7);
}

void
AnytoneSatelliteConfig::SatelliteElement::setPerigee(double peri) {
  int ddd = peri; peri -= ddd;
  int ffff = peri*10000;

  QString fmt = QString("%1.%2")
      .arg(ddd, 3, 10, QChar(' '))
      .arg(ffff, 4, 10, QChar('0'));

  writeASCII(Offset::perigee(), fmt, 8);
}

void
AnytoneSatelliteConfig::SatelliteElement::setAnomaly(double ma) {
  int ddd = ma; ma -= ddd;
  int ffff = ma*10000;

  QString fmt = QString("%1.%2")
      .arg(ddd, 3, 10, QChar(' '))
      .arg(ffff, 4, 10, QChar('0'));

  writeASCII(Offset::anomaly(), fmt, 8);
}

void
AnytoneSatelliteConfig::SatelliteElement::setMeanMotion(double mm) {
  int dd = mm; mm -= dd;
  int ffffffff = mm*100000000;

  QString fmt = QString("%1.%2")
      .arg(dd, 2, 10, QChar(' '))
      .arg(ffffffff, 8, 10, QChar('0'));

  writeASCII(Offset::meanMotion(), fmt, 11);
}

void
AnytoneSatelliteConfig::SatelliteElement::setRevolution(unsigned int num) {
  writeASCII(Offset::revolution(), QString("%1").arg(num, 5, 10, QChar('0')), 5);
}


void
AnytoneSatelliteConfig::SatelliteElement::setDownlink(const Frequency &f) {
  setUInt32_le(Offset::downlinkFrequency(), f.inHz()/10);
}

void
AnytoneSatelliteConfig::SatelliteElement::setDownlinkTone(const SelectiveCall &tone) {
  if (tone.isInvalid()) {
    setUInt8(Offset::downlinkToneType(), (int)ToneType::None);
  } else if (tone.isCTCSS()) {
    setUInt8(Offset::downlinkToneType(), (int)ToneType::CTCSS);
    setUInt8(Offset::downlinkCTCSS(), AnytoneCodeplug::CTCSS::encode(tone));
  } else if (tone.isDCS()) {
    uint16_t val = tone.binCode();
    if (tone.isInverted()) val |= (1<<9);
    setUInt8(Offset::downlinkToneType(), (int)ToneType::DCS);
    setUInt16_le(Offset::downlinkDCS(), val);
  }
}

void
AnytoneSatelliteConfig::SatelliteElement::setUplink(const Frequency &f) {
  setUInt32_le(Offset::uplinkFrequency(), f.inHz()/10);
}

void
AnytoneSatelliteConfig::SatelliteElement::setUplinkTone(const SelectiveCall &tone) {
  if (tone.isInvalid()) {
    setUInt8(Offset::uplinkToneType(), (int)ToneType::None);
  } else if (tone.isCTCSS()) {
    setUInt8(Offset::uplinkToneType(), (int)ToneType::CTCSS);
    setUInt8(Offset::uplinkCTCSS(), AnytoneCodeplug::CTCSS::encode(tone));
  } else if (tone.isDCS()) {
    uint16_t val = tone.binCode();
    if (tone.isInverted()) val |= (1<<9);
    setUInt8(Offset::uplinkToneType(), (int)ToneType::DCS);
    setUInt16_le(Offset::uplinkDCS(), val);
  }
}


bool
AnytoneSatelliteConfig::SatelliteElement::encode(const Satellite &sat, const ErrorStack &err) {
  Q_UNUSED(err)

  setName(sat.name());
  setEpoch(sat.epoch());
  setMeanMotionDerivative(sat.meanMotionDerivative());
  setInclination(sat.inclination());
  setAscension(sat.ascension());
  setEccentricity(sat.eccentricity());
  setPerigee(sat.perigee());
  setAnomaly(sat.meanAnomaly());
  setMeanMotion(sat.meanMotion());
  setRevolution(sat.revolutionNumber());

  setDownlink(sat.fmDownlink());
  setDownlinkTone(sat.fmDownlinkTone());
  setUplink(sat.fmUplink());
  setUplinkTone(sat.fmUplinkTone());

  return true;
}



/* ********************************************************************************************* *
 * Implementation of AnytoneSatelliteConfig
 * ********************************************************************************************* */
AnytoneSatelliteConfig::AnytoneSatelliteConfig(QObject *parent)
  : SatelliteConfig{parent}
{
  addImage("AnyTone Satellite Configuration");
  image(0).addElement(Offset::satellites(), 0xa000);
}


AnytoneSatelliteConfig::SatelliteElement
AnytoneSatelliteConfig::satellite(unsigned int idx) {
  return SatelliteElement(data(Offset::satellites() + idx*Offset::betweenSatellites()));
}


bool
AnytoneSatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  unsigned int numSat = std::min(Limit::satellites(), db->count());

  for (unsigned int i=0; i<numSat; i++) {
    if (! satellite(i).encode(db->getAt(i), err)) {
      errMsg(err) << "Cannot encode satellite '" << db->getAt(i).name()
                  << "at index " << i << ".";
      return false;
    }
  }

  return true;
}
