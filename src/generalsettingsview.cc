#include "generalsettingsview.hh"

#include "channel_type_edit.hh"
#include "config.hh"
#include "ui_generalsettingsview.h"

GeneralSettingsView::GeneralSettingsView(Config *config, QWidget *parent)
  : QWidget(parent), ui(new Ui::GeneralSettingsView), _config(config) {
  ui->setupUi(this);

  ui->powerValue->setItemData(0, (unsigned)Channel::Power::Max);
  ui->powerValue->setItemData(1, (unsigned)Channel::Power::High);
  ui->powerValue->setItemData(2, (unsigned)Channel::Power::Mid);
  ui->powerValue->setItemData(3, (unsigned)Channel::Power::Low);
  ui->powerValue->setItemData(4, (unsigned)Channel::Power::Min);

  ui->extensionView->setObjectName("radioSettingsExtension");
  ui->extensionView->setObject(_config->settings(), _config);
  ui->bootMelodyEdit->setMelody(_config->settings()->tone()->bootMelody());
  ui->callStartEdit->setMelody(_config->settings()->tone()->callStartMelody());
  ui->callEndEdit->setMelody(_config->settings()->tone()->callEndMelody());
  ui->channelIdleEdit->setMelody(_config->settings()->tone()->channelIdleMelody());
  ui->callResetEdit->setMelody(_config->settings()->tone()->callResetMelody());

  // update view from config
  onConfigModified();

  connect(_config, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
  connect(ui->introLine1, &QLineEdit::editingFinished, [this]() {
    this->_config->settings()->boot()->setMessage1(ui->introLine1->text().simplified());
  });
  connect(ui->introLine2, &QLineEdit::editingFinished, [this]() {
    this->_config->settings()->boot()->setMessage2(ui->introLine2->text().simplified());
  });

  connect(ui->squelch, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->audio()->setSquelch(Level::fromValue(value));
    if (this->ui->dmrSquelchDefault->isChecked())
      this->ui->dmrSquelch->setValue(value);
  });
  connect(ui->dmrSquelch, &QSpinBox::valueChanged, [this](int value) {
    if (! this->ui->dmrSquelchDefault->isChecked())
      this->_config->settings()->audio()->setDMRSquelch(Level::fromValue(value));
  });
  connect(ui->dmrSquelchDefault, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    if (Qt::CheckState::Checked == state) {
      this->_config->settings()->audio()->disableDMRSquelch();
      this->ui->dmrSquelch->setEnabled(false);
    } else {
      this->_config->settings()->audio()->setDMRSquelch(Level::fromValue(this->ui->dmrSquelch->value()));
      this->ui->dmrSquelch->setEnabled(true);
    }
  });
  connect(ui->mic, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->audio()->setMicGain(Level::fromValue(value));
  });
  connect(ui->fmMicGain, &QSpinBox::valueChanged, [this] (int value) {
    this->_config->settings()->audio()->setFMMicGain(Level::fromValue(value));
  });
  connect(ui->voxValue, &QSpinBox::valueChanged, [this](int value) {
    if (0 == value)
      this->_config->settings()->audio()->disableVox();
    else
      this->_config->settings()->audio()->setVox(Level::fromValue(value));
  });
  connect(ui->voxDelay, &QLineEdit::editingFinished, [this]() {
    Interval delay; delay.parse(this->ui->voxDelay->text().simplified());
    this->_config->settings()->audio()->setVOXDelay(delay);
  });
  connect(ui->maxSpeakerVolume, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->audio()->setMaxSpeakerVolume(Level::fromValue(value));
  });
  connect(ui->maxHeadphoneVolume, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->audio()->setMaxHeadphoneVolume(Level::fromValue(value));
  });
  connect(ui->speech, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    this->_config->settings()->audio()->enableSpeechSynthesis(Qt::CheckState::Checked == state);
  });

  connect(ui->disableAllTones, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    this->_config->settings()->tone()->enableSilent(Qt::Checked == state);
  });
  connect(ui->keyToneVolume, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->tone()->setKeyToneVolume(Level::fromValue(value));
  });
  connect(ui->bootMelodyEnable, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    this->_config->settings()->tone()->enableBootTone(Qt::Checked == state);
  });
  connect(ui->talkPermitSelect, &ChannelTypeEdit::typesChanged, [this](Channel::Types types) {
    this->_config->settings()->tone()->setTalkPermit(types);
  });
  connect(ui->callStartSelect, &ChannelTypeEdit::typesChanged, [this](Channel::Types types) {
    this->_config->settings()->tone()->setCallStart(types);
  });
  connect(ui->callEndSelect, &ChannelTypeEdit::typesChanged, [this](Channel::Types types) {
    this->_config->settings()->tone()->setCallEnd(types);
  });
  connect(ui->channelIdleSelect, &ChannelTypeEdit::typesChanged, [this](Channel::Types types) {
    this->_config->settings()->tone()->setChannelIdle(types);
  });
  connect(ui->callResetEnable, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    this->_config->settings()->tone()->enableCallReset(Qt::Checked == state);
  });



  connect(ui->powerValue, &QComboBox::currentIndexChanged, [this](int index) {
    this->_config->settings()->setPower(this->ui->powerValue->itemData(index).value<Channel::Power>());
  });
  connect(ui->totValue, &QSpinBox::valueChanged, [this](int value) {
    if (0 == value)
      this->_config->settings()->disableTOT();
    else
      this->_config->settings()->setTOT(Interval::fromSeconds(value));
  });
}

GeneralSettingsView::~GeneralSettingsView() {
  delete ui;
}

void
GeneralSettingsView::onConfigModified() {
  // boot settings
  ui->introLine1->setText(_config->settings()->boot()->message1());
  ui->introLine2->setText(_config->settings()->boot()->message2());

  // audio settings
  ui->squelch->setValue(_config->settings()->audio()->squelch().value());
  if (_config->settings()->audio()->dmrSquelchEnabled()) {
    ui->dmrSquelch->setValue(_config->settings()->audio()->dmrSquelch().value());
    ui->dmrSquelchDefault->setChecked(false);
    ui->dmrSquelch->setEnabled(true);
  } else {
    ui->dmrSquelch->setValue(3);
    ui->dmrSquelchDefault->setChecked(true);
    ui->dmrSquelch->setEnabled(false);
  }
  ui->mic->setValue(_config->settings()->audio()->micGain().value());
  ui->fmMicGain->setValue(_config->settings()->audio()->fmMicGain().value());
  ui->voxValue->setValue(_config->settings()->audio()->vox().value());
  ui->voxDelay->setText(_config->settings()->audio()->voxDelay().format());
  ui->speech->setChecked(_config->settings()->audio()->speechSynthesisEnabled());

  // Tone settings:
  ui->disableAllTones->setChecked(_config->settings()->tone()->silent());
  ui->keyToneVolume->setValue(_config->settings()->tone()->keyToneVolume().value());
  ui->bootMelodyEnable->setChecked(_config->settings()->tone()->bootToneEnabled());
  ui->talkPermitSelect->setTypes(_config->settings()->tone()->talkPermit());
  ui->callStartSelect->setTypes(_config->settings()->tone()->callStart());
  ui->callEndSelect->setTypes(_config->settings()->tone()->callEnd());
  ui->channelIdleSelect->setTypes(_config->settings()->tone()->channelIdle());
  ui->callResetEnable->setChecked(_config->settings()->tone()->callResetEnabled());

  // channel default settings
  switch(_config->settings()->power()) {
  case Channel::Power::Max: ui->powerValue->setCurrentIndex(0); break;
  case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
  case Channel::Power::Mid: ui->powerValue->setCurrentIndex(2); break;
  case Channel::Power::Low: ui->powerValue->setCurrentIndex(3); break;
  case Channel::Power::Min: ui->powerValue->setCurrentIndex(4); break;
  }
  if (_config->settings()->totDisabled())
    ui->totValue->setValue(0);
  else
    ui->totValue->setValue(_config->settings()->tot().seconds());
}
