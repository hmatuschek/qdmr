#ifndef EXTENSIONVIEW_HH
#define EXTENSIONVIEW_HH

#include <QWidget>
#include "extensionwrapper.hh"
#include "propertydelegate.hh"

namespace Ui {
  class ExtensionView;
}

class ConfigItem;


class ExtensionView : public QWidget
{
  Q_OBJECT

public:
  explicit ExtensionView(QWidget *parent = nullptr);
  ~ExtensionView();

public slots:
  void setObject(ConfigItem *obj, Config *context);

protected slots:
  void onSelectionChanged(const QItemSelection &current, const QItemSelection &last);
  void onCreate();
  void onDelete();
  void loadSectionState();
  void storeSectionState();

private:
  Ui::ExtensionView *ui;
  PropertyWrapper   *_model;
  ExtensionProxy     _proxy;
  PropertyDelegate   _editor;
};

#endif // EXTENSIONVIEW_HH
