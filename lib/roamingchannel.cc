#include "roamingchannel.hh"

/* ********************************************************************************************* *
 * Implementation of RoamingChannel
 * ********************************************************************************************* */
RoamingChannel::RoamingChannel(QObject *parent)
  : ConfigObject("rc", parent), _rxFrequency(0), _txFrequency(0), _overrideColorCode(false),
    _colorCode(0), _overrideTimeSlot(false), _timeSlot(DMRChannel::TimeSlot::TS1)
{
  // pass...
}

RoamingChannel::RoamingChannel(const RoamingChannel &other, QObject *parent)
  : ConfigObject("rc", parent)
{
  copy(other);
}

void
RoamingChannel::clear() {
  ConfigObject::clear();
  _rxFrequency = _txFrequency = 0;
  _overrideColorCode = false; _colorCode = 0;
  _overrideTimeSlot = false; _timeSlot = DMRChannel::TimeSlot::TS1;
}

ConfigItem *
RoamingChannel::clone() const {
  RoamingChannel *c = new RoamingChannel();
  if (! c->copy(*this)) {
    c->deleteLater();
    return nullptr;
  }
  return c;
}

double
RoamingChannel::rxFrequency() const {
  return _rxFrequency;
}
void
RoamingChannel::setRXFrequency(double f) {
  if (f == _rxFrequency)
    return;
  _rxFrequency = f;
  emit modified(this);
}

double
RoamingChannel::txFrequency() const {
  return _txFrequency;
}
void
RoamingChannel::setTXFrequency(double f) {
  if (f == _txFrequency)
    return;
  _txFrequency = f;
  emit modified(this);
}

bool
RoamingChannel::colorCodeOverridden() const {
  return _overrideColorCode;
}
void
RoamingChannel::overrideColorCode(bool override) {
  if (override == _overrideColorCode)
    return;
  _overrideColorCode = override;
  emit modified(this);
}
unsigned int
RoamingChannel::colorCode() const {
  return _colorCode;
}
void
RoamingChannel::setColorCode(unsigned int cc) {
  cc = std::min(15U, cc);
  if (_colorCode == cc)
    return;
  _colorCode = cc;
  emit modified(this);
}

bool
RoamingChannel::timeSlotOverridden() const {
  return _overrideTimeSlot;
}
void
RoamingChannel::overrideTimeSlot(bool override) {
  if (override == _overrideTimeSlot)
    return;
  _overrideTimeSlot = override;
  emit modified(this);
}
DMRChannel::TimeSlot
RoamingChannel::timeSlot() const {
  return _timeSlot;
}
void
RoamingChannel::setTimeSlot(DMRChannel::TimeSlot ts) {
  if (_timeSlot == ts)
    return;
  _timeSlot = ts;
  emit modified(this);
}

RoamingChannel *
RoamingChannel::fromDMRChannel(DMRChannel *ch, DMRChannel* ref) {
  RoamingChannel *rch = new RoamingChannel();
  rch->setName(QString("R %1").arg(ch->name()));
  rch->setRXFrequency(ch->rxFrequency());
  rch->setTXFrequency(ch->txFrequency());
  rch->overrideColorCode(true);
  rch->setColorCode(ch->colorCode());
  rch->overrideTimeSlot(true);
  rch->setTimeSlot(ch->timeSlot());
  if (nullptr != ref) {
    if (ch->colorCode() == ref->colorCode())
      rch->overrideColorCode(false);
    if (ch->timeSlot() == ref->timeSlot())
      rch->overrideTimeSlot(false);
  }
  return rch;
}

bool
RoamingChannel::parse(const YAML::Node &node, Context &ctx, const ErrorStack &err) {
  if (! ConfigObject::parse(node, ctx, err))
    return false;

  if (! node["timeSlot"]) {
    this->overrideTimeSlot(false);
  } else {
    if (! node["timeSlot"].IsScalar()) {
      errMsg(err) << node["timeSlot"].Mark().line << ":" << node["timeSlot"].Mark().column
                  << "Cannot parse 'timeSlot' of RoamingChannel: time slot is not scalar.";
      return false;
    }
    QMetaEnum e = QMetaEnum::fromType<DMRChannel::TimeSlot>();
    std::string key = node["timeSlot"].as<std::string>();
    bool ok=true; int value = e.keyToValue(key.c_str(), &ok);
    if (! ok) {
      QStringList lst;
      for (int i=0; i<e.keyCount(); i++)
        lst.push_back(e.key(i));
      errMsg(err) << node["timeSlot"].Mark().line << ":" << node["timeSlot"].Mark().column
                  << ": Unknown key '" << key.c_str() << "' for enum 'DMRChannel::TimeSlot'."
                  << " Expected one of " << lst.join(", ") << ".";
      return false;
    }

    // finally set property
    this->overrideTimeSlot(true);
    this->setTimeSlot((DMRChannel::TimeSlot)value);
  }

  if (! node["colorCode"]) {
    this->overrideTimeSlot(false);
  } else {
    if (! node["colorCode"].IsScalar()) {
      errMsg(err) << node["colorCode"].Mark().line << ":" << node["colorCode"].Mark().column
                  << "Cannot parse 'colorCode' of RoamingChannel: color code is not scalar.";
      return false;
    }
    // finally set property
    this->overrideColorCode(true);
    this->setColorCode(node["colorCode"].as<unsigned>());
  }

  return true;
}


bool
RoamingChannel::populate(YAML::Node &node, const Context &context, const ErrorStack &err) {
  // First, populate scriptable properties
  if (! ConfigObject::populate(node, context, err))
    return false;

  if (timeSlotOverridden()) {
    QMetaEnum e = QMetaEnum::fromType<DMRChannel::TimeSlot>();
    node["timeSlot"] = e.valueToKey((int)timeSlot());
  }

  if (colorCodeOverridden()) {
    node["colorCode"] = colorCode();
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RoamingChannelList
 * ********************************************************************************************* */
RoamingChannelList::RoamingChannelList(QObject *parent)
  : ConfigObjectList(RoamingChannel::staticMetaObject, parent)
{
  // pass...
}

RoamingChannel *
RoamingChannelList::channel(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<RoamingChannel>();
  return nullptr;
}

int
RoamingChannelList::add(ConfigObject *obj, int row) {
  if (obj && obj->is<RoamingChannel>())
    return ConfigObjectList::add(obj, row);
  return -1;
}

ConfigItem *
RoamingChannelList::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx)

  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create roaming zone: Expected object.";
    return nullptr;
  }

  return new RoamingChannel();
}
