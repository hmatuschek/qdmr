#ifndef CONFIGOBJECTTYPESELECTIONDIALOG_HH
#define CONFIGOBJECTTYPESELECTIONDIALOG_HH

#include <QDialog>

namespace Ui {
  class ConfigObjectTypeSelectionDialog;
}

class ConfigObjectTypeSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigObjectTypeSelectionDialog(const QMetaObject &cls, QWidget *parent = nullptr);
  ~ConfigObjectTypeSelectionDialog();

private:
  Ui::ConfigObjectTypeSelectionDialog *ui;
};

#endif // CONFIGOBJECTTYPESELECTIONDIALOG_HH
