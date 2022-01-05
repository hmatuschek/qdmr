#include "cdr300uv_codeplug.hh"
#define CODEPLUG_SIZE 0x01c000


CDR300UVCodeplug::CDR300UVCodeplug(QObject *parent)
  : KyderaCodeplug(parent)
{
  // The entire codeplug is a single blob
  image(0).addElement(0x00000000, CODEPLUG_SIZE);
}

