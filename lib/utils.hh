/** @defgroup util Utility functions and classes.
 * This module collects alls utility functions and classes. That is, functions to encode some
 * data and also classes implementing the DFU file format.
 */

#ifndef UTILS_HH
#define UTILS_HH

#include <QString>
#include <inttypes.h>

/** Decodes the unicode string stored in @c data of size @c size. The @c fill code also defines the
 * end-of-string symbol.
 * @returns The decoded string. */
QString decode_unicode(const uint16_t *data, size_t size, uint16_t fill=0x0000);
void encode_unicode(uint16_t *data, const QString &text, size_t size, uint16_t fill=0x0000);

QString decode_ascii(const uint8_t *data, size_t size, uint16_t fill=0x00);
void encode_ascii(uint8_t *data, const QString &text, size_t size, uint16_t fill=0x00);

double decode_frequency(uint32_t bcd);
uint32_t encode_frequency(double freq);

uint32_t decode_dmr_id(const uint8_t *id);
void encode_dmr_id(uint8_t *id, uint32_t num);

float decode_ctcss_tone(uint16_t data);
uint16_t encode_ctcss_tone(float tone);

#endif // UTILS_HH
