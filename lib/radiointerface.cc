#include "radiointerface.hh"

RadioInferface::RadioInferface(QObject *parent)
    : QObject(parent)
{
	// pass...
}

RadioInferface::~RadioInferface() {
  // pass...
}

bool
RadioInferface::reboot() {
  return true;
}


