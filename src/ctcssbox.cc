#include "ctcssbox.hh"

CTCSSBox::CTCSSBox(QWidget *parent)
  : QComboBox(parent)
{
  populateCTCSSBox(this);
}

void
populateCTCSSBox(QComboBox *box, Signaling::Code code) {
  for(unsigned i=Signaling::SIGNALING_NONE; i<=Signaling::DCS_754I; i++) {
    box->addItem(Signaling::codeLabel(Signaling::Code(i)), i);
    if (Signaling::Code(i) == code)
      box->setCurrentIndex(i);
  }
}


