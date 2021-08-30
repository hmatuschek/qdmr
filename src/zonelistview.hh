#ifndef ZONELISTVIEW_HH
#define ZONELISTVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class ZoneListView;
}

class ZoneListView : public QWidget
{
  Q_OBJECT

public:
  explicit ZoneListView(Config *config, QWidget *parent = nullptr);
  ~ZoneListView();

protected slots:
  void onAddZone();
  void onRemZone();
  void onEditZone(uint row);

private:
  Ui::ZoneListView *ui;
  Config *_config;
};

#endif // ZONELISTVIEW_HH
