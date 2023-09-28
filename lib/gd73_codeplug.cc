#include "gd73_codeplug.hh"

GD73Codeplug::GD73Codeplug(QObject *parent)
  : Codeplug{parent}
{
  addImage("Radioddity GD-73A/E codeplug");
  image(0).addElement(0x000000, 0x22014);
}

bool
GD73Codeplug::index(Config *config, Context &ctx, const ErrorStack &err) const {
  return false;
}

bool
GD73Codeplug::encode(Config *config, const Flags &flags, const ErrorStack &err) {
  return false;
}

bool
GD73Codeplug::decode(Config *config, const ErrorStack &err) {
  return false;
}
