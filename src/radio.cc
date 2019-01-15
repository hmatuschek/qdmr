#include "radio.hh"
#include "hidinterface.hh"
#include <QDebug>
#include "rd5r.hh"
#include "config.hh"


Radio::Radio(QObject *parent)
  : QThread(parent)
{
  // pass...
}


bool
Radio::verifyConfig(Config *config, QStringList &issues)
{
  if (config->name().size() > features().maxNameLength)
    issues.append(tr("Radio name of length %1 exceeds limit of %2 characters.")
                  .arg(config->name().size()).arg(features().maxNameLength));
  if (config->introLine1().size() > features().maxIntroLineLength)
    issues.append(tr("Intro line 1 of length %1 exceeds limit of %2 characters.")
                  .arg(config->introLine1().size()).arg(features().maxNameLength));
  if (config->introLine2().size() > features().maxIntroLineLength)
    issues.append(tr("Intro line 2 of length %1 exceeds limit of %2 characters.")
                  .arg(config->introLine2().size()).arg(features().maxNameLength));

  if (config->contacts()->count() > features().maxContacts)
    issues.append(tr("Number of contacts %1 exceeds limit of %2.")
                  .arg(config->contacts()->count()).arg(features().maxContacts));
  for (int i=0; i<config->contacts()->count(); i++) {
    Contact *contact = config->contacts()->contact(i);
    if (contact->name().size() > features().maxContactNameLength)
      issues.append(tr("Length of name of contact '%1' %2 exceeds limit of %3 characters.")
                    .arg(contact->name()).arg(contact->name().size()).arg(features().maxContactNameLength));
  }

  return 0 == issues.size();
}


Radio *
Radio::detect() {
  HID *hid = new HID(0x15a2, 0x0073);
  if (! hid->isOpen()) {
    qDebug() << "Cannot open HID.";
    hid->deleteLater();
    return nullptr;
  }

  QString id = hid->identify();
  hid->close();
  hid->deleteLater();

  qDebug() << "Found" << id;

  if ("BF-5R" == id)
    return new RD5R();

  return nullptr;
}
