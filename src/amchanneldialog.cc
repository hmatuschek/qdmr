#include "amchanneldialog.hh"
#include "channel.hh"
#include "ui_channeldialog.h"
#include "squelchedit.hh"
#include "config.hh"


AMChannelDialog::AMChannelDialog(Config *config, QWidget *parent)
  : ChannelDialog(config, parent), _channel(nullptr), _squelch(nullptr)
{
  ui->rightForm->addRow(tr("Squelch"),  _squelch = new ChannelSquelchEdit(_config->settings()->squelch()));
}

void
AMChannelDialog::setChannel(AMChannel *am) {
  ChannelDialog::setChannel(am);
  _channel = am;

}

void
AMChannelDialog::accept() {
  _squelch->accept();
  ChannelDialog::accept();
}

