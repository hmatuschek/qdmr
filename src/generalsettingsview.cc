#include "generalsettingsview.hh"
#include "ui_generalsettingsview.h"
#include "config.hh"


GeneralSettingsView::GeneralSettingsView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::GeneralSettingsView), _config(config)
{
  ui->setupUi(this);

  if (_config->radioIDs()->defaultId()) {
    ui->dmrID->setText(QString::number(_config->radioIDs()->defaultId()->id()));
    ui->radioName->setText(_config->radioIDs()->defaultId()->name());
  }
  ui->introLine1->setText(_config->introLine1());
  ui->introLine2->setText(_config->introLine2());
  ui->mic->setValue(_config->micLevel());
  ui->speech->setChecked(_config->speech());

  connect(_config, SIGNAL(modified(ConfigObject*)), this, SLOT(onConfigModified()));
  connect(ui->dmrID, SIGNAL(textEdited(QString)), this, SLOT(onDMRIDChanged()));
  connect(ui->radioName, SIGNAL(textEdited(QString)), this, SLOT(onNameChanged()));
  connect(ui->introLine1, SIGNAL(textEdited(QString)), this, SLOT(onIntroLine1Changed()));
  connect(ui->introLine2, SIGNAL(textEdited(QString)), this, SLOT(onIntroLine2Changed()));
  connect(ui->mic, SIGNAL(valueChanged(int)), this, SLOT(onMicLevelChanged()));
  connect(ui->speech, SIGNAL(stateChanged(int)), this, SLOT(onSpeechChanged()));
}

GeneralSettingsView::~GeneralSettingsView() {
  delete ui;
}

void
GeneralSettingsView::hideDMRID(bool hidden) {
  ui->dmrIDBox->setHidden(hidden);
}

void
GeneralSettingsView::onConfigModified() {
  if (_config->radioIDs()->defaultId()) {
    ui->dmrID->setText(QString::number(_config->radioIDs()->defaultId()->id()));
    ui->radioName->setText(_config->radioIDs()->defaultId()->name());
  } else {
    ui->dmrID->setText("0");
    ui->radioName->setText("");
  }
  ui->introLine1->setText(_config->introLine1());
  ui->introLine2->setText(_config->introLine2());
  ui->mic->setValue(_config->micLevel());
  ui->speech->setChecked(_config->speech());
}

void
GeneralSettingsView::onDMRIDChanged() {
  if (0 == _config->radioIDs()->count()) {
    int idx = _config->radioIDs()->addId("", ui->dmrID->text().toUInt());
    _config->radioIDs()->setDefaultId(idx);
  } else if (nullptr == _config->radioIDs()->defaultId()) {
    _config->radioIDs()->setDefaultId(0);
    _config->radioIDs()->defaultId()->setId(ui->dmrID->text().toUInt());
  } else {
    _config->radioIDs()->defaultId()->setId(ui->dmrID->text().toUInt());
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
  _config->setIntroLine1(ui->introLine1->text().simplified());
}

void
GeneralSettingsView::onIntroLine2Changed() {
  _config->setIntroLine2(ui->introLine2->text().simplified());
}

void
GeneralSettingsView::onMicLevelChanged() {
  _config->setMicLevel(ui->mic->value());
}

void
GeneralSettingsView::onSpeechChanged() {
  _config->setSpeech(ui->speech->isChecked());
}

