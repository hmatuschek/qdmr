#include "kydera_codeplug.hh"

KyderaCodeplug::KyderaCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // The codeplug consists of a single element.
  addImage("Kydera CDR-300UV codeplug");
}

bool
KyderaCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}


