#include "dr1801uv_codeplug.hh"

#define TOTAL_SIZE 0x0001dd90

DR1801UVCodeplug::DR1801UVCodeplug(QObject *parent)
  : Codeplug(parent)
{
  addImage("BTECH DR-1801UV Codeplug");
  image(0).addElement(0, TOTAL_SIZE);
}

bool
DR1801UVCodeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}

bool
DR1801UVCodeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
}

bool
DR1801UVCodeplug::decode(Config *config, const ErrorStack &err) {
  return false;
}


