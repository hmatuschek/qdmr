#include "extensionview.hh"
#include "ui_extensionview.h"
#include "propertydelegate.hh"
#include "extensionwrapper.hh"
#include "settings.hh"
#include <QMessageBox>


ExtensionView::ExtensionView(QWidget *parent) :
  QWidget(parent), ui(new Ui::ExtensionView), _model(nullptr)
{
  ui->setupUi(this);
  ui->view->setModel(&_proxy);
  ui->view->setItemDelegateForColumn(1, &_editor);
  ui->view->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);

  ui->create->setEnabled(false);
  ui->remove->setEnabled(false);

  connect(ui->view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  connect(ui->create, SIGNAL(clicked(bool)), this, SLOT(onCreate()));
  connect(ui->remove, SIGNAL(clicked(bool)), this, SLOT(onDelete()));
  connect(ui->view->header(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadSectionState()));
  connect(ui->view->header(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeSectionState()));
}

ExtensionView::~ExtensionView()
{
  delete ui;
}

void
ExtensionView::setObject(ConfigItem *obj, Config *context) {
  if (nullptr != _model)
    _model->deleteLater();
  _model = new PropertyWrapper(obj, this);
  _proxy.setSourceModel(_model);
  _editor.setConfig(context);
}

void
ExtensionView::onSelectionChanged(const QItemSelection &current, const QItemSelection &last) {
  Q_UNUSED(last)
  // If nothing is selected disable both
  if (current.isEmpty() || (nullptr == _model)) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(false);
    return;
  }

  // Get selected row
  QModelIndex row = ui->view->selectionModel()->selectedRows().first();
  if (! row.isValid()) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(false);
    return;
  }

  if (_model->isProperty(_proxy.mapToSource(row))) {
    ConfigItem *obj = _model->parentObject(_proxy.mapToSource(row));
    QMetaProperty prop = _model->propertyAt(_proxy.mapToSource(row));
    if ((nullptr == obj) || (! prop.isValid())) {
      ui->create->setEnabled(false);
      ui->remove->setEnabled(false);
      return;
    }

    if (propIsInstance<ConfigItem>(prop)) {
      ui->create->setEnabled(false);
      ui->remove->setEnabled(false);
      if (prop.read(obj).value<ConfigItem*>()) {
        ui->create->setEnabled(false);
        ui->remove->setEnabled(prop.isWritable());
      } else {
        ui->create->setEnabled(prop.isWritable());
        ui->remove->setEnabled(false);
      }
    } else if (propIsInstance<ConfigObjectList>(prop)) {
      ui->create->setEnabled(true);
      ui->remove->setEnabled(false);
    }
  } else if (_model->isListElement(_proxy.mapToSource(row))) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(true);
  }
}

void
ExtensionView::onCreate() {
  if ((! ui->view->selectionModel()->hasSelection()) || (nullptr == _model))
    return;

  QModelIndex item = _proxy.mapToSource(
        ui->view->selectionModel()->selectedRows(0).first());

  if (! _model->isProperty(item))
    return;

  QMetaProperty prop = _model->propertyAt(item);
  ConfigItem *obj = _model->parentObject(item);
  if ((nullptr == obj) || (! prop.isValid()))
    return;

  if (propIsInstance<ConfigItem>(prop) && (! _model->createInstanceAt(item))) {
    QMessageBox::critical(nullptr, tr("Cannot create extension."),
                          tr("Cannot create extension, consider reporting a bug."));
    return;
  } else if (propIsInstance<ConfigObjectList>(prop) && !_model->createElementAt(item)) {
    QMessageBox::critical(nullptr, tr("Cannot create list element."),
                          tr("Cannot create list element, consider reporting a bug."));
    return;
  }

  ui->view->selectionModel()->clearSelection();
}

void
ExtensionView::onDelete() {
  if ((! ui->view->selectionModel()->hasSelection()) || (nullptr == _model))
    return;
  QModelIndex item = _proxy.mapToSource(
        ui->view->selectionModel()->selectedRows(0).first());
  if (_model->isProperty(item))
    _model->deleteInstanceAt(item);
  else if (_model->isListElement(item))
    _model->deleteElementAt(item);
  ui->view->selectionModel()->clearSelection();
}

void
ExtensionView::loadSectionState() {
  Settings settings;
  ui->view->header()->restoreState(settings.headerState(this->objectName()));
}

void
ExtensionView::storeSectionState() {
  Settings settings;
  settings.setHeaderState(this->objectName(), ui->view->header()->saveState());
}
