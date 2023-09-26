#include "radioselectiondialog.hh"
#include "ui_radioselectiondialog.h"

RadioSelectionDialog::RadioSelectionDialog(const USBDeviceDescriptor &device, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RadioSelectionDialog)
{
  ui->setupUi(this);

  _radios = RadioInfo::allRadios(device);
  foreach (RadioInfo info, _radios) {
    ui->radios->addItem(QString("%1 %2").arg(info.manufacturer(), info.name()));
  }

  if (!_radios.isEmpty())
    ui->radios->setCurrentIndex(0);

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

RadioSelectionDialog::~RadioSelectionDialog()
{
  delete ui;
}

RadioInfo
RadioSelectionDialog::radioInfo() const {
  return _radios.at(ui->radios->currentIndex());
}


