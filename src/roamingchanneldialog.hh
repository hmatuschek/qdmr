#ifndef ROAMINGCHANNELDIALOG_HH
#define ROAMINGCHANNELDIALOG_HH

#include <QDialog>

// Forward declarations
namespace Ui {
  class RoamingChannelDialog;
}
class Config;
class RoamingChannel;


class RoamingChannelDialog : public QDialog
{
  Q_OBJECT

public:
  RoamingChannelDialog(Config *config, QWidget *parent = nullptr);
  RoamingChannelDialog(Config *config, RoamingChannel *channel, QWidget *parent = nullptr);
  ~RoamingChannelDialog();

  RoamingChannel *channel();

protected slots:
  void construct();
  void onOverrideTimeSlotToggled(bool override);
  void onOverrideColorCodeToggled(bool override);

private:
  Ui::RoamingChannelDialog *ui;
  RoamingChannel *_myChannel;
  RoamingChannel *_channel;
};

#endif // ROAMINGCHANNELDIALOG_HH
