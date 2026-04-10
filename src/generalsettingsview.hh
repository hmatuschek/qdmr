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
  void onIntroLine1Edited();
  void onIntroLine2Edited();
  void onMicLevelChanged();
  void onSpeechChanged();
  void onPowerChanged();
  void onSquelchChanged();
  void onTOTChanged();
  void onVOXChanged();

private:
  Ui::GeneralSettingsView *ui;
  Config *_config;
};

#endif // GENERALSETTINGSVIEW_HH
