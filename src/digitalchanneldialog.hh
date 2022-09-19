#ifndef DIGITALCHANNELDIALOG_HH
#define DIGITALCHANNELDIALOG_HH

#include <QDialog>
#include "config.hh"

#include "ui_digitalchanneldialog.h"

class DMRChannelDialog: public QDialog, private Ui::DigitalChannelDialog
{
  Q_OBJECT

public:
  DMRChannelDialog(Config *config, QWidget *parent=nullptr);
  DMRChannelDialog(Config *config, DMRChannel *channel, QWidget *parent=nullptr);

  DMRChannel *channel();

protected slots:
  void onRepeaterSelected(const QModelIndex &index);
  void onPowerDefaultToggled(bool checked);
  void onTimeoutDefaultToggled(bool checked);
  void onVOXDefaultToggled(bool checked);

protected:
  void construct();

protected:
  Config *_config;
  DMRChannel *_myChannel;
  DMRChannel *_channel;
};




#endif // DIGITALCHANNELDIALOG_HH
