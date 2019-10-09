#ifndef CHANNELVALIDATOR_HH
#define CHANNELVALIDATOR_HH

#include <QValidator>

class ChannelList;

class ChannelValidator: public QValidator
{
  Q_OBJECT

public:
  ChannelValidator(ChannelList *lst, QObject *parent=nullptr);

  QValidator::State validate(QString &input, int &pos) const;

protected:
  ChannelList *_channels;
};



#endif // CHANNELVALIDATOR_HH
