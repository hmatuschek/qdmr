#ifndef POSITIONINGSYSTEMLISTVIEW_HH
#define POSITIONINGSYSTEMLISTVIEW_HH

#include <QWidget>

class Config;

namespace Ui {
  class PositioningSystemListView;
}


class PositioningSystemListView : public QWidget
{
  Q_OBJECT

public:
  explicit PositioningSystemListView(Config *config, QWidget *parent = nullptr);
  ~PositioningSystemListView();

protected slots:
  void onAddGPS();
  void onAddAPRS();
  void onRemGPS();
  void onEditGPS(unsigned);
  void onHideGPSNote();
  void loadPositioningSectionState();
  void storePositioningSectionState();

private:
  Ui::PositioningSystemListView *ui;
  Config *_config;
};

#endif // POSITIONINGSYSTEMLISTVIEW_HH
