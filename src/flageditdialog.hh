#ifndef FLAGEDITDIALOG_HH
#define FLAGEDITDIALOG_HH

#include <QDialog>
#include <QMetaEnum>

namespace Ui {
  class FlagEditDialog;
}

class FlagEditDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FlagEditDialog(QMetaEnum metaFlag, QWidget *parent = nullptr);
  ~FlagEditDialog();

  int value() const;
  void setValue(int value);

private:
  Ui::FlagEditDialog *ui;
  QMetaEnum _metaFlag;
};

#endif // FLAGEDITDIALOG_HH
