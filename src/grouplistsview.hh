#ifndef GROUPLISTSVIEW_HH
#define GROUPLISTSVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class GroupListsView;
}


class GroupListsView : public QWidget
{
  Q_OBJECT

public:
  explicit GroupListsView(Config *config, QWidget *parent = nullptr);
  ~GroupListsView();

protected slots:
  void onAddRxGroup();
  void onRemRxGroup();
  void onEditRxGroup(uint row);

private:
  Ui::GroupListsView *ui;
  Config *_config;
};

#endif // GROUPLISTSVIEW_HH
