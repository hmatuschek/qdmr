#include "verifydialog.hh"
#include <QPushButton>

VerifyDialog::VerifyDialog(const QList<VerifyIssue> &issues, QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);

  bool valid = true;
  foreach (VerifyIssue issue, issues) {
    QListWidgetItem *item = new QListWidgetItem(issue.message());
    if (VerifyIssue::ERROR == issue.type()) {
      item->setForeground(Qt::red);
      valid = false;
    } else if (VerifyIssue::WARNING == issue.type()) {
      item->setForeground(Qt::blue);
    }
    listWidget->addItem(item);
  }

  QPushButton *ignore = buttonBox->button(QDialogButtonBox::Ignore);
  ignore->setEnabled(valid);
}
