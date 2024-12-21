#include "transponderfrequencydelegate.hh"
#include "satellitedatabase.hh"


/* ********************************************************************************************* *
 * Implementation of TransponderFrequencyEditor
 * ********************************************************************************************* */
TransponderFrequencyEditor::TransponderFrequencyEditor(unsigned int satId, bool uplink, Transponder::Mode mode,
                                                       const TransponderDatabase &transponder, QWidget *parent)
  : QComboBox(parent)
{
  setEditable(true);
  for (const Transponder &tp: transponder) {
    if (tp.satellite() != satId)
      continue;
    if (tp.mode() != mode)
      continue;

    Frequency f = uplink ? tp.uplink() : tp.downlink();
    if (0 == f.inHz())
      continue;

    addItem(QString("%1 (%2)").arg(tp.name(), f.format()),
            QVariant::fromValue(f));
  }
}

void
TransponderFrequencyEditor::setFrequency(const Frequency &current) {
  for (int i=0; i<count(); i++) {
    if (itemData(i).value<Frequency>() == current) {
      setCurrentIndex(i);
      break;
    }
  }
}

Frequency
TransponderFrequencyEditor::frequency() const {
  return currentData().value<Frequency>();
}



/* ********************************************************************************************* *
 * Implementation of TransponderFrequencyDelegate
 * ********************************************************************************************* */
TransponderFrequencyDelegate::TransponderFrequencyDelegate(bool uplink, Transponder::Mode mode, QObject *parent)
  : QStyledItemDelegate(parent), _uplink(uplink), _mode(mode)
{
  // pass...
}


QWidget *
TransponderFrequencyDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option);
  auto model = qobject_cast<const SatelliteDatabase *>(index.model());
  unsigned int satId = model->getAt(index.row()).id();
  return new TransponderFrequencyEditor(satId, _uplink, _mode, model->transponders(), parent);
}


void
TransponderFrequencyDelegate::setEditorData(QWidget *editor, const QModelIndex index) {
  auto feditor = qobject_cast<TransponderFrequencyEditor*>(editor);
  feditor->setFrequency(index.data(Qt::EditRole).value<Frequency>());
}


void
TransponderFrequencyDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  model->setData(
        index, QVariant::fromValue(qobject_cast<TransponderFrequencyEditor*>(editor)->frequency()));
}

