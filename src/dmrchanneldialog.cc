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
  : ChannelDialog(config, parent), _clone(nullptr), _orig(), _dmrId(nullptr), _admit(nullptr),
  _colorcode(nullptr), _timeSlot(nullptr), _groupList(nullptr), _contact(nullptr),
  _aprs(nullptr), _roam(nullptr)
{
  auto app = qobject_cast<Application *>(qApp);
  auto completer = new RepeaterCompleter(2, app->repeater(), this);
  auto filter = new DMRRepeaterFilter(app->repeater(), app->position(), this);
  filter->setSourceModel(app->repeater());
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
  if (_clone) {
    delete _clone;
    _clone = nullptr;
  }

  _orig = ch;
  if (_orig.isNull())
    return;

  _clone = _orig->clone()->as<DMRChannel>();
  _clone->setParent(this);

  ChannelDialog::setChannel(_clone);


  _dmrId->setRadioId(_clone->radioId());
  _admit->setAdmit(_clone->admit());
  _colorcode->setValue(_clone->colorCode());
  _timeSlot->setSlot(_clone->timeSlot());
  _groupList->setGroupList(_clone->groupList());
  _contact->setContact(_clone->contact());
  _aprs->setAPRSSystem(_clone->aprs());
  _roam->setRoamingZone(_clone->roaming());
}


void
DMRChannelDialog::accept()
{
  _clone->setRadioId(_dmrId->radioId());
  _clone->setAdmit(_admit->admit());
  _clone->setColorCode(_colorcode->value());
  _clone->setTimeSlot(_timeSlot->slot());
  _clone->setGroupList(_groupList->groupList());
  _clone->setContact(_contact->contact());
  _clone->setAPRS(_aprs->aprsSystem());
  _clone->setRoaming(_roam->roamingZone());

  ChannelDialog::accept();

  _orig->copy(*_clone);
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

