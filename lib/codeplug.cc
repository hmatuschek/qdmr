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
  // Search for element that contains address
  for  (int i=0; i<image(0).numElements(); i++) {
    if ((offset >= image(0).element(i).address()) &&
        (offset < (image(0).element(i).address()+image(0).element(i).data().size())))
    {
      return (unsigned char *)(image(0).element(i).data().data()+
                               (offset-image(0).element(i).address()));
    }
  }
  return nullptr;
}

const unsigned char *
CodePlug::data(uint32_t offset) const {
  // Search for element that contains address
  for  (int i=0; i<image(0).numElements(); i++) {
    if ((offset >= image(0).element(i).address()) &&
        (offset < (image(0).element(i).address()+image(0).element(i).data().size())))
    {
      return (const unsigned char *)(image(0).element(i).data().data()+
                                     (offset-image(0).element(i).address()));
    }
  }
  return nullptr;
}
