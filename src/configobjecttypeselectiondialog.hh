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
  explicit ConfigObjectTypeSelectionDialog(const QList<QMetaObject> &cls, QWidget *parent = nullptr);
  ~ConfigObjectTypeSelectionDialog();

  const QMetaObject &selectedType() const;

protected slots:
  void onSelectionChanged(int currentIndex);

private:
  Ui::ConfigObjectTypeSelectionDialog *ui;
  QList<QMetaObject> _types;
};

#endif // CONFIGOBJECTTYPESELECTIONDIALOG_HH
