#include "dm2017_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"

#define NUM_CHANNELS         3000
#define ADDR_CHANNELS    0x110000
#define CHANNEL_SIZE     0x000030


MD2017Codeplug::MD2017Codeplug(QObject *parent)
  : TyTCodeplug(parent)
{
  addImage("TYT MD-UV390 Codeplug");
  image(0).addElement(0x002000, 0x3e000);
  image(0).addElement(0x110000, 0x90000);

  // Clear entire codeplug
  clear();
}

MD2017Codeplug::~MD2017Codeplug() {
  // pass...
}


void
MD2017Codeplug::clearChannels() {
  // Clear channels
  for (int i=0; i<NUM_CHANNELS; i++)
    ChannelElement(data(ADDR_CHANNELS+i*CHANNEL_SIZE)).clear();
}

bool
MD2017Codeplug::encodeChannels(Config *config, const Flags &flags) {
  CodeplugContext ctx(config);

  // Define Channels
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (i < config->channelList()->count()) {
      chan.fromChannelObj(config->channelList()->channel(i), ctx);
    } else {
      chan.clear();
    }
  }
  return true;
}

bool
MD2017Codeplug::createChannels(Config *config, CodeplugContext &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (Channel *obj = chan.toChannelObj())
      ctx.addChannel(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode codeplug: Invlaid channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

bool
MD2017Codeplug::linkChannels(Config *config, CodeplugContext &ctx) {
  for (int i=0; i<config->channelList()->count(); i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.linkChannelObj(config->channelList()->channel(i), config)) {
      _errorMessage = QString("Cannot decode codeplug: Cannot link channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}
