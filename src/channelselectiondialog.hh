#ifndef CHANNELSELECTIONDIALOG_HH
#define CHANNELSELECTIONDIALOG_HH

#include <QDialog>

class Channel;
class ChannelList;
class ChannelComboBox;

class ChannelSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  ChannelSelectionDialog(ChannelList *lst, bool includeSelectedChannel=false, QWidget *parent=nullptr);

  Channel *channel() const;

protected:
  ChannelComboBox *_channel;
};


#endif // CHANNELSELECTIONDIALOG_HH
