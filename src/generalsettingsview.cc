#include "generalsettingsview.hh"
#include "ui_generalsettingsview.h"
#include "config.hh"


GeneralSettingsView::GeneralSettingsView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::GeneralSettingsView), _config(config)
{
  ui->setupUi(this);

  if (_config->radioIDs()->defaultId()) {
    ui->dmrID->setText(QString::number(_config->radioIDs()->defaultId()->number()));
    ui->radioName->setText(_config->radioIDs()->defaultId()->name());
  }
  ui->introLine1->setText(_config->settings()->introLine1());
  ui->introLine2->setText(_config->settings()->introLine2());
  ui->mic->setValue(_config->settings()->micLevel());
  ui->speech->setChecked(_config->settings()->speech());

  ui->powerValue->setItemData(0, (unsigned)Channel::Power::Max);
  ui->powerValue->setItemData(1, (unsigned)Channel::Power::High);
  ui->powerValue->setItemData(2, (unsigned)Channel::Power::Mid);
  ui->powerValue->setItemData(3, (unsigned)Channel::Power::Low);
  ui->powerValue->setItemData(4, (unsigned)Channel::Power::Min);

  switch(_config->settings()->power()) {
  case Channel::Power::Max: ui->powerValue->setCurrentIndex(0); break;
  case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
  case Channel::Power::Mid: ui->powerValue->setCurrentIndex(2); break;
  case Channel::Power::Low: ui->powerValue->setCurrentIndex(3); break;
  case Channel::Power::Min: ui->powerValue->setCurrentIndex(4); break;
  }

  ui->squelchValue->setValue(config->settings()->squelch());
  ui->totValue->setValue(config->settings()->tot());
  ui->voxValue->setValue(config->settings()->vox());

  ui->extensionView->setObject(config->settings());

  connect(_config, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
  connect(ui->dmrID, SIGNAL(textEdited(QString)), this, SLOT(onDMRIDChanged()));
  connect(ui->radioName, SIGNAL(textEdited(QString)), this, SLOT(onNameChanged()));
  connect(ui->introLine1, SIGNAL(textEdited(QString)), this, SLOT(onIntroLine1Changed()));
  connect(ui->introLine2, SIGNAL(textEdited(QString)), this, SLOT(onIntroLine2Changed()));
  connect(ui->mic, SIGNAL(valueChanged(int)), this, SLOT(onMicLevelChanged()));
  connect(ui->speech, SIGNAL(stateChanged(int)), this, SLOT(onSpeechChanged()));
  connect(ui->powerValue, SIGNAL(currentIndexChanged(int)), this, SLOT(onPowerChanged()));
  connect(ui->squelchValue, SIGNAL(valueChanged(int)), this, SLOT(onSquelchChanged()));
  connect(ui->totValue, SIGNAL(valueChanged(int)), this, SLOT(onTOTChanged()));
  connect(ui->voxValue, SIGNAL(valueChanged(int)), this, SLOT(onVOXChanged()));
}

GeneralSettingsView::~GeneralSettingsView() {
  delete ui;
}

void
GeneralSettingsView::hideDMRID(bool hidden) {
  ui->dmrIDBox->setHidden(hidden);
}

void
GeneralSettingsView::hideExtensions(bool hidden) {
  ui->extensionBox->setHidden(hidden);
}

void
GeneralSettingsView::onConfigModified() {
  if (_config->radioIDs()->defaultId()) {
    ui->dmrID->setText(QString::number(_config->radioIDs()->defaultId()->number()));
    ui->radioName->setText(_config->radioIDs()->defaultId()->name());
  } else {
    ui->dmrID->setText("0");
    ui->radioName->setText("");
  }
  ui->introLine1->setText(_config->settings()->introLine1());
  ui->introLine2->setText(_config->settings()->introLine2());
  ui->mic->setValue(_config->settings()->micLevel());
  ui->speech->setChecked(_config->settings()->speech());
}

void
GeneralSettingsView::onDMRIDChanged() {
  if (0 == _config->radioIDs()->count()) {
    int idx = _config->radioIDs()->addId("", ui->dmrID->text().toUInt());
    _config->radioIDs()->setDefaultId(idx);
  } else if (nullptr == _config->radioIDs()->defaultId()) {
    _config->radioIDs()->setDefaultId(0);
    _config->radioIDs()->defaultId()->setNumber(ui->dmrID->text().toUInt());
  } else {
    _config->radioIDs()->defaultId()->setNumber(ui->dmrID->text().toUInt());
  }
}

void
GeneralSettingsView::onNameChanged() {
  if (0 == _config->radioIDs()->count()) {
    int idx = _config->radioIDs()->addId(ui->radioName->text().simplified(), 0);
    _config->radioIDs()->setDefaultId(idx);
  } else if (nullptr == _config->radioIDs()->defaultId()) {
    _config->radioIDs()->setDefaultId(0);
    _config->radioIDs()->defaultId()->setName(ui->radioName->text().simplified());
  } else {
    _config->radioIDs()->defaultId()->setName(ui->radioName->text().simplified());
  }
}

void
GeneralSettingsView::onIntroLine1Changed() {
  _config->settings()->setIntroLine1(ui->introLine1->text().simplified());
}

void
GeneralSettingsView::onIntroLine2Changed() {
  _config->settings()->setIntroLine2(ui->introLine2->text().simplified());
}

void
GeneralSettingsView::onMicLevelChanged() {
  _config->settings()->setMicLevel(ui->mic->value());
}

void
GeneralSettingsView::onSpeechChanged() {
  _config->settings()->enableSpeech(ui->speech->isChecked());
}

void
GeneralSettingsView::onPowerChanged() {
  _config->settings()->setPower(Channel::Power(ui->powerValue->currentData().toUInt()));
}
void
GeneralSettingsView::onSquelchChanged() {
  _config->settings()->setSquelch(ui->squelchValue->value());
}
void
GeneralSettingsView::onTOTChanged() {
  _config->settings()->setTOT(ui->totValue->value());
}
void
GeneralSettingsView::onVOXChanged() {
  _config->settings()->setVOX(ui->voxValue->value());
}
