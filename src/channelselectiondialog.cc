#include "channelselectiondialog.hh"
#include "channel.hh"
#include "channelcombobox.hh"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>


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
