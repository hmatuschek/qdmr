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
CodePlug::data(uint32_t offset, uint32_t img) {
  // Search for element that contains address
  for  (int i=0; i<image(img).numElements(); i++) {
    if ((offset >= image(img).element(i).address()) &&
        (offset < (image(img).element(i).address()+image(img).element(i).data().size())))
    {
      return (unsigned char *)(image(img).element(i).data().data()+
                               (offset-image(img).element(i).address()));
    }
  }
  return nullptr;
}

const unsigned char *
CodePlug::data(uint32_t offset, uint32_t img) const {
  // Search for element that contains address
  for  (int i=0; i<image(img).numElements(); i++) {
    if ((offset >= image(img).element(i).address()) &&
        (offset < (image(img).element(i).address()+image(img).element(i).data().size())))
    {
      return (const unsigned char *)(image(img).element(i).data().data()+
                                     (offset-image(img).element(i).address()));
    }
  }
  return nullptr;
}
