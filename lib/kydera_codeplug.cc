#include "kydera_codeplug.hh"

KyderaCodeplug::KyderaCodeplug(QObject *parent)
  : Codeplug(parent)
{
  // pass...
}

bool
KyderaCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}


