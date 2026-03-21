#include "utils.hh"
#include <QRegularExpression>
#include <QVector>
#include <QHash>
#include <cmath>
#include <QRegularExpression>
#include <yaml-cpp/yaml.h>

// Maps APRS icon number to code-char
static QVector<char> aprsIconCodeTable{
  '!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0',
  '1','2','3','4','5','6', '7','8','9',':',';','<','=','>','?','@',
  'A','B','C','D','E','F', 'G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V', 'W','X','Y','Z','[','/',']','^','_','`',
  'a','b','c','d','e','f', 'g','h','i','j','k','l','m','n','o','p',
  'q','r','s','t','u','v','w','x','y','z','{','|','}','~'};

static QHash<unsigned, QString> aprsIconNameTable{
  {(unsigned)FMAPRSSystem::Icon::None, ""},
  {(unsigned)FMAPRSSystem::Icon::PoliceStation, "Police station"},
  {(unsigned)FMAPRSSystem::Icon::Digipeater, "Digipeater"},
  {(unsigned)FMAPRSSystem::Icon::Phone, "Phone"},
  {(unsigned)FMAPRSSystem::Icon::DXCluster, "DX cluster"},
  {(unsigned)FMAPRSSystem::Icon::HFGateway, "HF gateway"},
  {(unsigned)FMAPRSSystem::Icon::SmallPlane, "Small plane"},
  {(unsigned)FMAPRSSystem::Icon::MobileSatelliteStation, "Mobile Satellite station"},
  {(unsigned)FMAPRSSystem::Icon::WheelChair, "Wheel chair"},
  {(unsigned)FMAPRSSystem::Icon::Snowmobile, "Snowmobile"},
  {(unsigned)FMAPRSSystem::Icon::RedCross, "Red cross"},
  {(unsigned)FMAPRSSystem::Icon::BoyScout, "Boy scout"},
  {(unsigned)FMAPRSSystem::Icon::Home,  "Home"},
  {(unsigned)FMAPRSSystem::Icon::X, "X"},
  {(unsigned)FMAPRSSystem::Icon::RedDot, "Red dot"},
  {(unsigned)FMAPRSSystem::Icon::Circle0, "Circle 0"},
  {(unsigned)FMAPRSSystem::Icon::Circle1, "Circle 1"},
  {(unsigned)FMAPRSSystem::Icon::Circle2, "Circle 2"},
  {(unsigned)FMAPRSSystem::Icon::Circle3, "Circle 3"},
  {(unsigned)FMAPRSSystem::Icon::Circle4, "Circle 4"},
  {(unsigned)FMAPRSSystem::Icon::Circle5, "Circle 5"},
  {(unsigned)FMAPRSSystem::Icon::Circle6, "Circle 6"},
  {(unsigned)FMAPRSSystem::Icon::Circle7, "Circle 7"},
  {(unsigned)FMAPRSSystem::Icon::Circle8, "Circle 8"},
  {(unsigned)FMAPRSSystem::Icon::Circle9, "Circle 9"},
  {(unsigned)FMAPRSSystem::Icon::Fire, "Fire"},
  {(unsigned)FMAPRSSystem::Icon::Campground, "Campground"},
  {(unsigned)FMAPRSSystem::Icon::Motorcycle, "Motorcycle"},
  {(unsigned)FMAPRSSystem::Icon::RailEngine, "Rail engine"},
  {(unsigned)FMAPRSSystem::Icon::Car, "Car"},
  {(unsigned)FMAPRSSystem::Icon::FileServer, "File server"},
  {(unsigned)FMAPRSSystem::Icon::HCFuture, "HC future"},
  {(unsigned)FMAPRSSystem::Icon::AidStation, "Aid station"},
  {(unsigned)FMAPRSSystem::Icon::BBS, "BBS"},
  {(unsigned)FMAPRSSystem::Icon::Canoe, "Canoe"},
  {(unsigned)FMAPRSSystem::Icon::Eyeball, "Eyeball"},
  {(unsigned)FMAPRSSystem::Icon::Tractor, "Tractor"},
  {(unsigned)FMAPRSSystem::Icon::GridSquare, "Grid square"},
  {(unsigned)FMAPRSSystem::Icon::Hotel, "Hotel"},
  {(unsigned)FMAPRSSystem::Icon::TCPIP, "TCP/IP"},
  {(unsigned)FMAPRSSystem::Icon::School, "School"},
  {(unsigned)FMAPRSSystem::Icon::Logon, "Logon"},
  {(unsigned)FMAPRSSystem::Icon::MacOS, "MacOS"},
  {(unsigned)FMAPRSSystem::Icon::NTSStation, "NTS station"},
  {(unsigned)FMAPRSSystem::Icon::Balloon, "Balloon"},
  {(unsigned)FMAPRSSystem::Icon::PoliceCar, "Police car"},
  {(unsigned)FMAPRSSystem::Icon::TBD, "TBD"},
  {(unsigned)FMAPRSSystem::Icon::RV, "RV"},
  {(unsigned)FMAPRSSystem::Icon::Shuttle, "Shuttle"},
  {(unsigned)FMAPRSSystem::Icon::SSTV, "SSTV"},
  {(unsigned)FMAPRSSystem::Icon::Bus, "Bus"},
  {(unsigned)FMAPRSSystem::Icon::ATV, "ATV"},
  {(unsigned)FMAPRSSystem::Icon::WXService, "WX service"},
  {(unsigned)FMAPRSSystem::Icon::Helo, "Helo"},
  {(unsigned)FMAPRSSystem::Icon::Yacht, "Yacht"},
  {(unsigned)FMAPRSSystem::Icon::Windows, "Windows"},
  {(unsigned)FMAPRSSystem::Icon::Jogger, "Jogger"},
  {(unsigned)FMAPRSSystem::Icon::Triangle, "Triangle"},
  {(unsigned)FMAPRSSystem::Icon::PBBS, "PBBS"},
  {(unsigned)FMAPRSSystem::Icon::LargePlane, "Large plane"},
  {(unsigned)FMAPRSSystem::Icon::WXStation, "WX station"},
  {(unsigned)FMAPRSSystem::Icon::DishAntenna, "Dish antenna"},
  {(unsigned)FMAPRSSystem::Icon::Ambulance, "Ambulance"},
  {(unsigned)FMAPRSSystem::Icon::Bike, "Bike"},
  {(unsigned)FMAPRSSystem::Icon::ICP, "ICP"},
  {(unsigned)FMAPRSSystem::Icon::FireStation, "Fire station"},
  {(unsigned)FMAPRSSystem::Icon::Horse, "Horse"},
  {(unsigned)FMAPRSSystem::Icon::FireTruck, "Fire truck"},
  {(unsigned)FMAPRSSystem::Icon::Glider, "Glider"},
  {(unsigned)FMAPRSSystem::Icon::Hospital, "Hospital"},
  {(unsigned)FMAPRSSystem::Icon::IOTA, "IOTA"},
  {(unsigned)FMAPRSSystem::Icon::Jeep, "Jeep"},
  {(unsigned)FMAPRSSystem::Icon::SmallTruck, "Small truck"},
  {(unsigned)FMAPRSSystem::Icon::Laptop, "Laptop"},
  {(unsigned)FMAPRSSystem::Icon::MicE, "Mic-E"},
  {(unsigned)FMAPRSSystem::Icon::Node, "Node"},
  {(unsigned)FMAPRSSystem::Icon::EOC, "EOC"},
  {(unsigned)FMAPRSSystem::Icon::Rover, "Rover"},
  {(unsigned)FMAPRSSystem::Icon::Grid, "Grid"},
  {(unsigned)FMAPRSSystem::Icon::Antenna, "Antenna"},
  {(unsigned)FMAPRSSystem::Icon::PowerBoat, "Power boat"},
  {(unsigned)FMAPRSSystem::Icon::TruckStop, "Truck stop"},
  {(unsigned)FMAPRSSystem::Icon::TruckLarge, "Truck large"},
  {(unsigned)FMAPRSSystem::Icon::Van, "Van"},
  {(unsigned)FMAPRSSystem::Icon::Water, "Water"},
  {(unsigned)FMAPRSSystem::Icon::XAPRS, "XAPRS"},
  {(unsigned)FMAPRSSystem::Icon::Yagi, "Yagi"},
  {(unsigned)FMAPRSSystem::Icon::Shelter, "Shelter"}};

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
  Q_UNUSED(fill)
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

SelectiveCall
decode_ctcss_tone_table(uint16_t data) {
  if (data == 0xffff)
    return SelectiveCall();

  unsigned tag = data >> 14;
  unsigned a = (data >> 12) & 3;
  unsigned b = (data >> 8) & 15;
  unsigned c = (data >> 4) & 15;
  unsigned d = data & 15;

  switch (tag) {
  case 2:
    // DCS Normal
    return SelectiveCall(100*b+10*c+1*d, false);
  case 3:
    // DCS Inverted
    return SelectiveCall(100*b+10*c+1*d, true);
  default:
    break;
  }

  // CTCSS
  return SelectiveCall(100.0*a+10.0*b+1.0*c+0.1*d);
}


uint16_t
encode_ctcss_tone_table(const SelectiveCall &code)
{
  unsigned tag=0xff, a=0xf, b=0xf, c=0xf, d=0xf;

  // Disabled
  if (code.isInvalid())
    return 0xffff;

  if (code.isCTCSS()) {
    // CTCSS tone
    tag = 0;
    unsigned val = code.Hz() * 10.0 + 0.5;
    a = val / 1000;
    b = (val / 100) % 10;
    c = (val / 10) % 10;
    d = val % 10;
  } else if (code.isDCS()) {
    // DCS normal
    if (code.isInverted())
      tag = 3;
    else
      tag = 2;
    unsigned val = code.octalCode();
    a = 0;
    b = (val / 100) % 10;
    c = (val / 10) % 10;
    d = val % 10;
  }

  return (a << 12) | (b << 8) | (c << 4) | d | (tag << 14);
}


bool
validDMRNumber(const QString &text) {
  return QRegularExpression("^[0-9]+$").match(text).hasMatch();
}

bool
validDTMFNumber(const QString &text) {
  return QRegularExpression("^[0-9a-dA-D\\*#]+$").match(text).hasMatch();
}

QString
aprsicon2config(FMAPRSSystem::Icon icon) {
  if ((FMAPRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(unsigned(icon))))
    return "-";
  return QString("\"%1\"").arg(aprsIconNameTable.value((unsigned)icon));
}

QString
aprsicon2name(FMAPRSSystem::Icon icon) {
  if ((FMAPRSSystem::Icon::None == icon) || (! aprsIconCodeTable.contains(unsigned(icon))))
    return "";
  return aprsIconNameTable.value((unsigned)icon);
}

FMAPRSSystem::Icon
name2aprsicon(const QString &name) {
  if (name.isEmpty())
    return FMAPRSSystem::Icon::None;

  FMAPRSSystem::Icon icon = FMAPRSSystem::Icon::None;
  int best = levDist(name, "");

  QHash<unsigned, QString>::const_iterator item=aprsIconNameTable.constBegin();
  for(; item != aprsIconNameTable.constEnd(); item++) {
    int dist = levDist(name, item.value());
    if (dist < best) {
      icon = (FMAPRSSystem::Icon)item.key();
      best = dist;
    }
  }

  return icon;
}

char
aprsicon2iconcode(FMAPRSSystem::Icon icon) {
 unsigned num = unsigned(FMAPRSSystem::ICON_MASK & unsigned(icon));
 if (num >= unsigned(aprsIconCodeTable.size()))
   return '"';
 return aprsIconCodeTable[num];
}

char
aprsicon2tablecode(FMAPRSSystem::Icon icon) {
  unsigned tab = (FMAPRSSystem::TABLE_MASK & unsigned(icon));
  switch (tab) {
  case FMAPRSSystem::SECONDARY_TABLE: return '\\';
  case FMAPRSSystem::PRIMARY_TABLE: return '/';
  }
  return '/';
}

FMAPRSSystem::Icon
code2aprsicon(char table, char icon) {
  unsigned num = (FMAPRSSystem::ICON_MASK & unsigned(FMAPRSSystem::Icon::None));
  if (aprsIconCodeTable.contains(icon))
    num = aprsIconCodeTable.indexOf(icon);
  if ('/' == table)
    num = num | FMAPRSSystem::PRIMARY_TABLE;
  else if ('\\' == table)
    num = num | FMAPRSSystem::PRIMARY_TABLE;
  return FMAPRSSystem::Icon(num);
}

int
levDist(const QString &source, const QString &target, Qt::CaseSensitivity cs) {
  // Mostly stolen from https://qgis.org/api/2.14/qgsstringutils_8cpp_source.html

  if (0 == QString::compare(source,target, cs)) {
    return 0;
  }

  const int sourceCount = source.size();
  const int targetCount = target.size();

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


QGeoCoordinate
loc2deg(const QString &loc) {
  double lon = 0, lat = 0, dlon = 20, dlat = 10;
  if (2 > loc.size())
    return QGeoCoordinate();
  QChar l = loc[0].toUpper();
  QChar c = loc[1].toUpper();
  lon += double(int(l.toLatin1())-'A')*dlon;
  lat += double(int(c.toLatin1())-'A')*dlat;

  if (4 > loc.size()) {
    lon = lon - 180;
    lat = lat - 90;
    // Offset places coordinate in the middle of the square
    return QGeoCoordinate(lat+dlat/2, lon+dlon/2);
  }

  dlon /= 10; dlat /= 10;
  l = loc[2].toUpper();
  c = loc[3].toUpper();
  lon += double(int(l.toLatin1())-'0')*dlon;
  lat += double(int(c.toLatin1())-'0')*dlat;

  if (6 > loc.size()){
    lon = lon - 180;
    lat = lat - 90;
    // Offset places coordinate in the middle of the square
    return QGeoCoordinate(lat+dlat/2, lon+dlon/2);
  }

  dlon /= 24; dlat /= 24;
  l = loc[4].toUpper();
  c = loc[5].toUpper();
  lon += double(int(l.toLatin1())-'A')*dlon;
  lat += double(int(c.toLatin1())-'A')*dlat;

  if (8 > loc.size()) {
    lon = lon - 180;
    lat = lat - 90;
    // Offset places coordinate in the middle of the square
    return QGeoCoordinate(lat+dlat/2, lon+dlon/2);
  }

  dlon /= 10; dlat /= 10;
  l = loc[6].toUpper();
  c = loc[7].toUpper();
  lon += double(int(l.toLatin1())-'0')*dlon;
  lat += double(int(c.toLatin1())-'0')*dlat;

  if (10 > loc.size()) {
    lon = lon - 180;
    lat = lat - 90;
    // Offset places coordinate in the middle of the square
    return QGeoCoordinate(lat+dlat/2, lon+dlon/2);
  }

  dlon /= 24; dlat /= 24;
  l = loc[8].toUpper();
  c = loc[9].toUpper();
  lon += double(int(l.toLatin1())-'A')*dlon;
  lat += double(int(c.toLatin1())-'A')*dlat;

  lon = lon - 180;
  lat = lat - 90;
  // Offset places coordinate in the middle of the square
  return QGeoCoordinate(lat+dlat/2, lon+dlon/2);
}


QString
deg2loc(const QGeoCoordinate &coor, unsigned int size) {
  if (!coor.isValid())
    return QString();

  QString loc;
  double lon = (coor.longitude()+180)/360;
  double lat = (coor.latitude()+90)/180;

  size += (size % 2);
  if (2 > size)
    return loc;

  lon *= 18; lat *= 18;
  char l = lon; lon -= l;
  char c = lat; lat -= c;
  loc.append(QChar::fromLatin1(l+'A')); loc.append(QChar::fromLatin1(c+'A'));

  if (4 > size)
    return loc;

  lon *= 10; lat *= 10;
  l = lon; lon -= l;
  c = lat; lat -= c;
  loc.append(QChar::fromLatin1(l+'0')); loc.append(QChar::fromLatin1(c+'0'));

  if (6 > size)
    return loc;

  lon *= 24; lat *= 24;
  l = lon; lon -= l;
  c = lat; lat -= c;
  loc.append(QChar::fromLatin1(l+'a')); loc.append(QChar::fromLatin1(c+'a'));

  if (8 > size)
    return loc;

  lon *= 10; lat *= 10;
  l = lon; lon -= l;
  c = lat; lat -= c;
  loc.append(QChar::fromLatin1(l+'0')); loc.append(QChar::fromLatin1(c+'0'));

  if (10 > size)
    return loc;

  lon *= 24; lat *= 24;
  l = lon; //lon -= l;
  c = lat; //lat -= c;
  loc.append(QChar::fromLatin1(l+'a')); loc.append(QChar::fromLatin1(c+'a'));

  return loc;
}


