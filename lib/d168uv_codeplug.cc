#include "d168uv_codeplug.hh"
#include "config.h"

#include <QTimeZone>
#include <QtEndian>


/* ******************************************************************************************** *
 * Implementation of D168UVCodeplug
 * ******************************************************************************************** */
D168UVCodeplug::D168UVCodeplug(const QString &label, QObject *parent)
  : D878UVCodeplug(label, parent)
{
  // pass...
}

D168UVCodeplug::D168UVCodeplug(QObject *parent)
  : D878UVCodeplug("AnyTone AT-D168UV Codeplug", parent)
{
  // pass...
}

