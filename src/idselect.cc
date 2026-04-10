#include "idselect.hh"
#include "config.hh"
#include "radioid.hh"


DMRIdSelect::DMRIdSelect(Config *config, QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("[Default]"), QVariant::fromValue(DefaultRadioID::get()));
  for (int i=0; i<config->radioIDs()->count(); i++) {
    if (config->radioIDs()->get(i)->is<DMRRadioID>()) {
      DMRRadioID *id = config->radioIDs()->get(i)->as<DMRRadioID>();
      addItem(id->name(), QVariant::fromValue(id));
    }
  }
}


void
DMRIdSelect::setRadioId(DMRRadioID *id) {
  for (int i=0; i<count(); i++) {
    if (itemData(i).value<DMRRadioID*>() == id) {
      setCurrentIndex(i);
      break;
    }
  }
}

DMRRadioID *
DMRIdSelect::radioId() const {
  return currentData().value<DMRRadioID*>();
}
