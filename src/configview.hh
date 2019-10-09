#ifndef CONFIGVIEW_HH
#define CONFIGVIEW_HH

#include "config.hh"
#include <QWidget>
#include <QLineEdit>

class ConfigView: public QWidget
{
  Q_OBJECT

public:
  ConfigView(Config *conf, QWidget *parent=nullptr);

protected slots:
  void onConfigModified();
  void onIdModified();
  void onNameModified();
  void onIntro1Modified();
  void onIntro2Modified();

protected:
  Config *_config;
  QLineEdit *_id;
  QLineEdit *_name;
  QLineEdit *_intro1;
  QLineEdit *_intro2;
};

#endif // CONFIGVIEW_HH
