#include "roamingchannelselectiondialog.hh"
#include "config.hh"

#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>

MultiRoamingChannelSelectionDialog::MultiRoamingChannelSelectionDialog(
    Config *config, QWidget *parent)
  : QDialog(parent), _channels(nullptr)
{
  setWindowTitle(tr("Select roaming channels"));
  _channels = new QListWidget();
  for (int i=0; i<config->roamingChannels()->count(); i++) {
    QListWidgetItem *item = new QListWidgetItem(config->roamingChannels()->channel(i)->name());
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, QVariant::fromValue(config->roamingChannels()->channel(i)));
    item->setCheckState(Qt::Unchecked);
    _channels->addItem(item);
  }

  QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_channels);
  layout->addWidget(box);
  setLayout(layout);

  connect(box, SIGNAL(accepted()), this, SLOT(accept()));
  connect(box, SIGNAL(rejected()), this, SLOT(reject()));
}

QList<RoamingChannel *>
MultiRoamingChannelSelectionDialog::channels() const {
  QList<RoamingChannel *> channels;
  for (int i=0; i<_channels->count(); i++) {
    if (Qt::Checked ==_channels->item(i)->checkState())
      channels.push_back(_channels->item(i)->data(Qt::UserRole).value<RoamingChannel*>());
  }
  return channels;
}
