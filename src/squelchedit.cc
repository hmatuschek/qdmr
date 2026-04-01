#include "squelchedit.hh"
#include "ui_squelchedit.h"
#include "channel.hh"



ChannelSquelchEdit::ChannelSquelchEdit(Level globalDefault, QWidget *parent)
  : QWidget(parent) , ui(new Ui::SquelchEdit), _globalDefault(globalDefault)
{
  ui->setupUi(this);
  ui->defaultBox->setChecked(true);
  ui->valueBox->setEnabled(false);
  ui->valueBox->setValue(_globalDefault.value());
  connect(ui->defaultBox, &QCheckBox::toggled, [this](bool checked) { this->ui->valueBox->setEnabled(!checked); });
}

ChannelSquelchEdit::~ChannelSquelchEdit() {
  delete ui;
}


void
ChannelSquelchEdit::setChannel(AnalogChannel *ch) {
  _channel = ch;
  if (ch->defaultSquelch()) {
    ui->defaultBox->setChecked(true);
    ui->valueBox->setValue(_globalDefault.value());
  } else
    ui->valueBox->setValue(ch->squelch().value());
}


void
ChannelSquelchEdit::accept() {
  if (ui->defaultBox->isChecked()) {
    _channel->setSquelchDefault();
  } else {
    _channel->setSquelch(Level::fromValue(ui->valueBox->value()));
  }
}


