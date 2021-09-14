#include "anytone_codeplug.hh"

AnytoneCodeplug::AnytoneCodeplug(QObject *parent)
  : CodePlug(parent)
{
  // pass...
}

AnytoneCodeplug::~AnytoneCodeplug() {
  // pass...
}

bool
AnytoneCodeplug::index(Config *config, Context &ctx) const {
  return true;
}


