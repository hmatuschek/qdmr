#include "d890uv_codeplug.hh"


/* *********************************************************************************************
 * Implementation of D890UVCodeplug
 * ********************************************************************************************* */
D890UVCodeplug::D890UVCodeplug(const QString &label, QObject *parent)
  : AnytoneCodeplug(label, parent)
{
  // pass...
}

D890UVCodeplug::D890UVCodeplug(QObject *parent)
  : AnytoneCodeplug{"AnyTone AT-D890UV Codeplug", parent}
{
  // pass...
}

