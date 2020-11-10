#include "channelcombobox.hh"
#include "channel.hh"
#include <QCompleter>


/* ********************************************************************************************* *
 * Implementation of ChannelComboBox
 * ********************************************************************************************* */
ChannelComboBox::ChannelComboBox(ChannelList *list, bool includeSelectedChannel, QWidget *parent)
  : QComboBox(parent)
{
  setInsertPolicy(QComboBox::NoInsert);
  setEditable(true);
  completer()->setCompletionMode(QCompleter::PopupCompletion);
  if (includeSelectedChannel)
    addItem(SelectedChannel::get()->name(), QVariant::fromValue(SelectedChannel::get()));
  for (int i=0; i<list->rowCount(QModelIndex()); i++)
    addItem(list->channel(i)->name(), QVariant::fromValue(list->channel(i)));
}

Channel *
ChannelComboBox::channel() const {
  return currentData().value<Channel*>();
}


