#include "propertydelegate.hh"
#include "extensionwrapper.hh"

#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include "configreference.hh"
#include "extensionwrapper.hh"

PropertyDelegate::PropertyDelegate(QObject *parent)
  : QItemDelegate(parent)
{
  // pass...
}

const PropertyWrapper *
PropertyDelegate::getModel(const QAbstractItemModel *model) {
  while (const QAbstractProxyModel *proxy = qobject_cast<const QAbstractProxyModel *>(model))
    model = proxy->sourceModel();
  return qobject_cast<const PropertyWrapper*>(model);
}

QWidget *
PropertyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  const PropertyWrapper *model = getModel(index.model());
  QMetaProperty prop = model->propertyAt(index);

  if (! prop.isValid())
    return nullptr;

  // Dispatch by type
  if (prop.isEnumType()) {
    return new QComboBox(parent);
  } else if (QVariant::Bool == prop.type()) {
    return new QComboBox(parent);
  } else if (QVariant::Int == prop.type()) {
    QSpinBox *edit = new QSpinBox(parent);
    edit->setMinimum(std::numeric_limits<int>::min());
    edit->setMaximum(std::numeric_limits<int>::max());
    return edit;
  } else if (QVariant::UInt == prop.type()) {
    QSpinBox *edit = new QSpinBox(parent);
    edit->setMinimum(0);
    edit->setMaximum(std::numeric_limits<int>::max());
    return edit;
  } else if (QVariant::Double == prop.type()) {
    QLineEdit *edit = new QLineEdit(parent);
    edit->setValidator(new QDoubleValidator(edit));
    return edit;
  } else if (QVariant::String == prop.type()) {
    return new QLineEdit(parent);
  } else if (prop.read(this).value<ConfigObjectReference *>()) {
    ConfigObjectReference *ref = prop.read(this).value<ConfigObjectReference *>();
    return new QComboBox(parent);
  } else if (propIsInstance<ConfigItem>(prop)) {
    return nullptr;
  }
  return nullptr;
}

void
PropertyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  const PropertyWrapper *model = getModel(index.model());
  ConfigItem *obj = model->parentObject(index);
  QMetaProperty prop = model->propertyAt(index);
  if (! prop.isValid())
    return;
  // Dispatch by type
  if (prop.isEnumType()) {
    QComboBox *box = dynamic_cast<QComboBox *>(editor);
    QMetaEnum etype = prop.enumerator();
    for (int i=0; i<etype.keyCount(); i++) {
        box->addItem(etype.key(i), QVariant(etype.value(i)));
        if (etype.value(i) == prop.read(obj).toInt())
          box->setCurrentIndex(i);
    }
  } else if (QVariant::Bool == prop.type()) {
    QComboBox *box = dynamic_cast<QComboBox *>(editor);
    box->addItem(tr("False"), false);
    box->addItem(tr("True"), true);
    if (prop.read(obj).toBool())
      box->setCurrentIndex(1);
    else
      box->setCurrentIndex(0);
  } else if ((QVariant::Int == prop.type()) || (QVariant::UInt == prop.type())) {
    dynamic_cast<QSpinBox *>(editor)->setValue(prop.read(obj).toInt());
  } else if (QVariant::Double == prop.type()) {
    dynamic_cast<QLineEdit *>(editor)->setText(QString::number(prop.read(obj).toDouble()));
  } else if (QVariant::String == prop.type()) {
    dynamic_cast<QLineEdit *>(editor)->setText(prop.read(obj).toString());
  } else if (prop.read(this).value<ConfigObjectReference *>()) {
    QComboBox *box = dynamic_cast<QComboBox*>(editor);
    box->addItem(tr("[None]"), QVariant::fromValue<ConfigObject*>(nullptr));
  }
}

void
PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *abstractmodel, const QModelIndex &index) const {
  Q_UNUSED(abstractmodel);
  const PropertyWrapper *model = getModel(index.model());
  ConfigItem *obj = model->parentObject(index);
  QMetaProperty prop = model->propertyAt(index);
  if (! prop.isValid())
    return;
  // Dispatch by type
  if (prop.isEnumType()) {
    prop.write(obj, dynamic_cast<QComboBox *>(editor)->currentData());
  } else if (QVariant::Bool == prop.type()) {
    prop.write(obj, dynamic_cast<QComboBox *>(editor)->currentData());
  } else if ((QVariant::Int == prop.type()) || (QVariant::UInt == prop.type())) {
    prop.write(obj, dynamic_cast<QSpinBox *>(editor)->value());
  } else if (QVariant::Double == prop.type()) {
    prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text().toDouble());
  } else if (QVariant::String == prop.type()) {
    prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text());
  } else if (prop.read(this).value<ConfigObjectReference *>()) {
    QComboBox *box = dynamic_cast<QComboBox*>(editor);
    box->addItem(tr("[None]"));
  }
}
