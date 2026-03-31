#ifndef FMCHANNELDIALOG_H
#define FMCHANNELDIALOG_H

#include "channeldialog.hh"
#include "config.hh"

class ChannelSquelchEdit;
class FMAdmitSelect;
class SelectiveCallBox;
class BandwidthSelect;
class FMAPRSSelect;


class FMChannelDialog: public ChannelDialog
{
  Q_OBJECT

public:
  FMChannelDialog(Config *config, QWidget *parent=nullptr);

  void setChannel(FMChannel *fm);

public slots:
  void accept() override;

protected slots:
  void onRepeaterSelected(const QModelIndex &index);
  void onHideChannelHint();

protected:
  QPointer<FMChannel> _channel;
  ChannelSquelchEdit *_squelch;
  FMAdmitSelect   *_admit;
  SelectiveCallBox *_rxTone, *_txTone;
  BandwidthSelect *_bandwidth;
  FMAPRSSelect *_aprs;
};


#endif // FMCHANNELDIALOG_H
