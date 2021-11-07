#include "errormessageview.hh"
#include "ui_errormessageview.h"

ErrorMessageView::ErrorMessageView(const ErrorStack &stack, QWidget *parent) :
  QDialog(parent), ui(new Ui::ErrorMessageView)
{
  ui->setupUi(this);
  if (! stack.hasErrorMessages()) {
    setWindowTitle(tr("Error: Unknown."));
    ui->errorMessage->setText("An unknown error has orccured.");
    ui->errorStack->setVisible(false);
    return;
  }

  setWindowTitle(tr("Error: %1").arg(stack.errorMessage(0).message()));
  if (1 == stack.errorMessageCount())
    ui->errorMessage->setText(stack.errorMessage(0).message());
  else
    ui->errorMessage->setText(
          stack.errorMessage(0).message() +
          ":" + stack.errorMessage(stack.errorMessageCount()-1).message());
  ui->errorStack->setText(stack.formatErrorMessages());
}

ErrorMessageView::~ErrorMessageView()
{
  delete ui;
}
