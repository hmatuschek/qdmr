#ifndef DEVICESELECTIONDIALOG_HH
#define DEVICESELECTIONDIALOG_HH

#include <QDialog>
#include "usbdevice.hh"

namespace Ui {
  class DeviceSelectionDialog;
}

class DeviceSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DeviceSelectionDialog(const QList<USBDeviceDescriptor> &interfaces, QWidget *parent = nullptr);
  ~DeviceSelectionDialog();

  USBDeviceDescriptor device() const;

protected slots:
  void onDeviceSelected(int idx);

private:
  Ui::DeviceSelectionDialog *ui;
  const QList<USBDeviceDescriptor> &_interfaces;
};

#endif // DEVICESELECTIONDIALOG_HH
