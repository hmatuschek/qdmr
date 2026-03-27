#include "satellitetransponderdialog.hh"
#include "ui_satellitetransponderdialog.h"

SatelliteTransponderDialog::SatelliteTransponderDialog(const Satellite &sat, TransponderDatabase &transponder, QWidget *parent)
  : QDialog(parent) , ui(new Ui::SatelliteTransponderDialog), _satellite(sat)
{
  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("edit-satellites"));

  ui->fmDownlinkFrequency->populate(_satellite.id(), false, Transponder::Mode::FM, transponder);
  ui->fmDownlinkFrequency->setFrequency(_satellite.fmDownlink());
  ui->fmDownlinkTone->setSelectiveCall(_satellite.fmDownlinkTone());
  ui->fmUplinkFrequency->populate(_satellite.id(), true, Transponder::Mode::FM, transponder);
  ui->fmUplinkFrequency->setFrequency(_satellite.fmUplink());
  ui->fmUplinkTone->setSelectiveCall(_satellite.fmUplinkTone());

  ui->aprsDownlinkFrequency->populate(_satellite.id(), false, Transponder::Mode::APRS, transponder);
  ui->aprsDownlinkFrequency->setFrequency(_satellite.aprsDownlink());
  ui->aprsDownlinkTone->setSelectiveCall(sat.aprsDownlinkTone());
  ui->aprsUplinkFrequency->populate(_satellite.id(), true, Transponder::Mode::APRS, transponder);
  ui->aprsUplinkFrequency->setFrequency(_satellite.aprsUplink());
  ui->aprsUplinkTone->setSelectiveCall(_satellite.aprsUplinkTone());

  ui->beaconFrequency->populate(_satellite.id(), false, Transponder::Mode::CW, transponder);
  ui->beaconFrequency->setFrequency(_satellite.beacon());
}

SatelliteTransponderDialog::~SatelliteTransponderDialog() {
  delete ui;
}


const Satellite &
SatelliteTransponderDialog::satellite() const {
  return _satellite;
}

void
SatelliteTransponderDialog::accept() {
  _satellite.setFMUplink(ui->fmUplinkFrequency->frequency());
  _satellite.setFMUplinkTone(ui->fmUplinkTone->selectiveCall());
  _satellite.setFMDownlink(ui->fmDownlinkFrequency->frequency());
  _satellite.setFMDownlinkTone(ui->fmDownlinkTone->selectiveCall());

  _satellite.setAPRSUplink(ui->aprsUplinkFrequency->frequency());
  _satellite.setAPRSUplinkTone(ui->aprsUplinkTone->selectiveCall());
  _satellite.setAPRSDownlink(ui->aprsDownlinkFrequency->frequency());
  _satellite.setAPRSDownlinkTone(ui->aprsDownlinkTone->selectiveCall());

  _satellite.setBeacon(ui->beaconFrequency->frequency());

  QDialog::accept();
}

