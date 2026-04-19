#include "melody_edit.hh"

#include "melody.hh"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSpinBox>

MelodyEdit::MelodyEdit(QWidget *parent) :
  QWidget(parent), _melody(), _bpm(new QSpinBox(this)),
  _melodyEdit(new QLineEdit(this)) {
  _bpm->setRange(10,1000);
  _bpm->setSuffix(tr("bpm", "Beats per minute. Unit in a spin box."));
  _melodyEdit->setToolTip(tr("Specify the melody in Lilypond format.",
    "Tooltip for a melody entrie field."));
  _melodyEdit->setValidator(
    new QRegularExpressionValidator(
      QRegularExpression(R"(([a-zA-Z]+)([,]+|[']+|)(1|2|4|8|16|)(\.|))"
        R"((\h+([a-zA-Z]+)([,]+|[']+|)(1|2|4|8|16|)(\.|))*)")));
  _melodyEdit->setPlaceholderText(tr("Melody"));

  auto play = new QPushButton();
  play->setIcon(QIcon::fromTheme("media-play"));
  play->setEnabled(false);
  play->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  auto hbox = new QHBoxLayout(this);
  hbox->addWidget(_bpm);
  hbox->addWidget(_melodyEdit);
  hbox->addWidget(play);
  setLayout(hbox);

  _bpm->setEnabled(false);
  _melodyEdit->setEnabled(false);

  connect(_bpm, &QSpinBox::valueChanged, [this](int value) {
    if (!_melody.isNull())
      _melody->setBPM(value);
  });
  connect(_melodyEdit, &QLineEdit::editingFinished, [this]() {
    if (! _melody.isNull())
      _melody->fromLilypond(this->_melodyEdit->text().simplified());
  });
}

void
MelodyEdit::setMelody(Melody *melody) {
  _melody = melody;
  if (! _melody.isNull()) {
    onMelodyChanged(_melody);
    connect(_melody, &ConfigItem::modified, this, &MelodyEdit::onMelodyChanged);
  }
  _bpm->setEnabled(! _melody.isNull());
  _melodyEdit->setEnabled(! _melody.isNull());
}

void
MelodyEdit::onMelodyChanged(ConfigItem *item) {
  Q_UNUSED(item);
  _bpm->setValue(_melody->bpm());
  _melodyEdit->setText(_melody->toLilypond());
}