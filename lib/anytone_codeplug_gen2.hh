#ifndef ANYTONECODEPLUGGEN2_HH
#define ANYTONECODEPLUGGEN2_HH

#include "anytone_codeplugbase.hh"
#include "signaling.hh"


/** Base class of the second generation of AnyTone codeplugs, starting with the AT-D890UV.
 *
 * The second generation shares the transfer protocol with the first one (see
 * @c AnytoneCodeplugGen1), but breaks large parts of the binary codeplug format. Hence it does
 * not reuse any of the first generation codeplug elements.
 *
 * In contrast to the first generation, a codeplug written to the radio is never initialized from
 * scratch. It only ever updates the codeplug read from the radio, and only writes the regions it
 * models. Records (channels, zones, etc.) are encoded from a template holding the
 * values the vendor CPS writes, for all fields that are not modelled.
 *
 * @ingroup anytone */
class AnytoneCodeplugGen2 : public AnytoneCodeplugBase
{
  Q_OBJECT

public:
  /** Encodes CTCSS tones as table indices. */
  struct CTCSS {
  public:
    /** Encodes the given CTCSS tone. Returns @c customIndex() if the tone is not in the table. */
    static uint8_t encode(const SelectiveCall &tone);
    /** Decodes the given CTCSS table index. */
    static SelectiveCall decode(uint8_t idx);
    /** The index, marking a custom CTCSS tone. */
    static constexpr uint8_t customIndex() { return 51; }

  protected:
    /** The CTCSS tone table. */
    static const SelectiveCall _codeTable[];
  };

  /** A bitmap of valid records, a set bit marks a valid record. */
  class BitmapElement: public Codeplug::BitmapElement
  {
  public:
    /** Constructor. */
    BitmapElement(uint8_t *ptr, size_t size);
  };

  /** A bitmap of valid records, a cleared bit marks a valid record. */
  class InvertedBitmapElement: public Codeplug::InvertedBitmapElement
  {
  public:
    /** Constructor. */
    InvertedBitmapElement(uint8_t *ptr, size_t size);
  };

protected:
  /** Hidden constructor.
   * @param label Specifies the label of the image, the codeplug is stored in. */
  explicit AnytoneCodeplugGen2(const QString &label, QObject *parent=nullptr);

public:
  /** Clears the codeplug, leaving an empty image. */
  void clear() override;

  /** Splits zones with channels for VFO A and B into two zones, as the radio only knows a single
   * channel list per zone. */
  Config *preprocess(Config *config, const ErrorStack &err) const override;
  /** Merges zones split by @c preprocess. */
  bool postprocess(Config *config, const ErrorStack &err) const override;

protected:
  /** The label of the image. */
  QString _label;
};

#endif // ANYTONECODEPLUGGEN2_HH
