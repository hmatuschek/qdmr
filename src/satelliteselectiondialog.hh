#ifndef SATELLITESELECTIONDIALOG_HH
#define SATELLITESELECTIONDIALOG_HH

#include <QDialog>

namespace Ui {
  class SatelliteSelectionDialog;
}

class OrbitalElementsDatabase;


class SatelliteSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SatelliteSelectionDialog(OrbitalElementsDatabase *db, QWidget *parent = nullptr);
  ~SatelliteSelectionDialog();

  unsigned int satellite() const;

private:
  Ui::SatelliteSelectionDialog *ui;
  OrbitalElementsDatabase *_database;
};

#endif // SATELLITESELECTIONDIALOG_HH
