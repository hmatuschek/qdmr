#include "propertydelegate.hh"
#include "extensionwrapper.hh"

#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include "configreference.hh"
#include "extensionwrapper.hh"
#include "config.hh"
#include "frequency.hh"
#include "interval.hh"


PropertyDelegate::PropertyDelegate(QObject *parent)
  : QItemDelegate(parent), _config(nullptr)
{
  // pass...
}

void
PropertyDelegate::setConfig(Config *config) {
  _config = config;
}

const PropertyWrapper *
PropertyDelegate::getModel(const QAbstractItemModel *model) {
  while (const QAbstractProxyModel *proxy = qobject_cast<const QAbstractProxyModel *>(model))
    model = proxy->sourceModel();
  return qobject_cast<const PropertyWrapper*>(model);
}

QWidget *
PropertyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  Q_UNUSED(option)
  const PropertyWrapper *model = getModel(index.model());
  QMetaProperty prop = model->propertyAt(index);
  ConfigItem *obj = model->parentObject(index);

  if (! prop.isValid())
    return nullptr;

  // Dispatch by type
  if (prop.isEnumType()) {
    return new QComboBox(parent);
  } else if (QMetaType::Bool == prop.typeId()) {
    return new QComboBox(parent);
  } else if (QMetaType::Int == prop.typeId()) {
    QSpinBox *edit = new QSpinBox(parent);
    edit->setMinimum(std::numeric_limits<int>::min());
    edit->setMaximum(std::numeric_limits<int>::max());
    return edit;
  } else if (QMetaType::UInt == prop.typeId()) {
    QSpinBox *edit = new QSpinBox(parent);
    edit->setMinimum(0);
    edit->setMaximum(std::numeric_limits<int>::max());
    return edit;
  } else if (QMetaType::Double == prop.typeId()) {
    QLineEdit *edit = new QLineEdit(parent);
    edit->setValidator(new QDoubleValidator(edit));
    return edit;
  } else if (QMetaType::QString == prop.typeId()) {
    return new QLineEdit(parent);
  } else if (QString("Frequency") == prop.typeName()) {
    return new QLineEdit(parent);
  } else if (QString("Interval") == prop.typeName()) {
    return new QLineEdit(parent);
  } else if (prop.read(obj).value<ConfigObjectReference *>()) {
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
  } else if (QMetaType::Bool == prop.typeId()) {
    QComboBox *box = dynamic_cast<QComboBox *>(editor);
    box->addItem(tr("False"), false);
    box->addItem(tr("True"), true);
    if (prop.read(obj).toBool())
      box->setCurrentIndex(1);
    else
      box->setCurrentIndex(0);
  } else if ((QMetaType::Int == prop.typeId()) || (QMetaType::UInt == prop.typeId())) {
    dynamic_cast<QSpinBox *>(editor)->setValue(prop.read(obj).toInt());
  } else if (QMetaType::Double == prop.typeId()) {
    dynamic_cast<QLineEdit *>(editor)->setText(QString::number(prop.read(obj).toDouble()));
  } else if (QMetaType::QString == prop.typeId()) {
    dynamic_cast<QLineEdit *>(editor)->setText(prop.read(obj).toString());
  } else if (QString("Frequency") == prop.typeName()) {
    dynamic_cast<QLineEdit *>(editor)->setText(prop.read(obj).value<Frequency>().format());
  } else if (QString("Interval") == prop.typeName()) {
    dynamic_cast<QLineEdit *>(editor)->setText(prop.read(obj).value<Interval>().format());
  } else if (prop.read(obj).value<ConfigObjectReference *>()) {
    ConfigObjectReference *ref = prop.read(obj).value<ConfigObjectReference *>();
    // Find all matching elements in config that can be referenced
    QSet<ConfigItem *> items;
    if (_config)
      _config->findItemsOfTypes(ref->elementTypeNames(), items);
    // Assemble combo box with references
    QComboBox *box = dynamic_cast<QComboBox*>(editor);
    box->addItem(tr("[None]"), QVariant::fromValue<ConfigObject*>(nullptr));
    foreach (ConfigItem *item, items) {
      if (! item->is<ConfigObject>())
        continue;
      ConfigObject *o = item->as<ConfigObject>();
      box->addItem(o->name(),QVariant::fromValue<ConfigObject*>(o));
    }
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
  } else if (QMetaType::Bool == prop.typeId()) {
    prop.write(obj, dynamic_cast<QComboBox *>(editor)->currentData());
  } else if ((QMetaType::Int == prop.typeId()) || (QMetaType::UInt == prop.typeId())) {
    prop.write(obj, dynamic_cast<QSpinBox *>(editor)->value());
  } else if (QMetaType::Double == prop.typeId()) {
    prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text().toDouble());
  } else if (QMetaType::QString == prop.typeId()) {
    prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text());
  } else if (QString("Frequency") == prop.typeName()) {
    Frequency f;
    if (f.parse(dynamic_cast<QLineEdit *>(editor)->text()))
      prop.write(obj, QVariant::fromValue(f));
  } else if (QString("Interval") == prop.typeName()) {
    Interval I;
    if (I.parse(dynamic_cast<QLineEdit *>(editor)->text()))
      prop.write(obj, QVariant::fromValue(I));
  } else if (prop.read(obj).value<ConfigObjectReference *>()) {
    ConfigObjectReference *ref = prop.read(obj).value<ConfigObjectReference *>();
    ref->set(dynamic_cast<QComboBox *>(editor)->currentData().value<ConfigObject*>());
  }
}
