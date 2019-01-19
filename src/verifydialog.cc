#include "verifydialog.hh"

VerifyDialog::VerifyDialog(const QList<VerifyIssue> &issues, QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);

  foreach (VerifyIssue issue, issues) {
    QListWidgetItem *item = new QListWidgetItem(issue.message());
    listWidget->addItem(item);
  }
}
