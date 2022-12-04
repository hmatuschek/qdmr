#include "radio.hh"

#include "anytone_interface.hh"
#include "radioddity_interface.hh"
#include "tyt_interface.hh"
#include "dr1801uv_interface.hh"

#include "rd5r.hh"
#include "gd77.hh"
#include "md390.hh"
#include "uv390.hh"
#include "md2017.hh"
#include "dm1701.hh"
#include "opengd77.hh"
#include "d868uv.hh"
#include "d878uv.hh"
#include "d878uv2.hh"
#include "d578uv.hh"
#include "dr1801uv.hh"

#include "config.hh"
#include "logger.hh"

#include <QSet>


/* ******************************************************************************************** *
 * Implementation of Radio
 * ******************************************************************************************** */
Radio::Radio(QObject *parent)
  : QThread(parent), _task(StatusIdle)
{
  // pass...
}

Radio::~Radio() {
  // pass...
}

const CallsignDB *
Radio::callsignDB() const {
  return nullptr;
}

CallsignDB *
Radio::callsignDB() {
  return nullptr;
}


Radio *
Radio::detect(const USBDeviceDescriptor &descr, const RadioInfo &force, const ErrorStack &err) {
  if (! descr.isValid()) {
    errMsg(err) << "Cannot detect radio: Invalid interface descriptor.";
    return nullptr;
  }
  logDebug() << "Try to detect radio at " << descr.description() << ".";

  if (AnytoneInterface::interfaceInfo() == descr) {
    AnytoneInterface *anytone = new AnytoneInterface(descr, err);
    if (anytone->isOpen()) {
      RadioInfo id = anytone->identifier(err);
      if ((id.isValid() && (RadioInfo::D868UVE == id.id())) || (force.isValid() && (RadioInfo::D868UVE == force.id()))) {
        return new D868UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UV == id.id())) || (force.isValid() && (RadioInfo::D878UV == force.id()))) {
        return new D878UV(anytone);
      } else if ((id.isValid() && (RadioInfo::D878UVII == id.id())) || (force.isValid() && (RadioInfo::D878UVII == force.id()))) {
        return new D878UV2(anytone);
      } else if ((id.isValid() && (RadioInfo::D578UV == id.id())) || (force.isValid() && (RadioInfo::D578UV == force.id()))) {
        return new D578UV(anytone);
      } else if (id.isValid()) {
        errMsg(err) << tr("Unhandled device %1 '%2'. Device known but not implemented yet.")
                       .arg(id.manufacturer())
                       .arg(id.name());
      } else {
        errMsg(err) << tr("Unknown AnyTone (or similar) device.");
      }
      anytone->close();
      anytone->deleteLater();
      return nullptr;
    }
    anytone->deleteLater();
    return nullptr;
  } else if (OpenGD77Interface::interfaceInfo() == descr) {
    OpenGD77Interface *ogd77 = new OpenGD77Interface(descr, err);
    if (ogd77->isOpen()) {
      RadioInfo id = ogd77->identifier();
      if ((id.isValid() && (RadioInfo::OpenGD77 == id.id())) || (force.isValid() && (RadioInfo::OpenGD77 == force.id()))) {
        return new OpenGD77(ogd77);
      } else {
        errMsg(err) << "Unhandled device " << id.manufacturer() << " " << id.name()
                    << ". Device known but not implemented yet.";
      }
      ogd77->close();
      ogd77->deleteLater();
      return nullptr;
    }
    ogd77->deleteLater();
    return nullptr;
  } else if (TyTInterface::interfaceInfo() == descr) {
    TyTInterface *dfu = new TyTInterface(descr, err);
    if (dfu->isOpen()) {
      RadioInfo id = dfu->identifier();
      if ((id.isValid() && (RadioInfo::MD390 == id.id())) || (force.isValid() && (RadioInfo::MD390 == force.id()))) {
        return new MD390(dfu);
      } else if ((id.isValid() && (RadioInfo::UV390 == id.id())) || (force.isValid() && (RadioInfo::UV390 == force.id()))) {
        return new UV390(dfu);
      } else if ((id.isValid() && (RadioInfo::MD2017 == id.id())) || (force.isValid() && (RadioInfo::MD2017 == force.id()))) {
        return new MD2017(dfu);
      } else if ((id.isValid() && (RadioInfo::DM1701 == id.id())) || (force.isValid() && (RadioInfo::DM1701 == force.id()))) {
        logDebug() << "Create DM-1701 radio object.";
        return new DM1701(dfu);
      } else {
        errMsg(err) << "Unhandled device " << id.manufacturer() << " " << id.name()
                    << ". Device known but not implemented yet.";
      }
      dfu->close();
      dfu->deleteLater();
      return nullptr;
    }
    dfu->deleteLater();
    return nullptr;
  } else if (RadioddityInterface::interfaceInfo() == descr) {
    RadioddityInterface *hid = new RadioddityInterface(descr, err);
    if (hid->isOpen()) {
      RadioInfo id = hid->identifier();
      if ((id.isValid() && (RadioInfo::RD5R == id.id())) || (force.isValid() && (RadioInfo::RD5R == force.id()))) {
        return new RD5R(hid);
      } else if ((id.isValid() && (RadioInfo::GD77 == id.id())) || (force.isValid() && (RadioInfo::GD77 == force.id()))) {
        return new GD77(hid);
      } else if (id.isValid()) {
        errMsg(err) << "Unhandled device " << id.manufacturer() << " " << id.name()
                    << ". Device known but not implemented yet.";
      } else {
        errMsg(err) << "Unhandled device " << id.manufacturer() << " " << id.name()
                    << ". Device not known.";
      }
      hid->close();
      hid->deleteLater();
      return nullptr;
    }
    hid->deleteLater();
    return nullptr;
  } else if (DR1801UVInterface::interfaceInfo() == descr) {
    DR1801UVInterface *dif = new DR1801UVInterface(descr, err);
    if (dif->isOpen()) {
      RadioInfo id = dif->identifier(err);
      if (((id.isValid()) && (RadioInfo::DR1801UV == id.id())) ||
          (force.isValid() && (RadioInfo::DR1801UV==force.id()))) {
        return new DR1801UV(dif);
      } else if (id.isValid()) {
        errMsg(err) << "Unhandled device " << id.manufacturer() << " " << id.name()
                    << ". Device known but not implemented yet.";
      } else {
        errMsg(err) << "Unknown device or failed connection to the device.";
      }
      dif->close();
      dif->deleteLater();
      return nullptr;
    }
    dif->deleteLater();
    return nullptr;
  }
  return nullptr;
}

Radio::Status
Radio::status() const {
  return _task;
}

const ErrorStack &
Radio::errorStack() const {
  return _errorStack;
}
