#include "generalsettingsview.hh"
#include "ui_generalsettingsview.h"
#include "config.hh"


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

  // update view from config
  onConfigModified();

  connect(_config, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModified()));
  connect(ui->introLine1, &QLineEdit::editingFinished, [this]() {
    this->_config->settings()->setIntroLine1(ui->introLine1->text().simplified());
  });
  connect(ui->introLine2, &QLineEdit::editingFinished, [this]() {
    this->_config->settings()->setIntroLine1(ui->introLine2->text().simplified());
  });

  connect(ui->mic, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->setMicLevel(Level::fromValue(value));
  });
  connect(ui->fmMicGain, &QSpinBox::valueChanged, [this] (int value) {
    this->_config->settings()->audio()->setFMMicGain(Level::fromValue(value));
  });
  connect(ui->voxValue, &QSpinBox::valueChanged, [this](int value) {
    if (0 == value)
      this->_config->settings()->disableVOX();
    else
      this->_config->settings()->setVOX(Level::fromValue(value));
  });
  connect(ui->voxDelay, &QLineEdit::editingFinished, [this]() {
    Interval delay; delay.parse(this->ui->voxDelay->text().simplified());
    this->_config->settings()->audio()->setVOXDelay(delay);
  });
  connect(ui->speech, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
    this->_config->settings()->enableSpeech(Qt::CheckState::Checked == state);
  });

  connect(ui->powerValue, &QComboBox::currentIndexChanged, [this](int index) {
    this->_config->settings()->setPower(this->ui->powerValue->itemData(index).value<Channel::Power>());
  });
  connect(ui->squelchValue, &QSpinBox::valueChanged, [this](int value) {
    this->_config->settings()->setSquelch(Level::fromValue(value));
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
  ui->introLine1->setText(_config->settings()->introLine1());
  ui->introLine2->setText(_config->settings()->introLine2());

  // audio settings
  ui->mic->setValue(_config->settings()->micLevel().value());
  ui->fmMicGain->setValue(_config->settings()->audio()->fmMicGain().value());
  ui->voxValue->setValue(_config->settings()->vox().value());
  ui->voxDelay->setText(_config->settings()->audio()->voxDelay().format());
  ui->speech->setChecked(_config->settings()->speech());

  // channel default settings
  switch(_config->settings()->power()) {
  case Channel::Power::Max: ui->powerValue->setCurrentIndex(0); break;
  case Channel::Power::High: ui->powerValue->setCurrentIndex(1); break;
  case Channel::Power::Mid: ui->powerValue->setCurrentIndex(2); break;
  case Channel::Power::Low: ui->powerValue->setCurrentIndex(3); break;
  case Channel::Power::Min: ui->powerValue->setCurrentIndex(4); break;
  }
  ui->squelchValue->setValue(_config->settings()->squelch().value());
  if (_config->settings()->totDisabled())
    ui->totValue->setValue(0);
  else
    ui->totValue->setValue(_config->settings()->tot().seconds());
}
