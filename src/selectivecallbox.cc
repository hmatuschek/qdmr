#include "selectivecallbox.hh"
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

SelectiveCallBox::SelectiveCallBox(QWidget *parent)
  : QWidget{parent}, _typeSelection(nullptr), _stack(nullptr), _ctcss(nullptr), _dcs(nullptr),
    _inverted(nullptr)
{
  _typeSelection = new QComboBox();
  _typeSelection->addItem(tr("None"));
  _typeSelection->addItem(tr("CTCSS"));
  _typeSelection->addItem(tr("DCS"));

  _stack = new QStackedWidget();
  auto emptyLayout = new QHBoxLayout();
  auto emptyLabel = new QLabel("None");
  emptyLayout->addWidget(emptyLabel);
  auto emptyWidget = new QWidget();
  emptyWidget->setLayout(emptyLayout);
  _stack->addWidget(emptyWidget);

  // CTCSS settings
  _ctcss = new QComboBox();
  _ctcss->setEditable(true);
  auto ctcssLayout = new QHBoxLayout();
  ctcssLayout->addWidget(_ctcss);
  ctcssLayout->addWidget(new QLabel(tr("Hz")));
  auto ctcssWidget = new QWidget();
  ctcssWidget->setLayout(ctcssLayout);
  _stack->addWidget(ctcssWidget);

  // DCS settings
  _dcs = new QComboBox();
  _dcs->setEditable(true);
  _inverted = new QCheckBox(tr("Inverted"));
  auto dscLayout = new QHBoxLayout();
  dscLayout->addWidget(_dcs);
  dscLayout->addWidget(_inverted);
  auto dcsWidget = new QWidget();
  dcsWidget->setLayout(dscLayout);
  _stack->addWidget(dcsWidget);

  connect(_typeSelection, &QComboBox::currentTextChanged, [=]() {
    _stack->setCurrentIndex(_typeSelection->currentIndex());
  });

  foreach (const SelectiveCall &call, SelectiveCall::standard()) {
    if (call.isInvalid())
      continue;
    if (call.isCTCSS())
      _ctcss->addItem(QString("%1.%2").arg(call.mHz()/1000).arg((call.mHz()/100)%10),
                      QVariant::fromValue(call));
    else if (call.isDCS() && (! call.isInverted()))
      _dcs->addItem(QString("%1").arg(call.binCode(), 3, 8, QChar('0')),
                    QVariant(call.binCode()));
  }

  auto layout = new QHBoxLayout();
  layout->addWidget(_typeSelection,0);
  layout->addWidget(_stack, 1);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  setLayout(layout);
}


void
SelectiveCallBox::setSelectiveCall(const SelectiveCall &call) {
  if (call.isInvalid()) {
    _typeSelection->setCurrentIndex(0);
  } else if (call.isCTCSS()) {
    _typeSelection->setCurrentIndex(1);
    int idx = _ctcss->findData(QVariant::fromValue(call));
    if (idx >= 0)
      _ctcss->setCurrentIndex(idx);
    else
      _ctcss->setEditText(call.format());
  } else if (call.isDCS()) {
    _typeSelection->setCurrentIndex(2);
    int idx = _dcs->findData(QVariant(call.binCode()));
    if (idx >= 0)
      _dcs->setCurrentIndex(idx);
    else
      _dcs->setEditText(QString("%1").arg(call.binCode(), 3, 8, QChar('0')));
    _inverted->setChecked(call.isInverted());
  }
}

SelectiveCall
SelectiveCallBox::selectiveCall() const {
  if (0 == _typeSelection->currentIndex()) {
    return SelectiveCall();
  } else if (1 == _typeSelection->currentIndex()) {
    int idx = _ctcss->currentIndex();
    if (0 >= idx)
      return _ctcss->currentData().value<SelectiveCall>();
    return SelectiveCall::parseCTCSS(_ctcss->currentText());
  } else if (2 == _typeSelection->currentIndex()) {
    int idx = _dcs->currentIndex();
    if (0 >= idx)
      return SelectiveCall(_dcs->currentData().toUInt(), _inverted->isChecked());
    return SelectiveCall(_dcs->currentText().toUInt(), _inverted->isChecked());
  }

  return SelectiveCall();
}
