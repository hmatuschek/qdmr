#ifndef ROAMINGZONELISTVIEW_HH
#define ROAMINGZONELISTVIEW_HH

#include <QWidget>

class Config;

namespace Ui {
  class RoamingZoneListView;
}



class RoamingZoneListView : public QWidget
{
  Q_OBJECT

public:
  explicit RoamingZoneListView(Config *_config, QWidget *parent = nullptr);
  ~RoamingZoneListView();

protected slots:
  void onAddRoamingZone();
  void onGenRoamingZone();
  void onRemRoamingZone();
  void onEditRoamingZone(unsigned row);
  void onHideRoamingNote();

private:
  Ui::RoamingZoneListView *ui;
  Config *_config;
};

#endif // ROAMINGZONELISTVIEW_HH
