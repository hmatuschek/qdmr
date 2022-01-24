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
  connect(ui->create, SIGNAL(clicked(bool)), this, SLOT(onCreateExtension()));
  connect(ui->remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteExtension()));
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
ExtensionView::setObject(ConfigItem *obj) {
  if (nullptr != _model)
    _model->deleteLater();
  _model = new PropertyWrapper(obj, this);
  _proxy.setSourceModel(_model);
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

  ConfigItem *obj = _model->parentObject(_proxy.mapToSource(row));
  QMetaProperty prop = _model->propertyAt(_proxy.mapToSource(row));
  if ((nullptr == obj) || (! prop.isValid())) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(false);
    return;
  }

  if (! propIsInstance<ConfigItem>(prop)) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(false);
  } else if (prop.read(obj).value<ConfigItem*>()) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(prop.isWritable());
  } else {
    ui->create->setEnabled(prop.isWritable());
    ui->remove->setEnabled(false);
  }
}

void
ExtensionView::onCreateExtension() {
  if ((! ui->view->selectionModel()->hasSelection()) || (nullptr == _model))
    return;
  QModelIndex item = _proxy.mapToSource(
        ui->view->selectionModel()->selectedRows(0).first());
  if (! _model->createInstanceAt(item))
    QMessageBox::critical(nullptr, tr("Cannot create extension."),
                          tr("Cannot create extension, consider reporting a bug."));
  ui->view->selectionModel()->clearSelection();
}

void
ExtensionView::onDeleteExtension() {
  if ((! ui->view->selectionModel()->hasSelection()) || (nullptr == _model))
    return;
  QModelIndex item = _proxy.mapToSource(
        ui->view->selectionModel()->selectedRows(0).first());
  _model->deleteInstanceAt(item);
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
