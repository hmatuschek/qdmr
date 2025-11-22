#ifndef SATELLITEDATABASE_HH
#define SATELLITEDATABASE_HH

#include "errorstack.hh"
#include "frequency.hh"
#include "signaling.hh"
#include "transponderdatabase.hh"
#include "orbitalelementsdatabase.hh"


/** Extends the @c OrbitalElement to include transponder information for a satellite.
 *
 * This represents a single user-curated satellite. The orbital elements are updated automatically,
 * while the transponder information remains set by the user.
 *
 * @ingroup sat */
class Satellite: public OrbitalElement
{
public:
  /** Default constructor. */
  Satellite();
  /** Constructor from orbital element. */
  Satellite(const OrbitalElement &orbit);
  /** Copy constructor. */
  Satellite(const Satellite &sat) = default;
  /** Copy assignment. */
  Satellite &operator =(const Satellite &other) = default;

  /** Returns the name of the satellite. */
  const QString &name() const;
  /** Sets the name of the satellite. */
  void setName(const QString &name);

  /** Returns the FM uplink frequency. */
  const Frequency &fmUplink() const;
  /** Sets the FM uplink frequency. */
  void setFMUplink(const Frequency &f);
  /** Returns the FM downlink frequency. */
  const Frequency &fmDownlink() const;
  /** Sets the FM downlink frequency. */
  void setFMDownlink(const Frequency &f);
  /** Returns the FM uplink tone (i.e., TX tone). */
  const SelectiveCall &fmUplinkTone() const;
  /** Sets the FM uplink sub tone. */
  void setFMUplinkTone(const SelectiveCall &tone);
  /** Returns the FM downlink tone (i.e., RX tone). */
  const SelectiveCall &fmDownlinkTone() const;
  /** Sets the FM downlink sub tone. */
  void setFMDownlinkTone(const SelectiveCall &tone);

  /** Returns the APRS uplink frequency. */
  const Frequency &aprsUplink() const;
  /** Sets the APRS uplink frequency. */
  void setAPRSUplink(const Frequency &f);
  /** Returns the APRS downlink frequency. */
  const Frequency &aprsDownlink() const;
  /** Sets the APRS downlink frequency. */
  void setAPRSDownlink(const Frequency &f);
  /** Returns the APRS uplink tone (i.e., TX tone). */
  const SelectiveCall &aprsUplinkTone() const;
  /** Sets the APRS uplink sub tone. */
  void setAPRSUplinkTone(const SelectiveCall &tone);
  /** Returns the APRS downlink tone (i.e., RX tone). */
  const SelectiveCall &aprsDownlinkTone() const;
  /** Sets the APRS downlink sub tone. */
  void setAPRSDownlinkTone(const SelectiveCall &tone);
  /** Returns a beacon frequency. */
  const Frequency &beacon() const;
  /** Sets a beacon frequency. */
  void setBeacon(const Frequency &f);

  /** Serializes the satellite into a JSON object. */
  QJsonObject toJson() const;

public:
  /** Constructs a satellite from the given JSON object and orbital element database. */
  static Satellite fromJson(const QJsonObject &obj, const OrbitalElementsDatabase &db);

protected:
  /** Holds the name of the satellite. */
  QString _name;
  /** Holds the FM uplink frequency. */
  Frequency _fmUplink;
  /** Holds the FM downlink frequency. */
  Frequency _fmDownlink;
  /** Holds the FM uplink tone. */
  SelectiveCall _fmUplinkTone;
  /** Holds the FM downlink tone. */
  SelectiveCall _fmDownlinkTone;
  /** Holds the APRS uplink frequency. */
  Frequency _aprsUplink;
  /** Holds the APRS downlink frequency. */
  Frequency _aprsDownlink;
  /** Holds the APRS uplink tone. */
  SelectiveCall _aprsUplinkTone;
  /** Holds the APRS downlink tone. */
  SelectiveCall _aprsDownlinkTone;
  /** Holds a beacon frequency. */
  Frequency _beacon;
};



/** A table holding all known satellites.
 * This table merges oribtal with transponder information.
 * @ingroup sat */
class SatelliteDatabase : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructs a new satellite database. */
  explicit SatelliteDatabase(unsigned int updatePeriodDays=7, QObject *parent = nullptr);

  /** Returns the orbital element database. */
  const OrbitalElementsDatabase &orbitalElements() const;
  /** Returns the orbital element database. */
  OrbitalElementsDatabase &orbitalElements();
  /** Returns the transponder database. */
  const TransponderDatabase &transponders() const;
  /** Returns the transponder database. */
  TransponderDatabase &transponders();

  /** Returns the number of configured satellites. */
  unsigned int count() const;
  /** Returns the i-th satellite. */
  const Satellite &getAt(unsigned int idx) const;
  /** Appends a satellite. */
  void add(const Satellite &sat);
  /** Removes one or more satellites. */
  bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex());

  /** Implements the QAbstractTableModel interface. Returns some flags for the given cell. */
  Qt::ItemFlags flags(const QModelIndex &index) const;
  /** Implements the QAbstractTableModel interface.
   * Returns the number of rows in the table. That is, the number of satellites. */
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  /** Implements the QAbstractTableModel interface. Returns the number of columns in the table. */
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  /** Implements the QAbstractTableModel interface. Returns the header labels. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  /** Implements the QAbstractTableModel interface. Returns the data for the cell. */
  QVariant data(const QModelIndex &index, int role) const;
  /** Implements the QAbstractTableModel interface. Sets the data for a cell. */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
  /** Triggers a download of the orbital and transponder databases. */
  void update();
  /** Loads the user-curated satellite database. */
  void load();
  /** Saves the user-curated satellite database. */
  bool save(const ErrorStack &err = ErrorStack()) const;

signals:
  /** Gets emitted once the satellites has been loaded. */
  void loaded();
  /** Gets emitted if the loading one of the sources fails. */
  void error(const QString &msg);

private:
  /** Holds all satellites sorted by their catalog number. */
  QVector<Satellite>  _satellites;
  /** Holds the orbital element database. */
  OrbitalElementsDatabase _orbitalElements;
  /** Holds the transponder database. */
  TransponderDatabase _transponders;
};

#endif // SATELLITEDATABASE_HH
