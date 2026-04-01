#ifndef M17CHANNELDIALOG_HH
#define M17CHANNELDIALOG_HH

#include "channeldialog.hh"
#include "channel.hh"
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

class Config;
class M17ContactSelect;


class M17ChannelModeSelect: public QComboBox
{
Q_OBJECT

public:
  explicit M17ChannelModeSelect(QWidget *parent=nullptr);

  void setMode(M17Channel::Mode mode);
  M17Channel::Mode mode() const;
};


class M17ChannelDialog : public ChannelDialog
{
  Q_OBJECT

public:
  M17ChannelDialog(Config *config, QWidget *parent = nullptr);

  void setChannel(M17Channel *ch);

public slots:
  void accept() override;

private:
  QPointer<M17Channel> _channel;
  M17ChannelModeSelect *_mode;
  QSpinBox *_access;
  M17ContactSelect *_contact;
  QCheckBox *_aprs;
};

#endif // M17CHANNELDIALOG_HH
