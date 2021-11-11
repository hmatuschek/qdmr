#ifndef DIGITALCHANNELDIALOG_HH
#define DIGITALCHANNELDIALOG_HH

#include <QDialog>
#include "config.hh"

#include "ui_digitalchanneldialog.h"

class DigitalChannelDialog: public QDialog, private Ui::DigitalChannelDialog
{
  Q_OBJECT

public:
  DigitalChannelDialog(Config *config, QWidget *parent=nullptr);
  DigitalChannelDialog(Config *config, DigitalChannel *channel, QWidget *parent=nullptr);

  DigitalChannel *channel();

protected slots:
  void onRepeaterSelected(const QModelIndex &index);
  void onPowerDefaultToggled(bool checked);
  void onTimeoutDefaultToggled(bool checked);
  void onVOXDefaultToggled(bool checked);

protected:
  void construct();

protected:
  Config *_config;
  DigitalChannel *_myChannel;
  DigitalChannel *_channel;
};




#endif // DIGITALCHANNELDIALOG_HH
