#include "contactselectiondialog.hh"
#include "contact.hh"

#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

/* ********************************************************************************************* *
 * Implementation of MultiGroupCallSelectionDialog
 * ********************************************************************************************* */
MultiGroupCallSelectionDialog::MultiGroupCallSelectionDialog(ContactList *contacts, bool showPrivateCalls, QWidget *parent)
  : QDialog(parent)
{
  _contacts = new QListWidget();
  QCheckBox *showPrivCall = new QCheckBox(tr("Show private calls"));
  showPrivCall->setChecked(showPrivateCalls);

  for (int i=0; i<contacts->count(); i++) {
    Contact *contact = contacts->contact(i);
    if (! contact->is<DigitalContact>())
      continue;
    DigitalContact *digi = contact->as<DigitalContact>();
    QListWidgetItem *item = new QListWidgetItem(digi->name());
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, QVariant::fromValue(digi));
    item->setCheckState(Qt::Unchecked);
    _contacts->addItem(item);
    // Hide private calls if showPrivateCall is false (default)
    item->setHidden((DigitalContact::PrivateCall == digi->type()) && (! showPrivateCalls));
  }

  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(showPrivCall, SIGNAL(toggled(bool)), this, SLOT(showPrivateCallsToggled(bool)));

  _label = new QLabel(tr("Select a group call:"));
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_label);
  layout->addWidget(_contacts);
  layout->addWidget(showPrivCall);
  layout->addWidget(bbox);
  setLayout(layout);
}


void
MultiGroupCallSelectionDialog::setLabel(const QString &text) {
  _label->setText(text);
}

QList<DigitalContact *>
MultiGroupCallSelectionDialog::contacts() {
  QList<DigitalContact *> contacts;
  for (int i=0; i<_contacts->count(); i++) {
    if (Qt::Checked == _contacts->item(i)->checkState())
      contacts.push_back(_contacts->item(i)->data(Qt::UserRole).value<DigitalContact *>());
  }
  return contacts;
}

void
MultiGroupCallSelectionDialog::showPrivateCallsToggled(bool show) {
  for (int i=0; i<_contacts->count(); i++) {
    if (DigitalContact::PrivateCall == _contacts->item(i)->data(Qt::UserRole).value<DigitalContact *>()->type())
      _contacts->item(i)->setHidden(! show);
  }
}
