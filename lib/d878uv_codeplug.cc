#include "d878uv_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"
#include <QTimeZone>


/* ******************************************************************************************** *
 * Implementation of D878UVCodeplug
 * ******************************************************************************************** */
D878UVCodeplug::D878UVCodeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Anytone AT-D878UV Codeplug");
  // Clear entire codeplug
  clear();
}

void
D878UVCodeplug::clear() {
}

bool
D878UVCodeplug::encode(Config *config) {
}

bool
D878UVCodeplug::decode(Config *config) {
}
