#ifndef CHANNELCOMBOBOX_HH
#define CHANNELCOMBOBOX_HH

#include <QComboBox>

class Channel;
class ChannelList;

class ChannelComboBox: public QComboBox
{
  Q_OBJECT

public:
  ChannelComboBox(ChannelList *lst, QWidget *parent=nullptr);

  Channel *channel() const;
};



#endif // CHANNELCOMBOBOX_HH
