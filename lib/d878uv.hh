/** @defgroup d878uv Anytone AT-D878UV
 * Device specific classes for Anytone AT-D878UV.
 *
 * \image html d878uv.jpg "AT-D878UV" width=200px
 * \image latex d878uv.jpg "AT-D878UV" width=200px
 *
 * @ingroup anytone */
#ifndef __D878UV_HH__
#define __D878UV_HH__

#include "anytone_radio.hh"
#include "anytone_interface.hh"


/** Implements an interface to Anytone AT-D878UV VHF/UHF 7W DMR (Tier I & II) radios.
 *
 * The reverse-engineering of the D878UVCodeplug was quiet hard as it is huge and the radio
 * provides a lot of bells and whistles. Moreover, the binary code-plug file created by the
 * windows CPS does not directly relate to the data being written to the device. These two issues
 * (a lot of features and a huge code-plug) require that the transfer of the code-plug to the
 * device is performed in 4 steps.
 *
 * First only the bitmaps of all lists are downloaded from the device. Then all elements that are
 * not touched or only updated by the common code-plug config are downloaded. Then, the common
 * config gets applied to the binary code-plug. That is, all channels, contacts, zones, group-lists
 * and scan-lists are generated and their bitmaps gets updated accordingly. Also the general config
 * gets updated from the common code-plug settings. Finally, the resulting binary code-plug gets
 * written back to the device.
 *
 * This rather complex method of writing a code-plug to the device is needed to maintain all
 * settings within the radio that are not defined within the common code-plug config while keeping
 * the amount of data being read from and written to the device small.
 *
 * @ingroup d878uv */
class D878UV: public AnytoneRadio
{
	Q_OBJECT

public:
  /** Do not construct this class directly, rather use @c Radio::detect. */
  explicit D878UV(AnytoneInterface *device=nullptr, QObject *parent=nullptr);

  const RadioLimits &limits() const;

  /** Returns the default radio information. The actual instance may have different properties
   * due to variants of the same radio. */
  static RadioInfo defaultRadioInfo();

private:
  RadioLimits *_limits;
};

#endif // __D878UV_HH__
