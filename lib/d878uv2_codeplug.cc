#include "d878uv2_codeplug.hh"
#include "config.hh"
#include "utils.hh"
#include "channel.hh"
#include "gpssystem.hh"
#include "userdatabase.hh"
#include "config.h"
#include "logger.hh"

#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of D878UV2Codeplug
 * ******************************************************************************************** */
D878UV2Codeplug::D878UV2Codeplug(QObject *parent)
  : D878UVCodeplug(parent)
{
}

