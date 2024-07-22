#include "verifydialog.hh"
#include "radiolimits.hh"
#include "application.hh"
#include <QPushButton>

VerifyDialog::VerifyDialog(const RadioLimitContext &issues, bool upload, QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);

  Application *app = qobject_cast<Application *>(QApplication::instance());

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
      if (app->isDarkMode())
        item->setForeground(Qt::white);
      else
        item->setForeground(Qt::black);
      break;
    case RadioLimitIssue::Critical:
      if (app->isDarkMode())
        item->setForeground(Qt::red);
      else
        item->setForeground(Qt::darkRed);
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
