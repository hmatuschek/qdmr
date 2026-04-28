#ifndef GENERALSETTINGSVIEW_HH
#define GENERALSETTINGSVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class GeneralSettingsView;
}


class GeneralSettingsView : public QWidget
{
  Q_OBJECT

public:
  explicit GeneralSettingsView(Config *config, QWidget *parent = nullptr);
  ~GeneralSettingsView();

protected slots:
  void onConfigModified();

private:
  Ui::GeneralSettingsView *ui;
  Config *_config;
};

#endif // GENERALSETTINGSVIEW_HH
