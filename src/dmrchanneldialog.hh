#ifndef DMRCHANNELDIALOG_H
#define DMRCHANNELDIALOG_H

#include <QDialog>
#include "config.hh"

#include "ui_dmrchanneldialog.h"

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
  void onHideChannelHint();

private slots:
  void onRxFrequencyEdited();
  void onTxFrequencyEdited();
  void onOffsetFrequencyEdited();
  void onOffsetCurrentIndexChanged(int index);
  void updateOffsetFrequency();

protected:
  void construct();

private:
  void updateComboBox();

protected:
  Config *_config;
  DMRChannel *_myChannel;
  DMRChannel *_channel;
};

#endif // DMRCHANNELDIALOG_H
