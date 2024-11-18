#include "signaling.hh"

#include <QHash>
#include <QVector>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


/* ********************************************************************************************* *
 * Implementation of SelectiveCall
 * ********************************************************************************************* */
QVector<SelectiveCall>
SelectiveCall::_standard = {
  SelectiveCall(),
   67.0,  71.9,  74.4,  77.0,  79.7,  82.5,  85.4,  88.5,  91.5,  94.8,  97.4, 100.0, 103.5, 107.2,
  110.9, 114.8, 118.8, 123.0, 127.3, 131.8, 136.5, 141.3, 146.2, 151.4, 156.7, 162.2, 167.9, 173.8,
  179.9, 186.2, 192.8, 203.5, 210.7, 218.1, 225.7, 233.6, 241.8, 250.3,
  { 23, false}, { 25, false}, { 26, false}, { 31, false}, { 32, false}, { 36, false}, { 43, false},
  { 47, false}, { 51, false}, { 53, false}, { 54, false}, { 71, false}, { 72, false}, { 73, false},
  { 74, false}, {114, false}, {115, false}, {116, false}, {122, false}, {125, false}, {131, false},
  {132, false}, {134, false}, {143, false}, {145, false}, {152, false}, {155, false}, {156, false},
  {162, false}, {165, false}, {172, false}, {174, false}, {205, false}, {212, false}, {223, false},
  {225, false}, {226, false}, {243, false}, {244, false}, {245, false}, {246, false}, {251, false},
  {252, false}, {255, false}, {261, false}, {263, false}, {265, false}, {266, false}, {267, false},
  {271, false}, {274, false}, {306, false}, {311, false}, {315, false}, {325, false}, {331, false},
  {332, false}, {343, false}, {346, false}, {351, false}, {356, false}, {364, false}, {365, false},
  {371, false}, {411, false}, {412, false}, {413, false}, {423, false}, {431, false}, {432, false},
  {445, false}, {446, false}, {452, false}, {454, false}, {455, false}, {462, false}, {464, false},
  {465, false}, {466, false}, {503, false}, {506, false}, {516, false}, {523, false}, {526, false},
  {532, false}, {546, false}, {565, false}, {606, false}, {612, false}, {624, false}, {627, false},
  {631, false}, {632, false}, {654, false}, {662, false}, {664, false}, {703, false}, {712, false},
  {723, false}, {731, false}, {732, false}, {734, false}, {743, false}, {754, false},
  { 23,  true}, { 25,  true}, { 26,  true}, { 31,  true}, { 32,  true}, { 36,  true}, { 43,  true},
  { 47,  true}, { 51,  true}, { 53,  true}, { 54,  true}, { 71,  true}, { 72,  true}, { 73,  true},
  { 74,  true}, {114,  true}, {115,  true}, {116,  true}, {122,  true}, {125,  true}, {131,  true},
  {132,  true}, {134,  true}, {143,  true}, {145,  true}, {152,  true}, {155,  true}, {156,  true},
  {162,  true}, {165,  true}, {172,  true}, {174,  true}, {205,  true}, {212,  true}, {223,  true},
  {225,  true}, {226,  true}, {243,  true}, {244,  true}, {245,  true}, {246,  true}, {251,  true},
  {252,  true}, {255,  true}, {261,  true}, {263,  true}, {265,  true}, {266,  true}, {267,  true},
  {271,  true}, {274,  true}, {306,  true}, {311,  true}, {315,  true}, {325,  true}, {331,  true},
  {332,  true}, {343,  true}, {346,  true}, {351,  true}, {356,  true}, {364,  true}, {365,  true},
  {371,  true}, {411,  true}, {412,  true}, {413,  true}, {423,  true}, {431,  true}, {432,  true},
  {445,  true}, {446,  true}, {452,  true}, {454,  true}, {455,  true}, {462,  true}, {464,  true},
  {465,  true}, {466,  true}, {503,  true}, {506,  true}, {516,  true}, {523,  true}, {526,  true},
  {532,  true}, {546,  true}, {565,  true}, {606,  true}, {612,  true}, {624,  true}, {627,  true},
  {631,  true}, {632,  true}, {654,  true}, {662,  true}, {664,  true}, {703,  true}, {712,  true},
  {723,  true}, {731,  true}, {732,  true}, {734,  true}, {743,  true}, {754,  true}
};

SelectiveCall::SelectiveCall()
  : type(Type::None), dcs{0,false}
{
  // Pass...
}

SelectiveCall::SelectiveCall(double ctcssFreq)
  : type(Type::CTCSS), ctcss(ctcssFreq*10)
{
  // pass...
}

SelectiveCall::SelectiveCall(unsigned int octalDSCCode, bool inverted)
  : type(Type::DCS), dcs{0, inverted}
{
  unsigned int e = 1;
  while (octalDSCCode) {
    dcs.code += std::min(7U, (octalDSCCode % 10)) * e;
    e *= 8; octalDSCCode /= 10;
  }
}

bool
SelectiveCall::operator==(const SelectiveCall &other) const {
  if (type != other.type)
    return false;
  if (Type::CTCSS == type)
    return ctcss == other.ctcss;
  return (dcs.code == other.dcs.code) && (dcs.inverted == other.dcs.inverted);
}

bool
SelectiveCall::operator !=(const SelectiveCall &other) const {
  return !(*this == other);
}

bool
SelectiveCall::isInvalid() const {
  return Type::None == type;
}

bool
SelectiveCall::isValid() const {
  return Type::None != type;
}

bool
SelectiveCall::isCTCSS() const {
  return Type::CTCSS == type;
}

bool
SelectiveCall::isDCS() const {
  return Type::DCS == type;
}

double
SelectiveCall::Hz() const {
  return double(ctcss)/10;
}

unsigned int
SelectiveCall::mHz() const {
  return ((unsigned int)ctcss)*100;
}

unsigned int
SelectiveCall::binCode() const {
  return dcs.code;
}

unsigned int
SelectiveCall::octalCode() const {
  unsigned int o=0, e=1, c=dcs.code;
  while (c) {
    o += (c%8)*e;
    e *= 10; c/= 8;
  }
  return o;
}

bool
SelectiveCall::isInverted() const {
  return dcs.inverted;
}

QString
SelectiveCall::format() const {
  if (! isValid())
    return QString();
  if (isCTCSS())
    return QString("%1 Hz").arg(Hz(), 0, 'f', 1);
  return QString("%1%2")
      .arg(isInverted() ? "i" : "n")
      .arg(binCode(), 3, 8, QChar('0'));
}

SelectiveCall
SelectiveCall::parseCTCSS(const QString &text) {
  QRegularExpression re(R"(([0-9]+(?:\.[0-9]|))\s*(?:Hz|))");
  QRegularExpressionMatch match = re.match(text);
  if (! match.isValid())
    return SelectiveCall();
  return SelectiveCall(match.captured(1).toDouble());
}


SelectiveCall
SelectiveCall::parseDCS(const QString &text) {
  QRegularExpression re(R"(([\-iInN]?)([0-7]{1,3}))");
  QRegularExpressionMatch match = re.match(text);
  if (! match.isValid())
    return SelectiveCall();
  bool inverted = false;
  if (("-" == match.captured(1)) || ("i" == match.captured(1)) || ("I" == match.captured(1)))
    inverted = true;
  return SelectiveCall(match.captured(2).toUInt(), inverted);
}

SelectiveCall
SelectiveCall::fromBinaryDCS(unsigned int code, bool inverted) {
  unsigned int o=0, e=1;
  while (code) {
    o += (code % 8) * e;
    e *= 10; code /= 10;
  }
  return SelectiveCall(o, inverted);
}

const QVector<SelectiveCall> &
SelectiveCall::standard() {
  return _standard;
}

