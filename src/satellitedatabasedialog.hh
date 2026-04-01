#ifndef SATELLITEDATABASEDIALOG_HH
#define SATELLITEDATABASEDIALOG_HH

#include <QDialog>

namespace Ui {
  class SatelliteDatabaseDialog;
}

class SatelliteDatabase;


class SatelliteDatabaseDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SatelliteDatabaseDialog(SatelliteDatabase *db, QWidget *parent = nullptr);
  ~SatelliteDatabaseDialog();

private slots:
  void onEditTransponder(const QModelIndex &idx);
  void onAddSatellite();
  void onDeleteSatellite();

private:
  Ui::SatelliteDatabaseDialog *ui;
  SatelliteDatabase *_database;
};

#endif // SATELLITEDATABASEDIALOG_HH
