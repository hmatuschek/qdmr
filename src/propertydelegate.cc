#include "propertydelegate.hh"
#include "extensionwrapper.hh"

#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include "configreference.hh"


PropertyDelegate::PropertyDelegate(QObject *parent)
  : QItemDelegate(parent)
{
  // pass...
}

QWidget *
PropertyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  const ExtensionWrapper *model = dynamic_cast<const ExtensionWrapper *>(index.model());
  if (model->isProperty(index)) {
    QMetaProperty prop = model->propertyAt(index);
    if (! prop.isValid())
      return nullptr;
    // Dispatch by type
    if (prop.isEnumType()) {
      return new QComboBox(parent);
    } else if (QString("bool") == prop.typeName()) {
      // should be handled by flags
      return QItemDelegate::createEditor(parent, option, index);
    } else if (QString("int") == prop.typeName()) {
      QSpinBox *edit = new QSpinBox(parent);
      edit->setMinimum(std::numeric_limits<int>::min());
      edit->setMaximum(std::numeric_limits<int>::max());
      return edit;
    } else if (QString("uint") == prop.typeName()) {
      QSpinBox *edit = new QSpinBox(parent);
      edit->setMinimum(0);
      edit->setMaximum(std::numeric_limits<int>::max());
      return edit;
    } else if (QString("double") == prop.typeName()) {
      QLineEdit *edit = new QLineEdit(parent);
      edit->setValidator(new QDoubleValidator(edit));
      return edit;
    } else if (QString("QString") == prop.typeName()) {
      return new QLineEdit(parent);
    } else if (prop.read(this).value<ConfigObjectReference *>()) {
      ConfigObjectReference *ref = prop.read(this).value<ConfigObjectReference *>();
      return new QComboBox(parent);
    }
  }
  return nullptr;
}

void
PropertyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  const ExtensionWrapper *model = dynamic_cast<const ExtensionWrapper *>(index.model());
  if (model->isProperty(index)) {
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
    } else if (QString("bool") == prop.typeName()) {
      // should be handled by flags
      return QItemDelegate::setEditorData(editor, index);
    } else if ((QString("int") == prop.typeName()) || (QString("uint") == prop.typeName())) {
      dynamic_cast<QSpinBox *>(editor)->setValue(prop.read(obj).toInt());
    } else if (QString("double") == prop.typeName()) {
      dynamic_cast<QLineEdit *>(editor)->setText(QString::number(prop.read(obj).toDouble()));
    } else if (QString("QString") == prop.typeName()) {
      dynamic_cast<QLineEdit *>(editor)->setText(prop.read(obj).toString());
    } else if (prop.read(this).value<ConfigObjectReference *>()) {
      QComboBox *box = dynamic_cast<QComboBox*>(editor);
      box->addItem(tr("[None]"));
    }
  }
}

void
PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *abstractmodel, const QModelIndex &index) const {
  ExtensionWrapper *model = dynamic_cast<ExtensionWrapper *>(abstractmodel);
  if (model->isProperty(index)) {
    ConfigItem *obj = model->parentObject(index);
    QMetaProperty prop = model->propertyAt(index);
    if (! prop.isValid())
      return;
    // Dispatch by type
    if (prop.isEnumType()) {
      prop.write(obj, dynamic_cast<QComboBox *>(editor)->currentData());
    } else if (QString("bool") == prop.typeName()) {
      // should be handled by flags
      return QItemDelegate::setModelData(editor, abstractmodel, index);
    } else if ((QString("int") == prop.typeName()) || (QString("uint") == prop.typeName())) {
      prop.write(obj, dynamic_cast<QSpinBox *>(editor)->value());
    } else if (QString("double") == prop.typeName()) {
      prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text().toDouble());
    } else if (QString("QString") == prop.typeName()) {
      prop.write(obj, dynamic_cast<QLineEdit *>(editor)->text());
    } else if (prop.read(this).value<ConfigObjectReference *>()) {
      QComboBox *box = dynamic_cast<QComboBox*>(editor);
      box->addItem(tr("[None]"));
    }
  }
}
