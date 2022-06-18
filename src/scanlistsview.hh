#ifndef SCANLISTSVIEW_HH
#define SCANLISTSVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class ScanListsView;
}

class ScanListsView : public QWidget
{
  Q_OBJECT

public:
  explicit ScanListsView(Config *config, QWidget *parent = nullptr);
  ~ScanListsView();

protected slots:
  void onAddScanList();
  void onRemScanList();
  void onEditScanList(unsigned row);

private:
  Ui::ScanListsView *ui;
  Config *_config;
};

#endif // SCANLISTSVIEW_HH
