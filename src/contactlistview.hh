#ifndef CONTACTLISTVIEW_HH
#define CONTACTLISTVIEW_HH

#include <QWidget>

class Config;
namespace Ui {
  class ContactListView;
}

class ContactListView : public QWidget
{
  Q_OBJECT

public:
  explicit ContactListView(Config *config, QWidget *parent = nullptr);
  ~ContactListView();

protected slots:
  void onAddDMRContact();
  void onAddM17Contact();
  void onAddDTMFContact();
  void onRemContact();
  void onEditContact(unsigned row);

  void loadHeaderState();
  void storeHeaderState();

private:
  Ui::ContactListView *ui;
  Config *_config;
};

#endif // CONTACTLISTVIEW_HH
