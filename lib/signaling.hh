#ifndef SIGNALING_HH
#define SIGNALING_HH

#include <QString>

/** Contains the enum and utility functions to deal with analog signaling like
 * CTCSS and DCS.
 * @ingroup conf */
namespace Signaling
{
  /** This huge enum lists all possible RX/TX tones that can be associated with analog channels.
   * That is, all valid CTCSS tones and DSC numbers (both normal @c DCS_*N and inverted @c DCS_*I).
   * If @c SIGNALING_NONE is selected, no RX/TX tone is used. */
  typedef enum {
    SIGNALING_NONE = 0,
    CTCSS_67_0Hz,  CTCSS_71_0Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_9Hz,  CTCSS_82_5Hz,
    CTCSS_85_4Hz,  CTCSS_88_5Hz,  CTCSS_91_5Hz,  CTCSS_94_8Hz,  CTCSS_97_4Hz,  CTCSS_100_0Hz,
    CTCSS_103_5Hz, CTCSS_107_2Hz, CTCSS_110_9Hz, CTCSS_114_8Hz, CTCSS_118_8Hz, CTCSS_123_0Hz,
    CTCSS_127_3Hz, CTCSS_131_8Hz, CTCSS_136_5Hz, CTCSS_141_3Hz, CTCSS_146_2Hz, CTCSS_151_4Hz,
    CTCSS_156_7Hz, CTCSS_162_2Hz, CTCSS_167_9Hz, CTCSS_173_8Hz, CTCSS_179_9Hz, CTCSS_186_2Hz,
    CTCSS_192_8Hz, CTCSS_203_5Hz, CTCSS_210_7Hz, CTCSS_218_1Hz, CTCSS_225_7Hz, CTCSS_233_6Hz,
    CTCSS_241_8Hz, CTCSS_250_3Hz,
    DCS_023N, DCS_025N, DCS_026N, DCS_031N, DCS_032N, DCS_036N, DCS_043N, DCS_047N, DCS_051N,
    DCS_053N, DCS_054N, DCS_071N, DCS_072N, DCS_073N, DCS_074N, DCS_114N, DCS_115N, DCS_116N,
    DCS_122N, DCS_125N, DCS_131N, DCS_132N, DCS_134N, DCS_143N, DCS_145N, DCS_152N, DCS_155N,
    DCS_156N, DCS_162N, DCS_165N, DCS_172N, DCS_174N, DCS_205N, DCS_212N, DCS_223N, DCS_225N,
    DCS_226N, DCS_243N, DCS_244N, DCS_245N, DCS_246N, DCS_251N, DCS_252N, DCS_255N, DCS_261N,
    DCS_263N, DCS_265N, DCS_266N, DCS_267N, DCS_271N, DCS_274N, DCS_306N, DCS_311N, DCS_315N,
    DCS_325N, DCS_331N, DCS_332N, DCS_343N, DCS_346N, DCS_351N, DCS_356N, DCS_364N, DCS_365N,
    DCS_371N, DCS_411N, DCS_412N, DCS_413N, DCS_423N, DCS_431N, DCS_432N, DCS_445N, DCS_446N,
    DCS_452N, DCS_454N, DCS_455N, DCS_462N, DCS_464N, DCS_465N, DCS_466N, DCS_503N, DCS_506N,
    DCS_516N, DCS_523N, DCS_526N, DCS_532N, DCS_546N, DCS_565N, DCS_606N, DCS_612N, DCS_624N,
    DCS_627N, DCS_631N, DCS_632N, DCS_654N, DCS_662N, DCS_664N, DCS_703N, DCS_712N, DCS_723N,
    DCS_731N, DCS_732N, DCS_734N, DCS_743N, DCS_754N,
    DCS_023I, DCS_025I, DCS_026I, DCS_031I, DCS_032I, DCS_036I, DCS_043I, DCS_047I, DCS_051I,
    DCS_053I, DCS_054I, DCS_071I, DCS_072I, DCS_073I, DCS_074I, DCS_114I, DCS_115I, DCS_116I,
    DCS_122I, DCS_125I, DCS_131I, DCS_132I, DCS_134I, DCS_143I, DCS_145I, DCS_152I, DCS_155I,
    DCS_156I, DCS_162I, DCS_165I, DCS_172I, DCS_174I, DCS_205I, DCS_212I, DCS_223I, DCS_225I,
    DCS_226I, DCS_243I, DCS_244I, DCS_245I, DCS_246I, DCS_251I, DCS_252I, DCS_255I, DCS_261I,
    DCS_263I, DCS_265I, DCS_266I, DCS_267I, DCS_271I, DCS_274I, DCS_306I, DCS_311I, DCS_315I,
    DCS_325I, DCS_331I, DCS_332I, DCS_343I, DCS_346I, DCS_351I, DCS_356I, DCS_364I, DCS_365I,
    DCS_371I, DCS_411I, DCS_412I, DCS_413I, DCS_423I, DCS_431I, DCS_432I, DCS_445I, DCS_446I,
    DCS_452I, DCS_454I, DCS_455I, DCS_462I, DCS_464I, DCS_465I, DCS_466I, DCS_503I, DCS_506I,
    DCS_516I, DCS_523I, DCS_526I, DCS_532I, DCS_546I, DCS_565I, DCS_606I, DCS_612I, DCS_624I,
    DCS_627I, DCS_631I, DCS_632I, DCS_654I, DCS_662I, DCS_664I, DCS_703I, DCS_712I, DCS_723I,
    DCS_731I, DCS_732I, DCS_734I, DCS_743I, DCS_754I
  } Code;

  /** Returns @c true if the given Signaling::Code enum entry refers to a CTCSS frequency. */
  bool isCTCSS(Code code);
  /** Returns @c true if the given frequency is a valid CTCSS frequency. */
  bool isCTCSSFrequency(float freq);
  /** Maps CTCSS enum to CTCSS frequency.
   * Returns @c 0.0 if no valid CTCSS enum element is given (e.g., @c SIGNALING_NONE or one of the
   * DCS enum elements). */
  float toCTCSSFrequency(Code code);
  /** Maps a CTCSS frequency to the corresponding Signaling::Code enum element.
   * Retuns @c SIGNALING_NONE if an invalid CTCSS frequency is given. */
  Code fromCTCSSFrequency(float freq);

  /** Returns @c true if a valid DCS code number is given. */
  bool isDCSNumber(uint16_t num);
  /** Returns @c true if the given DCS code is not inverted. */
  bool isDCSNormal(Code code);
  /** Returns @c true if the given DCS code is inverted. */
  bool isDCSInverted(Code code);
  /** Maps a DCS Signaling::Code to the corresponding DCS number. */
  uint16_t toDCSNumber(Code code);
  /** Maps a DCS number to the corresponding DCS Signaling::Code enum element.
   * The @c inverted parameter specifies whether an inverted code is used.
   * Retruns SIGNALING_NONE if an invalid DCS number is given. */
  Code fromDCSNumber(uint16_t num, bool inverted);

  /** Maps a Signaling::Code enum element to its text label. */
  QString codeLabel(Code code);
  /** Represents the given signaling code as a string as used in config files. */
  QString configString(Code code);
}

#endif // SIGNALING_HH
