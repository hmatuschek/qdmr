#include "fmchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "settings.hh"
#include "repeatercompleter.hh"
#include "repeaterdatabase.hh"
#include "ui_channeldialog.h"
#include "squelchedit.hh"
#include "admitselect.hh"
#include "selectivecallbox.hh"
#include "bandwidthselect.hh"
#include "aprsselect.hh"



/* ********************************************************************************************* *
 * Implementation of FMChannelDialog
 * ********************************************************************************************* */
FMChannelDialog::FMChannelDialog(Config *config, QWidget *parent)
  : ChannelDialog(config, parent), _channel(nullptr), _squelch(nullptr), _admit(nullptr),
  _rxTone(nullptr), _txTone(nullptr), _bandwidth(nullptr), _aprs(nullptr)
{
  /*Settings settings;
  if (settings.hideChannelNote()) {
    ui->hintLabel->setVisible(false);
    layout()->invalidate();
    adjustSize();
  }
  connect(ui->hintLabel, SIGNAL(linkActivated(QString)), this, SLOT(onHideChannelHint()));*/

  Application *app = qobject_cast<Application *>(qApp);
  FMRepeaterFilter *filter = new FMRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new RepeaterCompleter(2, app->repeater(), this);
  completer->setModel(filter);
  ui->channelName->setCompleter(completer);
  connect(completer, QOverload<const QModelIndex &>::of(&QCompleter::activated),
          this, &FMChannelDialog::onRepeaterSelected);

  ui->rightForm->addRow(tr("Squelch"),
                        _squelch = new ChannelSquelchEdit(config->settings()->squelch()));
  ui->rightForm->addRow(tr("Tx Admit"),
                        _admit = new FMAdmitSelect());
  ui->rightForm->addRow(tr("Rx Tone"),
                        _rxTone = new SelectiveCallBox());
  ui->rightForm->addRow(tr("Tx Tone"),
                        _txTone = new SelectiveCallBox());
  ui->rightForm->addRow(tr("Bandwidth"),
                        _bandwidth = new BandwidthSelect());
  ui->rightForm->addRow(tr("APRS"),
                        _aprs = new FMAPRSSelect(config));
}


void
FMChannelDialog::setChannel(FMChannel *channel) {
  ChannelDialog::setChannel(channel);
  _channel = channel;

  _squelch->setChannel(_channel);
  _admit->setAdmit(_channel->admit());
  _rxTone->setSelectiveCall(_channel->rxTone());
  _txTone->setSelectiveCall(_channel->txTone());
  _bandwidth->setBandwidth(_channel->bandwidth());
  _aprs->setAPRSSystem(_channel->aprs());
}

void
FMChannelDialog::accept()
{
  _squelch->accept();
  _channel->setAdmit(_admit->admit());
  _channel->setRXTone(_rxTone->selectiveCall());
  _channel->setTXTone(_txTone->selectiveCall());
  _channel->setBandwidth(_bandwidth->bandwidth());
  _channel->setAPRS(_aprs->aprsSystem());

  ChannelDialog::accept();
}

void
FMChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  Application *app = qobject_cast<Application *>(qApp);

  QModelIndex src = qobject_cast<QAbstractProxyModel*>(
        ui->channelName->completer()->completionModel())->mapToSource(index);
  src = qobject_cast<QAbstractProxyModel*>(
        ui->channelName->completer()->model())->mapToSource(src);
  Frequency rx = app->repeater()->get(src.row()).rxFrequency();
  Frequency tx = app->repeater()->get(src.row()).txFrequency();
  _rxTone->setSelectiveCall(app->repeater()->get(src.row()).rxTone());
  _txTone->setSelectiveCall(app->repeater()->get(src.row()).txTone());
  ui->rxFrequency->setText(rx.format());
  ui->txFrequency->setText(tx.format());
  updateOffsetFrequency();
}


void
FMChannelDialog::onHideChannelHint() {
  /*Settings settings;
  settings.setHideChannelNote(true);
  hintLabel->setVisible(false);
  layout()->invalidate();
  adjustSize();*/
}

