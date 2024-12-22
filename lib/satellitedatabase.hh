#ifndef SATELLITEDATABASE_HH
#define SATELLITEDATABASE_HH

#include "errorstack.hh"
#include "frequency.hh"
#include "signaling.hh"
#include "transponderdatabase.hh"
#include "orbitalelementsdatabase.hh"



class Satellite: public OrbitalElement
{
public:
  Satellite();
  Satellite(const OrbitalElement &orbit);

  Satellite(const Satellite &sat) = default;
  Satellite &operator =(const Satellite &other) = default;

  const QString &name() const;
  void setName(const QString &name);

  const Frequency &fmUplink() const;
  void setFMUplink(const Frequency &f);
  const Frequency &fmDownlink() const;
  void setFMDownlink(const Frequency &f);
  const SelectiveCall &fmUplinkTone() const;
  const SelectiveCall &fmDownlinkTone() const;

  const Frequency &aprsUplink() const;
  void setAPRSUplink(const Frequency &f);
  const Frequency &aprsDownlink() const;
  void setAPRSDownlink(const Frequency &f);
  const SelectiveCall &aprsUplinkTone() const;
  const SelectiveCall &aprsDownlinkTone() const;

  const Frequency &beacon() const;
  void setBeacon(const Frequency &f);

  QJsonObject toJson() const;

public:
  static Satellite fromJson(const QJsonObject &obj, const OrbitalElementsDatabase &db);

protected:
  QString _name;
  Frequency _fmUplink;
  Frequency _fmDownlink;
  SelectiveCall _fmUplinkTone;
  SelectiveCall _fmDownlinkTone;
  Frequency _aprsUplink;
  Frequency _aprsDownlink;
  SelectiveCall _aprsUplinkTone;
  SelectiveCall _aprsDownlinkTone;
  Frequency _beacon;
};



/** A table holding all known satellites.
 * This table merges oribtal with transponder information. */
class SatelliteDatabase : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit SatelliteDatabase(unsigned int updatePeriodDays=7, QObject *parent = nullptr);

  const OrbitalElementsDatabase &orbitalElements() const;
  OrbitalElementsDatabase &orbitalElements();
  const TransponderDatabase &transponders() const;
  TransponderDatabase &transponders();

  unsigned int count() const;
  const Satellite &getAt(unsigned int idx) const;
  void add(const Satellite &sat);
  bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex());

  Qt::ItemFlags flags(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
  void update();
  void load();
  bool save(const ErrorStack &err = ErrorStack()) const;

signals:
  /** Gets emitted once the satellites has been loaded. */
  void loaded();
  /** Gets emitted if the loading one of the sources fails. */
  void error(const QString &msg);

private:
  /** Holds all sattellites sorted by their catalog number. */
  QVector<Satellite>  _satellites;
  OrbitalElementsDatabase _orbitalElements;
  TransponderDatabase _transponders;
};

#endif // SATELLITEDATABASE_HH
