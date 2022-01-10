#include "deviceselectiondialog.hh"
#include "ui_deviceselectiondialog.h"

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

  // Update message
  if (1 == _interfaces.size()) {
    ui->message->setText(tr("The device found is not considered save to access. Are you sure that "
                            "this is the connected radio?"));
  } else {
    ui->message->setText(tr("There are more than one matching devices. Please select one."));
  }

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DeviceSelectionDialog::~DeviceSelectionDialog() {
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

