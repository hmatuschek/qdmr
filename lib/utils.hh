/** @defgroup util Utility functions and classes.
 * This module collects all utility functions and classes. That is, functions to encode some
 * data and also classes implementing the DFU file format.
 */

#ifndef UTILS_HH
#define UTILS_HH

#include <QString>
#include <inttypes.h>

#include "signaling.hh"
#include "gpssystem.hh"
#include <QGeoCoordinate>

/** Decodes the unicode string stored in @c data of size @c size. The @c fill code also defines the
 * end-of-string symbol.
 * @returns The decoded string. */
QString decode_unicode(const uint16_t *data, size_t size, uint16_t fill=0x0000);
/** Encodes the string @c text as unicode and stores the result into @c data using up-to @c size
 * 16bit words in data. The @c fill word specifies the fill and end-of-string word. */
void encode_unicode(uint16_t *data, const QString &text, size_t size, uint16_t fill=0x0000);

/** Decodes the ascii string in @c data into a @c QString of up-to size length. The @c fill word
 * specifies the fill and end-of-string word. */
QString decode_ascii(const uint8_t *data, size_t size, uint16_t fill=0x00);
/** Encodes the given QString @c text of up-to size length as ASCII into @c data using the
 * @c fill word as fill and end-of-string word. */
void encode_ascii(uint8_t *data, const QString &text, size_t size, uint16_t fill=0x00);

/** Decodes the UTF-8 string in @c data into a @c QString of up-to size length. The @c fill word
 * specifies the fill and end-of-string word. */
QString decode_utf8(const uint8_t *data, size_t size, uint16_t fill=0x00);
/** Encodes the given QString @c text of up-to size length as UTF-8 into @c data using the
 * @c fill word as fill and end-of-string word. */
void encode_utf8(uint8_t *data, const QString &text, size_t size, uint16_t fill=0x00);

/** Decodes an 8 digit BCD encoded frequency (in MHz). */
double decode_frequency(uint32_t bcd);
/** Eecodes an 8 digit BCD encoded frequency (in MHz). */
uint32_t encode_frequency(double freq);

/** Decodes binary (24bit) encoded DMR ID. */
uint32_t decode_dmr_id_bin(const uint8_t *id);
/** Encodes binary (24bit) encoded DMR ID. */
void encode_dmr_id_bin(uint8_t *id, uint32_t num);
/** Decodes bcd (32bit) encoded DMR ID, little endian. */
uint32_t decode_dmr_id_bcd(const uint8_t *id);
/** Decodes bcd (32bit) encoded DMR ID, big endian. */
uint32_t decode_dmr_id_bcd_le(const uint8_t *id);
/** Encodes bcd (32bit) encoded DMR ID, little endian. */
void encode_dmr_id_bcd(uint8_t *id, uint32_t num);
/** Encodes bcd (32bit) encoded DMR ID, big endian. */
void encode_dmr_id_bcd_le(uint8_t *id, uint32_t num);

QString decode_dtmf_bin(const uint8_t *num, int size=16, uint8_t fill=0xff);
bool encode_dtmf_bin(const QString &number, uint8_t *num, int size=16, uint8_t fill=0xff);

QString decode_dtmf_bcd_be(const uint8_t *num, int digits);
bool encode_dtmf_bcd_be(const QString &number, uint8_t *num, int size, uint8_t fill);

/** Decodes the CTCSS tone or DCS code to @c Signaling::Code. */
Signaling::Code decode_ctcss_tone_table(uint16_t data);
/** Encodes the CTCSS tone or DCS code from @c Signaling::Code. */
uint16_t encode_ctcss_tone_table(Signaling::Code code);
/** Interprets the given number as octal and returns the decimal representation.
 * @param oct The octal number between 0-7777.*/
uint16_t oct_to_dec(uint16_t oct);
/** Returns the given number as octal value.
 * @param dec The decimal number between 0-4095*/
uint16_t dec_to_oct(uint16_t dec);

/** Validates a DMR ID number. */
bool validDMRNumber(const QString &text);
/** Validates a DTMF number. */
bool validDTMFNumber(const QString &text);

/** Formats a frequency in MHz passed as double. */
QString format_frequency(double MHz);

QString aprsicon2config(APRSSystem::Icon icon);
QString aprsicon2name(APRSSystem::Icon icon);
APRSSystem::Icon name2aprsicon(const QString &name);
char aprsicon2iconcode(APRSSystem::Icon icon);
char aprsicon2tablecode(APRSSystem::Icon icon);
APRSSystem::Icon code2aprsicon(char table, char icon);

/** Implements the Levenshtein distance between two strings.
 * That is, the number of edits (insert, delete or replace operations) needed to turn source
 * into target. */
int levDist(const QString &source, const QString &target,
            Qt::CaseSensitivity cs=Qt::CaseInsensitive);

/** Increases the given size to be aligned with the given block size. */
uint32_t align_size(uint32_t size, uint32_t block);
/** Decreases the address to be aligned with the given block size. */
uint32_t align_addr(uint32_t addr, uint32_t block);

QGeoCoordinate loc2deg(const QString &loc);
QString deg2loc(const QGeoCoordinate &coor);

#endif // UTILS_HH
