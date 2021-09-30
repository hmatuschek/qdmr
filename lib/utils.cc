#include "utils.hh"
#include <QRegExp>
#include <QVector>
#include <QHash>
#include <cmath>

// Maps APRS icon number to code-char
static QVector<char> aprsIconCodeTable{
  '!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0',
  '1','2','3','4','5','6', '7','8','9',':',';','<','=','>','?','@',
  'A','B','C','D','E','F', 'G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V', 'W','X','Y','Z','[','/',']','^','_','`',
  'a','b','c','d','e','f', 'g','h','i','j','k','l','m','n','o','p',
  'q','r','s','t','u','v','w','x','y','z','{','|','}','~'};

static QHash<unsigned, QString> aprsIconNameTable{
  {(unsigned)APRSSystem::Icon::None, ""},
  {(unsigned)APRSSystem::Icon::PoliceStation, "Police station"},
  {(unsigned)APRSSystem::Icon::Digipeater, "Digipeater"},
  {(unsigned)APRSSystem::Icon::Phone, "Phone"},
  {(unsigned)APRSSystem::Icon::DXCluster, "DX cluster"},
  {(unsigned)APRSSystem::Icon::HFGateway, "HF gateway"},
  {(unsigned)APRSSystem::Icon::SmallPlane, "Small plane"},
  {(unsigned)APRSSystem::Icon::MobileSatelliteStation, "Mobile Satellite station"},
  {(unsigned)APRSSystem::Icon::WheelChair, "Wheel chair"},
  {(unsigned)APRSSystem::Icon::Snowmobile, "Snowmobile"},
  {(unsigned)APRSSystem::Icon::RedCross, "Red cross"},
  {(unsigned)APRSSystem::Icon::BoyScout, "Boy scout"},
  {(unsigned)APRSSystem::Icon::Home,  "Home"},
  {(unsigned)APRSSystem::Icon::X, "X"},
  {(unsigned)APRSSystem::Icon::RedDot, "Red dot"},
  {(unsigned)APRSSystem::Icon::Circle0, "Circle 0"},
  {(unsigned)APRSSystem::Icon::Circle1, "Circle 1"},
  {(unsigned)APRSSystem::Icon::Circle2, "Circle 2"},
  {(unsigned)APRSSystem::Icon::Circle3, "Circle 3"},
  {(unsigned)APRSSystem::Icon::Circle4, "Circle 4"},
  {(unsigned)APRSSystem::Icon::Circle5, "Circle 5"},
  {(unsigned)APRSSystem::Icon::Circle6, "Circle 6"},
  {(unsigned)APRSSystem::Icon::Circle7, "Circle 7"},
  {(unsigned)APRSSystem::Icon::Circle8, "Circle 8"},
  {(unsigned)APRSSystem::Icon::Circle9, "Circle 9"},
  {(unsigned)APRSSystem::Icon::Fire, "Fire"},
  {(unsigned)APRSSystem::Icon::Campground, "Campground"},
  {(unsigned)APRSSystem::Icon::Motorcycle, "Motorcycle"},
  {(unsigned)APRSSystem::Icon::RailEngine, "Rail engine"},
  {(unsigned)APRSSystem::Icon::Car, "Car"},
  {(unsigned)APRSSystem::Icon::FileServer, "File server"},
  {(unsigned)APRSSystem::Icon::HCFuture, "HC future"},
  {(unsigned)APRSSystem::Icon::AidStation, "Aid station"},
  {(unsigned)APRSSystem::Icon::BBS, "BBS"},
  {(unsigned)APRSSystem::Icon::Canoe, "Canoe"},
  {(unsigned)APRSSystem::Icon::Eyeball, "Eyeball"},
  {(unsigned)APRSSystem::Icon::Tractor, "Tractor"},
  {(unsigned)APRSSystem::Icon::GridSquare, "Grid square"},
  {(unsigned)APRSSystem::Icon::Hotel, "Hotel"},
  {(unsigned)APRSSystem::Icon::TCPIP, "TCP/IP"},
  {(unsigned)APRSSystem::Icon::School, "School"},
  {(unsigned)APRSSystem::Icon::Logon, "Logon"},
  {(unsigned)APRSSystem::Icon::MacOS, "MacOS"},
  {(unsigned)APRSSystem::Icon::NTSStation, "NTS station"},
  {(unsigned)APRSSystem::Icon::Balloon, "Balloon"},
  {(unsigned)APRSSystem::Icon::PoliceCar, "Police car"},
  {(unsigned)APRSSystem::Icon::TBD, "TBD"},
  {(unsigned)APRSSystem::Icon::RV, "RV"},
  {(unsigned)APRSSystem::Icon::Shuttle, "Shuttle"},
  {(unsigned)APRSSystem::Icon::SSTV, "SSTV"},
  {(unsigned)APRSSystem::Icon::Bus, "Bus"},
  {(unsigned)APRSSystem::Icon::ATV, "ATV"},
  {(unsigned)APRSSystem::Icon::WXService, "WX service"},
  {(unsigned)APRSSystem::Icon::Helo, "Helo"},
  {(unsigned)APRSSystem::Icon::Yacht, "Yacht"},
  {(unsigned)APRSSystem::Icon::Windows, "Windows"},
  {(unsigned)APRSSystem::Icon::Jogger, "Jogger"},
  {(unsigned)APRSSystem::Icon::Triangle, "Triangle"},
  {(unsigned)APRSSystem::Icon::PBBS, "PBBS"},
  {(unsigned)APRSSystem::Icon::LargePlane, "Large plane"},
  {(unsigned)APRSSystem::Icon::WXStation, "WX station"},
  {(unsigned)APRSSystem::Icon::DishAntenna, "Dish antenna"},
  {(unsigned)APRSSystem::Icon::Ambulance, "Ambulance"},
  {(unsigned)APRSSystem::Icon::Bike, "Bike"},
  {(unsigned)APRSSystem::Icon::ICP, "ICP"},
  {(unsigned)APRSSystem::Icon::FireStation, "Fire station"},
  {(unsigned)APRSSystem::Icon::Horse, "Horse"},
  {(unsigned)APRSSystem::Icon::FireTruck, "Fire truck"},
  {(unsigned)APRSSystem::Icon::Glider, "Glider"},
  {(unsigned)APRSSystem::Icon::Hospital, "Hospital"},
  {(unsigned)APRSSystem::Icon::IOTA, "IOTA"},
  {(unsigned)APRSSystem::Icon::Jeep, "Jeep"},
  {(unsigned)APRSSystem::Icon::SmallTruck, "Small truck"},
  {(unsigned)APRSSystem::Icon::Laptop, "Laptop"},
  {(unsigned)APRSSystem::Icon::MicE, "Mic-E"},
  {(unsigned)APRSSystem::Icon::Node, "Node"},
  {(unsigned)APRSSystem::Icon::EOC, "EOC"},
  {(unsigned)APRSSystem::Icon::Rover, "Rover"},
  {(unsigned)APRSSystem::Icon::Grid, "Grid"},
  {(unsigned)APRSSystem::Icon::Antenna, "Antenna"},
  {(unsigned)APRSSystem::Icon::PowerBoat, "Power boat"},
  {(unsigned)APRSSystem::Icon::TruckStop, "Truck stop"},
  {(unsigned)APRSSystem::Icon::TruckLarge, "Truck large"},
  {(unsigned)APRSSystem::Icon::Van, "Van"},
  {(unsigned)APRSSystem::Icon::Water, "Water"},
  {(unsigned)APRSSystem::Icon::XAPRS, "XAPRS"},
  {(unsigned)APRSSystem::Icon::Yagi, "Yagi"},
  {(unsigned)APRSSystem::Icon::Shelter, "Shelter"}};

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
  for (size_t i=0; (i<size) && (0!=data[i]) && (fill!=data[i]); i++)
    res.append(QChar(data[i]));
  return res;
}

void
encode_ascii(uint8_t *data, const QString &text, size_t size, uint16_t fill) {
  for (size_t i=0; (i<size); i++) {
    if (i<size_t(text.size()))
      data[i] = text.at(i).toLatin1();
    else
      data[i] = fill;
  }
}

QString
decode_utf8(const uint8_t *data, size_t size, uint16_t fill) {
  return QString::fromUtf8((const char *)data, size);
}

void
encode_utf8(uint8_t *data, const QString &text, size_t size, uint16_t fill) {
  QByteArray buffer = text.toUtf8();
  memset(data, fill, size);
  memcpy(data, buffer.data(), std::min(size_t(buffer.size()), size));
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

uint32_t decode_dmr_id_bcd_le(const uint8_t *id) {
  return ((id[3] >> 4) * 10000000
      + (id[3] & 15) * 1000000
      + (id[2] >> 4) * 100000
      + (id[2] & 15) * 10000
      + (id[1] >> 4) * 1000
      + (id[1] & 15) * 100
      + (id[0] >> 4) * 10
      + (id[0] & 15));
}

void encode_dmr_id_bcd(uint8_t *id, uint32_t no) {
  id[0] = ((no / 10000000) << 4)    | ((no / 1000000) % 10);
  id[1] = ((no / 100000 % 10) << 4) | ((no / 10000) % 10);
  id[2] = ((no / 1000 % 10) << 4)   | ((no / 100) % 10);
  id[3] = ((no / 10 % 10) << 4)     | (no % 10);
}

void encode_dmr_id_bcd_le(uint8_t *id, uint32_t no) {
  id[3] = ((no / 10000000) << 4)    | ((no / 1000000) % 10);
  id[2] = ((no / 100000 % 10) << 4) | ((no / 10000) % 10);
  id[1] = ((no / 1000 % 10) << 4)   | ((no / 100) % 10);
  id[0] = ((no / 10 % 10) << 4)     | (no % 10);
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

QString
decode_dtmf_bcd_be(const uint8_t *num, int digits) {
  QString number;
  for (int i=0; i<digits; i++) {
    uint8_t d = (0 == (i%2)) ? (((num[i/2])>>4)&0xf) : ((num[i/2])&0xf);
    number.append(bin_dtmf_tab[d]);
  }
  return number;
}

bool
encode_dtmf_bcd_be(const QString &number, uint8_t *num, int size, uint8_t fill) {
  memset(num, fill, size);
  QString tmp = number.simplified().toUpper();
  for (int i=0; i<tmp.size(); i++) {
    if (0 == (i%2)) {
      num[i/2] = ((bin_dtmf_tab.indexOf(tmp[i].toLatin1()))<<4);
    } else {
      num[i/2] |= bin_dtmf_tab.indexOf(tmp[i].toLatin1());
    }
  }
  return true;
}

Signaling::Code
decode_ctcss_tone_table(uint16_t data) {
  if (data == 0xffff)
    return Signaling::SIGNALING_NONE;

  unsigned tag = data >> 14;
  unsigned a = (data >> 12) & 3;
  unsigned b = (data >> 8) & 15;
  unsigned c = (data >> 4) & 15;
  unsigned d = data & 15;

  switch (tag) {
  case 2:
    // DCS Normal
    return Signaling::fromDCSNumber(100*b+10*c+1*d, false);
  case 3:
    // DCS Inverted
    return Signaling::fromDCSNumber(100*b+10*c+1*d, true);
  default:
    break;
  }

  // CTCSS
  return Signaling::fromCTCSSFrequency(100.0*a+10.0*b+1.0*c+0.1*d);
}


uint16_t
encode_ctcss_tone_table(Signaling::Code code)
{
  unsigned tag=0xff, a=0xf, b=0xf, c=0xf, d=0xf;

  // Disabled
  if (Signaling::SIGNALING_NONE == code)
    return 0xffff;

  if (Signaling::isCTCSS(code)) {
    // CTCSS tone
    tag = 0;
    unsigned val = Signaling::toCTCSSFrequency(code) * 10.0 + 0.5;
    a = val / 1000;
    b = (val / 100) % 10;
    c = (val / 10) % 10;
    d = val % 10;
  } else if (Signaling::isDCSNormal(code)) {
    // DCS normal
    tag = 2;
    unsigned val = Signaling::toDCSNumber(code);
    a = 0;
    b = (val / 100) % 10;
    c = (val / 10) % 10;
    d = val % 10;
  } else if (Signaling::isDCSInverted(code)) {
    // DCS inverted
    tag = 3;
    unsigned val = Signaling::toDCSNumber(code);
    a = 0;
    b = (val / 100) % 10;
    c = (val / 10) % 10;
    d = val % 10;
  }

  return (a << 12) | (b << 8) | (c << 4) | d | (tag << 14);
}


uint16_t oct_to_dec(uint16_t oct) {
  uint16_t a = oct % 10; oct /= 10;
  uint16_t b = oct % 10; oct /= 10;
  uint16_t c = oct % 10; oct /= 10;
  uint16_t d = oct % 10; oct /= 10;
  if ((a>7) || (b>7) || (c>6) || (d>7) || (oct>0))
    return 0;
  return (((d*8+c)*8 + b)*8 + a);
}

uint16_t dec_to_oct(uint16_t dec) {
  uint16_t a = dec % 8; dec /= 8;
  uint16_t b = dec % 8; dec /= 8;
  uint16_t c = dec % 8; dec /= 8;
  uint16_t d = dec % 8; dec /= 8;
  if (dec>0)
    return 0;
  return (((d*10+c)*10 + b)*10 + a);
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

QString
aprsicon2config(APRSSystem::Icon icon) {
  if ((APRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(unsigned(icon))))
    return "-";
  return QString("\"%1\"").arg(aprsIconNameTable.value((unsigned)icon));
}

QString
aprsicon2name(APRSSystem::Icon icon) {
  if ((APRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(unsigned(icon))))
    return "";
  return aprsIconNameTable.value((unsigned)icon);
}

APRSSystem::Icon
name2aprsicon(const QString &name) {
  if (name.isEmpty())
    return APRSSystem::Icon::None;

  APRSSystem::Icon icon = APRSSystem::Icon::None;
  int best = levDist(name, "");

  QHash<unsigned, QString>::const_iterator item=aprsIconNameTable.constBegin();
  for(; item != aprsIconNameTable.constEnd(); item++) {
    int dist = levDist(name, item.value());
    if (dist < best) {
      icon = (APRSSystem::Icon)item.key();
      best = dist;
    }
  }

  return icon;
}

char
aprsicon2iconcode(APRSSystem::Icon icon) {
 unsigned num = unsigned(APRSSystem::ICON_MASK & unsigned(icon));
 if (num >= unsigned(aprsIconCodeTable.size()))
   return '"';
 return aprsIconCodeTable[num];
}

char
aprsicon2tablecode(APRSSystem::Icon icon) {
  unsigned tab = (APRSSystem::TABLE_MASK & unsigned(icon));
  switch (tab) {
  case APRSSystem::SECONDARY_TABLE: return '\\';
  case APRSSystem::PRIMARY_TABLE: return '/';
  }
  return '/';
}

APRSSystem::Icon
code2aprsicon(char table, char icon) {
  unsigned num = (APRSSystem::ICON_MASK & unsigned(APRSSystem::Icon::None));
  if (aprsIconCodeTable.contains(icon))
    num = aprsIconCodeTable.indexOf(icon);
  if ('/' == table)
    num = num | APRSSystem::PRIMARY_TABLE;
  else if ('\\' == table)
    num = num | APRSSystem::PRIMARY_TABLE;
  return APRSSystem::Icon(num);
}

int
levDist(const QString &source, const QString &target, Qt::CaseSensitivity cs) {
  // Mostly stolen from https://qgis.org/api/2.14/qgsstringutils_8cpp_source.html

  if (0 == QString::compare(source,target, cs)) {
    return 0;
  }

  const int sourceCount = source.count();
  const int targetCount = target.count();

  if (source.isEmpty())
    return targetCount;
  if (target.isEmpty())
    return sourceCount;
  if (sourceCount > targetCount)
    return levDist(target, source, cs);


  QVector<int> column;
  column.fill(0, targetCount + 1);

  QVector<int> previousColumn;
  previousColumn.reserve(targetCount + 1);
  for (int i = 0; i < targetCount + 1; i++)
    previousColumn.append(i);

  for (int i = 0; i < sourceCount; i++) {
    column[0] = i + 1;
    for (int j = 0; j < targetCount; j++) {
      column[j + 1] = std::min(
      {
              1 + column.at(j),
              1 + previousColumn.at(1 + j),
              previousColumn.at(j) + (QString::compare(source.at(i),target.at(j), cs) ? 1 : 0)
            });
    }
    column.swap(previousColumn);
  }

  return previousColumn.at(targetCount);
}

uint32_t
align_size(uint32_t size, uint32_t block) {
  if (0 == (size % block))
    return size;
  return (size + (block - (size%block)));
}

uint32_t
align_addr(uint32_t addr, uint32_t block) {
  if (0 == (addr % block))
    return addr;
  return (addr - (addr%block));
}
