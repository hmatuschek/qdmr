#include "codeplug.hh"
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of CodePlug::Flags
 * ********************************************************************************************* */
CodePlug::Flags::Flags()
  : updateCodePlug(true), autoEnableGPS(false), autoEnableRoaming(false)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodePlug
 * ********************************************************************************************* */
CodePlug::CodePlug(QObject *parent)
  : DFUFile(parent)
{
	// pass...
}

CodePlug::~CodePlug() {
	// pass...
}
