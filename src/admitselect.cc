#include "admitselect.hh"

/* ****************************************************************************************** *
 * FMAdmitSelect
 * ****************************************************************************************** */
FMAdmitSelect::FMAdmitSelect(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("Always"), QVariant::fromValue(FMChannel::Admit::Always));
  addItem(tr("Channel Free"), QVariant::fromValue(FMChannel::Admit::Free));
  addItem(tr("Other Tone"), QVariant::fromValue(FMChannel::Admit::Tone));
}

void
FMAdmitSelect::setAdmit(FMChannel::Admit admit) {
  for (int i=0; i<count(); i++)
    if (itemData(i).value<FMChannel::Admit>() == admit)
      setCurrentIndex(i);
}


FMChannel::Admit
FMAdmitSelect::admit() const {
  return currentData().value<FMChannel::Admit>();
}



/* ****************************************************************************************** *
 * DMRAdmitSelect
 * ****************************************************************************************** */
DMRAdmitSelect::DMRAdmitSelect(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("Always"), QVariant::fromValue(DMRChannel::Admit::Always));
  addItem(tr("Channel Free"), QVariant::fromValue(DMRChannel::Admit::Free));
  addItem(tr("Other Color-code"), QVariant::fromValue(DMRChannel::Admit::ColorCode));
}

void
DMRAdmitSelect::setAdmit(DMRChannel::Admit admit) {
  for (int i=0; i<count(); i++)
    if (itemData(i).value<DMRChannel::Admit>() == admit)
      setCurrentIndex(i);
}


DMRChannel::Admit
DMRAdmitSelect::admit() const {
  return currentData().value<DMRChannel::Admit>();
}

