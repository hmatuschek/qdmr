#ifndef CHANNELSELECTIONDIALOG_HH
#define CHANNELSELECTIONDIALOG_HH

#include <QDialog>

class Channel;
class ChannelList;
class ChannelComboBox;
class QListWidget;

class ChannelSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  ChannelSelectionDialog(ChannelList *lst, bool includeSelectedChannel=false, QWidget *parent=nullptr);

  Channel *channel() const;

protected:
  ChannelComboBox *_channel;
};


class MultiChannelSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  MultiChannelSelectionDialog(ChannelList *lst, bool includeSelectedChannel=false, QWidget *parent=nullptr);

  QList<Channel *> channel() const;

protected:
  QListWidget *_channel;
};


#endif // CHANNELSELECTIONDIALOG_HH
