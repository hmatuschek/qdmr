#include "errormessageview.hh"
#include "ui_errormessageview.h"

ErrorMessageView::ErrorMessageView(const ErrorStack &stack, QWidget *parent) :
  QDialog(parent), ui(new Ui::ErrorMessageView)
{
  ui->setupUi(this);

  QFont font = ui->errorMessage->font(); font.setBold(true);ui->errorMessage->setFont(font);

  if (stack.isEmpty()) {
    setWindowTitle(tr("Error: Unknown."));
    ui->errorMessage->setText("An unknown error has orccured.");
    ui->errorStack->setVisible(false);
    return;
  }

  setWindowTitle(tr("Error: %1").arg(stack.message(0).message()));
  if (1 == stack.count())
    ui->errorMessage->setText(stack.message(0).message());
  else
    ui->errorMessage->setText(
          stack.message(0).message() +
          ":" + stack.message(stack.count()-1).message());
  ui->errorStack->setText(stack.format());
}

ErrorMessageView::~ErrorMessageView()
{
  delete ui;
}
