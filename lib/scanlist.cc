#include "scanlist.hh"
#include "channel.hh"
#include "config.hh"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>


/* ********************************************************************************************* *
 * Implementation of ScanList
 * ********************************************************************************************* */
ScanList::ScanList(QObject *parent)
  : ConfigObject(parent), _channels(), _primary(), _secondary(), _revert(), _tyt(nullptr)
{
  // Register "selected" channel tags for primary, secondary, revert and the channel list.
  Context::setTag(staticMetaObject.className(), "primary", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "secondary", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "revert", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "channels", "!selected", SelectedChannel::get());
}

ScanList::ScanList(const QString &name, QObject *parent)
  : ConfigObject(name, parent), _channels(), _primary(), _secondary(), _revert(), _tyt(nullptr)
{
  // Register "selected" channel tags for primary, secondary, revert and the channel list.
  Context::setTag(staticMetaObject.className(), "primary", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "secondary", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "revert", "!selected", SelectedChannel::get());
  Context::setTag(staticMetaObject.className(), "channels", "!selected", SelectedChannel::get());
}

ScanList &
ScanList::operator =(const ScanList &other) {
  copy(other);
  return *this;
}

ConfigItem *
ScanList::clone() const {
  ScanList *list = new ScanList();
  if (! list->copy(*this)) {
    list->deleteLater();
    return nullptr;
  }
  return list;
}

void
ScanList::clear() {
  _name.clear();
  _primary.clear();
  _secondary.clear();
  _revert.clear();
  _channels.clear();
  emit modified(this);
}

const ChannelRefList *
ScanList::channels() const {
  return &_channels;
}

ChannelRefList *
ScanList::channels() {
  return &_channels;
}

int
ScanList::count() const {
  return _channels.count();
}

bool
ScanList::contains(Channel *channel) const {
  return (0 <= _channels.indexOf(channel));
}

Channel *
ScanList::channel(int idx) const {
  return _channels.get(idx)->as<Channel>();
}

int
ScanList::addChannel(Channel *channel, int idx) {
  idx = _channels.add(channel, idx);
  if (0 > idx)
    return idx;
  return idx;
}

bool
ScanList::remChannel(int idx) {
  return _channels.del(_channels.get(idx));
  emit modified(this);
  return true;
}

bool
ScanList::remChannel(Channel *channel) {
  return _channels.del(channel);
}


const ChannelReference *
ScanList::primary() const {
  return &_primary;
}

ChannelReference *
ScanList::primary() {
  return &_primary;
}

Channel *
ScanList::primaryChannel() const {
  return _primary.as<Channel>();
}

void
ScanList::setPrimaryChannel(Channel *channel) {
  _primary.set(channel);
  emit modified(this);
}


const ChannelReference *
ScanList::secondary() const {
  return &_secondary;
}

ChannelReference *
ScanList::secondary() {
  return &_secondary;
}

Channel *
ScanList::secondaryChannel() const {
  return _secondary.as<Channel>();
}

void
ScanList::setSecondaryChannel(Channel *channel) {
  _secondary.set(channel);
  emit modified(this);
}


const ChannelReference *
ScanList::revert() const {
  return &_revert;
}

ChannelReference *
ScanList::revert() {
  return &_revert;
}

Channel *
ScanList::revertChannel() const {
  return _revert.as<Channel>();
}

void
ScanList::setRevertChannel(Channel *channel) {
  _revert.set(channel);
  emit modified(this);
}

TyTScanListExtension *
ScanList::tytScanListExtension() const {
  return _tyt;
}
void
ScanList::setTyTScanListExtension(TyTScanListExtension *tyt) {
  if (_tyt) {
    _tyt->deleteLater();
    _tyt = nullptr;
  }
  _tyt = tyt;
  if (_tyt)
    _tyt->setParent(this);
}


/* ********************************************************************************************* *
 * Implementation of ScanLists
 * ********************************************************************************************* */
ScanLists::ScanLists(QObject *parent)
  : ConfigObjectList(ScanList::staticMetaObject, parent)
{
  // pass...
}

ScanList *
ScanLists::scanlist(int idx) const {
  if (ConfigItem *obj = get(idx))
    return obj->as<ScanList>();
  return nullptr;
}

int
ScanLists::add(ConfigObject *obj, int row, bool unique) {
  if (obj && obj->is<ScanList>())
    return ConfigObjectList::add(obj, row, unique);
  return -1;
}

ConfigItem *
ScanLists::allocateChild(const YAML::Node &node, ConfigItem::Context &ctx, const ErrorStack &err) {
  Q_UNUSED(ctx);

  if (! node)
    return nullptr;

  if (! node.IsMap()) {
    errMsg(err) << node.Mark().line << ":" << node.Mark().column
                << ": Cannot create scan list: Expected object.";
    return nullptr;
  }

  return new ScanList();
}
