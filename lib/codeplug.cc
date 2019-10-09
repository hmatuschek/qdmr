#include "codeplug.hh"
#include "config.hh"


CodePlug::CodePlug(QObject *parent)
  : DFUFile(parent)
{
	// pass...
}

CodePlug::~CodePlug() {
	// pass...
}

unsigned char *
CodePlug::data(uint32_t offset) {
	//return &_radio_mem[offset];
  return (unsigned char *)&image(0).element(0).data().data()[offset];
}

const unsigned char *
CodePlug::data(uint32_t offset) const {
  //return &_radio_mem[offset];
  return (unsigned char *)&image(0).element(0).data().data()[offset];
}
