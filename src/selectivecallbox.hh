#ifndef SELECTIVECALLBOX_H
#define SELECTIVECALLBOX_H

#include <QWidget>
#include "signaling.hh"
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QStyledItemDelegate>


class SelectiveCallBox : public QWidget
{
  Q_OBJECT

public:
  explicit SelectiveCallBox(QWidget *parent = nullptr);

  void setSelectiveCall(const SelectiveCall &call);
  SelectiveCall selectiveCall() const;

signals:
  void selected(const SelectiveCall &call);

private:
  QComboBox *_typeSelection;
  QStackedWidget *_stack;
  QComboBox *_ctcss;
  QComboBox *_dcs;
  QCheckBox *_inverted;
};


class SelectiveCallDelegate: public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit SelectiveCallDelegate(QObject *parent=nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex index);
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};


#endif // SELECTIVECALLBOX_H
