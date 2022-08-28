#ifndef CONTACTSELECTIONDIALOG_HH
#define CONTACTSELECTIONDIALOG_HH

#include <QList>
#include <QDialog>

class DigitalContact;
class ContactList;
class QListWidget;
class QLabel;


class MultiGroupCallSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  explicit MultiGroupCallSelectionDialog(ContactList *contacts, bool showPrivateCalls=false, QWidget *parent=nullptr);

  QList<DigitalContact *> contacts();

  void setLabel(const QString &text);

protected slots:
  void showPrivateCallsToggled(bool show);

protected:
  QLabel *_label;
  QListWidget *_contacts;
};

#endif // CONTACTSELECTIONDIALOG_HH
