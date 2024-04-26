#include "configmergedialog.hh"
#include "ui_configmergedialog.h"
#include "configmergevisitor.hh"


ConfigMergeDialog::ConfigMergeDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConfigMergeDialog)
{
  ui->setupUi(this);

  ui->itemStrategy->setItemData(0, QVariant((uint) ConfigMergeVisitor::ItemStrategy::Ignore));
  ui->itemStrategy->setItemData(1, QVariant((uint) ConfigMergeVisitor::ItemStrategy::Override));
  ui->itemStrategy->setItemData(2, QVariant((uint) ConfigMergeVisitor::ItemStrategy::Duplicate));

  ui->setStrategy->setItemData(0, QVariant((uint) ConfigMergeVisitor::SetStrategy::Ignore));
  ui->setStrategy->setItemData(0, QVariant((uint) ConfigMergeVisitor::SetStrategy::Override));
  ui->setStrategy->setItemData(0, QVariant((uint) ConfigMergeVisitor::SetStrategy::Duplicate));
  ui->setStrategy->setItemData(0, QVariant((uint) ConfigMergeVisitor::SetStrategy::Merge));

  onItemStrategySelected(ui->itemStrategy->currentIndex());

  connect(ui->itemStrategy, SIGNAL(currentIndexChanged(int)),
          this, SLOT(onItemStrategySelected(int)));
  connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

ConfigMergeDialog::~ConfigMergeDialog() {
  delete ui;
}


ConfigMergeVisitor::ItemStrategy
ConfigMergeDialog::itemStrategy() const {
  return (ConfigMergeVisitor::ItemStrategy)ui->itemStrategy->currentData().toUInt();
}

ConfigMergeVisitor::SetStrategy
ConfigMergeDialog::setStrategy() const {
  return (ConfigMergeVisitor::SetStrategy)ui->setStrategy->currentData().toUInt();
}


void
ConfigMergeDialog::onItemStrategySelected(int index) {
  switch ((ConfigMergeVisitor::ItemStrategy)ui->itemStrategy->itemData(index).toUInt()) {
  case ConfigMergeVisitor::ItemStrategy::Ignore:
    ui->itemStrategyLabel->setText(tr("Ignores any duplicate item."));
    break;
  case ConfigMergeVisitor::ItemStrategy::Override:
    ui->itemStrategyLabel->setText(tr("Replaces any duplicate item with the imported one."));
    break;
  case ConfigMergeVisitor::ItemStrategy::Duplicate:
    ui->itemStrategyLabel->setText(tr("Imports any duplicate item with a modified name."));
    break;
  }
}

void
ConfigMergeDialog::onSetStrategySelected(int index) {
  switch ((ConfigMergeVisitor::SetStrategy)ui->setStrategy->itemData(index).toUInt()) {
  case ConfigMergeVisitor::SetStrategy::Ignore:
    ui->itemStrategyLabel->setText(tr("Ignores any duplicate set."));
    break;
  case ConfigMergeVisitor::SetStrategy::Override:
    ui->itemStrategyLabel->setText(tr("Replaces any duplicate set with the imported one."));
    break;
  case ConfigMergeVisitor::SetStrategy::Duplicate:
    ui->itemStrategyLabel->setText(tr("Imports any duplicate set with a modified name."));
    break;
  case ConfigMergeVisitor::SetStrategy::Merge:
    ui->itemStrategyLabel->setText(tr("Merges duplicate sets."));
    break;
  }
}
