#include "verifydialog.hh"
#include "radiolimits.hh"
#include <QPushButton>

VerifyDialog::VerifyDialog(const RadioLimitContext &issues, bool upload, QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);

  bool valid = true;
  for (int i=0; i<issues.count(); i++) {
    const RadioLimitIssue &issue = issues.message(i);
    QListWidgetItem *item = new QListWidgetItem(issue.message());
    // Dispatch by severity
    switch (issue.severity()) {
    case RadioLimitIssue::Silent:
      break;
    case RadioLimitIssue::Hint:
      item->setForeground(Qt::gray);
      break;
    case RadioLimitIssue::Warning:
      item->setForeground(Qt::black);
      break;
    case RadioLimitIssue::Critical:
      item->setForeground(Qt::red);
      valid = false;
      break;
    }
    listWidget->addItem(item);
  }

  if (upload) {
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QPushButton *ignore = buttonBox->button(QDialogButtonBox::Ok);
    ignore->setEnabled(valid);
  }
}
