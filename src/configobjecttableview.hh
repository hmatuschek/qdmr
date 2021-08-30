#ifndef CONFIGOBJECTTABLEVIEW_HH
#define CONFIGOBJECTTABLEVIEW_HH

#include <QWidget>
#include "configitemwrapper.hh"

class QHeaderView;
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
  QPair<int,int> selection() const;

  QHeaderView *header() const;

signals:
  void doubleClicked(uint row);

protected slots:
  void onMoveItemUp();
  void onMoveItemDown();
  void onDoubleClicked(QModelIndex idx);

private:
  Ui::ConfigObjectTableView *ui;
};

#endif // CONFIGOBJECTTABLEVIEW_HH
