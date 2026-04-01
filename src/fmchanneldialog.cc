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
  : ChannelDialog(config, parent), _clone(nullptr), _orig(nullptr), _squelch(nullptr), _admit(nullptr),
  _rxTone(nullptr), _txTone(nullptr), _bandwidth(nullptr), _aprs(nullptr)
{
  auto app = qobject_cast<Application *>(qApp);
  auto filter = new FMRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
  auto completer = new RepeaterCompleter(2, app->repeater(), this);
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
  if (_clone) {
    delete _clone;
    _clone = nullptr;
  }

  _orig = channel;
  if (_orig.isNull()) return;
  _clone = _orig->clone()->as<FMChannel>();
  _clone->setParent(this);
  ChannelDialog::setChannel(_clone);

  _squelch->setChannel(_clone);
  _admit->setAdmit(_clone->admit());
  _rxTone->setSelectiveCall(_clone->rxTone());
  _txTone->setSelectiveCall(_clone->txTone());
  _bandwidth->setBandwidth(_clone->bandwidth());
  _aprs->setAPRSSystem(_clone->aprs());
}

void
FMChannelDialog::accept()
{
  _squelch->accept();
  _clone->setAdmit(_admit->admit());
  _clone->setRXTone(_rxTone->selectiveCall());
  _clone->setTXTone(_txTone->selectiveCall());
  _clone->setBandwidth(_bandwidth->bandwidth());
  _clone->setAPRS(_aprs->aprsSystem());

  ChannelDialog::accept();

  _orig->copy(*_clone);
}

void
FMChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  auto app = qobject_cast<Application *>(qApp);

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



