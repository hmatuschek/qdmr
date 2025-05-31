#ifndef TRANSPONDERDATABASE_HH
#define TRANSPONDERDATABASE_HH

#include <QAbstractTableModel>
#include <QNetworkAccessManager>

#include "frequency.hh"


/** Represents a single transponder of a satellite. */
struct Transponder
{

public:
  /** Possible transponder types. */
  enum class Type {
    Transponder,  ///< Proper transponder (repeater).
    Transmitter   ///< Just a transmitter (beacon).
  };

  /** Possible transponder modes. */
  enum class Mode {
    FM,   ///< Plain FM.
    CW,   ///< Simple CW.
    APRS, ///< AFSK APRS.
    BPSK  ///< BPSK.
  };

public:
  /** Default constructor. */
  Transponder();

  /** Returns @c true, if the transponder is valid.
   * I.e., it is associated with a satellite. */
  bool isValid() const;

  /** Returns the NORAD id of the associated satellite. */
  unsigned int satellite() const;
  /** Returns the transponder type. */
  Type type() const;
  /** Returns the transponder mode. */
  Mode mode() const;

  /** Returns a descriptive name of the transponder. */
  const QString &name() const;

  /** Returns the uplink frequency, if there is one.
   * An upload frequency is usually missing, if the type is @c Type::Transmitter. */
  const Frequency &uplink() const;
  /** Retruns the downlink frequency. */
  const Frequency &downlink() const;

public:
  /** Parses a transponder from the given SatNOGS JSON object. */
  static Transponder fromSATNOGS(const QJsonObject &obj);

protected:
  /** Holds the NORAD id of the satellite. */
  unsigned int _satellite;
  /** Holds the transponder type. */
  Type _type;
  /** Holds the transponder mode. */
  Mode _mode;
  /** Holds the name. */
  QString _name;
  /** Holds the downlink frequency. */
  Frequency _downlink;
  /** Holds the uplink frequency. */
  Frequency _uplink;
};



/** Implements the database of all known transponder.
 * @ingroup sat */
class TransponderDatabase : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Just a const iterator over all transponder. */
  typedef QVector<Transponder>::const_iterator const_iterator;

public:
  /** Constructor.
   * @param autoLoad If @c true, the transponder information gets downloaded and loaded
   *        automatically.
   * @param updatePeriod Specifies the maximum age of the cache in days.
   * @param parent The QObject parent. */
  explicit TransponderDatabase(bool autoLoad, unsigned int updatePeriod = 7, QObject *parent = nullptr);

  /** The current age of the cache. */
  unsigned int dbAge() const;

  /** Returns the i-th transponder. */
  const Transponder &getAt(unsigned int idx) const;

  /** Implements the QAbstractTableModel interface.
   * Returns the number of rows in the table. That is, the number of transponder. */
  int rowCount(const QModelIndex &parent) const;
  /** Implements the QAbstractTableModel interface. Returns the number of columns in the table. */
  int columnCount(const QModelIndex &parent) const;
  /** Implements the QAbstractTableModel interface. Returns the data for the cell. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Returns an iterator, pointing at the first transponder. */
  const_iterator begin() const;
  /** Returns an iterator, pointing right after the last transponder. */
  const_iterator end() const;

public slots:
  /** Downloads and loads all transponder information. */
  void load();

signals:
  /** Gets emitted once the transponder has been loaded. */
  void loaded();
  /** Gets emitted if the loading one of the sources fails. */
  void error(const QString &msg);

public slots:
  /** Starts the download of the transponder. */
  void download();

private slots:
  /** Gets called whenever the transponder download is complete. */
  void downloadFinished(QNetworkReply *reply);

protected:
  /** Loads the transponder information from the given file. */
  bool load(const QString &filename);

private:
  /** The update period of the transponders in days. */
  unsigned int _updatePeriod;
  /** Holds all transponder sorted by the catalog number of their sats. */
  QVector<Transponder>  _transponders;
  /** The network access used for downloading. */
  QNetworkAccessManager _network;

};

#endif // TRANSPONDERDATABASE_HH
