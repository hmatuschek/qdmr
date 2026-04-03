#ifndef CHANNELDIALOG_HH
#define CHANNELDIALOG_HH

#include <QDialog>
#include <QPointer>

namespace Ui {
class ChannelDialog;
}
class Config;
class Channel;


class ChannelDialog : public QDialog
{
  Q_OBJECT

public:
  ChannelDialog(Config *config, QWidget *parent=nullptr);
  ~ChannelDialog() override;

  void setChannel(Channel *mychannel);

public slots:
  void accept() override;

protected slots:
  void updateOffsetFrequency();

private slots:
  void onRxFrequencyEdited();
  void onTxFrequencyEdited();
  void onOffsetFrequencyEdited();
  void onOffsetDirectionChanged(int index);
  void onHideChannelHint();

private:
  void updateComboBox();

protected:
  Ui::ChannelDialog *ui;
  QPointer<Config> _config;
  QPointer<Channel> _channel;
};

#endif // CHANNELDIALOG_HH
