#include "opengd77_satelliteconfig.hh"
#include "satellitedatabase.hh"


/* ********************************************************************************************* *
 * Implementation of OpenGD77SatelliteConfig::SatelliteElement
 * ********************************************************************************************* */
OpenGD77SatelliteConfig::SatelliteElement::SatelliteElement(uint8_t *ptr, size_t size)
  : Element(ptr, size)
{
  // pass...
}

OpenGD77SatelliteConfig::SatelliteElement::SatelliteElement(uint8_t *ptr)
  : Element(ptr, size())
{
  // pass...
}


void
OpenGD77SatelliteConfig::SatelliteElement::clear() {
  memset(_data, 0, size());
}


void
OpenGD77SatelliteConfig::SatelliteElement::writeDigit(const Offset::Bit &offset, uint8_t digit) {
  // Must be bit 0 or 4 (BCD)
  if (offset.bit % 4)
    return;

  uint8_t val = getUInt8(offset.byte);
  val &= ~(0xf << offset.bit);
  val |= ((digit & 0xf) << offset.bit);
  setUInt8(offset.byte, val);
}


void
OpenGD77SatelliteConfig::SatelliteElement::writeInteger(const Offset::Bit &offset, int value, bool sign, unsigned int dec) {
  unsigned int o = 0;
  // Must be bit 0 or 4 (BCD)
  if (offset.bit % 4)
    return;
  if (0 == dec)
    return;

  if (sign && 0 > value)
    writeDigit(offset + o, 0xc); // '-' ?!?
  else
    writeDigit(offset + o, 0xb); // blank

  o += 4*(dec-1);
  for (int i=dec; i>0; i--, o = o - 4) {
    if (value)
      writeDigit(offset + o, value % 10);
    else
      writeDigit(offset + o, 0xb);
    value /= 10;
  }
}


void
OpenGD77SatelliteConfig::SatelliteElement::writeFractional(const Offset::Bit &offset, double value, bool sign, unsigned int frac) {
  unsigned int o = 0;
  if (offset.bit % 4)
    return;

  if (0 == frac)
    return;

  if (sign && (0 > value))
    writeDigit(offset + o, 0xc);
  else
    writeDigit(offset + o, 0xb);
  o += 4;

  value -= int(value);
  for (unsigned int i=0; i<frac; i++, o += 4) {
    value *= 10;
    writeDigit(offset + o, int(value));
    value -= int(value);
  }
}


void
OpenGD77SatelliteConfig::SatelliteElement::writeFixedPoint(const Offset::Bit &offset, double value, bool sign, unsigned int dec, unsigned int frac) {
  unsigned int o = 0;
  writeInteger(offset + o, value, sign, dec);
  o += 4*dec + (sign ? 4 : 0);

  writeDigit(offset + o, 0xa);
  o += 4;

  value = std::abs(value);
  value -= int(value);
  writeFractional(offset + o, value, false, frac);
}


void
OpenGD77SatelliteConfig::SatelliteElement::setName(const QString &name) {
  writeASCII(Offset::name(), name, Limit::nameLength(), 0x00);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setEpoch(const OrbitalElement::Epoch &epoch) {
  writeInteger(Offset::epochYear(), epoch.year%100, false, 2);
  writeFixedPoint(Offset::epochJulienDay(), epoch.toEpoch(), false, 3, 8);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setMeanMotionDerivative(double dmm) {
  writeFixedPoint(Offset::meanMotionDerivative(), dmm, true, 0, 8);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setInclination(double incl) {
  writeFixedPoint(Offset::inclination(), incl, false, 3, 4);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setAscension(double asc) {
  writeFixedPoint(Offset::ascension(), asc, false, 3, 4);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setEccentricity(double ecc) {
  writeFractional(Offset::eccentricity(), ecc, false, 7);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setPerigee(double arg) {
  writeFixedPoint(Offset::perigee(), arg, false, 3, 4);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setMeanAnomaly(double ma) {
  writeFixedPoint(Offset::meanAnomaly(), ma, false, 3, 4);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setMeanMotion(double mm) {
  writeFixedPoint(Offset::meanMotion(), mm, false, 2, 8);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setRevolutionNumber(unsigned int num) {
  writeInteger(Offset::revolutionNumber(), num, false, 5);
}


void
OpenGD77SatelliteConfig::SatelliteElement::setFMDownlink(const Frequency &f) {
  setUInt32_le(Offset::fmDownlink(), f.inHz()/10);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setFMUplink(const Frequency &f) {
  setUInt32_le(Offset::fmUplink(), f.inHz()/10);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setCTCSS(const SelectiveCall &call) {
  if (! call.isCTCSS())
    return;
  setUInt32_le(Offset::ctcss(), call.mHz()/100);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setAPRSDownlink(const Frequency &f) {
  setUInt32_le(Offset::aprsDownlink(), f.inHz()/10);
}

void
OpenGD77SatelliteConfig::SatelliteElement::setAPRSUplink(const Frequency &f) {
  setUInt32_le(Offset::aprsUplink(), f.inHz()/10);
}


void
OpenGD77SatelliteConfig::SatelliteElement::setBeacon(const Frequency &f) {
  setUInt32_le(Offset::beacon(), f.inHz()/10);
}


void
OpenGD77SatelliteConfig::SatelliteElement::setAPRSPath(const QString &path) {
  writeASCII(Offset::aprsPath(), path, Limit::pathLength(), 0x00);
}


bool
OpenGD77SatelliteConfig::SatelliteElement::encode(const Satellite &sat, const ErrorStack &err) {
  Q_UNUSED(err);

  // meta
  setName(sat.name());

  // orbital elements
  setMeanMotion(sat.meanMotion());
  setMeanMotionDerivative(sat.meanMotionDerivative());
  setInclination(sat.inclination());
  setAscension(sat.ascension());
  setEccentricity(sat.eccentricity());
  setPerigee(sat.perigee());
  setMeanAnomaly(sat.meanAnomaly());
  setRevolutionNumber(sat.revolutionNumber());

  // transponder
  setFMDownlink(sat.fmDownlink());
  setFMUplink(sat.fmUplink());
  setCTCSS(sat.fmUplinkTone());
  setAPRSDownlink(sat.aprsDownlink());
  setAPRSUplink(sat.aprsUplink());
  setBeacon(sat.beacon());

  /// @bug set APRS path

  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77SatelliteConfig
 * ********************************************************************************************* */
OpenGD77SatelliteConfig::OpenGD77SatelliteConfig(QObject *parent)
  : SatelliteConfig{parent}
{
  addImage("OpenGD77 satellite configuration EEPROM");
  addImage("OpenGD77 satellite configuration FLASH");
  image(FLASH).addElement(0x00000000, 0x11a0);
}

OpenGD77SatelliteConfig::SatelliteElement
OpenGD77SatelliteConfig::satellite(unsigned int idx) {
  return SatelliteElement(data(Offset::satellites() + idx*Offset::betweenSatellites(), FLASH));
}

bool
OpenGD77SatelliteConfig::encode(SatelliteDatabase *db, const ErrorStack &err) {
  for (unsigned int i=0; i<Limit::satellites(); i++) {
    SatelliteElement el = satellite(i);
    el.clear();
    if (db->count() <= i)
      continue;
    if (! el.encode(db->getAt(i), err)) {
      errMsg(err) << "Cannot encode satellite '" << db->getAt(i).name()
                  << "' at index " << i << ".";
      return false;
    }
  }

  return true;
}
