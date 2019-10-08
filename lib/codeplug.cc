#include "codeplug.hh"
#include "config.hh"


CodePlug::CodePlug(QObject *parent)
    : QObject(parent)
{
	// pass...
}

CodePlug::~CodePlug() {
	// pass...
}

unsigned char *
CodePlug::data() {
	return _radio_mem;
}
