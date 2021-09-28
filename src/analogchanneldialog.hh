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
  AnalogChannelDialog(Config *config, AnalogChannel *channel, QWidget *parent=nullptr);

  AnalogChannel *channel();

protected:
  void construct();

protected slots:
  void onRepeaterSelected(const QModelIndex &index);
  void onPowerDefaultToggled(bool checked);
  void onTimeoutDefaultToggled(bool checked);
  void onSquelchDefaultToggled(bool checked);
  void onVOXDefaultToggled(bool checked);

protected:
  Config *_config;
  AnalogChannel *_channel;
};


#endif // ANALOGCHANNELDIALOG_HH
