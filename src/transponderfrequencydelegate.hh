#ifndef TRANSPONDERFREQUENCYDELEGATE_HH
#define TRANSPONDERFREQUENCYDELEGATE_HH

#include <QComboBox>
#include <QStyledItemDelegate>

#include "transponderdatabase.hh"



class TransponderFrequencyEditor: public QComboBox
{
  Q_OBJECT

public:
  explicit TransponderFrequencyEditor(
      unsigned int satId, bool uplink, Transponder::Mode mode,
      const TransponderDatabase &transponder, QWidget *parent=nullptr);

  void setFrequency(const Frequency &current);
  Frequency frequency() const;
};



class TransponderFrequencyDelegate: public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit TransponderFrequencyDelegate(bool uplink, Transponder::Mode mode, QObject *parent=nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex index);
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

protected:
  bool _uplink;
  Transponder::Mode _mode;
};

#endif // TRANSPONDERFREQUENCYDELEGATE_HH
