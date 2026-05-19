#ifndef SATELLITETRANSPONDERDIALOG_HH
#define SATELLITETRANSPONDERDIALOG_HH

#include <QDialog>
#include "satellitedatabase.hh"


namespace Ui {
class SatelliteTransponderDialog;
}

class SatelliteTransponderDialog : public QDialog
{
  Q_OBJECT

public:
  SatelliteTransponderDialog(const Satellite &sat, TransponderDatabase &transponder, QWidget *parent = nullptr);
  ~SatelliteTransponderDialog();

  const Satellite &satellite() const;

protected:
  void closeEvent(QCloseEvent *event) override;

public slots:
  void accept() override;

private:
  Ui::SatelliteTransponderDialog *ui;
  Satellite _satellite;
};

#endif // SATELLITETRANSPONDERDIALOG_HH
