#include "channelselectiondialog.hh"
#include "channel.hh"
#include "channelcombobox.hh"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>


/* ********************************************************************************************* *
 * Implementation of ChannelSelectionDialog
 * ********************************************************************************************* */
ChannelSelectionDialog::ChannelSelectionDialog(ChannelList *lst, bool includeSelectedChannel, QWidget *parent)
  : QDialog(parent)
{
  _channel = new ChannelComboBox(lst, includeSelectedChannel);
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(new QLabel(tr("Select a channel:")));
  layout->addWidget(_channel);
  layout->addWidget(bbox);
  setLayout(layout);
}

Channel *
ChannelSelectionDialog::channel() const {
  return _channel->channel();
}


/* ********************************************************************************************* *
 * Implementation of MultiChannelSelectionDialog
 * ********************************************************************************************* */
MultiChannelSelectionDialog::MultiChannelSelectionDialog(ChannelList *lst, bool includeSelectedChannel, QWidget *parent)
  : QDialog(parent)
{
  _channel = new QListWidget();
  if (includeSelectedChannel) {
    QListWidgetItem *item = new QListWidgetItem(tr("[Selected]"));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, QVariant::fromValue(SelectedChannel::get()));
    item->setCheckState(Qt::Unchecked);
    _channel->addItem(item);
  }
  for (int i=0; i<lst->count(); i++) {
    Channel *channel = lst->channel(i);
    QListWidgetItem *item = new QListWidgetItem(channel->name());
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, QVariant::fromValue(channel));
    item->setCheckState(Qt::Unchecked);
    _channel->addItem(item);
  }
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(new QLabel(tr("Select a channel:")));
  layout->addWidget(_channel);
  layout->addWidget(bbox);
  setLayout(layout);
}

QList<Channel *>
MultiChannelSelectionDialog::channel() const {
  QList<Channel *> channels;
  for (int i=0; i<_channel->count(); i++) {
    if (Qt::Checked == _channel->item(i)->checkState())
      channels.push_back(_channel->item(i)->data(Qt::UserRole).value<Channel *>());
  }
  return channels;
}

