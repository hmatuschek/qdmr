#ifndef ANALOGCHANNELDIALOG_HH
#define ANALOGCHANNELDIALOG_HH

#include <QDialog>
#include "config.hh"

#include "ui_analogchanneldialog.h"

class AnalogChannelDialog: public QDialog, private Ui::AnalogChannelDialog
{
  Q_OBJECT

public:
  AnalogChannelDialog(Config *config, QWidget *parent=nullptr);
  AnalogChannelDialog(Config *config, FMChannel *channel, QWidget *parent=nullptr);

  FMChannel *channel();

protected:
  void construct();

protected slots:
  void onRepeaterSelected(const QModelIndex &index);
  void onPowerDefaultToggled(bool checked);
  void onTimeoutDefaultToggled(bool checked);
  void onSquelchDefaultToggled(bool checked);
  void onVOXDefaultToggled(bool checked);
  void onHideChannelHint();

protected:
  Config *_config;
  FMChannel *_myChannel;
  FMChannel *_channel;
};


#endif // ANALOGCHANNELDIALOG_HH
