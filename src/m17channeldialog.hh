#ifndef M17CHANNELDIALOG_HH
#define M17CHANNELDIALOG_HH

#include <QDialog>
class Config;
class M17Channel;

namespace Ui {
  class M17ChannelDialog;
}

class M17ChannelDialog : public QDialog
{
  Q_OBJECT

public:
  M17ChannelDialog(Config *config, QWidget *parent = nullptr);
  M17ChannelDialog(Config *config, M17Channel *channel, QWidget *parent = nullptr);
  ~M17ChannelDialog();

  M17Channel *channel();

protected:
  void construct();

private:
  Ui::M17ChannelDialog *ui;
  Config *_config;
  M17Channel *_channel;
  M17Channel *_myChannel;
};

#endif // M17CHANNELDIALOG_HH
