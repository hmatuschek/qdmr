#include "aprsselect.hh"
#include "gpssystem.hh"
#include "config.hh"

/* ******************************************************************************************* *
 * FM APRS selection
 * ******************************************************************************************* */
FMAPRSSelect::FMAPRSSelect(Config *config, QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("[None]"), QVariant::fromValue<FMAPRSSystem *>(nullptr));
  for (int i=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<FMAPRSSystem>()) {
      auto sys = config->posSystems()->system(i)->as<FMAPRSSystem>();
      addItem(sys->name(), QVariant::fromValue(sys));
    }
  }
}


void
FMAPRSSelect::setAPRSSystem(FMAPRSSystem *sys) {
  for (int i=0; i<count(); i++) {
    if (itemData(i).value<FMAPRSSystem*>() == sys) {
      setCurrentIndex(i);
      break;
    }
  }
}

FMAPRSSystem *
FMAPRSSelect::aprsSystem() const {
  return currentData().value<FMAPRSSystem*>();
}



/* ******************************************************************************************* *
 * Any (FM & DMR) APRS selection
 * ******************************************************************************************* */
APRSSelect::APRSSelect(Config *config, QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("[None]"), QVariant::fromValue<PositionReportingSystem *>(nullptr));
  for (int i=0; i<config->posSystems()->count(); i++) {
    if (config->posSystems()->system(i)->is<PositionReportingSystem>()) {
      auto sys = config->posSystems()->system(i)->as<PositionReportingSystem>();
      addItem(sys->name(), QVariant::fromValue(sys));
    }
  }
}


void
APRSSelect::setAPRSSystem(PositionReportingSystem *sys) {
  for (int i=0; i<count(); i++) {
    if (itemData(i).value<PositionReportingSystem*>() == sys) {
      setCurrentIndex(i);
      break;
    }
  }
}

PositionReportingSystem *
APRSSelect::aprsSystem() const {
  return currentData().value<PositionReportingSystem*>();
}
