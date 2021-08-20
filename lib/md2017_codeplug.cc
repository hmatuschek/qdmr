#include "md2017_codeplug.hh"
#include "codeplugcontext.hh"
#include "logger.hh"

#define NUM_CHANNELS         3000
#define ADDR_CHANNELS    0x110000
#define CHANNEL_SIZE     0x000040

#define NUM_CONTACTS        10000
#define ADDR_CONTACTS    0x140000
#define CONTACT_SIZE     0x000024

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
MD2017Codeplug::createChannels(CodeplugContext &ctx) {
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
MD2017Codeplug::linkChannels(CodeplugContext &ctx) {
  for (int i=0; i<NUM_CHANNELS; i++) {
    ChannelElement chan(data(ADDR_CHANNELS+i*CHANNEL_SIZE));
    if (! chan.isValid())
      break;
    if (! chan.linkChannelObj(ctx.getChannel(i+1), ctx)) {
      _errorMessage = QString("Cannot decode TyT codeplug: Cannot link channel at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}

void
MD2017Codeplug::clearContacts() {
  // Clear contacts
  for (int i=0; i<NUM_CONTACTS; i++)
    ContactElement(data(ADDR_CONTACTS+i*CONTACT_SIZE)).clear();
}

bool
MD2017Codeplug::encodeContacts(Config *config, const Flags &flags) {
  // Encode contacts
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (i < config->contacts()->digitalCount())
      cont.fromContactObj(config->contacts()->digitalContact(i));
    else
      cont.clear();
  }
  return true;
}

bool
MD2017Codeplug::createContacts(CodeplugContext &ctx) {
  for (int i=0; i<NUM_CONTACTS; i++) {
    ContactElement cont(data(ADDR_CONTACTS+i*CONTACT_SIZE));
    if (! cont.isValid())
      break;
    if (DigitalContact *obj = cont.toContactObj())
      ctx.addDigitalContact(obj, i+1);
    else {
      _errorMessage = QString("Cannot decode TyT codeplug: Invlaid contact at index %1.").arg(i);
      logError() << _errorMessage;
      return false;
    }
  }
  return true;
}
