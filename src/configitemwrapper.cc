#include "configitemwrapper.hh"
#include <cmath>


/* ********************************************************************************************* *
 * Implementation of GenericListWrapper
 * ********************************************************************************************* */
GenericListWrapper::GenericListWrapper(AbstractConfigObjectList *list, QObject *parent)
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

bool
GenericListWrapper::moveUp(int row) {
  if ((0>=row) || (row>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  _list->moveUp(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveUp(int first, int last) {
  if ((0>=first) || (last>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), first-1);
  _list->moveUp(first, last);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveDown(int row) {
  if ((0>row) || ((row+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  _list->moveDown(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveDown(int first, int last) {
  if ((0>first) || ((last+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), last+2);
  _list->moveDown(first, last);
  endMoveRows();
  emit modified();
  return true;
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
GenericTableWrapper::GenericTableWrapper(AbstractConfigObjectList *list, QObject *parent)
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
ChannelListWrapper::ChannelListWrapper(ChannelList *list, QObject *parent)
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

  Channel *channel = dynamic_cast<ChannelList *>(_list)->channel(index.row());

  switch (index.column()) {
  case 0:
    if (channel->is<AnalogChannel>())
      return tr("Analog");
    else
      return tr("Digital");
  case 1:
    return channel->name();
  case 2:
    return formatFrequency(channel->rxFrequency());
  case 3:
    if (channel->txFrequency()<channel->rxFrequency())
      return formatFrequency(channel->txFrequency()-channel->rxFrequency());
    else
      return formatFrequency(channel->txFrequency());
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
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      switch (digi->admit()) {
      case DigitalChannel::AdmitNone: return tr("Always");
      case DigitalChannel::AdmitFree: return tr("Free");
      case DigitalChannel::AdmitColorCode: return tr("Color");
      }
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
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
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      return digi->colorCode();
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 10:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      return (DigitalChannel::TimeSlot1 == digi->timeslot()) ? 1 : 2;
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 11:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->rxGroupList()) {
        return digi->rxGroupList()->name();
      } else {
        return tr("-");
      }
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 12:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->txContact())
        return digi->txContact()->name();
      else
        return tr("-");
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 13:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->radioId())
        return digi->radioId()->id();
      else
        return tr("[Default]");
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 14:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->posSystem())
        return digi->posSystem()->name();
      else
        return tr("-");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (analog->aprsSystem())
        return analog->aprsSystem()->name();
      else
        return tr("-");
    }
    break;
  case 15:
    if (DigitalChannel *digi = channel->as<DigitalChannel>()) {
      if (digi->roaming())
        return digi->roaming()->name();
      else
        return tr("-");
    } else if (channel->is<AnalogChannel>()) {
      return tr("[None]");
    }
    break;
  case 16:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (0 == analog->squelch()) {
        return tr("Off");
      } else
        return analog->squelch();
    }
    break;
  case 17:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->rxTone()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->rxTone());
    }
    break;
  case 18:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->txTone()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->txTone());
    }
    break;
  case 19:
    if (channel->is<DigitalChannel>()) {
      return tr("[None]");
    } else if (AnalogChannel *analog = channel->as<AnalogChannel>()) {
      if (AnalogChannel::BWWide == analog->bandwidth()) {
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


/* ********************************************************************************************* *
 * Implementation of ContactListWrapper
 * ********************************************************************************************* */
ContactListWrapper::ContactListWrapper(ContactList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
ContactListWrapper::rowCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return _list->count();
}

int
ContactListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 4;
}

QVariant
ContactListWrapper::data(const QModelIndex &index, int role) const {
  if ((!index.isValid()) || (index.row()>=_list->count()))
    return QVariant();

  if (Qt::DisplayRole == role) {
    Contact *contact = _list->get(index.row())->as<Contact>();
    if (contact->is<DTMFContact>()) {
      DTMFContact *dtmf = contact->as<DTMFContact>();
      switch (index.column()) {
        case 0:
          return tr("DTMF");
        case 1:
          return dtmf->name();
        case 2:
          return dtmf->number();
        case 3:
          return (dtmf->rxTone() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    } else if (contact->is<DigitalContact>()) {
      DigitalContact *digi = contact->as<DigitalContact>();
      switch (index.column()) {
        case 0:
          switch (digi->type()) {
            case DigitalContact::PrivateCall: return tr("Private Call");
            case DigitalContact::GroupCall: return tr("Group Call");
            case DigitalContact::AllCall: return tr("All Call");
          }
        case 1:
          return digi->name();
        case 2:
          return digi->number();
        case 3:
          return (digi->rxTone() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    }
  }
  return QVariant();
}


QVariant
ContactListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation)) {
    return QVariant();
  }
  if (0 == section) {
    return tr("Type");
  } else if (1 == section) {
    return tr("Name");
  } else if (2 == section) {
    return tr("Number");
  } else if (3 == section) {
    return tr("RX Tone");
  }
  return QVariant();
}



/* ********************************************************************************************* *
 * Implementation of RadioIdListWrapper
 * ********************************************************************************************* */
RadioIdListWrapper::RadioIdListWrapper(RadioIDList *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}


int
RadioIdListWrapper::rowCount(const QModelIndex &parent) const {
  return _list->count();
}

QVariant
RadioIdListWrapper::data(const QModelIndex &index, int role) const {
  if (index.row() >= _list->count())
    return QVariant();
  RadioID *id = _list->get(index.row())->as<RadioID>();
  if (Qt::DisplayRole == role)
    return QVariant(QString::number(id->id()));
  else if (Qt::EditRole == role)
    return QVariant(id->id());
  return QVariant();
}
