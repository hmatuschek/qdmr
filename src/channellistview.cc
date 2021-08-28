#include "channellistview.hh"
#include "config.hh"
#include <QPushButton>
#include <QMessageBox>
#include "analogchanneldialog.hh"
#include "digitalchanneldialog.hh"
#include "configitemwrapper.hh"


/* ********************************************************************************************* *
 * Implementation of ChannelListView
 * ********************************************************************************************* */
ChannelListView::ChannelListView(Config *config, QWidget *parent)
  : QWidget(parent), _config(config), _list(config->channelList()), _view(nullptr)
{
  QPushButton *up   = new QPushButton(QIcon("://icons/up.png"),"");
  QPushButton *down = new QPushButton(QIcon("://icons/down.png"), "");
  up->setToolTip(tr("Move channel up in list."));
  down->setToolTip(tr("Move channel down in list."));
  QPushButton *adda = new QPushButton(tr("Add Analog Channel"));
  QPushButton *addd = new QPushButton(tr("Add Digital Channel"));
  QPushButton *rem  = new QPushButton(tr("Delete Channel"));

  _view = new QTableView();
  _view->setModel(new ChannelListWrapper(_list, _view));
  _view->setColumnWidth(0, 60);
  _view->setColumnWidth(1, 120);
  _view->setColumnWidth(2, 80);
  _view->setColumnWidth(3, 80);
  _view->setColumnWidth(4, 50);
  _view->setColumnWidth(5, 60);
  _view->setColumnWidth(6, 50);
  _view->setColumnWidth(7, 60);
  _view->setColumnWidth(8, 120);
  _view->setColumnWidth(9, 30);
  _view->setColumnWidth(10, 30);
  _view->setColumnWidth(11, 120);
  _view->setColumnWidth(12, 120);
  _view->setColumnWidth(13, 60);
  _view->setColumnWidth(14, 60);
  _view->setColumnWidth(15, 60);
  _view->setColumnWidth(16, 60);

  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addWidget(_view,1);
  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addWidget(up);
  vbox->addWidget(down);
  hbox->addLayout(vbox);
  layout->addLayout(hbox);

  hbox = new QHBoxLayout();
  hbox->addWidget(adda);
  hbox->addWidget(addd);
  hbox->addWidget(rem);
  layout->addLayout(hbox);
  setLayout(layout);

  connect(up,   SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(down, SIGNAL(clicked()), this, SLOT(onChannelDown()));
  connect(adda, SIGNAL(clicked()), this, SLOT(onAddAnalogChannel()));
  connect(addd, SIGNAL(clicked()), this, SLOT(onAddDigitalChannel()));
  connect(rem,  SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditChannel(const QModelIndex &)));
}

void
ChannelListView::onChannelUp() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0==selected.row()))
    return;
  if (_config->channelList()->moveUp(selected.row()))
    _view->selectRow(selected.row()-1);
}

void
ChannelListView::onChannelDown() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (_config->channelList()->count()==(selected.row()+1)))
    return;
  if (_config->channelList()->moveDown(selected.row()))
    _view->selectRow(selected.row()+1);
}

void
ChannelListView::onAddAnalogChannel() {
  AnalogChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  _list->add(dialog.channel());
}

void
ChannelListView::onAddDigitalChannel() {
  DigitalChannelDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  _list->add(dialog.channel());
}

void
ChannelListView::onRemChannel() {
  QModelIndex selected =_view->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete channel"),
                             tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  QString name = _list->channel(selected.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete channel?"), tr("Delete channel %1?").arg(name)))
    return;

  _list->del(_list->get(selected.row()));
}

void
ChannelListView::onEditChannel(const QModelIndex &index) {
  Channel *channel = _config->channelList()->channel(index.row());
  if (! channel)
    return;
  if (channel->is<AnalogChannel>()) {
    AnalogChannelDialog dialog(_config, channel->as<AnalogChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else {
    DigitalChannelDialog dialog(_config, channel->as<DigitalChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  }
}


