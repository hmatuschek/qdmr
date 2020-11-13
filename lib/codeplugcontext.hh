#ifndef CODEPLUGCONTEXT_HH
#define CODEPLUGCONTEXT_HH

#include <QHash>
#include "config.hh"

class CodeplugContext
{
public:
  CodeplugContext(Config *config);

  Config *config() const;

  bool hasChannel(int index) const;
  bool addChannel(Channel *ch, int index);
  Channel *getChannel(int index) const;

protected:
  Config *_config;
  QHash<int, int> _channelTable;
};

#endif // CODEPLUGCONTEXT_HH
