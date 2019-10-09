#include "utils.hh"
#include <QDebug>

QString
decode_unicode(const uint16_t *data, size_t size, uint16_t fill) {
  QString res; res.reserve(size);
  for (size_t i=0; (i<size) && (fill!=data[i]); i++) {
    res.append(QChar(data[i]));
  }
  return res;
}

void
encode_unicode(uint16_t *data, const QString &text, size_t size, uint16_t fill) {
  for (size_t i=0; (i<size); i++) {
    if (i<size_t(text.size()))
      data[i] = text.at(i).unicode();
    else
      data[i] = fill;
  }
}

QString
decode_ascii(const uint8_t *data, size_t size, uint16_t fill) {
  QString res; res.reserve(size);
  for (size_t i=0; (i<size) && (fill!=data[i]); i++)
    res.append(QChar(data[i]));
  return res;
}

void
encode_ascii(uint8_t *data, const QString &text, size_t size, uint16_t fill) {
  for (size_t i=0; (i<size); i++) {
    if (i<size_t(text.size()))
      data[i] = text.at(i).unicode();
    else
      data[i] = fill;
  }
}

inline int iround(double x) {
  if (x >= 0)
    return (int)(x + 0.5);
  return -(int)(-x + 0.5);
}


double
decode_frequency(uint32_t bcd) {
  double freq = 100.0*((bcd>> 28) & 15) + 10.0*((bcd >> 24) & 15) + 1.0*((bcd >> 20) & 15)
      + 0.1*((bcd >> 16) & 15) + 0.01*((bcd >> 12) & 15) + 0.001*((bcd >> 8) & 15);
  if ((bcd & 0xff) != 0) {
    freq += 0.0001*((bcd >> 4) & 15);
    if ((bcd & 0x0f) != 0)
      freq += 0.00001*(bcd & 15);
  }
  return freq;
}

uint32_t
encode_frequency(double freq) {
  uint32_t hz = iround(freq * 1000000.0);
  uint32_t a  = (hz / 100000000) % 10;
  uint32_t b  = (hz / 10000000)  % 10;
  uint32_t c  = (hz / 1000000)   % 10;
  uint32_t d  = (hz / 100000)    % 10;
  uint32_t e  = (hz / 10000)     % 10;
  uint32_t f  = (hz / 1000)      % 10;
  uint32_t g  = (hz / 100)       % 10;
  uint32_t h  = (hz / 10)        % 10;

  return g << 28 | h << 24 | e << 20 | f << 16 | c << 12 | d << 8 | a << 4 | b;
}


uint32_t decode_dmr_id(const uint8_t *id) {
  return ( (id[0]) | (id[1] << 8) | (id[2] << 16) );
}

void encode_dmr_id(uint8_t *id, uint32_t no) {
  id[0] = no;
  id[1] = no >> 8;
  id[2] = no >> 16;
}


float decode_ctcss_tone(uint16_t data) {
  if (data == 0xffff)
    return 0.0;

  unsigned tag = data >> 14;
  unsigned a = (data >> 12) & 3;
  unsigned b = (data >> 8) & 15;
  unsigned c = (data >> 4) & 15;
  unsigned d = data & 15;

  switch (tag) {
    case 2:
    case 3:
      return 0;
    default:
      return 100.0*a+10.0*b+1.0*c+0.1*d;
  }
}

#define NCTCSS  50
static const unsigned CTCSS_TONES [NCTCSS] = {
  670,  693,  719,  744,  770,  797,  825,  854,  885,  915,
  948,  974, 1000, 1035, 1072, 1109, 1148, 1188, 1230, 1273,
  1318, 1365, 1413, 1462, 1514, 1567, 1598, 1622, 1655, 1679,
  1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928, 1966, 1995,
  2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541,
};

uint16_t
encode_ctcss_tone(float hz)
{
  unsigned val, tag, a, b, c, d;

  // Disabled
  if (0 == hz)
    return 0xffff;

  //
  // CTCSS tone
  //
  // Round to integer.
  val = hz * 10.0 + 0.5;

  // Find a valid index in CTCSS table.
  int i;
  for (i=0; i<NCTCSS; i++)
    if (CTCSS_TONES[i] == val)
      break;
  if (i >= NCTCSS)
    return -1;

  a = val / 1000;
  b = val / 100 % 10;
  c = val / 10 % 10;
  d = val % 10;
  tag = 0;

  return (a << 12) | (b << 8) | (c << 4) | d | (tag << 14);
}
