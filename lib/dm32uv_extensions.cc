#include "dm32uv_extensions.hh"


DM32UVButtonSettingsExtension::DM32UVButtonSettingsExtension(QObject *parent)
  : ConfigItem(parent), _longPressDuration(Interval::fromSeconds(1)),
    _sk1Short(Function::None), _sk1Long(Function::None),
    _sk2Short(Function::None), _sk2Long(Function::None),
    _p1Short(Function::None), _p1Long(Function::None),
    _p2Short(Function::None), _p2Long(Function::None),
    _sideKeyLock(false)
{
}

ConfigItem *
DM32UVButtonSettingsExtension::clone() const {
  auto *copy = new DM32UVButtonSettingsExtension();
  if (!copy->copy(*this)) {
    delete copy;
    return nullptr;
  }
  return copy;
}

#define DM32UV_BUTTON_ACCESSORS(name, setter, type) \
  type DM32UVButtonSettingsExtension::name() const { return _##name; } \
  void DM32UVButtonSettingsExtension::setter(type value) { \
    if (_##name == value) return; \
    _##name = value; \
    emit modified(this); \
  }

DM32UV_BUTTON_ACCESSORS(longPressDuration, setLongPressDuration, Interval)
DM32UV_BUTTON_ACCESSORS(sk1Short, setSK1Short, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(sk1Long, setSK1Long, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(sk2Short, setSK2Short, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(sk2Long, setSK2Long, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(p1Short, setP1Short, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(p1Long, setP1Long, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(p2Short, setP2Short, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(p2Long, setP2Long, DM32UVButtonSettingsExtension::Function)
DM32UV_BUTTON_ACCESSORS(sideKeyLock, enableSideKeyLock, bool)

#undef DM32UV_BUTTON_ACCESSORS


DM32UVSettingsExtension::DM32UVSettingsExtension(QObject *parent)
  : ConfigExtension(parent), _buttons(new DM32UVButtonSettingsExtension(this))
{
  connect(_buttons, &DM32UVButtonSettingsExtension::modified,
          this, &DM32UVSettingsExtension::modified);
}

ConfigItem *
DM32UVSettingsExtension::clone() const {
  auto *copy = new DM32UVSettingsExtension();
  if (!copy->copy(*this)) {
    delete copy;
    return nullptr;
  }
  return copy;
}

DM32UVButtonSettingsExtension *
DM32UVSettingsExtension::buttons() const {
  return _buttons;
}
