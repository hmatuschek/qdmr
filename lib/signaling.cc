#include "signaling.hh"

#include <QHash>
#include <QVector>
#include <QObject>

using namespace Signaling;

/** Internal used translation table between CTCSS frequencies and AnalogChannel::Signaling codes. */
static QHash<float, Code> CTCSS_freq2code {
  {  0.0, SIGNALING_NONE},
  { 67.0,  CTCSS_67_0Hz}, { 71.0,  CTCSS_71_0Hz}, { 74.4,  CTCSS_74_4Hz}, { 77.0,  CTCSS_77_0Hz},
  { 79.9,  CTCSS_79_9Hz}, { 82.5,  CTCSS_82_5Hz}, { 85.4,  CTCSS_85_4Hz}, { 88.5,  CTCSS_88_5Hz},
  { 91.5,  CTCSS_91_5Hz}, { 94.8,  CTCSS_94_8Hz}, { 97.4,  CTCSS_97_4Hz}, {100.0, CTCSS_100_0Hz},
  {103.5, CTCSS_103_5Hz}, {107.2, CTCSS_107_2Hz}, {110.9, CTCSS_110_9Hz}, {114.8, CTCSS_114_8Hz},
  {118.8, CTCSS_118_8Hz}, {123.0, CTCSS_123_0Hz}, {127.3, CTCSS_127_3Hz}, {131.8, CTCSS_131_8Hz},
  {136.5, CTCSS_136_5Hz}, {141.3, CTCSS_141_3Hz}, {146.2, CTCSS_146_2Hz}, {151.4, CTCSS_151_4Hz},
  {156.7, CTCSS_156_7Hz}, {162.2, CTCSS_162_2Hz}, {167.9, CTCSS_167_9Hz}, {173.8, CTCSS_173_8Hz},
  {179.9, CTCSS_179_9Hz}, {186.2, CTCSS_186_2Hz}, {192.8, CTCSS_192_8Hz}, {203.5, CTCSS_203_5Hz},
  {210.7, CTCSS_210_7Hz}, {218.1, CTCSS_218_1Hz}, {225.7, CTCSS_225_7Hz}, {233.6, CTCSS_233_6Hz},
  {241.8, CTCSS_241_8Hz}, {250.3, CTCSS_250_3Hz}
};

/** Internal used translation table between CTCSS frequencies and AnalogChannel::Signaling codes. */
static QHash<Code, float> CTCSS_code2freq {
  {SIGNALING_NONE,   0.0},
  {CTCSS_67_0Hz,   67.0}, {CTCSS_71_0Hz,   71.0}, {CTCSS_74_4Hz,   74.4}, {CTCSS_77_0Hz,   77.0},
  {CTCSS_79_9Hz,   79.9}, {CTCSS_82_5Hz,   82.5}, {CTCSS_85_4Hz,   85.4}, {CTCSS_88_5Hz,   88.5},
  {CTCSS_91_5Hz,   91.5}, {CTCSS_94_8Hz,   94.8}, {CTCSS_97_4Hz,   97.4}, {CTCSS_100_0Hz, 100.0},
  {CTCSS_103_5Hz, 103.5}, {CTCSS_107_2Hz, 107.2}, {CTCSS_110_9Hz, 110.9}, {CTCSS_114_8Hz, 114.8},
  {CTCSS_118_8Hz, 118.8}, {CTCSS_123_0Hz, 123.0}, {CTCSS_127_3Hz, 127.3}, {CTCSS_131_8Hz, 131.8},
  {CTCSS_136_5Hz, 136.5}, {CTCSS_141_3Hz, 141.3}, {CTCSS_146_2Hz, 146.2}, {CTCSS_151_4Hz, 151.4},
  {CTCSS_156_7Hz, 156.7}, {CTCSS_162_2Hz, 162.2}, {CTCSS_167_9Hz, 167.9}, {CTCSS_173_8Hz, 173.8},
  {CTCSS_179_9Hz, 179.9}, {CTCSS_186_2Hz, 186.2}, {CTCSS_192_8Hz, 192.8}, {CTCSS_203_5Hz, 203.5},
  {CTCSS_210_7Hz, 210.7}, {CTCSS_218_1Hz, 218.1}, {CTCSS_225_7Hz, 225.7}, {CTCSS_233_6Hz, 233.6},
  {CTCSS_241_8Hz, 241.8}, {CTCSS_250_3Hz, 250.3}
};


float
Signaling::toCTCSSFrequency(Code code) {
  if (! CTCSS_code2freq.contains(code))
    return SIGNALING_NONE;
  return CTCSS_code2freq[code];
}

bool
Signaling::isCTCSSFrequency(float freq) {
  if (0.0 == freq)
    return false;
  return CTCSS_freq2code.contains(freq);
}

Signaling::Code
Signaling::fromCTCSSFrequency(float f) {
  if ((0 == f) || (! CTCSS_freq2code.contains(f)))
    return SIGNALING_NONE;
  return CTCSS_freq2code[f];
}

QString
Signaling::codeLabel(Code code) {
  if (SIGNALING_NONE == code)
    return QObject::tr("[None]");
  else if ((CTCSS_67_0Hz <= code) && (CTCSS_250_3Hz >= code))   // If CTCSS signaling
    return QObject::tr("CTCSS %1Hz").arg(toCTCSSFrequency(code));
  else if ((DCS_023N <= code) && (DCS_754N >= code))
    return QObject::tr("DCS %1 N").arg("xxx");
  else
    return QObject::tr("DCS %1 I").arg("xxx");
}
