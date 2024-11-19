#ifndef SELECTIVECALLBOX_H
#define SELECTIVECALLBOX_H

#include <QWidget>
#include "signaling.hh"
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>


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

#endif // SELECTIVECALLBOX_H
