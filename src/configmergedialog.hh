#ifndef CONFIGMERGEDIALOG_HH
#define CONFIGMERGEDIALOG_HH

#include <QDialog>
#include "configmergevisitor.hh"

namespace Ui {
  class ConfigMergeDialog;
}

class ConfigMergeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigMergeDialog(QWidget *parent = nullptr);
  ~ConfigMergeDialog();

  ConfigMergeVisitor::ItemStrategy itemStrategy() const;
  ConfigMergeVisitor::SetStrategy setStrategy() const;

private slots:
  void onItemStrategySelected(int idx);
  void onSetStrategySelected(int idx);

private:
  Ui::ConfigMergeDialog *ui;
};

#endif // CONFIGMERGEDIALOG_HH
