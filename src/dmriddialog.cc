#include "dmriddialog.hh"
#include "ui_dmriddialog.h"
#include "settings.hh"
#include <QIntValidator>

DMRIDDialog::DMRIDDialog(DMRRadioID *radioid, QWidget *parent) :
  QDialog(parent), ui(new Ui::DMRIDDialog), _myID(new DMRRadioID(this)), _editID(radioid)
{
  ui->setupUi(this);
  _myID->copy(*_editID);

  construct();
}

DMRIDDialog::DMRIDDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::DMRIDDialog), _myID(new DMRRadioID(this)), _editID(nullptr)
{
  ui->setupUi(this);

  construct();
}

void
DMRIDDialog::construct() {
  Settings settings;
  ui->name->setText(_myID->name());
  ui->dmrID->setText(QString::number(_myID->number()));
  ui->dmrID->setValidator(new QIntValidator(1, 16777215));

  if (! settings.showCommercialFeatures())
    ui->tabWidget->tabBar()->hide();

  ui->extensionView->setObjectName("dmrRadioIdExtension");
  ui->extensionView->setObject(_myID);
}

DMRIDDialog::~DMRIDDialog() {
  delete ui;
}

DMRRadioID *
DMRIDDialog::radioId() {
  _myID->setName(ui->name->text().simplified());
  _myID->setNumber(ui->dmrID->text().toUInt());

  if (_editID) {
    _editID->copy(*_myID);
    _myID->deleteLater();
    _myID = _editID;
  } else {
    _myID->setParent(nullptr);
  }
  return _myID;
}
