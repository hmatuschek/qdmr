#include "deviceselectiondialog.hh"
#include "ui_deviceselectiondialog.h"
#include "settings.hh"
#include <QCloseEvent>


DeviceSelectionDialog::DeviceSelectionDialog(const QList<USBDeviceDescriptor> &interfaces, QWidget *parent) :
  QDialog(parent), ui(new Ui::DeviceSelectionDialog), _interfaces(interfaces)
{
  ui->setupUi(this);

  // Populate combo box
  foreach (USBDeviceDescriptor dev, _interfaces) {
    ui->comboBox->addItem(dev.description());
  }
  connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceSelected(int)));

  // Select first device
  if (_interfaces.count())
    ui->comboBox->setCurrentIndex(0);

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  restoreGeometry(Settings().windowState(objectName()));
}

DeviceSelectionDialog::~DeviceSelectionDialog() {
  Settings().setWindowState(objectName(), saveGeometry());
  delete ui;
}

USBDeviceDescriptor
DeviceSelectionDialog::device() const {
  return _interfaces.at(ui->comboBox->currentIndex());
}

void
DeviceSelectionDialog::onDeviceSelected(int idx) {
  // Update description.
  USBDeviceDescriptor device = _interfaces.at(idx);
  ui->description->setText(device.description() + ". " + device.longDescription());
}

