#include "dmrchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include <QSpinBox>
#include "rxgrouplistdialog.hh"
#include "repeatercompleter.hh"
#include "repeaterdatabase.hh"
#include "settings.hh"
#include "idselect.hh"
#include "admitselect.hh"
#include "timeslotselect.hh"
#include "dmrcontactdialog.hh"
#include "aprsselect.hh"
#include "roamingzonedialog.hh"
#include "ui_channeldialog.h"



/* ********************************************************************************************* *
 * Implementation of DMRChannelDialog
 * ********************************************************************************************* */
DMRChannelDialog::DMRChannelDialog(Config *config, QWidget *parent)
  : ChannelDialog(config, parent), _channel(), _dmrId(nullptr), _admit(nullptr),
  _colorcode(nullptr), _timeSlot(nullptr), _groupList(nullptr), _contact(nullptr),
  _aprs(nullptr), _roam(nullptr)
{
  Settings settings;
  if (settings.hideChannelNote()) {
    ui->hintLabel->setVisible(false);
    layout()->invalidate();
    adjustSize();
  }
  connect(ui->hintLabel, SIGNAL(linkActivated(QString)), this, SLOT(onHideChannelHint()));

  Application *app = qobject_cast<Application *>(qApp);
  DMRRepeaterFilter *filter = new DMRRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new RepeaterCompleter(2, app->repeater(), this);
  completer->setModel(filter);
  ui->channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  ui->rightForm->addRow(tr("Radio Id"),
                        _dmrId = new DMRIdSelect(config));
  ui->rightForm->addRow(tr("Tx Admit"),
                        _admit = new DMRAdmitSelect());
  ui->rightForm->addRow(tr("Color-code"),
                        _colorcode = new QSpinBox());
  _colorcode->setRange(0,15);
  ui->rightForm->addRow(tr("Time-slot"),
                        _timeSlot = new TimeslotSelect());
  ui->rightForm->addRow(tr("Group list"),
                        _groupList = new RXGroupListBox(config->rxGroupLists()));
  ui->rightForm->addRow(tr("Tx Contact"),
                        _contact = new DMRContactSelect(config));
  ui->rightForm->addRow(tr("APRS"),
                        _aprs = new APRSSelect(config));
  ui->rightForm->addRow(tr("Roaming zone"),
                        _roam = new RoamingZoneSelect(config));
}


void
DMRChannelDialog::setChannel(DMRChannel *ch) {
  ChannelDialog::setChannel(ch);
  _channel = ch;

  _dmrId->setRadioId(_channel->radioId());
  _admit->setAdmit(_channel->admit());
  _colorcode->setValue(_channel->colorCode());
  _timeSlot->setSlot(_channel->timeSlot());
  _groupList->setGroupList(_channel->groupList());
  _contact->setContact(_channel->contact());
  _aprs->setAPRSSystem(_channel->aprs());
  _roam->setRoamingZone(_channel->roaming());
}

void
DMRChannelDialog::accept()
{
  _channel->setRadioId(_dmrId->radioId());
  _channel->setAdmit(_admit->admit());
  _channel->setColorCode(_colorcode->value());
  _channel->setTimeSlot(_timeSlot->slot());
  _channel->setGroupList(_groupList->groupList());
  _channel->setContact(_contact->contact());
  _channel->setAPRS(_aprs->aprsSystem());
  _channel->setRoaming(_roam->roamingZone());

  ChannelDialog::accept();
}

void
DMRChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  Application *app = qobject_cast<Application *>(qApp);

  QModelIndex src = qobject_cast<QAbstractProxyModel*>(
        ui->channelName->completer()->completionModel())->mapToSource(index);
  src = qobject_cast<QAbstractProxyModel*>(
        ui->channelName->completer()->model())->mapToSource(src);
  Frequency rx = app->repeater()->get(src.row()).rxFrequency();
  Frequency tx = app->repeater()->get(src.row()).txFrequency();
  _colorcode->setValue(app->repeater()->get(src.row()).colorCode());
  ui->rxFrequency->setText(rx.format());
  ui->txFrequency->setText(tx.format());
  updateOffsetFrequency();
}

void
DMRChannelDialog::onHideChannelHint() {
  Settings settings;
  settings.setHideChannelNote(true);
  ui->hintLabel->setVisible(false);
  layout()->invalidate();
  adjustSize();
}

