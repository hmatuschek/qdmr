#include "signaling.hh"

#include <QHash>
#include <QVector>
#include <QObject>



/** Internal used translation table between CTCSS frequencies and AnalogChannel::Signaling codes. */
static QHash<float, Signaling::Code> CTCSS_freq2code {
  {  0.0, Signaling::SIGNALING_NONE},
  { 67.0, Signaling::CTCSS_67_0Hz},  { 71.9, Signaling::CTCSS_71_9Hz},  { 74.4, Signaling::CTCSS_74_4Hz},  { 77.0,  Signaling::CTCSS_77_0Hz},
  { 79.7, Signaling::CTCSS_79_7Hz},  { 82.5, Signaling::CTCSS_82_5Hz},  { 85.4, Signaling::CTCSS_85_4Hz},  { 88.5,  Signaling::CTCSS_88_5Hz},
  { 91.5, Signaling::CTCSS_91_5Hz},  { 94.8, Signaling::CTCSS_94_8Hz},  { 97.4, Signaling::CTCSS_97_4Hz},  {100.0, Signaling::CTCSS_100_0Hz},
  {103.5, Signaling::CTCSS_103_5Hz}, {107.2, Signaling::CTCSS_107_2Hz}, {110.9, Signaling::CTCSS_110_9Hz}, {114.8, Signaling::CTCSS_114_8Hz},
  {118.8, Signaling::CTCSS_118_8Hz}, {123.0, Signaling::CTCSS_123_0Hz}, {127.3, Signaling::CTCSS_127_3Hz}, {131.8, Signaling::CTCSS_131_8Hz},
  {136.5, Signaling::CTCSS_136_5Hz}, {141.3, Signaling::CTCSS_141_3Hz}, {146.2, Signaling::CTCSS_146_2Hz}, {151.4, Signaling::CTCSS_151_4Hz},
  {156.7, Signaling::CTCSS_156_7Hz}, {162.2, Signaling::CTCSS_162_2Hz}, {167.9, Signaling::CTCSS_167_9Hz}, {173.8, Signaling::CTCSS_173_8Hz},
  {179.9, Signaling::CTCSS_179_9Hz}, {186.2, Signaling::CTCSS_186_2Hz}, {192.8, Signaling::CTCSS_192_8Hz}, {203.5, Signaling::CTCSS_203_5Hz},
  {210.7, Signaling::CTCSS_210_7Hz}, {218.1, Signaling::CTCSS_218_1Hz}, {225.7, Signaling::CTCSS_225_7Hz}, {233.6, Signaling::CTCSS_233_6Hz},
  {241.8, Signaling::CTCSS_241_8Hz}, {250.3, Signaling::CTCSS_250_3Hz}
};

/** Internal used translation table between CTCSS frequencies and AnalogChannel::Signaling codes. */
static QHash<Signaling::Code, float> CTCSS_code2freq {
  {Signaling::SIGNALING_NONE,   0.0},
  {Signaling::CTCSS_67_0Hz,   67.0}, {Signaling::CTCSS_71_9Hz,   71.9}, {Signaling::CTCSS_74_4Hz,   74.4}, {Signaling::CTCSS_77_0Hz,   77.0},
  {Signaling::CTCSS_79_7Hz,   79.7}, {Signaling::CTCSS_82_5Hz,   82.5}, {Signaling::CTCSS_85_4Hz,   85.4}, {Signaling::CTCSS_88_5Hz,   88.5},
  {Signaling::CTCSS_91_5Hz,   91.5}, {Signaling::CTCSS_94_8Hz,   94.8}, {Signaling::CTCSS_97_4Hz,   97.4}, {Signaling::CTCSS_100_0Hz, 100.0},
  {Signaling::CTCSS_103_5Hz, 103.5}, {Signaling::CTCSS_107_2Hz, 107.2}, {Signaling::CTCSS_110_9Hz, 110.9}, {Signaling::CTCSS_114_8Hz, 114.8},
  {Signaling::CTCSS_118_8Hz, 118.8}, {Signaling::CTCSS_123_0Hz, 123.0}, {Signaling::CTCSS_127_3Hz, 127.3}, {Signaling::CTCSS_131_8Hz, 131.8},
  {Signaling::CTCSS_136_5Hz, 136.5}, {Signaling::CTCSS_141_3Hz, 141.3}, {Signaling::CTCSS_146_2Hz, 146.2}, {Signaling::CTCSS_151_4Hz, 151.4},
  {Signaling::CTCSS_156_7Hz, 156.7}, {Signaling::CTCSS_162_2Hz, 162.2}, {Signaling::CTCSS_167_9Hz, 167.9}, {Signaling::CTCSS_173_8Hz, 173.8},
  {Signaling::CTCSS_179_9Hz, 179.9}, {Signaling::CTCSS_186_2Hz, 186.2}, {Signaling::CTCSS_192_8Hz, 192.8}, {Signaling::CTCSS_203_5Hz, 203.5},
  {Signaling::CTCSS_210_7Hz, 210.7}, {Signaling::CTCSS_218_1Hz, 218.1}, {Signaling::CTCSS_225_7Hz, 225.7}, {Signaling::CTCSS_233_6Hz, 233.6},
  {Signaling::CTCSS_241_8Hz, 241.8}, {Signaling::CTCSS_250_3Hz, 250.3}
};

static QHash<uint16_t, Signaling::Code> DCS_N_num2code {
  {  0, Signaling::SIGNALING_NONE},
  { 23, Signaling::DCS_023N}, { 25, Signaling::DCS_025N}, { 26, Signaling::DCS_026N}, { 31, Signaling::DCS_031N}, { 32, Signaling::DCS_032N},
  { 36, Signaling::DCS_036N}, { 43, Signaling::DCS_043N}, { 47, Signaling::DCS_047N}, { 51, Signaling::DCS_051N}, { 53, Signaling::DCS_053N},
  { 54, Signaling::DCS_054N}, { 71, Signaling::DCS_071N}, { 72, Signaling::DCS_072N}, { 73, Signaling::DCS_073N}, { 74, Signaling::DCS_074N},
  {114, Signaling::DCS_114N}, {115, Signaling::DCS_115N}, {116, Signaling::DCS_116N}, {122, Signaling::DCS_122N}, {125, Signaling::DCS_125N},
  {131, Signaling::DCS_131N}, {132, Signaling::DCS_132N}, {134, Signaling::DCS_134N}, {143, Signaling::DCS_143N}, {145, Signaling::DCS_145N},
  {152, Signaling::DCS_152N}, {155, Signaling::DCS_155N}, {156, Signaling::DCS_156N}, {162, Signaling::DCS_162N}, {165, Signaling::DCS_165N},
  {172, Signaling::DCS_172N}, {174, Signaling::DCS_174N}, {205, Signaling::DCS_205N}, {212, Signaling::DCS_212N}, {223, Signaling::DCS_223N},
  {225, Signaling::DCS_225N}, {226, Signaling::DCS_226N}, {243, Signaling::DCS_243N}, {244, Signaling::DCS_244N}, {245, Signaling::DCS_245N},
  {246, Signaling::DCS_246N}, {251, Signaling::DCS_251N}, {252, Signaling::DCS_252N}, {255, Signaling::DCS_255N}, {261, Signaling::DCS_261N},
  {263, Signaling::DCS_263N}, {265, Signaling::DCS_265N}, {266, Signaling::DCS_266N}, {267, Signaling::DCS_267N}, {271, Signaling::DCS_271N},
  {274, Signaling::DCS_274N}, {306, Signaling::DCS_306N}, {311, Signaling::DCS_311N}, {315, Signaling::DCS_315N}, {325, Signaling::DCS_325N},
  {331, Signaling::DCS_331N}, {332, Signaling::DCS_332N}, {343, Signaling::DCS_343N}, {346, Signaling::DCS_346N}, {351, Signaling::DCS_351N},
  {356, Signaling::DCS_356N}, {364, Signaling::DCS_364N}, {365, Signaling::DCS_365N}, {371, Signaling::DCS_371N}, {411, Signaling::DCS_411N},
  {412, Signaling::DCS_412N}, {413, Signaling::DCS_413N}, {423, Signaling::DCS_423N}, {431, Signaling::DCS_431N}, {432, Signaling::DCS_432N},
  {445, Signaling::DCS_445N}, {446, Signaling::DCS_446N}, {452, Signaling::DCS_452N}, {454, Signaling::DCS_454N}, {455, Signaling::DCS_455N},
  {462, Signaling::DCS_462N}, {464, Signaling::DCS_464N}, {465, Signaling::DCS_465N}, {466, Signaling::DCS_466N}, {503, Signaling::DCS_503N},
  {506, Signaling::DCS_506N}, {516, Signaling::DCS_516N}, {523, Signaling::DCS_523N}, {526, Signaling::DCS_526N}, {532, Signaling::DCS_532N},
  {546, Signaling::DCS_546N}, {565, Signaling::DCS_565N}, {606, Signaling::DCS_606N}, {612, Signaling::DCS_612N}, {624, Signaling::DCS_624N},
  {627, Signaling::DCS_627N}, {631, Signaling::DCS_631N}, {632, Signaling::DCS_632N}, {654, Signaling::DCS_654N}, {662, Signaling::DCS_662N},
  {664, Signaling::DCS_664N}, {703, Signaling::DCS_703N}, {712, Signaling::DCS_712N}, {723, Signaling::DCS_723N}, {731, Signaling::DCS_731N},
  {732, Signaling::DCS_732N}, {734, Signaling::DCS_734N}, {743, Signaling::DCS_743N}, {754, Signaling::DCS_754N}
};

static QHash<Signaling::Code, uint16_t> DCS_N_code2num {
  {Signaling::SIGNALING_NONE, 0},
  {Signaling::DCS_023N,  23}, {Signaling::DCS_025N,  25}, {Signaling::DCS_026N,  26}, {Signaling::DCS_031N,  31}, {Signaling::DCS_032N,  32},
  {Signaling::DCS_036N,  36}, {Signaling::DCS_043N,  43}, {Signaling::DCS_047N,  47}, {Signaling::DCS_051N,  51}, {Signaling::DCS_053N,  53},
  {Signaling::DCS_054N,  54}, {Signaling::DCS_071N,  71}, {Signaling::DCS_072N,  72}, {Signaling::DCS_073N,  73}, {Signaling::DCS_074N,  74},
  {Signaling::DCS_114N, 114}, {Signaling::DCS_115N, 115}, {Signaling::DCS_116N, 116}, {Signaling::DCS_122N, 122}, {Signaling::DCS_125N, 125},
  {Signaling::DCS_131N, 131}, {Signaling::DCS_132N, 132}, {Signaling::DCS_134N, 134}, {Signaling::DCS_143N, 143}, {Signaling::DCS_145N, 145},
  {Signaling::DCS_152N, 152}, {Signaling::DCS_155N, 155}, {Signaling::DCS_156N, 156}, {Signaling::DCS_162N, 162}, {Signaling::DCS_165N, 165},
  {Signaling::DCS_172N, 172}, {Signaling::DCS_174N, 174}, {Signaling::DCS_205N, 205}, {Signaling::DCS_212N, 212}, {Signaling::DCS_223N, 223},
  {Signaling::DCS_225N, 225}, {Signaling::DCS_226N, 226}, {Signaling::DCS_243N, 243}, {Signaling::DCS_244N, 244}, {Signaling::DCS_245N, 245},
  {Signaling::DCS_246N, 246}, {Signaling::DCS_251N, 251}, {Signaling::DCS_252N, 252}, {Signaling::DCS_255N, 255}, {Signaling::DCS_261N, 261},
  {Signaling::DCS_263N, 263}, {Signaling::DCS_265N, 265}, {Signaling::DCS_266N, 266}, {Signaling::DCS_267N, 267}, {Signaling::DCS_271N, 271},
  {Signaling::DCS_274N, 274}, {Signaling::DCS_306N, 306}, {Signaling::DCS_311N, 311}, {Signaling::DCS_315N, 315}, {Signaling::DCS_325N, 325},
  {Signaling::DCS_331N, 331}, {Signaling::DCS_332N, 332}, {Signaling::DCS_343N, 343}, {Signaling::DCS_346N, 346}, {Signaling::DCS_351N, 351},
  {Signaling::DCS_356N, 356}, {Signaling::DCS_364N, 364}, {Signaling::DCS_365N, 365}, {Signaling::DCS_371N, 371}, {Signaling::DCS_411N, 411},
  {Signaling::DCS_412N, 412}, {Signaling::DCS_413N, 413}, {Signaling::DCS_423N, 423}, {Signaling::DCS_431N, 431}, {Signaling::DCS_432N, 432},
  {Signaling::DCS_445N, 445}, {Signaling::DCS_446N, 446}, {Signaling::DCS_452N, 452}, {Signaling::DCS_454N, 454}, {Signaling::DCS_455N, 455},
  {Signaling::DCS_462N, 462}, {Signaling::DCS_464N, 464}, {Signaling::DCS_465N, 465}, {Signaling::DCS_466N, 466}, {Signaling::DCS_503N, 503},
  {Signaling::DCS_506N, 506}, {Signaling::DCS_516N, 516}, {Signaling::DCS_523N, 523}, {Signaling::DCS_526N, 526}, {Signaling::DCS_532N, 532},
  {Signaling::DCS_546N, 546}, {Signaling::DCS_565N, 565}, {Signaling::DCS_606N, 606}, {Signaling::DCS_612N, 612}, {Signaling::DCS_624N, 624},
  {Signaling::DCS_627N, 627}, {Signaling::DCS_631N, 631}, {Signaling::DCS_632N, 632}, {Signaling::DCS_654N, 654}, {Signaling::DCS_662N, 662},
  {Signaling::DCS_664N, 664}, {Signaling::DCS_703N, 703}, {Signaling::DCS_712N, 712}, {Signaling::DCS_723N, 723}, {Signaling::DCS_731N, 731},
  {Signaling::DCS_732N, 732}, {Signaling::DCS_734N, 734}, {Signaling::DCS_743N, 743}, {Signaling::DCS_754N, 754}
};

static QHash<uint16_t, Signaling::Code> DCS_I_num2code {
  {  0, Signaling::SIGNALING_NONE},
  { 23, Signaling::DCS_023I}, { 25, Signaling::DCS_025I}, { 26, Signaling::DCS_026I}, { 31, Signaling::DCS_031I}, { 32, Signaling::DCS_032I},
  { 36, Signaling::DCS_036I}, { 43, Signaling::DCS_043I}, { 47, Signaling::DCS_047I}, { 51, Signaling::DCS_051I}, { 53, Signaling::DCS_053I},
  { 54, Signaling::DCS_054I}, { 71, Signaling::DCS_071I}, { 72, Signaling::DCS_072I}, { 73, Signaling::DCS_073I}, { 74, Signaling::DCS_074I},
  {114, Signaling::DCS_114I}, {115, Signaling::DCS_115I}, {116, Signaling::DCS_116I}, {122, Signaling::DCS_122I}, {125, Signaling::DCS_125I},
  {131, Signaling::DCS_131I}, {132, Signaling::DCS_132I}, {134, Signaling::DCS_134I}, {143, Signaling::DCS_143I}, {145, Signaling::DCS_145I},
  {152, Signaling::DCS_152I}, {155, Signaling::DCS_155I}, {156, Signaling::DCS_156I}, {162, Signaling::DCS_162I}, {165, Signaling::DCS_165I},
  {172, Signaling::DCS_172I}, {174, Signaling::DCS_174I}, {205, Signaling::DCS_205I}, {212, Signaling::DCS_212I}, {223, Signaling::DCS_223I},
  {225, Signaling::DCS_225I}, {226, Signaling::DCS_226I}, {243, Signaling::DCS_243I}, {244, Signaling::DCS_244I}, {245, Signaling::DCS_245I},
  {246, Signaling::DCS_246I}, {251, Signaling::DCS_251I}, {252, Signaling::DCS_252I}, {255, Signaling::DCS_255I}, {261, Signaling::DCS_261I},
  {263, Signaling::DCS_263I}, {265, Signaling::DCS_265I}, {266, Signaling::DCS_266I}, {267, Signaling::DCS_267I}, {271, Signaling::DCS_271I},
  {274, Signaling::DCS_274I}, {306, Signaling::DCS_306I}, {311, Signaling::DCS_311I}, {315, Signaling::DCS_315I}, {325, Signaling::DCS_325I},
  {331, Signaling::DCS_331I}, {332, Signaling::DCS_332I}, {343, Signaling::DCS_343I}, {346, Signaling::DCS_346I}, {351, Signaling::DCS_351I},
  {356, Signaling::DCS_356I}, {364, Signaling::DCS_364I}, {365, Signaling::DCS_365I}, {371, Signaling::DCS_371I}, {411, Signaling::DCS_411I},
  {412, Signaling::DCS_412I}, {413, Signaling::DCS_413I}, {423, Signaling::DCS_423I}, {431, Signaling::DCS_431I}, {432, Signaling::DCS_432I},
  {445, Signaling::DCS_445I}, {446, Signaling::DCS_446I}, {452, Signaling::DCS_452I}, {454, Signaling::DCS_454I}, {455, Signaling::DCS_455I},
  {462, Signaling::DCS_462I}, {464, Signaling::DCS_464I}, {465, Signaling::DCS_465I}, {466, Signaling::DCS_466I}, {503, Signaling::DCS_503I},
  {506, Signaling::DCS_506I}, {516, Signaling::DCS_516I}, {523, Signaling::DCS_523I}, {526, Signaling::DCS_526I}, {532, Signaling::DCS_532I},
  {546, Signaling::DCS_546I}, {565, Signaling::DCS_565I}, {606, Signaling::DCS_606I}, {612, Signaling::DCS_612I}, {624, Signaling::DCS_624I},
  {627, Signaling::DCS_627I}, {631, Signaling::DCS_631I}, {632, Signaling::DCS_632I}, {654, Signaling::DCS_654I}, {662, Signaling::DCS_662I},
  {664, Signaling::DCS_664I}, {703, Signaling::DCS_703I}, {712, Signaling::DCS_712I}, {723, Signaling::DCS_723I}, {731, Signaling::DCS_731I},
  {732, Signaling::DCS_732I}, {734, Signaling::DCS_734I}, {743, Signaling::DCS_743I}, {754, Signaling::DCS_754I}
};

static QHash<Signaling::Code, uint16_t> DCS_I_code2num {
  {Signaling::SIGNALING_NONE, 0},
  {Signaling::DCS_023I,  23}, {Signaling::DCS_025I,  25}, {Signaling::DCS_026I,  26}, {Signaling::DCS_031I,  31}, {Signaling::DCS_032I,  32},
  {Signaling::DCS_036I,  36}, {Signaling::DCS_043I,  43}, {Signaling::DCS_047I,  47}, {Signaling::DCS_051I,  51}, {Signaling::DCS_053I,  53},
  {Signaling::DCS_054I,  54}, {Signaling::DCS_071I,  71}, {Signaling::DCS_072I,  72}, {Signaling::DCS_073I,  73}, {Signaling::DCS_074I,  74},
  {Signaling::DCS_114I, 114}, {Signaling::DCS_115I, 115}, {Signaling::DCS_116I, 116}, {Signaling::DCS_122I, 122}, {Signaling::DCS_125I, 125},
  {Signaling::DCS_131I, 131}, {Signaling::DCS_132I, 132}, {Signaling::DCS_134I, 134}, {Signaling::DCS_143I, 143}, {Signaling::DCS_145I, 145},
  {Signaling::DCS_152I, 152}, {Signaling::DCS_155I, 155}, {Signaling::DCS_156I, 156}, {Signaling::DCS_162I, 162}, {Signaling::DCS_165I, 165},
  {Signaling::DCS_172I, 172}, {Signaling::DCS_174I, 174}, {Signaling::DCS_205I, 205}, {Signaling::DCS_212I, 212}, {Signaling::DCS_223I, 223},
  {Signaling::DCS_225I, 225}, {Signaling::DCS_226I, 226}, {Signaling::DCS_243I, 243}, {Signaling::DCS_244I, 244}, {Signaling::DCS_245I, 245},
  {Signaling::DCS_246I, 246}, {Signaling::DCS_251I, 251}, {Signaling::DCS_252I, 252}, {Signaling::DCS_255I, 255}, {Signaling::DCS_261I, 261},
  {Signaling::DCS_263I, 263}, {Signaling::DCS_265I, 265}, {Signaling::DCS_266I, 266}, {Signaling::DCS_267I, 267}, {Signaling::DCS_271I, 271},
  {Signaling::DCS_274I, 274}, {Signaling::DCS_306I, 306}, {Signaling::DCS_311I, 311}, {Signaling::DCS_315I, 315}, {Signaling::DCS_325I, 325},
  {Signaling::DCS_331I, 331}, {Signaling::DCS_332I, 332}, {Signaling::DCS_343I, 343}, {Signaling::DCS_346I, 346}, {Signaling::DCS_351I, 351},
  {Signaling::DCS_356I, 356}, {Signaling::DCS_364I, 364}, {Signaling::DCS_365I, 365}, {Signaling::DCS_371I, 371}, {Signaling::DCS_411I, 411},
  {Signaling::DCS_412I, 412}, {Signaling::DCS_413I, 413}, {Signaling::DCS_423I, 423}, {Signaling::DCS_431I, 431}, {Signaling::DCS_432I, 432},
  {Signaling::DCS_445I, 445}, {Signaling::DCS_446I, 446}, {Signaling::DCS_452I, 452}, {Signaling::DCS_454I, 454}, {Signaling::DCS_455I, 455},
  {Signaling::DCS_462I, 462}, {Signaling::DCS_464I, 464}, {Signaling::DCS_465I, 465}, {Signaling::DCS_466I, 466}, {Signaling::DCS_503I, 503},
  {Signaling::DCS_506I, 506}, {Signaling::DCS_516I, 516}, {Signaling::DCS_523I, 523}, {Signaling::DCS_526I, 526}, {Signaling::DCS_532I, 532},
  {Signaling::DCS_546I, 546}, {Signaling::DCS_565I, 565}, {Signaling::DCS_606I, 606}, {Signaling::DCS_612I, 612}, {Signaling::DCS_624I, 624},
  {Signaling::DCS_627I, 627}, {Signaling::DCS_631I, 631}, {Signaling::DCS_632I, 632}, {Signaling::DCS_654I, 654}, {Signaling::DCS_662I, 662},
  {Signaling::DCS_664I, 664}, {Signaling::DCS_703I, 703}, {Signaling::DCS_712I, 712}, {Signaling::DCS_723I, 723}, {Signaling::DCS_731I, 731},
  {Signaling::DCS_732I, 732}, {Signaling::DCS_734I, 734}, {Signaling::DCS_743I, 743}, {Signaling::DCS_754I, 754}
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
