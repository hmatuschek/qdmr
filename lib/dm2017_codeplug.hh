#ifndef DM2017CODEPLUG_HH
#define DM2017CODEPLUG_HH

#include "tyt_codeplug.hh"

class MD2017Codeplug : public TyTCodeplug
{
  Q_OBJECT

public:
  explicit MD2017Codeplug(QObject *parent = nullptr);
  virtual ~MD2017Codeplug();

public:
  void clearChannels();
  bool encodeChannels(Config *config, const Flags &flags);
  bool createChannels(Config *config, CodeplugContext &ctx);
  bool linkChannels(Config *config, CodeplugContext &ctx);
};

#endif // DM2017CODEPLUG_HH
