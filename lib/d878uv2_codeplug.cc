#include "d878uv2_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>


#define NUM_APRS_RX_ENTRY         32
#define ADDR_APRS_RX_ENTRY        0x02501800 // Address of APRS RX list
#define APRS_RX_ENTRY_SIZE        0x00000008 // Size of each APRS RX entry
static_assert(
  APRS_RX_ENTRY_SIZE == sizeof(D878UV2Codeplug::aprs_rx_entry_t),
  "D878UV2Codeplug::aprs_rx_entry_t size check failed.");

#define ADDR_APRS_SET_EXT         0x025010A0 // Address of APRS settings extension
#define APRS_SET_EXT_SIZE         0x00000060 // Size of APRS settings extension
static_assert(
  APRS_SET_EXT_SIZE == sizeof(D878UV2Codeplug::aprs_setting_ext_t),
  "D878UV2Codeplug::aprs_setting_ext_t size check failed.");

#define ADDR_UNKNOWN_SETTING      0x02500600 // Address of unknown settings
#define UNKNOWN_SETTING_SIZE      0x00000030 // Size of unknown settings.

/* ******************************************************************************************** *
 * Implementation of D878UV2Codeplug
 * ******************************************************************************************** */
D878UV2Codeplug::D878UV2Codeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
  // pass...
}

void
D878UV2Codeplug::allocateUpdated() {
  // allocate everything from D878UV codeplug
  D878UVCodeplug::allocateUpdated();

  // allocate unknown settings
  image(0).addElement(ADDR_UNKNOWN_SETTING, UNKNOWN_SETTING_SIZE);

  // allocate APRS RX list
  image(0).addElement(ADDR_APRS_RX_ENTRY, NUM_APRS_RX_ENTRY*APRS_RX_ENTRY_SIZE);
  // allocate APRS settings extension
  image(0).addElement(ADDR_APRS_SET_EXT, APRS_SET_EXT_SIZE);
}



