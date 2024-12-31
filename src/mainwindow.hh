#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

class Config;
class GeneralSettingsView;
class RadioIDListView;
class RoamingZoneListView;
class ExtensionView;
namespace Ui {
  class MainWindow;
}


class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(Config *config, QWidget *parent=nullptr);

public slots:
  void applySettings();

protected:
  void closeEvent(QCloseEvent *event);

private:
  Ui::MainWindow *ui;
  GeneralSettingsView *_generalSettings;
  RadioIDListView *_radioIdTab;
  RoamingZoneListView *_roamingZoneList;
  ExtensionView *_extensionView;
};

#endif // MAINWINDOW_HH
