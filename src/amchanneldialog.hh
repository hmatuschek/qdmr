#ifndef AMCHANNELDIALOG_HH
#define AMCHANNELDIALOG_HH

#include "channeldialog.hh"

class Config;
class AMChannel;
class ChannelSquelchEdit;


class AMChannelDialog : public ChannelDialog
{
  Q_OBJECT

public:
  AMChannelDialog(Config *config, QWidget *parent=nullptr);

  void setChannel(AMChannel *am);

public slots:
  void accept() override;

protected:
  Config *_config;
  QPointer<AMChannel> _channel;
  ChannelSquelchEdit *_squelch;
};

#endif // AMCHANNELDIALOG_HH
