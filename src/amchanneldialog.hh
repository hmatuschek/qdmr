#ifndef AMCHANNELDIALOG_HH
#define AMCHANNELDIALOG_HH

#include <QDialog>

namespace Ui {
class AMChannelDialog;
}
class Config;
class AMChannel;


class AMChannelDialog : public QDialog
{
  Q_OBJECT

public:
  AMChannelDialog(Config *config, QWidget *parent=nullptr);
  AMChannelDialog(Config *config, AMChannel *channel, QWidget *parent=nullptr);
  ~AMChannelDialog();

  AMChannel *channel();

protected:
  void construct();

protected slots:
  void onPowerDefaultToggled(bool checked);
  void onTimeoutDefaultToggled(bool checked);
  void onSquelchDefaultToggled(bool checked);
  void onVOXDefaultToggled(bool checked);

private slots:
  void onRxFrequencyEdited();
  void onTxFrequencyEdited();
  void onOffsetFrequencyEdited();
  void onOffsetDirectionChanged(int index);
  void updateOffsetFrequency();

private:
  void updateComboBox();

protected:
  Config *_config;
  AMChannel *_myChannel;
  AMChannel *_channel;

private:
  Ui::AMChannelDialog *ui;
};

#endif // AMCHANNELDIALOG_HH
