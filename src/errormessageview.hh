#ifndef ERRORMESSAGEVIEW_HH
#define ERRORMESSAGEVIEW_HH

#include <QDialog>
#include "errorstack.hh"

namespace Ui {
  class ErrorMessageView;
}

class ErrorMessageView : public QDialog
{
  Q_OBJECT

public:
  explicit ErrorMessageView(const ErrorStack &stack, QWidget *parent = nullptr);
  ~ErrorMessageView();

private:
  Ui::ErrorMessageView *ui;
};

#endif // ERRORMESSAGEVIEW_HH
