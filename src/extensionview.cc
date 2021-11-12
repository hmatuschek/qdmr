#include "extensionview.hh"
#include "ui_extensionview.h"
#include "propertydelegate.hh"
#include "extensionwrapper.hh"
#include <QMessageBox>


ExtensionView::ExtensionView(QWidget *parent) :
  QWidget(parent), ui(new Ui::ExtensionView), _model(nullptr)
{
  ui->setupUi(this);
  ui->view->setModel(&_proxy);
  ui->view->setItemDelegateForColumn(1, &_editor);
  ui->view->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);

  connect(ui->view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
          this, SLOT(onSelectedRowChanged(QModelIndex,QModelIndex)));
  connect(ui->create, SIGNAL(clicked(bool)), this, SLOT(onCreateExtension()));
  connect(ui->remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteExtension()));
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
ExtensionView::onSelectedRowChanged(const QModelIndex &current, const QModelIndex &last) {
  Q_UNUSED(last)
  if (nullptr == _model)
    return;

  ConfigItem *obj = _model->parentObject(_proxy.mapToSource(current));
  QMetaProperty prop = _model->propertyAt(_proxy.mapToSource(current));
  if ((nullptr == obj) || (! prop.isValid()))
    return;
  if (! propIsInstance<ConfigItem>(prop)) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(false);
  } else if (prop.read(obj).value<ConfigItem*>()) {
    ui->create->setEnabled(false);
    ui->remove->setEnabled(true);
  } else {
    ui->create->setEnabled(true);
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
}

void
ExtensionView::onDeleteExtension() {
  if ((! ui->view->selectionModel()->hasSelection()) || (nullptr == _model))
    return;
  QModelIndex item = _proxy.mapToSource(
        ui->view->selectionModel()->selectedRows(0).first());
  _model->deleteInstanceAt(item);
}
