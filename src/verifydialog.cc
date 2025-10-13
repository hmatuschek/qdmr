#include "verifydialog.hh"
#include "radiolimits.hh"
#include "application.hh"
#include <QPushButton>
#include <QLabel>


VerifyDialog::VerifyDialog(const RadioLimitContext &issues, bool upload, QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);
  uploadMessage->setVisible(false);

  // Sort issues by severity
  QList<RadioLimitIssue> issueList;
  for (int i=0; i<issues.count(); i++)
    issueList.append(issues.message(i));
  std::sort(issueList.begin(), issueList.end(), [](const RadioLimitIssue &a, const RadioLimitIssue &b) {
    return a.severity() > b.severity();
  });

  Application *app = qobject_cast<Application *>(QApplication::instance());
  bool valid = true;
  foreach(auto issue, issueList) {
    auto item = new QTreeWidgetItem(treeWidget);
    auto label = new QLabel(issue.message());
    label->setMargin(5);
    label->setWordWrap(true);
    treeWidget->setItemWidget(item, 0, label);
    // Dispatch by severity
    switch (issue.severity()) {
    case RadioLimitIssue::Silent:
      break;
    case RadioLimitIssue::Hint:
      label->setStyleSheet("color: gray;");
      item->setIcon(0, QIcon::fromTheme("symbol-info"));
      break;
    case RadioLimitIssue::Warning:
      if (app->isDarkMode())
        label->setStyleSheet("color: white;");
      else
        label->setStyleSheet("color: black;");
      item->setIcon(0, QIcon::fromTheme("symbol-warning"));
      break;
    case RadioLimitIssue::Critical:
      if (app->isDarkMode())
        label->setStyleSheet("color: red;");
      else
        label->setStyleSheet("color: darkred;");
      valid = false;
      item->setIcon(0, QIcon::fromTheme("symbol-error"));
      break;
    }
    // Add traceback (in reverse order)
    foreach(auto step, issue.stack()) {
      item->insertChild(0, new QTreeWidgetItem({step}));
    }
  }

  if (upload) {
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QPushButton *ignore = buttonBox->button(QDialogButtonBox::Ok);
    ignore->setEnabled(valid);
    uploadMessage->setVisible(! valid);
  }
}
