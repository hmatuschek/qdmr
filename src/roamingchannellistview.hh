#ifndef ROAMINGCHANNELLISTVIEW_HH
#define ROAMINGCHANNELLISTVIEW_HH

#include <QWidget>

// Forward declarations
namespace Ui {
  class RoamingChannelListView;
}
class Config;


class RoamingChannelListView : public QWidget
{
  Q_OBJECT

public:
  explicit RoamingChannelListView(Config *config, QWidget *parent = nullptr);
  ~RoamingChannelListView();

protected slots:
  void onAddChannel();
  void onEditChannel(unsigned int idx);
  void onRemChannel();
  void onHideRoamingNote();

protected:
  /** Holds a weak reference to the abstract config. */
  Config *_config;

private:
  Ui::RoamingChannelListView *ui;
};

#endif // ROAMINGCHANNELLISTVIEW_HH
