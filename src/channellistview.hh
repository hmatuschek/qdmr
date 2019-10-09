#ifndef CHANNELLISTVIEW_HH
#define CHANNELLISTVIEW_HH

#include "channel.hh"

#include <QWidget>
#include <QTableView>

class ChannelListView: public QWidget
{
	Q_OBJECT

public:
	ChannelListView(Config *config, QWidget *parent=nullptr);

protected slots:
  void onChannelUp();
  void onChannelDown();
  void onAddAnalogChannel();
  void onAddDigitalChannel();
  void onRemChannel();
  void onEditChannel(const QModelIndex &index);

protected:
	Config *_config;
	ChannelList *_list;
	QTableView *_view;
};


#endif // CHANNELLISTVIEW_HH
