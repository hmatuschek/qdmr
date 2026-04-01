#include "m17channeldialog.hh"
#include "rxgrouplistdialog.hh"
#include "channel.hh"
#include "config.hh"
#include "m17contactdialog.hh"
#include "ui_channeldialog.h"


/* ****************************************************************************************** *
 * N17 channel mode selection
 * ****************************************************************************************** */
M17ChannelModeSelect::M17ChannelModeSelect(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("Voice"), QVariant::fromValue(M17Channel::Mode::Voice));
  addItem(tr("Data"), QVariant::fromValue(M17Channel::Mode::Data));
  addItem(tr("Voice + Data"), QVariant::fromValue(M17Channel::Mode::VoiceAndData));
}


void
M17ChannelModeSelect::setMode(M17Channel::Mode mode) {
  for (int i=0; i<count(); i++) {
    if (itemData(i).value<M17Channel::Mode>() == mode) {
      setCurrentIndex(i);
      break;
    }
  }
}


M17Channel::Mode
M17ChannelModeSelect::mode() const {
  return currentData().value<M17Channel::Mode>();
}




/* ****************************************************************************************** *
 * N17 channel dialog
 * ****************************************************************************************** */
M17ChannelDialog::M17ChannelDialog(Config *config, QWidget *parent)
  : ChannelDialog(config, parent), _channel(), _mode(nullptr), _access(nullptr),
  _contact(nullptr)
{
  ui->rightForm->addRow(tr("Channel mode"),
                        _mode = new M17ChannelModeSelect());
  ui->rightForm->addRow(tr("Access number"),
                        _access = new QSpinBox());
  _access->setRange(0,15);
  ui->rightForm->addRow(tr("Tx contact"),
                        _contact = new M17ContactSelect(config));
  ui->rightForm->addRow(tr("Send position"),
                        _aprs = new QCheckBox(""));
}


void
M17ChannelDialog::setChannel(M17Channel *ch) {
  ChannelDialog::setChannel(ch);
  _channel = ch;

  _mode->setMode(_channel->mode());
  _access->setValue(_channel->accessNumber());
  _contact->setContact(_channel->contact());
  _aprs->setChecked(_channel->aprsEnabled());
}


void
M17ChannelDialog::accept() {
  _channel->setMode(_mode->mode());
  _channel->setAccessNumber(_access->value());
  _channel->setContact(_contact->contact());
  _channel->enableAPRS(_aprs->isChecked());

  ChannelDialog::accept();
}
