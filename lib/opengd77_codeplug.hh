#ifndef OPENGD77_CODEPLUG_HH
#define OPENGD77_CODEPLUG_HH

#include "gd77_codeplug.hh"

/** Represents, encodes and decodes the device specific codeplug for Open GD-77 firmware.
 * *
 * @section ogd77cpl Codeplug structure within radio
 * The memory representation of the codeplug within the radio is divided into two images
 * (EEPROM and Flash) and each image again into two sections.

 * The first segment of the EEPROM image starts at the address 0x000e0 and ends at 0x06000, while
 * the second EEPROM section starts at 0x07500 and ends at 0x0b000.
 *
 * The first segment of the Flash image starts at the address 0x00000 and ends at 0x011a0, while the
 * second Flash section starts at 0x7b000 and ends at 0x8ee60.
 *
 * <table>
 *  <tr><th>Start</th>   <th>End</th>      <th>Size</th>    <th>Content</th></tr>
 *  <tr><th colspan="4">First EEPROM segment 0x000e0-0x06000</th></tr>
 *  <tr><td>0x000e0</td> <td>0x000ec</td> <td>0x000c</td> <td>General settings, see @c GD77Codeplug::general_settings_t.</td></tr>
 *  <tr><td>0x000ec</td> <td>0x00128</td> <td>0x003c</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x00128</td> <td>0x01370</td> <td>0x1248</td> <td>32 message texts, see @c GD77Codeplug::msgtab_t</td></tr>
 *  <tr><td>0x01370</td> <td>0x01790</td> <td>0x0420</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x01790</td> <td>0x02dd0</td> <td>0x1640</td> <td>64 scan lists, see @c GD77Codeplug::scanlist_t</td></tr>
 *  <tr><td>0x02dd0</td> <td>0x03780</td> <td>0x09b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x03780</td> <td>0x05390</td> <td>0x1c10</td> <td>First 128 chanels (bank 0), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><th colspan="4">Second EEPROM segment 0x07500-0x0b000</th></tr>
 *  <tr><td>0x07500</td> <td>0x07540</td> <td>0x0040</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x07540</td> <td>0x07560</td> <td>0x0020</td> <td>2 intro lines, @c GD77Codeplug::intro_text_t</td></tr>
 *  <tr><td>0x07560</td> <td>0x07c00</td> <td>0x06a0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08000</td> <td>0x08010</td> <td>0x0010</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x08010</td> <td>0x0af10</td> <td>0x2f00</td> <td>250 zones, see @c GD77Codeplug::zonetab_t</td></tr>
 *  <tr><th colspan="4">First Flash segment 0x00000-0x011a0</th></tr>
 *  <tr><th colspan="4">Second Flash segment 0x7b000-0x8ee60</th></tr>
 *  <tr><td>0x7b100</td> <td>0x7b1b0</td> <td>0x00b0</td> <td>??? Unknown ???</td></tr>
 *  <tr><td>0x7b1b0</td> <td>0x87620</td> <td>0xc470</td> <td>Remaining 896 chanels (bank 1-7), see @c GD77Codeplug::bank_t</td></tr>
 *  <tr><td>0x87620</td> <td>0x8d620</td> <td>0x6000</td> <td>1024 contacts, see @c GD77Codeplug::contact_t.</td></tr>
 *  <tr><td>0x8d620</td> <td>0x8e2a0</td> <td>0x0c80</td> <td>64 RX group lists, see @c GD77Codeplug::grouptab_t</td></tr>
 *  <tr><td>0x8e2a0</td> <td>0x8e300</td> <td>0x0060</td> <td>??? Unknown ???</td></tr>
 * </table>
 * @ingroup ogd77 */
class OpenGD77Codeplug: public GD77Codeplug
{
	Q_OBJECT

public:
  static const uint32_t EEPROM = 0;
  static const uint32_t FLASH  = 1;

public:
  /** Constructs an empty codeplug for the GD-77. */
  explicit OpenGD77Codeplug(QObject *parent=nullptr);

	/** Decodes the binary codeplug and stores its content in the given generic configuration. */
	bool decode(Config *config);
  /** Encodes the given generic configuration as a binary codeplug. */
  bool encode(Config *config);
};

#endif // OPENGD77_CODEPLUG_HH
