#include "channel_type_edit.hh"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QMetaEnum>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "channel.hh"

ChannelTypeEdit::ChannelTypeEdit(QWidget *parent)
  : QWidget(parent), _types(Channel::Type::None)
{
  _label = new QLabel();
  _label->setText(QMetaEnum::fromType<Channel::Type>().valueToKeys(_types));
  _label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  auto edit = new QPushButton();
  edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  edit->setIcon(QIcon::fromTheme("edit-edit"));
  auto box = new QHBoxLayout();
  box->addWidget(edit);
  box->addWidget(_label);
  setLayout(box);

  connect(edit, &QPushButton::clicked, this, &ChannelTypeEdit::onEditClicked);
}

Channel::Types
ChannelTypeEdit::types() const {
  return _types;
}

void ChannelTypeEdit::setTypes(Channel::Types types) {
  _types = types;
  _label->setText(QMetaEnum::fromType<Channel::Type>().valueToKeys(_types));
}

void ChannelTypeEdit::onEditClicked() {
  ChannelTypeSelectionDialog dialog(_types);
  if (QDialog::Accepted != dialog.exec())
    return;
  setTypes(dialog.types());
  emit typesChanged(types());
}

ChannelTypeSelectionDialog::ChannelTypeSelectionDialog(Channel::Types types, QWidget *parent)
  : QDialog(parent), _list(new QListWidget(this))
{
  setWindowTitle(tr("Select channel types"));

  auto e = QMetaEnum::fromType<Channel::Type>();
  for (int i=1; i<e.keyCount(); i++) {
    auto item = new QListWidgetItem(e.key(i));
    auto value = (Channel::Type)e.value(i);
    item->setData(Qt::UserRole, QVariant::fromValue(value));
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setCheckState(types.testFlag(value) ? Qt::Checked : Qt::Unchecked);
    _list->addItem(item);
  }
  auto bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  auto box = new QVBoxLayout();
  box->addWidget(_list);
  box->addWidget(bbox);
  setLayout(box);

  connect(bbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Channel::Types
ChannelTypeSelectionDialog::types() const {
  Channel::Types types = Channel::Type::None;
  for (int i=0; i<_list->count(); i++) {
    auto item = _list->item(i);
    if (item->checkState() == Qt::Checked)
      types |= item->data(Qt::UserRole).value<Channel::Type>();
  }
  return types;
}