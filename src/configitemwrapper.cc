#include "configitemwrapper.hh"
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of GenericListWrapper
 * ********************************************************************************************* */
GenericListWrapper::GenericListWrapper(Configuration::List *list, QObject *parent)
  : QAbstractListModel(parent), _list(list)
{
  if (nullptr == _list)
    return;

  connect(_list, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_list, SIGNAL(destroyed(QObject*)), this, SLOT(onListDeleted(QObject*)));
  connect(_list, SIGNAL(itemModified(int)), this, SLOT(onItemModified(int)));
  connect(_list, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
}

int
GenericListWrapper::rowCount(const QModelIndex &index) const {
  if (nullptr == _list)
    return 0;
  return _list->count();
}

int
GenericListWrapper::columnCount(const QModelIndex &index) const {
  if (nullptr == _list)
    return 0;
  return 1;
}


void
GenericListWrapper::onListDeleted() {
  beginResetModel();
  _list = nullptr;
  endResetModel();
}

void
GenericListWrapper::onItemRemoved(int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  endRemoveRows();
}

void
GenericListWrapper::onItemModified(int idx) {
  dataChanged(index(idx),index(idx));
}


/* ********************************************************************************************* *
 * Implementation of GenericTableWrapper
 * ********************************************************************************************* */
GenericTableWrapper::GenericTableWrapper(Configuration::List *list, QObject *parent)
  : QAbstractTableModel(parent), _list(list)
{
  if (nullptr == _list)
    return;

  connect(_list, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_list, SIGNAL(destroyed(QObject*)), this, SLOT(onListDeleted(QObject*)));
  connect(_list, SIGNAL(itemModified(int)), this, SLOT(onItemModified(int)));
  connect(_list, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
}

int
GenericTableWrapper::rowCount(const QModelIndex &index) const {
  if (nullptr == _list)
    return 0;
  return _list->count();
}

void
GenericTableWrapper::onListDeleted() {
  beginResetModel();
  _list = nullptr;
  endResetModel();
}

void
GenericTableWrapper::onItemRemoved(int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  endRemoveRows();
}

void
GenericTableWrapper::onItemModified(int idx) {
  dataChanged(index(idx,0),index(idx,columnCount()-1));
}


/* ********************************************************************************************* *
 * Implementation of ChannelListWrapper
 * ********************************************************************************************* */
ChannelListWrapper::ChannelListWrapper(Configuration::ChannelList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
ChannelListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 20;
}

inline QString formatFrequency(float f) {
  int val = std::round(f*10000);
  return QString::number(double(val)/10000, 'f', 4);
}

QVariant
ChannelListWrapper::data(const QModelIndex &index, int role) const {
  if (nullptr == _list)
    return QVariant();

  if ((! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  Configuration::Channel *channel = _list->get(index.row())->as<Configuration::Channel>();
  if (nullptr == channel)
    return QVariant();

  switch (index.column()) {
  case 0:
    if (channel->is<Configuration::AnalogChannel>())
      return tr("Analog");
    else
      return tr("Digital");
  case 1:
    return channel->name();
  case 2:
    return formatFrequency(channel->rx());
  case 3:
    if (channel->tx()<channel->rx())
      return formatFrequency(channel->tx()-channel->rx());
    else
      return formatFrequency(channel->tx());
  case 4:
    switch (channel->power()) {
    case Channel::MaxPower: return tr("Max");
    case Channel::HighPower: return tr("High");
    case Channel::MidPower: return tr("Mid");
    case Channel::LowPower: return tr("Low");
    case Channel::MinPower: return tr("Min");
    }
  case 5:
    if (0 == channel->txTimeout())
      return tr("-");
    return QString::number(channel->txTimeout());
  case 6:
    return channel->rxOnly() ? tr("On") : tr("Off");
  case 7:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      switch (digi->admit()) {
      case DigitalChannel::AdmitNone: return tr("Always");
      case DigitalChannel::AdmitFree: return tr("Free");
      case DigitalChannel::AdmitColorCode: return tr("Color");
      }
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      switch (analog->admit()) {
      case AnalogChannel::AdmitNone: return tr("Always");
      case AnalogChannel::AdmitFree: return tr("Free");
      case AnalogChannel::AdmitTone: return tr("Tone");
      }
    }
    break;
  case 8:
    if (channel->scanList()) {
      return channel->scanList()->name();
    } else {
      return tr("-");
    }
  case 9:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      return digi->colorCode();
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 10:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      return (DigitalChannel::TimeSlot1 == digi->timeSlot()) ? 1 : 2;
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 11:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      if (digi->groupList()) {
        return digi->groupList()->name();
      } else {
        return tr("-");
      }
    } else if (channel->is<Configuration::AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 12:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      if (digi->txContact())
        return digi->txContact()->name();
      else
        return tr("-");
    } else if (channel->is<Configuration::AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 13:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      if (digi->radioId())
        return digi->radioId()->number();
      else
        return tr("[Default]");
    } else if (channel->is<Configuration::AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 14:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      if (digi->aprs())
        return digi->aprs()->name();
      else
        return tr("-");
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      if (analog->aprs())
        return analog->aprs()->name();
      else
        return tr("-");
    }
    break;
  case 15:
    if (Configuration::DigitalChannel *digi = channel->as<Configuration::DigitalChannel>()) {
      if (digi->roamingZone())
        return digi->roamingZone()->name();
      else
        return tr("-");
    } else if (channel->is<Configuration::AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 16:
    if (channel->is<Configuration::DigitalChannel>()) {
      return tr("[None]");
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      if (0 == analog->squelch()) {
        return tr("Off");
      } else
        return analog->squelch();
    }
    break;
  case 17:
    if (channel->is<Configuration::DigitalChannel>()) {
      return tr("[None]");
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->rxSignalling()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->rxSignalling());
    }
    break;
  case 18:
    if (channel->is<Configuration::DigitalChannel>()) {
      return tr("[None]");
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->txSignalling()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->txSignalling());
    }
    break;
  case 19:
    if (channel->is<Configuration::DigitalChannel>()) {
      return tr("[None]");
    } else if (Configuration::AnalogChannel *analog = channel->as<Configuration::AnalogChannel>()) {
      if (AnalogChannel::BWWide == analog->bandWidth()) {
        return tr("Wide");
      } else
        return tr("Narrow");
    }
    break;

  default:
    break;
  }

  return QVariant();
}

QVariant
ChannelListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Type");
  case 1: return tr("Name");
  case 2: return tr("Rx Frequency");
  case 3: return tr("Tx Frequency");
  case 4: return tr("Power");
  case 5: return tr("Timeout");
  case 6: return tr("Rx Only");
  case 7: return tr("Admit");
  case 8: return tr("Scanlist");
  case 9: return tr("CC");
  case 10: return tr("TS");
  case 11: return tr("RX Group List");
  case 12: return tr("TX Contact");
  case 13: return tr("DMR ID");
  case 14: return tr("GPS/APRS");
  case 15: return tr("Roaming");
  case 16: return tr("Squelch");
  case 17: return tr("Rx Tone");
  case 18: return tr("Tx Tone");
  case 19: return tr("Bandwidth");
    default:
      break;
  }
  return QVariant();
}
