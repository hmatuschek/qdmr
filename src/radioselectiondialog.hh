#ifndef RADIOSELECTIONDIALOG_HH
#define RADIOSELECTIONDIALOG_HH

#include <QDialog>
#include "usbdevice.hh"
#include "radioinfo.hh"

namespace Ui {
  class RadioSelectionDialog;
}

class RadioSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit RadioSelectionDialog(const USBDeviceDescriptor &device, QWidget *parent = nullptr);
  ~RadioSelectionDialog();

  RadioInfo radioInfo() const;

private:
  Ui::RadioSelectionDialog *ui;
  QList<RadioInfo> _radios;
};

#endif // RADIOSELECTIONDIALOG_HH
