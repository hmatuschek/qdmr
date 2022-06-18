#ifndef RADIOIDLISTVIEW_HH
#define RADIOIDLISTVIEW_HH

#include <QWidget>

class Config;

namespace Ui {
  class RadioIDListView;
}


class RadioIDListView : public QWidget
{
  Q_OBJECT

public:
  explicit RadioIDListView(Config *config, QWidget *parent = nullptr);
  ~RadioIDListView();

protected slots:
  void onConfigModified();
  void onAddID();
  void onDeleteID();
  void onEditID(unsigned row);
  void onDefaultIDSelected(int idx);
  void loadHeaderState();
  void storeHeaderState();

private:
  Ui::RadioIDListView *ui;
  Config *_config;
};

#endif // RADIOIDLISTVIEW_HH
