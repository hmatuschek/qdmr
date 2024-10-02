#include "ctcssbox.hh"

CTCSSBox::CTCSSBox(QWidget *parent)
  : QComboBox(parent)
{
  populateCTCSSBox(this);
}

void
populateCTCSSBox(QComboBox *box, const SelectiveCall &call) {
  foreach (SelectiveCall c, SelectiveCall::standard()) {
    int i = box->count();
    box->addItem(c.format(), QVariant::fromValue(c));
    if (c == call)
      box->setCurrentIndex(i);
  }
}


