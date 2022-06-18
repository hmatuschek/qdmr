#ifndef CHANNELLISTVIEW_HH
#define CHANNELLISTVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class ChannelListView;
}

class ChannelListView : public QWidget
{
  Q_OBJECT

public:
  explicit ChannelListView(Config *config, QWidget *parent = nullptr);
  ~ChannelListView();

protected slots:
  void onAddAnalogChannel();
  void onAddDigitalChannel();
  void onCloneChannel();
  void onRemChannel();
  void onEditChannel(unsigned row);
  void loadChannelListSectionState();
  void storeChannelListSectionState();



private:
  Ui::ChannelListView *ui;
  Config *_config;
};

#endif // CHANNELLISTVIEW_HH
