#include "selectivecallbox.hh"
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>


/* ********************************************************************************************* *
 * SelectiveCallBox
 * ********************************************************************************************* */
SelectiveCallBox::SelectiveCallBox(QWidget *parent)
  : QWidget{parent}, _typeSelection(nullptr), _stack(nullptr), _ctcss(nullptr), _dcs(nullptr),
    _inverted(nullptr)
{
  _typeSelection = new QComboBox();
  _typeSelection->addItem(tr("None"));
  _typeSelection->addItem(tr("CTCSS"));
  _typeSelection->addItem(tr("DCS"));

  _stack = new QStackedWidget();
  _stack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  _stack->setContentsMargins(0,0,0,0);
  auto emptyLayout = new QHBoxLayout();
  emptyLayout->setContentsMargins(0,0,0,0);
  auto emptyLabel = new QLabel("None");
  emptyLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  emptyLayout->addWidget(emptyLabel);
  auto emptyWidget = new QWidget();
  emptyWidget->setLayout(emptyLayout);
  emptyWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  emptyWidget->setContentsMargins(0,0,0,0);
  _stack->addWidget(emptyWidget);

  // CTCSS settings
  _ctcss = new QComboBox();
  _ctcss->setEditable(true);
  auto ctcssLayout = new QHBoxLayout();
  ctcssLayout->setContentsMargins(0,0,0,0);
  ctcssLayout->addWidget(_ctcss);
  ctcssLayout->addWidget(new QLabel(tr("Hz")));
  auto ctcssWidget = new QWidget();
  ctcssWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  ctcssWidget->setContentsMargins(0,0,0,0);
  ctcssWidget->setLayout(ctcssLayout);
  _stack->addWidget(ctcssWidget);

  // DCS settings
  _dcs = new QComboBox();
  _dcs->setEditable(true);
  _inverted = new QCheckBox(tr("Inverted"));
  auto dscLayout = new QHBoxLayout();
  dscLayout->setContentsMargins(0,0,0,0);
  dscLayout->addWidget(_dcs);
  dscLayout->addWidget(_inverted);
  auto dcsWidget = new QWidget();
  dcsWidget->setLayout(dscLayout);
  dcsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  dcsWidget->setContentsMargins(0,0,0,0);
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

  connect(_ctcss->lineEdit(), &QLineEdit::editingFinished,
    this, &SelectiveCallBox::onCTCSSEdited);
  connect(_dcs->lineEdit(), &QLineEdit::editingFinished,
    this, &SelectiveCallBox::onDCSEdited);

  auto layout = new QHBoxLayout();
  layout->addWidget(_typeSelection,0);
  layout->addWidget(_stack, 1);
  layout->setContentsMargins(0,0,0,0);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setContentsMargins(0,0,0,0);
  setLayout(layout);
}


void
SelectiveCallBox::setSelectiveCall(const SelectiveCall &call) {
  if (call.isInvalid()) {
    _typeSelection->setCurrentIndex(0);
  } else if (call.isCTCSS()) {
    _typeSelection->setCurrentIndex(1);
    int idx = _ctcss->findData(QVariant::fromValue(call));
    if (idx >= 0) {
      _ctcss->setCurrentIndex(idx);
    } else {
      _ctcss->setEditText(QString("%1.%2").arg(call.mHz()/1000).arg((call.mHz()/100)%10));
      onCTCSSEdited();
    }
  } else if (call.isDCS()) {
    _typeSelection->setCurrentIndex(2);
    int idx = _dcs->findData(QVariant(call.binCode()));
    if (idx >= 0) {
      _dcs->setCurrentIndex(idx);
    } else {
      _dcs->setEditText(QString("%1").arg(call.binCode(), 3, 8, QChar('0')));
      onDCSEdited();
    }
    _inverted->setChecked(call.isInverted());
  }
}


SelectiveCall
SelectiveCallBox::selectiveCall() const {
  if (1 == _typeSelection->currentIndex()) {
    // handle CTCSS setting
    if (0 <= _ctcss->currentIndex())
      return _ctcss->currentData().value<SelectiveCall>();
  } else if (2 == _typeSelection->currentIndex()) {
    // handle DCS setting
    if (0 <= _dcs->currentIndex())
      return SelectiveCall(_dcs->currentData().toUInt(), _inverted->isChecked());
  }
  // none
  return {};
}


void
SelectiveCallBox::onCTCSSEdited() {
  const auto call = SelectiveCall::parseCTCSS(_ctcss->currentText());
  if (! call.isValid())
    return;

  for (int idx=0; idx<_ctcss->count(); idx++) {
    if (call == _ctcss->itemData(idx).value<SelectiveCall>()) {
      _ctcss->setCurrentIndex(idx);
      return;
    }
  }

  const int idx = _ctcss->count();
  _ctcss->addItem(QString("%1.%2").arg(call.mHz()/1000).arg((call.mHz()/100)%10),
                  QVariant::fromValue(call));
  _ctcss->setCurrentIndex(idx);
}


void
SelectiveCallBox::onDCSEdited() {
  const auto call = SelectiveCall::parseDCS(_dcs->currentText());
  if (! call.isValid())
    return;

  for (int idx=0; idx<_dcs->count(); idx++) {
    if (call == _dcs->itemData(idx).value<SelectiveCall>()) {
      _dcs->setCurrentIndex(idx);
      return;
    }
  }

  const int idx = _dcs->count();
  _dcs->addItem(QString("%1").arg(call.binCode(), 3, 8, QChar('0')),
                QVariant(call.binCode()));
  _dcs->setCurrentIndex(idx);
}



/* ********************************************************************************************* *
 * SelectiveCallDelegate
 * ********************************************************************************************* */
SelectiveCallDelegate::SelectiveCallDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
  // pass...
}


QWidget *
SelectiveCallDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option); Q_UNUSED(index);
  auto seditor = new SelectiveCallBox(parent);
  seditor->setSelectiveCall(index.data(Qt::EditRole).value<SelectiveCall>());
  return seditor;
}


void
SelectiveCallDelegate::setEditorData(QWidget *editor, const QModelIndex index) {
  auto seditor = qobject_cast<SelectiveCallBox*>(editor);
  seditor->setSelectiveCall(index.data(Qt::EditRole).value<SelectiveCall>());
}


void
SelectiveCallDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  auto seditor = qobject_cast<SelectiveCallBox*>(editor);
  model->setData(index, QVariant::fromValue(seditor->selectiveCall()));
}

