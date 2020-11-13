#include "d878uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>

#define NUM_CHANNELS              4000
#define NUM_CHANNEL_BANKS         32
#define ADDR_CHANNEL_BANK_0       0x00800000
#define ADDR_CHANNEL_BANK_SIZE    0x00020000
#define ADDR_CHANNEL_BANK_31      0x00fc0000
#define ADDR_CHANNEL_BANK_31_SIZE 0x00000880
#define ADDR_CHANNEL_BANK_OFFSET  0x00040000
#define ADDR_CHANNEL_BITMAP       0x024c1500
#define ADDR_CHANNEL_BITMAP_SIZE  0x00000240

using namespace Signaling;

Code ctcss_num2code[52] = {
  SIGNALING_NONE, // 62.5 not supported
  CTCSS_67_0Hz,  CTCSS_71_0Hz,  CTCSS_74_4Hz,  CTCSS_77_0Hz,  CTCSS_79_9Hz,  CTCSS_82_5Hz,
  CTCSS_85_4Hz,  CTCSS_88_5Hz,  CTCSS_91_5Hz,  CTCSS_94_8Hz,  CTCSS_97_4Hz,  CTCSS_100_0Hz,
  CTCSS_103_5Hz, CTCSS_107_2Hz, CTCSS_110_9Hz, CTCSS_114_8Hz, CTCSS_118_8Hz, CTCSS_123_0Hz,
  CTCSS_127_3Hz, CTCSS_131_8Hz, CTCSS_136_5Hz, CTCSS_141_3Hz, CTCSS_146_2Hz, CTCSS_151_4Hz,
  CTCSS_156_7Hz,
  SIGNALING_NONE, // 159.8 not supported
  CTCSS_162_2Hz,
  SIGNALING_NONE, // 165.5 not supported
  CTCSS_167_9Hz,
  SIGNALING_NONE, // 171.3 not supported
  CTCSS_173_8Hz,
  SIGNALING_NONE, // 177.3 not supported
  CTCSS_179_9Hz,
  SIGNALING_NONE, // 183.5 not supported
  CTCSS_186_2Hz,
  SIGNALING_NONE, // 189.9 not supported
  CTCSS_192_8Hz,
  SIGNALING_NONE, SIGNALING_NONE, // 196.6 & 199.5 not supported
  CTCSS_203_5Hz,
  SIGNALING_NONE, // 206.5 not supported
  CTCSS_210_7Hz, CTCSS_218_1Hz, CTCSS_225_7Hz,
  SIGNALING_NONE, // 229.1 not supported
  CTCSS_233_6Hz, CTCSS_241_8Hz, CTCSS_250_3Hz,
  SIGNALING_NONE, SIGNALING_NONE // 254.1 and custom CTCSS not supported.
};

uint8_t ctcss_code2num(Signaling::Code code) {
  for (uint8_t i=0; i<52; i++) {
    if (code == ctcss_num2code[i])
      return i;
  }
  return 0;
}

/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug::channel_t
 * ******************************************************************************************** */
D878UVCodeplug::channel_t::channel_t() {
  clear();
}

void
D878UVCodeplug::channel_t::clear() {
  memset(this, 0xff, sizeof(D878UVCodeplug::channel_t));
}

bool
D878UVCodeplug::channel_t::isValid() const {
  return (0 != name[0]) && (0xff != name[0]);
}

double
D878UVCodeplug::channel_t::getRXFrequency() const {
  return decode_frequency(qFromBigEndian(rx_frequency));
}

void
D878UVCodeplug::channel_t::setRXFrequency(double f) {
  rx_frequency = qToBigEndian(encode_frequency(f));
}

double
D878UVCodeplug::channel_t::getTXFrequency() const {
  double f = decode_frequency(qFromBigEndian(rx_frequency));
  switch ((RepeaterMode) repeater_mode) {
  case RM_SIMPLEX:
    break;
  case RM_TXNEG:
    f -= decode_frequency(qFromBigEndian(tx_offset));
    break;
  case RM_TXPOS:
    f += decode_frequency(qFromBigEndian(tx_offset));
    break;
  }
  return f;
}

void
D878UVCodeplug::channel_t::setTXFrequency(double f) {
  if (getRXFrequency() == f) {
    tx_offset = encode_frequency(0);
    repeater_mode = RM_SIMPLEX;
  } else if (getRXFrequency() > f) {
    tx_offset = qToBigEndian(encode_frequency(getRXFrequency()-f));
    repeater_mode = RM_TXNEG;
  } else {
    tx_offset = qToBigEndian(encode_frequency(f-getRXFrequency()));
    repeater_mode = RM_TXPOS;
  }
}

QString
D878UVCodeplug::channel_t::getName() const {
  return decode_ascii(name, 16, 0);
}

void
D878UVCodeplug::channel_t::setName(const QString &name) {
  encode_ascii(this->name, name, 16, 0);
}

Signaling::Code
D878UVCodeplug::channel_t::getRXTone() const {
  // If squelch is not SQ_TONE -> RX tone is ignored
  if (SQ_TONE != squelch_mode)
    return Signaling::SIGNALING_NONE;

  if (rx_ctcss && (ctcss_receive < 52))
    return ctcss_num2code[ctcss_receive];
  else if (rx_dcs && (qFromBigEndian(dcs_receive) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromBigEndian(dcs_receive)), false);
  else if (rx_dcs && (qFromBigEndian(dcs_receive) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromBigEndian(dcs_receive)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D878UVCodeplug::channel_t::setRXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    squelch_mode = SQ_CARRIER;
    rx_ctcss = rx_dcs = 0;
    ctcss_receive = dcs_receive = 0;
  } else if (Signaling::isCTCSS(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 1;
    rx_dcs = 0;
    ctcss_receive = ctcss_code2num(code);
    dcs_receive = 0;
  } else if (Signaling::isDCSNormal(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToBigEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    squelch_mode = SQ_TONE;
    rx_ctcss = 0;
    rx_dcs = 1;
    ctcss_receive = 0;
    dcs_receive = qToBigEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Signaling::Code
D878UVCodeplug::channel_t::getTXTone() const {
  if (tx_ctcss && (ctcss_transmit < 52))
    return ctcss_num2code[ctcss_transmit];
  else if (tx_dcs && (qFromBigEndian(dcs_transmit) < 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromBigEndian(dcs_transmit)), false);
  else if (tx_dcs && (qFromBigEndian(dcs_transmit) >= 512))
    return Signaling::fromDCSNumber(dec_to_oct(qFromBigEndian(dcs_transmit)-512), true);
 return Signaling::SIGNALING_NONE;
}

void
D878UVCodeplug::channel_t::setTXTone(Code code) {
  if (Signaling::SIGNALING_NONE == code) {
    tx_ctcss = tx_dcs = 0;
    ctcss_transmit = dcs_transmit = 0;
  } else if (Signaling::isCTCSS(code)) {
    tx_ctcss = 1;
    tx_dcs = 0;
    ctcss_transmit = ctcss_code2num(code);
    dcs_transmit = 0;
  } else if (Signaling::isDCSNormal(code)) {
    tx_ctcss = 0;
    tx_dcs = 1;
    ctcss_transmit = 0;
    dcs_transmit = qToBigEndian(oct_to_dec(Signaling::toDCSNumber(code)));
  } else if (Signaling::isDCSInverted(code)) {
    tx_ctcss = 0;
    tx_dcs = 1;
    ctcss_transmit = 0;
    dcs_transmit = qToBigEndian(oct_to_dec(Signaling::toDCSNumber(code))+512);
  }
}

Channel *
D878UVCodeplug::channel_t::toChannelObj() const {
  Channel::Power power = Channel::LowPower;
  switch ((channel_t::Power) this->power) {
  case POWER_LOW:
  case POWER_MIDDLE:
    power = Channel::LowPower;
    break;
  case POWER_HIGH:
  case POWER_TURBO:
    power = Channel::HighPower;
    break;
  }
  bool rxOnly = (1 == this->rx_only);

  Channel *ch;
  if (MODE_ANALOG == channel_mode) {
    AnalogChannel::Admit admit = AnalogChannel::AdmitNone;
    switch ((channel_t::Admit) tx_permit) {
    case ADMIT_ALWAYS:
      admit = AnalogChannel::AdmitNone;
      break;
    case ADMIT_CH_FREE:
      admit = AnalogChannel::AdmitFree;
      break;
    default:
      break;
    }
    AnalogChannel::Bandwidth bw = AnalogChannel::BWNarrow;
    if (BW_12_5_KHZ == bandwidth)
      bw = AnalogChannel::BWNarrow;
    else
      bw = AnalogChannel::BWWide;
    ch = new AnalogChannel(
          getName(), getRXFrequency(), getTXFrequency(), power, 0.0, rxOnly, admit,
          1, getRXTone(), getTXTone(), bw, nullptr);
  } else if (MODE_DIGITAL == channel_mode) {
    DigitalChannel::Admit admit = DigitalChannel::AdmitNone;
    switch ((channel_t::Admit) tx_permit) {
    case ADMIT_ALWAYS:
      admit = DigitalChannel::AdmitNone;
      break;
    case ADMIT_CH_FREE:
      admit = DigitalChannel::AdmitFree;
      break;
    case ADMIT_CC_SAME:
    case ADMIT_CC_DIFF:
      admit = DigitalChannel::AdmitColorCode;
      break;
    }
    DigitalChannel::TimeSlot ts = (slot2 ? DigitalChannel::TimeSlot2 : DigitalChannel::TimeSlot1);
    ch = new DigitalChannel(
          getName(), getRXFrequency(), getTXFrequency(), power, 0.0, rxOnly, admit,
          color_code, ts, nullptr, nullptr, nullptr, nullptr);
  } else {
    logError() << "Cannot create channel '" << getName()
               << "': Mixed channel types not supported.";
    return nullptr;
  }

  return ch;
}


void
D878UVCodeplug::channel_t::fromChannelObj(const Channel *c, const Config *conf) {
  // Pack common channel config
  // set channel name
  setName(c->name());
  // set rx and tx frequencies
  setRXFrequency(c->rxFrequency());
  setTXFrequency(c->txFrequency());
  // set power
  power = (Channel::LowPower == c->power()) ? POWER_LOW : POWER_HIGH;
  // set tx-enable
  rx_only = c->rxOnly() ? 1 : 0;
  // Link scan list if set
  if (nullptr == c->scanList())
    scan_list_index = 0xff;
  else
    scan_list_index = conf->scanlists()->indexOf(c->scanList());
  // Dispatch by channel type
  if (const AnalogChannel *ac = c->as<AnalogChannel>()) {
    // pack analog channel config
    // set admit criterion
    switch (ac->admit()) {
    case AnalogChannel::AdmitNone: tx_permit = ADMIT_ALWAYS; break;
    case AnalogChannel::AdmitFree: tx_permit = ADMIT_CH_FREE; break;
    case AnalogChannel::AdmitTone: tx_permit = ADMIT_ALWAYS; break;
    }
    // squelch mode
    squelch_mode = (Signaling::SIGNALING_NONE == ac->rxTone()) ? SQ_CARRIER : SQ_TONE;
    setRXTone(ac->rxTone());
    setTXTone(ac->txTone());
    // set bandwidth
    bandwidth = (AnalogChannel::BWNarrow == ac->bandwidth()) ? BW_12_5_KHZ : BW_25_KHZ;
  } else if (const DigitalChannel *dc = c->as<DigitalChannel>()) {
    // pack digital channel config.
    // set admit criterion
    switch(dc->admit()) {
    case DigitalChannel::AdmitNone: tx_permit = ADMIT_ALWAYS; break;
    case DigitalChannel::AdmitFree: tx_permit = ADMIT_CH_FREE; break;
    case DigitalChannel::AdmitColorCode: tx_permit = ADMIT_CC_SAME; break;
    }
    // set color code
    color_code = dc->colorCode();
    // set time-slot
    slot2 = (DigitalChannel::TimeSlot2 == dc->timeslot()) ? 1 : 0;
    // link transmit contact
    if (nullptr == dc->txContact())
      contact_index = 0xffff;
    else
      contact_index = conf->contacts()->indexOfDigital(dc->txContact());
    // link RX group list
    if (nullptr == dc->rxGroupList())
      group_list_index = 0xff;
    else
      group_list_index = conf->rxGroupLists()->indexOf(dc->rxGroupList());
  }
}


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Anytone AT-D878UV Codeplug");

  // Define channel banks
  uint32_t addr=ADDR_CHANNEL_BANK_0;
  for (uint32_t i=0; i<(NUM_CHANNEL_BANKS-1); i++, addr+=ADDR_CHANNEL_BANK_OFFSET)
    image(0).addElement(addr, ADDR_CHANNEL_BANK_SIZE);
  // last channel bank (idx 31) is smaller (contains channels 3969-4000 and VFO A, VFO B)
  image(0).addElement(ADDR_CHANNEL_BANK_31, ADDR_CHANNEL_BANK_31_SIZE);
  // Channel bitmap
  image(0).addElement(ADDR_CHANNEL_BITMAP, ADDR_CHANNEL_BITMAP_SIZE);

  // Clear entire codeplug
  clear();
}

void
D878UVCodeplug::clear() {
  for (int i=0; i<image(0).numElements(); i++) {
    memset(image(0).element(i).data().data(), 0, image(0).element(i).size());
  }
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    uint16_t bank = i/128;
    channel_t *ch = (channel_t *)data(ADDR_CHANNEL_BANK_0+bank*ADDR_CHANNEL_BANK_OFFSET);
    ch->clear();
  }
}

bool
D878UVCodeplug::encode(Config *config) {
  /// @bug Implement D878UV code-plug encoding.
  return true;
}

bool
D878UVCodeplug::decode(Config *config) {
  // Maps code-plug indices to objects
  CodeplugContext ctx(config);

  // Create channels
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(ADDR_CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    channel_t *ch = (channel_t *)data(ADDR_CHANNEL_BANK_0
                                      +bank*ADDR_CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));
    if (Channel *obj = ch->toChannelObj())
      ctx.addChannel(obj, i);
  }


  // Link channel objects
  for (uint16_t i=0; i<NUM_CHANNELS; i++) {
    // Check if channel is enabled:
    uint16_t  bit = i%8, byte = i/8, bank = i/128, idx = i%128;
    if (0 == (((*data(ADDR_CHANNEL_BITMAP+byte))>>bit) & 0x01))
      continue;
    channel_t *ch = (channel_t *)data(ADDR_CHANNEL_BANK_0
                                      +bank*ADDR_CHANNEL_BANK_OFFSET
                                      +idx*sizeof(channel_t));
    //if (ctx.hasChannel(i))
    //  ch->linkChannelObj(ctx.getChannel(i), ctx);
  }

  /// @bug Implement D878UV code-plug decoding.
  return true;
}
