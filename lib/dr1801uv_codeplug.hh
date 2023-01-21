#ifndef DR1801UVCODEPLUG_HH
#define DR1801UVCODEPLUG_HH

#include "codeplug.hh"

class DR1801UVCodeplug : public Codeplug
{
  Q_OBJECT

public:
  explicit DR1801UVCodeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err) const;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err);
  bool decode(Config *config, const ErrorStack &err);
};

#endif // DR1801UVCODEPLUG_HH
