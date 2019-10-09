#include "channelvalidator.hh"
#include "channel.hh"


/* ********************************************************************************************* *
 * Implementation of ChannelValidator
 * ********************************************************************************************* */
ChannelValidator::ChannelValidator(ChannelList *lst, QObject *parent)
  : QValidator(parent), _channels(lst)
{
  // pass...
}

QValidator::State
ChannelValidator::validate(QString &text, int &pos) const {
  Q_UNUSED(pos);
  if (text.size()<1)
    return QValidator::Intermediate;

  QValidator::State res = QValidator::Invalid;
  for (int i=0; i<_channels->count(); i++) {
    if (0 == text.compare(_channels->channel(i)->name(), Qt::CaseInsensitive))
      return QValidator::Acceptable;
    if (_channels->channel(i)->name().startsWith(text, Qt::CaseInsensitive))
      res = QValidator::Intermediate;
  }
  return res;
}


