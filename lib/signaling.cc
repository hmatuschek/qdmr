#include "signaling.hh"

#include <QHash>
#include <QVector>
#include <QObject>

using namespace Signaling;

/** Internal used translation table between CTCSS frequencies and AnalogChannel::Signaling codes. */
static QHash<float, Code> CTCSS_freq2code {
  {  0.0, SIGNALING_NONE},
  { 67.0,  CTCSS_67_0Hz}, { 71.0,  CTCSS_71_9Hz}, { 74.4,  CTCSS_74_4Hz}, { 77.0,  CTCSS_77_0Hz},
  { 79.9,  CTCSS_79_7Hz}, { 82.5,  CTCSS_82_5Hz}, { 85.4,  CTCSS_85_4Hz}, { 88.5,  CTCSS_88_5Hz},
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
  {CTCSS_67_0Hz,   67.0}, {CTCSS_71_9Hz,   71.0}, {CTCSS_74_4Hz,   74.4}, {CTCSS_77_0Hz,   77.0},
  {CTCSS_79_7Hz,   79.9}, {CTCSS_82_5Hz,   82.5}, {CTCSS_85_4Hz,   85.4}, {CTCSS_88_5Hz,   88.5},
  {CTCSS_91_5Hz,   91.5}, {CTCSS_94_8Hz,   94.8}, {CTCSS_97_4Hz,   97.4}, {CTCSS_100_0Hz, 100.0},
  {CTCSS_103_5Hz, 103.5}, {CTCSS_107_2Hz, 107.2}, {CTCSS_110_9Hz, 110.9}, {CTCSS_114_8Hz, 114.8},
  {CTCSS_118_8Hz, 118.8}, {CTCSS_123_0Hz, 123.0}, {CTCSS_127_3Hz, 127.3}, {CTCSS_131_8Hz, 131.8},
  {CTCSS_136_5Hz, 136.5}, {CTCSS_141_3Hz, 141.3}, {CTCSS_146_2Hz, 146.2}, {CTCSS_151_4Hz, 151.4},
  {CTCSS_156_7Hz, 156.7}, {CTCSS_162_2Hz, 162.2}, {CTCSS_167_9Hz, 167.9}, {CTCSS_173_8Hz, 173.8},
  {CTCSS_179_9Hz, 179.9}, {CTCSS_186_2Hz, 186.2}, {CTCSS_192_8Hz, 192.8}, {CTCSS_203_5Hz, 203.5},
  {CTCSS_210_7Hz, 210.7}, {CTCSS_218_1Hz, 218.1}, {CTCSS_225_7Hz, 225.7}, {CTCSS_233_6Hz, 233.6},
  {CTCSS_241_8Hz, 241.8}, {CTCSS_250_3Hz, 250.3}
};

static QHash<uint16_t, Code> DCS_N_num2code {
  {  0, SIGNALING_NONE},
  { 23, DCS_023N}, { 25, DCS_025N}, { 26, DCS_026N}, { 31, DCS_031N}, { 32, DCS_032N},
  { 36, DCS_036N}, { 43, DCS_043N}, { 47, DCS_047N}, { 51, DCS_051N}, { 53, DCS_053N},
  { 54, DCS_054N}, { 71, DCS_071N}, { 72, DCS_072N}, { 73, DCS_073N}, { 74, DCS_074N},
  {114, DCS_114N}, {115, DCS_115N}, {116, DCS_116N}, {122, DCS_122N}, {125, DCS_125N},
  {131, DCS_131N}, {132, DCS_132N}, {134, DCS_134N}, {143, DCS_143N}, {145, DCS_145N},
  {152, DCS_152N}, {155, DCS_155N}, {156, DCS_156N}, {162, DCS_162N}, {165, DCS_165N},
  {172, DCS_172N}, {174, DCS_174N}, {205, DCS_205N}, {212, DCS_212N}, {223, DCS_223N},
  {225, DCS_225N}, {226, DCS_226N}, {243, DCS_243N}, {244, DCS_244N}, {245, DCS_245N},
  {246, DCS_246N}, {251, DCS_251N}, {252, DCS_252N}, {255, DCS_255N}, {261, DCS_261N},
  {263, DCS_263N}, {265, DCS_265N}, {266, DCS_266N}, {267, DCS_267N}, {271, DCS_271N},
  {274, DCS_274N}, {306, DCS_306N}, {311, DCS_311N}, {315, DCS_315N}, {325, DCS_325N},
  {331, DCS_331N}, {332, DCS_332N}, {343, DCS_343N}, {346, DCS_346N}, {351, DCS_351N},
  {356, DCS_356N}, {364, DCS_364N}, {365, DCS_365N}, {371, DCS_371N}, {411, DCS_411N},
  {412, DCS_412N}, {413, DCS_413N}, {423, DCS_423N}, {431, DCS_431N}, {432, DCS_432N},
  {445, DCS_445N}, {446, DCS_446N}, {452, DCS_452N}, {454, DCS_454N}, {455, DCS_455N},
  {462, DCS_462N}, {464, DCS_464N}, {465, DCS_465N}, {466, DCS_466N}, {503, DCS_503N},
  {506, DCS_506N}, {516, DCS_516N}, {523, DCS_523N}, {526, DCS_526N}, {532, DCS_532N},
  {546, DCS_546N}, {565, DCS_565N}, {606, DCS_606N}, {612, DCS_612N}, {624, DCS_624N},
  {627, DCS_627N}, {631, DCS_631N}, {632, DCS_632N}, {654, DCS_654N}, {662, DCS_662N},
  {664, DCS_664N}, {703, DCS_703N}, {712, DCS_712N}, {723, DCS_723N}, {731, DCS_731N},
  {732, DCS_732N}, {734, DCS_734N}, {743, DCS_743N}, {754, DCS_754N}
};

static QHash<Code, uint16_t> DCS_N_code2num {
  {SIGNALING_NONE, 0},
  {DCS_023N,  23}, {DCS_025N,  25}, {DCS_026N,  26}, {DCS_031N,  31}, {DCS_032N,  32},
  {DCS_036N,  36}, {DCS_043N,  43}, {DCS_047N,  47}, {DCS_051N,  51}, {DCS_053N,  53},
  {DCS_054N,  54}, {DCS_071N,  71}, {DCS_072N,  72}, {DCS_073N,  73}, {DCS_074N,  74},
  {DCS_114N, 114}, {DCS_115N, 115}, {DCS_116N, 116}, {DCS_122N, 122}, {DCS_125N, 125},
  {DCS_131N, 131}, {DCS_132N, 132}, {DCS_134N, 134}, {DCS_143N, 143}, {DCS_145N, 145},
  {DCS_152N, 152}, {DCS_155N, 155}, {DCS_156N, 156}, {DCS_162N, 162}, {DCS_165N, 165},
  {DCS_172N, 172}, {DCS_174N, 174}, {DCS_205N, 205}, {DCS_212N, 212}, {DCS_223N, 223},
  {DCS_225N, 225}, {DCS_226N, 226}, {DCS_243N, 243}, {DCS_244N, 244}, {DCS_245N, 245},
  {DCS_246N, 246}, {DCS_251N, 251}, {DCS_252N, 252}, {DCS_255N, 255}, {DCS_261N, 261},
  {DCS_263N, 263}, {DCS_265N, 265}, {DCS_266N, 266}, {DCS_267N, 267}, {DCS_271N, 271},
  {DCS_274N, 274}, {DCS_306N, 306}, {DCS_311N, 311}, {DCS_315N, 315}, {DCS_325N, 325},
  {DCS_331N, 331}, {DCS_332N, 332}, {DCS_343N, 343}, {DCS_346N, 346}, {DCS_351N, 351},
  {DCS_356N, 356}, {DCS_364N, 364}, {DCS_365N, 365}, {DCS_371N, 371}, {DCS_411N, 411},
  {DCS_412N, 412}, {DCS_413N, 413}, {DCS_423N, 423}, {DCS_431N, 431}, {DCS_432N, 432},
  {DCS_445N, 445}, {DCS_446N, 446}, {DCS_452N, 452}, {DCS_454N, 454}, {DCS_455N, 455},
  {DCS_462N, 462}, {DCS_464N, 464}, {DCS_465N, 465}, {DCS_466N, 466}, {DCS_503N, 503},
  {DCS_506N, 506}, {DCS_516N, 516}, {DCS_523N, 523}, {DCS_526N, 526}, {DCS_532N, 532},
  {DCS_546N, 546}, {DCS_565N, 565}, {DCS_606N, 606}, {DCS_612N, 612}, {DCS_624N, 624},
  {DCS_627N, 627}, {DCS_631N, 631}, {DCS_632N, 632}, {DCS_654N, 654}, {DCS_662N, 662},
  {DCS_664N, 664}, {DCS_703N, 703}, {DCS_712N, 712}, {DCS_723N, 723}, {DCS_731N, 731},
  {DCS_732N, 732}, {DCS_734N, 734}, {DCS_743N, 743}, {DCS_754N, 754}
};

static QHash<uint16_t, Code> DCS_I_num2code {
  {  0, SIGNALING_NONE},
  { 23, DCS_023I}, { 25, DCS_025I}, { 26, DCS_026I}, { 31, DCS_031I}, { 32, DCS_032I},
  { 36, DCS_036I}, { 43, DCS_043I}, { 47, DCS_047I}, { 51, DCS_051I}, { 53, DCS_053I},
  { 54, DCS_054I}, { 71, DCS_071I}, { 72, DCS_072I}, { 73, DCS_073I}, { 74, DCS_074I},
  {114, DCS_114I}, {115, DCS_115I}, {116, DCS_116I}, {122, DCS_122I}, {125, DCS_125I},
  {131, DCS_131I}, {132, DCS_132I}, {134, DCS_134I}, {143, DCS_143I}, {145, DCS_145I},
  {152, DCS_152I}, {155, DCS_155I}, {156, DCS_156I}, {162, DCS_162I}, {165, DCS_165I},
  {172, DCS_172I}, {174, DCS_174I}, {205, DCS_205I}, {212, DCS_212I}, {223, DCS_223I},
  {225, DCS_225I}, {226, DCS_226I}, {243, DCS_243I}, {244, DCS_244I}, {245, DCS_245I},
  {246, DCS_246I}, {251, DCS_251I}, {252, DCS_252I}, {255, DCS_255I}, {261, DCS_261I},
  {263, DCS_263I}, {265, DCS_265I}, {266, DCS_266I}, {267, DCS_267I}, {271, DCS_271I},
  {274, DCS_274I}, {306, DCS_306I}, {311, DCS_311I}, {315, DCS_315I}, {325, DCS_325I},
  {331, DCS_331I}, {332, DCS_332I}, {343, DCS_343I}, {346, DCS_346I}, {351, DCS_351I},
  {356, DCS_356I}, {364, DCS_364I}, {365, DCS_365I}, {371, DCS_371I}, {411, DCS_411I},
  {412, DCS_412I}, {413, DCS_413I}, {423, DCS_423I}, {431, DCS_431I}, {432, DCS_432I},
  {445, DCS_445I}, {446, DCS_446I}, {452, DCS_452I}, {454, DCS_454I}, {455, DCS_455I},
  {462, DCS_462I}, {464, DCS_464I}, {465, DCS_465I}, {466, DCS_466I}, {503, DCS_503I},
  {506, DCS_506I}, {516, DCS_516I}, {523, DCS_523I}, {526, DCS_526I}, {532, DCS_532I},
  {546, DCS_546I}, {565, DCS_565I}, {606, DCS_606I}, {612, DCS_612I}, {624, DCS_624I},
  {627, DCS_627I}, {631, DCS_631I}, {632, DCS_632I}, {654, DCS_654I}, {662, DCS_662I},
  {664, DCS_664I}, {703, DCS_703I}, {712, DCS_712I}, {723, DCS_723I}, {731, DCS_731I},
  {732, DCS_732I}, {734, DCS_734I}, {743, DCS_743I}, {754, DCS_754I}
};

static QHash<Code, uint16_t> DCS_I_code2num {
  {SIGNALING_NONE, 0},
  {DCS_023I,  23}, {DCS_025I,  25}, {DCS_026I,  26}, {DCS_031I,  31}, {DCS_032I,  32},
  {DCS_036I,  36}, {DCS_043I,  43}, {DCS_047I,  47}, {DCS_051I,  51}, {DCS_053I,  53},
  {DCS_054I,  54}, {DCS_071I,  71}, {DCS_072I,  72}, {DCS_073I,  73}, {DCS_074I,  74},
  {DCS_114I, 114}, {DCS_115I, 115}, {DCS_116I, 116}, {DCS_122I, 122}, {DCS_125I, 125},
  {DCS_131I, 131}, {DCS_132I, 132}, {DCS_134I, 134}, {DCS_143I, 143}, {DCS_145I, 145},
  {DCS_152I, 152}, {DCS_155I, 155}, {DCS_156I, 156}, {DCS_162I, 162}, {DCS_165I, 165},
  {DCS_172I, 172}, {DCS_174I, 174}, {DCS_205I, 205}, {DCS_212I, 212}, {DCS_223I, 223},
  {DCS_225I, 225}, {DCS_226I, 226}, {DCS_243I, 243}, {DCS_244I, 244}, {DCS_245I, 245},
  {DCS_246I, 246}, {DCS_251I, 251}, {DCS_252I, 252}, {DCS_255I, 255}, {DCS_261I, 261},
  {DCS_263I, 263}, {DCS_265I, 265}, {DCS_266I, 266}, {DCS_267I, 267}, {DCS_271I, 271},
  {DCS_274I, 274}, {DCS_306I, 306}, {DCS_311I, 311}, {DCS_315I, 315}, {DCS_325I, 325},
  {DCS_331I, 331}, {DCS_332I, 332}, {DCS_343I, 343}, {DCS_346I, 346}, {DCS_351I, 351},
  {DCS_356I, 356}, {DCS_364I, 364}, {DCS_365I, 365}, {DCS_371I, 371}, {DCS_411I, 411},
  {DCS_412I, 412}, {DCS_413I, 413}, {DCS_423I, 423}, {DCS_431I, 431}, {DCS_432I, 432},
  {DCS_445I, 445}, {DCS_446I, 446}, {DCS_452I, 452}, {DCS_454I, 454}, {DCS_455I, 455},
  {DCS_462I, 462}, {DCS_464I, 464}, {DCS_465I, 465}, {DCS_466I, 466}, {DCS_503I, 503},
  {DCS_506I, 506}, {DCS_516I, 516}, {DCS_523I, 523}, {DCS_526I, 526}, {DCS_532I, 532},
  {DCS_546I, 546}, {DCS_565I, 565}, {DCS_606I, 606}, {DCS_612I, 612}, {DCS_624I, 624},
  {DCS_627I, 627}, {DCS_631I, 631}, {DCS_632I, 632}, {DCS_654I, 654}, {DCS_662I, 662},
  {DCS_664I, 664}, {DCS_703I, 703}, {DCS_712I, 712}, {DCS_723I, 723}, {DCS_731I, 731},
  {DCS_732I, 732}, {DCS_734I, 734}, {DCS_743I, 743}, {DCS_754I, 754}
};


bool
Signaling::isCTCSS(Code code) {
  return ((CTCSS_67_0Hz<=code) && (CTCSS_250_3Hz>=code));
}

bool
Signaling::isCTCSSFrequency(float freq) {
  if (0.0 == freq)
    return false;
  return CTCSS_freq2code.contains(freq);
}

float
Signaling::toCTCSSFrequency(Code code) {
  if (! CTCSS_code2freq.contains(code))
    return SIGNALING_NONE;
  return CTCSS_code2freq[code];
}

Signaling::Code
Signaling::fromCTCSSFrequency(float f) {
  if ((0 == f) || (! CTCSS_freq2code.contains(f)))
    return SIGNALING_NONE;
  return CTCSS_freq2code[f];
}


bool
Signaling::isDCSNumber(uint16_t num) {
  if (0 == num)
    return false;
  return DCS_N_num2code.contains(num);
}

bool
Signaling::isDCSNormal(Code code) {
  return ((DCS_023N <= code) && (DCS_754N >= code));
}

bool
Signaling::isDCSInverted(Code code) {
  return ((DCS_023I <= code) && (DCS_754I >= code));
}

uint16_t
Signaling::toDCSNumber(Code code) {
  if ((DCS_023N <= code) && (DCS_754N >= code) && DCS_N_code2num.contains(code))
    return DCS_N_code2num[code];
  else if ((DCS_023I <= code) && (DCS_754I >= code) && DCS_I_code2num.contains(code))
    return DCS_I_code2num[code];
  return 0;
}

Signaling::Code
Signaling::fromDCSNumber(uint16_t num, bool inverted) {
  if (inverted && DCS_I_num2code.contains(num))
    return DCS_I_num2code[num];
  else if ((!inverted) && DCS_N_num2code.contains(num))
    return DCS_N_num2code[num];
  return SIGNALING_NONE;
}


QString
Signaling::codeLabel(Code code) {
  if ((CTCSS_67_0Hz <= code) && (CTCSS_250_3Hz >= code))   // If CTCSS signaling
    return QObject::tr("CTCSS %1Hz").arg(toCTCSSFrequency(code));
  else if ((DCS_023N <= code) && (DCS_754N >= code))       // If DCS normal signaling
    return QObject::tr("DCS %1 N").arg(toDCSNumber(code));
  else if ((DCS_023I <= code) && (DCS_754I >= code))       // If DCS inverted signaling
    return QObject::tr("DCS %1 I").arg(toDCSNumber(code));
  else
    return QObject::tr("[None]");
}

QString
Signaling::configString(Code code) {
  if (Signaling::isCTCSS(code))
    return QString::number(Signaling::toCTCSSFrequency(code), 'f', 1);
  else if (Signaling::isDCSNormal(code))
    return QString("n%1").arg((int)Signaling::toDCSNumber(code), 3, 10, QChar('0'));
  else if (Signaling::isDCSInverted(code))
    return QString("i%1").arg((int)Signaling::toDCSNumber(code), 3, 10, QChar('0'));
  return "-";
}
