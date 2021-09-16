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

static QHash<uint, QString> aprsIconNameTable{
  {(uint)APRSSystem::Icon::None, ""},
  {(uint)APRSSystem::Icon::PoliceStation, "Police station"},
  {(uint)APRSSystem::Icon::Digipeater, "Digipeater"},
  {(uint)APRSSystem::Icon::Phone, "Phone"},
  {(uint)APRSSystem::Icon::DXCluster, "DX cluster"},
  {(uint)APRSSystem::Icon::HFGateway, "HF gateway"},
  {(uint)APRSSystem::Icon::SmallPlane, "Small plane"},
  {(uint)APRSSystem::Icon::MobileSatelliteStation, "Mobile Satellite station"},
  {(uint)APRSSystem::Icon::WheelChair, "Wheel chair"},
  {(uint)APRSSystem::Icon::Snowmobile, "Snowmobile"},
  {(uint)APRSSystem::Icon::RedCross, "Red cross"},
  {(uint)APRSSystem::Icon::BoyScout, "Boy scout"},
  {(uint)APRSSystem::Icon::Home,  "Home"},
  {(uint)APRSSystem::Icon::X, "X"},
  {(uint)APRSSystem::Icon::RedDot, "Red dot"},
  {(uint)APRSSystem::Icon::Circle0, "Circle 0"},
  {(uint)APRSSystem::Icon::Circle1, "Circle 1"},
  {(uint)APRSSystem::Icon::Circle2, "Circle 2"},
  {(uint)APRSSystem::Icon::Circle3, "Circle 3"},
  {(uint)APRSSystem::Icon::Circle4, "Circle 4"},
  {(uint)APRSSystem::Icon::Circle5, "Circle 5"},
  {(uint)APRSSystem::Icon::Circle6, "Circle 6"},
  {(uint)APRSSystem::Icon::Circle7, "Circle 7"},
  {(uint)APRSSystem::Icon::Circle8, "Circle 8"},
  {(uint)APRSSystem::Icon::Circle9, "Circle 9"},
  {(uint)APRSSystem::Icon::Fire, "Fire"},
  {(uint)APRSSystem::Icon::Campground, "Campground"},
  {(uint)APRSSystem::Icon::Motorcycle, "Motorcycle"},
  {(uint)APRSSystem::Icon::RailEngine, "Rail engine"},
  {(uint)APRSSystem::Icon::Car, "Car"},
  {(uint)APRSSystem::Icon::FileServer, "File server"},
  {(uint)APRSSystem::Icon::HCFuture, "HC future"},
  {(uint)APRSSystem::Icon::AidStation, "Aid station"},
  {(uint)APRSSystem::Icon::BBS, "BBS"},
  {(uint)APRSSystem::Icon::Canoe, "Canoe"},
  {(uint)APRSSystem::Icon::Eyeball, "Eyeball"},
  {(uint)APRSSystem::Icon::Tractor, "Tractor"},
  {(uint)APRSSystem::Icon::GridSquare, "Grid square"},
  {(uint)APRSSystem::Icon::Hotel, "Hotel"},
  {(uint)APRSSystem::Icon::TCPIP, "TCP/IP"},
  {(uint)APRSSystem::Icon::School, "School"},
  {(uint)APRSSystem::Icon::Logon, "Logon"},
  {(uint)APRSSystem::Icon::MacOS, "MacOS"},
  {(uint)APRSSystem::Icon::NTSStation, "NTS station"},
  {(uint)APRSSystem::Icon::Balloon, "Balloon"},
  {(uint)APRSSystem::Icon::PoliceCar, "Police car"},
  {(uint)APRSSystem::Icon::TBD, "TBD"},
  {(uint)APRSSystem::Icon::RV, "RV"},
  {(uint)APRSSystem::Icon::Shuttle, "Shuttle"},
  {(uint)APRSSystem::Icon::SSTV, "SSTV"},
  {(uint)APRSSystem::Icon::Bus, "Bus"},
  {(uint)APRSSystem::Icon::ATV, "ATV"},
  {(uint)APRSSystem::Icon::WXService, "WX service"},
  {(uint)APRSSystem::Icon::Helo, "Helo"},
  {(uint)APRSSystem::Icon::Yacht, "Yacht"},
  {(uint)APRSSystem::Icon::Windows, "Windows"},
  {(uint)APRSSystem::Icon::Jogger, "Jogger"},
  {(uint)APRSSystem::Icon::Triangle, "Triangle"},
  {(uint)APRSSystem::Icon::PBBS, "PBBS"},
  {(uint)APRSSystem::Icon::LargePlane, "Large plane"},
  {(uint)APRSSystem::Icon::WXStation, "WX station"},
  {(uint)APRSSystem::Icon::DishAntenna, "Dish antenna"},
  {(uint)APRSSystem::Icon::Ambulance, "Ambulance"},
  {(uint)APRSSystem::Icon::Bike, "Bike"},
  {(uint)APRSSystem::Icon::ICP, "ICP"},
  {(uint)APRSSystem::Icon::FireStation, "Fire station"},
  {(uint)APRSSystem::Icon::Horse, "Horse"},
  {(uint)APRSSystem::Icon::FireTruck, "Fire truck"},
  {(uint)APRSSystem::Icon::Glider, "Glider"},
  {(uint)APRSSystem::Icon::Hospital, "Hospital"},
  {(uint)APRSSystem::Icon::IOTA, "IOTA"},
  {(uint)APRSSystem::Icon::Jeep, "Jeep"},
  {(uint)APRSSystem::Icon::SmallTruck, "Small truck"},
  {(uint)APRSSystem::Icon::Laptop, "Laptop"},
  {(uint)APRSSystem::Icon::MicE, "Mic-E"},
  {(uint)APRSSystem::Icon::Node, "Node"},
  {(uint)APRSSystem::Icon::EOC, "EOC"},
  {(uint)APRSSystem::Icon::Rover, "Rover"},
  {(uint)APRSSystem::Icon::Grid, "Grid"},
  {(uint)APRSSystem::Icon::Antenna, "Antenna"},
  {(uint)APRSSystem::Icon::PowerBoat, "Power boat"},
  {(uint)APRSSystem::Icon::TruckStop, "Truck stop"},
  {(uint)APRSSystem::Icon::TruckLarge, "Truck large"},
  {(uint)APRSSystem::Icon::Van, "Van"},
  {(uint)APRSSystem::Icon::Water, "Water"},
  {(uint)APRSSystem::Icon::XAPRS, "XAPRS"},
  {(uint)APRSSystem::Icon::Yagi, "Yagi"},
  {(uint)APRSSystem::Icon::Shelter, "Shelter"}};

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
  if ((APRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(uint(icon))))
    return "-";
  return QString("\"%1\"").arg(aprsIconNameTable.value((uint)icon));
}

QString
aprsicon2name(APRSSystem::Icon icon) {
  if ((APRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(uint(icon))))
    return "";
  return aprsIconNameTable.value((uint)icon);
}

APRSSystem::Icon
name2aprsicon(const QString &name) {
  if (name.isEmpty())
    return APRSSystem::Icon::None;

  APRSSystem::Icon icon = APRSSystem::Icon::None;
  int best = levDist(name, "");

  QHash<uint, QString>::const_iterator item=aprsIconNameTable.constBegin();
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
 uint num = uint(APRSSystem::ICON_MASK & uint(icon));
 if (num >= uint(aprsIconCodeTable.size()))
   return '"';
 return aprsIconCodeTable[num];
}

char
aprsicon2tablecode(APRSSystem::Icon icon) {
  uint tab = (APRSSystem::TABLE_MASK & uint(icon));
  switch (tab) {
  case APRSSystem::SECONDARY_TABLE: return '\\';
  case APRSSystem::PRIMARY_TABLE: return '/';
  }
  return '/';
}

APRSSystem::Icon
code2aprsicon(char table, char icon) {
  uint num = (APRSSystem::ICON_MASK & uint(APRSSystem::Icon::None));
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
