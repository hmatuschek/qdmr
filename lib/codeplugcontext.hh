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

  bool hasDigitalContact(int index) const;
  bool addDigitalContact(DigitalContact *con, int index);
  DigitalContact *getDigitalContact(int index) const;

protected:
  Config *_config;
  QHash<int, int> _channelTable;
  QHash<int, int> _digitalContactTable;
};

#endif // CODEPLUGCONTEXT_HH
