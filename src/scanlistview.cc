#include "scanlistview.hh"

#include "config.hh"
#include "scanlistdialog.hh"
#include "configitemwrapper.hh"
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>


/* ********************************************************************************************* *
 * Implementation of ScanListView
 * ********************************************************************************************* */
ScanListsView::ScanListsView(Config *config, QWidget *parent)
  : QWidget(parent), _config(config)
{
  _view = new QListView();
  _view->setModel(new ScanListsWrapper(_config->scanlists()));
  connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditScanList(const QModelIndex &)));

  QPushButton *add = new QPushButton(tr("Add Scanlist"));
  QPushButton *rem = new QPushButton(tr("Delete Scanlist"));
  connect(add, SIGNAL(clicked()), this, SLOT(onAddScanList()));
  connect(rem, SIGNAL(clicked()), this, SLOT(onRemScanList()));

  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->addWidget(add);
  bbox->addWidget(rem);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_view);
  layout->addLayout(bbox);

  setLayout(layout);
}

void
ScanListsView::onAddScanList() {
  ScanListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  ScanList *scanlist = dialog.scanlist();
  _config->scanlists()->add(scanlist);
}

void
ScanListsView::onRemScanList() {
  QModelIndex idx = _view->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete scanlist"),
                             tr("Cannot delete scanlist: You have to select a scanlist first."));
    return;
  }

  QString name = _config->scanlists()->scanlist(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete scanlist?"), tr("Delete scanlist %1?").arg(name)))
    return;

  _config->scanlists()->del(_config->scanlists()->scanlist(idx.row()));
}

void
ScanListsView::onEditScanList(const QModelIndex &idx) {
  if (idx.row()>=_config->scanlists()->count())
    return;

  ScanListDialog dialog(_config, _config->scanlists()->scanlist(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.scanlist();

  emit _view->model()->dataChanged(idx,idx);
}


