#ifndef CONFIGOBJECTLISTVIEW_H
#define CONFIGOBJECTLISTVIEW_H

#include <QWidget>
#include "configitemwrapper.hh"

namespace Ui {
  class ConfigObjectListView;
}

class ConfigObjectListView : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigObjectListView(QWidget *parent = nullptr);
  ~ConfigObjectListView();

  GenericListWrapper *model() const;
  void setModel(GenericListWrapper *model);

  bool hasSelection() const;
  QPair<int,int> selection() const;

signals:
  void doubleClicked(unsigned row);

protected slots:
  void onMoveItemUp();
  void onMoveItemTenUp();
  void onMoveItemTop();

  void onMoveItemDown();
  void onMoveItemTenDown();
  void onMoveItemBottom();

  void onDoubleClicked(QModelIndex idx);

private:
  Ui::ConfigObjectListView *ui;
};

#endif // CONFIGOBJECTLISTVIEW_H
