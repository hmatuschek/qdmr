#ifndef CONFIGOBJECTTABLEVIEW_HH
#define CONFIGOBJECTTABLEVIEW_HH

#include <QWidget>
#include "configitemwrapper.hh"

class QHeaderView;
class QSortFilterProxyModel;

namespace Ui {
  class ConfigObjectTableView;
}


class ConfigObjectTableView : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigObjectTableView(QWidget *parent = nullptr);
  ~ConfigObjectTableView();

  GenericTableWrapper *model() const;
  void setModel(GenericTableWrapper *model);

  bool hasSelection() const;
  bool isFilteredOrSorted() const;
  QPair<int,int> selection() const;

  QHeaderView *header() const;

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
  void toggleSortFilter(bool sortFilter);

protected:
  QSortFilterProxyModel *proxy() const;
  bool canMove() const;

private:
  GenericTableWrapper *_model;
  Ui::ConfigObjectTableView *ui;
};

#endif // CONFIGOBJECTTABLEVIEW_HH
