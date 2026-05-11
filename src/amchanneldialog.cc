#include "amchanneldialog.hh"
#include "channel.hh"
#include "ui_channeldialog.h"
#include "squelchedit.hh"
#include "config.hh"


AMChannelDialog::AMChannelDialog(Config *config, QWidget *parent)
  : ChannelDialog(config, parent), _clone(nullptr), _orig(nullptr), _squelch(nullptr)
{
  ui->rightForm->addRow(tr("Squelch"),
    _squelch = new ChannelSquelchEdit(_config->settings()->audio()->squelch()));
}

void
AMChannelDialog::setChannel(AMChannel *am) {
  if (_clone) {
    delete _clone;
    _clone = nullptr;
  }

  _orig = am;
  if (_orig.isNull())
    return;

  _clone = _orig->clone()->as<AMChannel>();
  _clone->setParent(this);
  ChannelDialog::setChannel(_clone);

  _squelch->setChannel(_clone);
}

void
AMChannelDialog::accept() {
  _squelch->accept();

  ChannelDialog::accept();

  _orig->copy(*_clone);
}

