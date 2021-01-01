#ifndef CONTACTSELECTIONDIALOG_HH
#define CONTACTSELECTIONDIALOG_HH

#include <QList>
#include <QDialog>

class DigitalContact;
class ContactList;
class QListWidget;


class MultiGroupCallSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  explicit MultiGroupCallSelectionDialog(ContactList *contacts, QWidget *parent=nullptr);

  QList<DigitalContact *> contacts();

protected:
  QListWidget *_contacts;
};

#endif // CONTACTSELECTIONDIALOG_HH
