#ifndef SQUELCHEDIT_HH
#define SQUELCHEDIT_HH

#include "level.hh"
#include <QWidget>
#include <QPointer>

namespace Ui {
class SquelchEdit;
}

class AnalogChannel;

class ChannelSquelchEdit : public QWidget
{
  Q_OBJECT

public:
  explicit ChannelSquelchEdit(Level globalDefault=Level::fromValue(1), QWidget *parent = nullptr);
  ~ChannelSquelchEdit();

  void setChannel(AnalogChannel *ch);

public slots:
  virtual void accept();

private:
  Ui::SquelchEdit *ui;
  QPointer<AnalogChannel> _channel;
  Level _globalDefault;
};

#endif // SQUELCHEDIT_HH
