#include "utils.hh"
#include <QRegExp>
#include <QVector>
#include <QHash>
#include <cmath>

// Maps APRS icon number to code-char
static QVector<char> aprsIconCodeTable{
  '!','"','#','$','%','&','\'','(',')','*',
  '+',',','-','.','/','0','1','2','3','4',
  '5','6','7','8','9',':',';','<','=','>',
  '?','@','A','B','C','D','E','F','G','H',
  'H','I','J','K','L','M','N','O','P','Q',
  'R','S','T','U','V','W','X','Y','Z','[',
  '/',']','^','_','`','a','b','c','d','e',
  'f','g','h','i','j','k','l','m','n','o',
  'p','q','r','s','t','u','v','w','x','y',
  'z','{','|','}','~'};

static QHash<APRSSystem::Icon, QString> aprsIconNameTable{
  {APRSSystem::APRS_ICON_NO_SYMBOL, ""},
  {APRSSystem::APRS_ICON_POLICE_STN, "Police station"},
  {APRSSystem::APRS_ICON_DIGI, "Digipeater"},
  {APRSSystem::APRS_ICON_PHONE, "Phone"},
  {APRSSystem::APRS_ICON_DX_CLUSTER, "DX cluster"},
  {APRSSystem::APRS_ICON_HF_GATEWAY, "HF gateway"},
  {APRSSystem::APRS_ICON_PLANE_SMALL, "Small plane"},
  {APRSSystem::APRS_ICON_MOB_SAT_STN, "Mobile Satellite station"},
  {APRSSystem::APRS_ICON_WHEEL_CHAIR, "Wheel chair"},
  {APRSSystem::APRS_ICON_SNOWMOBILE, "Snowmobile"},
  {APRSSystem::APRS_ICON_RED_CROSS, "Red cross"},
  {APRSSystem::APRS_ICON_BOY_SCOUT, "Boy scout"},
  {APRSSystem::APRS_ICON_HOME,  QObject::tr("Home")},
  {APRSSystem::APRS_ICON_X, "X"},
  {APRSSystem::APRS_ICON_RED_DOT, "Red dot"},
  {APRSSystem::APRS_ICON_CIRCLE_0, "Circle 0"},
  {APRSSystem::APRS_ICON_CIRCLE_1, "Circle 1"},
  {APRSSystem::APRS_ICON_CIRCLE_2, "Circle 2"},
  {APRSSystem::APRS_ICON_CIRCLE_3, "Circle 3"},
  {APRSSystem::APRS_ICON_CIRCLE_4, "Circle 4"},
  {APRSSystem::APRS_ICON_CIRCLE_5, "Circle 5"},
  {APRSSystem::APRS_ICON_CIRCLE_6, "Circle 6"},
  {APRSSystem::APRS_ICON_CIRCLE_7, "Circle 7"},
  {APRSSystem::APRS_ICON_CIRCLE_8, "Circle 8"},
  {APRSSystem::APRS_ICON_CIRCLE_9, "Circle 9"},
  {APRSSystem::APRS_ICON_FIRE, "Fire"},
  {APRSSystem::APRS_ICON_CAMPGROUND, "Campground"},
  {APRSSystem::APRS_ICON_MOTORCYCLE, "Motorcycle"},
  {APRSSystem::APRS_ICON_RAIL_ENGINE, "Rail engine"},
  {APRSSystem::APRS_ICON_CAR, "Car"},
  {APRSSystem::APRS_ICON_FILE_SERVER, "File server"},
  {APRSSystem::APRS_ICON_HC_FUTURE, "HC future"},
  {APRSSystem::APRS_ICON_AID_STN, "Aid station"},
  {APRSSystem::APRS_ICON_BBS, "BBS"},
  {APRSSystem::APRS_ICON_CANOE, "Canoe"},
  {APRSSystem::APRS_ICON_EYEBALL, "Eyeball"},
  {APRSSystem::APRS_ICON_TRACTOR, "Tractor"},
  {APRSSystem::APRS_ICON_GRID_SQ, "Grid square"},
  {APRSSystem::APRS_ICON_HOTEL, "Hotel"},
  {APRSSystem::APRS_ICON_TCP_IP, "TCP/IP"},
  {APRSSystem::APRS_ICON_SCHOOL, "School"},
  {APRSSystem::APRS_ICON_USER_LOGON, "Logon"},
  {APRSSystem::APRS_ICON_MAC, "MacOS"},
  {APRSSystem::APRS_ICON_NTS_STN, "NTS station"},
  {APRSSystem::APRS_ICON_BALLOON, "Balloon"},
  {APRSSystem::APRS_ICON_POLICE, "Police car"},
  {APRSSystem::APRS_ICON_TBD, "TBD"},
  {APRSSystem::APRX_ICON_RV, "RV"},
  {APRSSystem::APRS_ICON_SHUTTLE, "Shuttle"},
  {APRSSystem::APRS_ICON_SSTV, "SSTV"},
  {APRSSystem::APRS_ICON_BUS, "Bus"},
  {APRSSystem::APRS_ICON_ATV, "ATV"},
  {APRSSystem::APRS_ICON_WX_SERVICE, "WX service"},
  {APRSSystem::APRS_ICON_HELO, "Helo"},
  {APRSSystem::APRS_ICON_YACHT, "Yacht"},
  {APRSSystem::APRS_ICON_WIN, "Windows"},
  {APRSSystem::APRS_ICON_JOGGER, "Jogger"},
  {APRSSystem::APRS_ICON_TRIANGLE, "Triangle"},
  {APRSSystem::APRS_ICON_PBBS, "PBBS"},
  {APRSSystem::APRS_ICON_PLANE_LARGE, "Large plane"},
  {APRSSystem::APRS_ICON_WX_STN, "WX station"},
  {APRSSystem::APRS_ICON_DISH_ANT, "Dish antenna"},
  {APRSSystem::APRS_ICON_AMBULANCE, "Ambulance"},
  {APRSSystem::APRS_ICON_BIKE, "Bike"},
  {APRSSystem::APRS_ICON_ICP, "ICP"},
  {APRSSystem::APRS_ICON_FIRE_STATION, "Fire station"},
  {APRSSystem::APRS_ICON_HORSE, "Horse"},
  {APRSSystem::APRS_ICON_FIRE_TRUCK, "Fire truck"},
  {APRSSystem::APRS_ICON_GLIDER, "Glider"},
  {APRSSystem::APRS_ICON_HOSPITAL, "Hospital"},
  {APRSSystem::APRS_ICON_IOTA, "IOTA"},
  {APRSSystem::APRS_ICON_JEEP, "Jeep"},
  {APRSSystem::APRS_ICON_TRUCK_SMALL, "Small truck"},
  {APRSSystem::APRS_ICON_LAPTOP, "Laptop"},
  {APRSSystem::APRS_ICON_MIC_E, "Mic-E"},
  {APRSSystem::APRS_ICON_NODE, "Node"},
  {APRSSystem::APRS_ICON_EOC, "EOC"},
  {APRSSystem::APRS_ICON_ROVER, "Rover"},
  {APRSSystem::APRS_ICON_GRID, "Grid"},
  {APRSSystem::APRS_ICON_ANTENNA, "Antenna"},
  {APRSSystem::APRS_ICON_POWER_BOAT, "Power boat"},
  {APRSSystem::APRS_ICON_TRUCK_STOP, "Truck stop"},
  {APRSSystem::APRS_ICON_TUCK_LARGE, "Truck large"},
  {APRSSystem::APRS_ICON_VAN, "Van"},
  {APRSSystem::APRS_ICON_WATER, "Water"},
  {APRSSystem::APRS_ICON_XAPRS, "XAPRS"},
  {APRSSystem::APRS_ICON_YAGI, "Yagi"},
  {APRSSystem::APRS_ICON_SHELTER, "Shelter"}};

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
aprsicon2name(APRSSystem::Icon icon) {
  if (APRSSystem::APRS_ICON_NO_SYMBOL == icon)
    return "-";
  return aprsIconNameTable.value(icon, "-");
}

APRSSystem::Icon
name2aprsicon(const QString &name) {
  if (name.isEmpty())
    return APRSSystem::APRS_ICON_NO_SYMBOL;

  APRSSystem::Icon icon = APRSSystem::APRS_ICON_NO_SYMBOL;
  int best = levDist(name, "");

  QHash<APRSSystem::Icon, QString>::const_iterator item=aprsIconNameTable.constBegin();
  for(; item != aprsIconNameTable.constEnd(); item++) {
    int dist = levDist(name, item.value());
    if (dist < best) {
      icon = item.key();
      best = dist;
    }
  }

  return icon;
}

char
aprsicon2iconcode(APRSSystem::Icon icon) {
 uint num = uint(APRSSystem::ICON_MASK & icon);
 if (num >= uint(aprsIconCodeTable.size()))
   return '"';
 return aprsIconCodeTable[num];
}

char
aprsicon2tablecode(APRSSystem::Icon icon) {
  uint tab = (APRSSystem::TABLE_MASK & icon);
  switch (tab) {
  case APRSSystem::SECONDARY_TABLE: return '\\';
  case APRSSystem::PRIMARY_TABLE: return '/';
  }
  return '/';
}

APRSSystem::Icon
code2aprsicon(char table, char icon) {
  uint num = (APRSSystem::ICON_MASK & APRSSystem::APRS_ICON_NO_SYMBOL);
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
