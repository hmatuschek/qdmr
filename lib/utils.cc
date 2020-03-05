#include "utils.hh"
#include <QRegExp>
#include <QVector>
#include <cmath>


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

double
decode_frequency(uint32_t bcd) {
  double freq =
      1e2  * ((bcd >> 28) & 0xf) +
      1e1  * ((bcd >> 24) & 0xf) +
      1.0  * ((bcd >> 20) & 0xf) +
      1e-1 * ((bcd >> 16) & 0xf) +
      1e-2 * ((bcd >> 12) & 0xf) +
      1e-3 * ((bcd >>  8) & 0xf) +
      1e-4 * ((bcd >>  4) & 0xf) +
      1e-5 * ((bcd >>  0) & 0xf);
  return freq;
}

uint32_t
encode_frequency(double freq) {
  uint32_t hz = std::round(freq * 1e6);
  uint32_t a  = (hz / 100000000) % 10;
  uint32_t b  = (hz /  10000000) % 10;
  uint32_t c  = (hz /   1000000) % 10;
  uint32_t d  = (hz /    100000) % 10;
  uint32_t e  = (hz /     10000) % 10;
  uint32_t f  = (hz /      1000) % 10;
  uint32_t g  = (hz /       100) % 10;
  uint32_t h  = (hz /        10) % 10;

  return (a << 28) + (b << 24) + (c << 20) + (d << 16) + (e << 12) + (f << 8) + (g << 4) + h;
}


uint32_t decode_dmr_id_bin(const uint8_t *id) {
  return ( (id[0]) | (id[1] << 8) | (id[2] << 16) );
}

void encode_dmr_id_bin(uint8_t *id, uint32_t no) {
  id[0] = no;
  id[1] = no >> 8;
  id[2] = no >> 16;
}


uint32_t decode_dmr_id_bcd(const uint8_t *id) {
  return ((id[0] >> 4) * 10000000
      + (id[0] & 15) * 1000000
      + (id[1] >> 4) * 100000
      + (id[1] & 15) * 10000
      + (id[2] >> 4) * 1000
      + (id[2] & 15) * 100
      + (id[3] >> 4) * 10
      + (id[3] & 15));
}

void encode_dmr_id_bcd(uint8_t *id, uint32_t no) {
  id[0] = ((no / 10000000) << 4)    | ((no / 1000000) % 10);
  id[1] = ((no / 100000 % 10) << 4) | ((no / 10000) % 10);
  id[2] = ((no / 1000 % 10) << 4)   | ((no / 100) % 10);
  id[3] = ((no / 10 % 10) << 4)     | (no % 10);
}

QVector<char> bin_dtmf_tab = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','*','#'};

QString
decode_dtmf_bin(const uint8_t *num, int size, uint8_t fill) {
  Q_UNUSED(fill);
  QString number;
  for (int i=0; (i<size)&&(num[i]<16); i++) {
    number.append(bin_dtmf_tab[num[i]]);
  }
  return number;
}

bool
encode_dtmf_bin(const QString &number, uint8_t *num, int size, uint8_t fill) {
  QString tmp = number.simplified().toUpper();
  for (int i=0; i<size; i++) {
    num[i] = fill;
    if (i>=tmp.size())
      continue;
    int idx = bin_dtmf_tab.indexOf(number.at(i).toLatin1());
    if (idx<0)
      continue;
    num[i] = idx;
  }
  return true;
}

float decode_ctcss_tone_table(uint16_t data) {
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
encode_ctcss_tone_table(float hz)
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
    return 0xffff;

  a = val / 1000;
  b = val / 100 % 10;
  c = val / 10 % 10;
  d = val % 10;
  tag = 0;

  return (a << 12) | (b << 8) | (c << 4) | d | (tag << 14);
}

bool
validDMRNumber(const QString &text) {
  return QRegExp("^[0-9]+$").exactMatch(text);
}

bool
validDTMFNumber(const QString &text) {
  return QRegExp("^[0-9a-dA-D\\*#]+$").exactMatch(text);
}

QString
format_frequency(double MHz) {
  return QString::number(MHz, 'f', 5);
}
