/** @defgroup sat Satellite tracking settings.
 * @ingroup conf */

#ifndef ORBITALELEMENTSDATABASE_HH
#define ORBITALELEMENTSDATABASE_HH

#include <QAbstractTableModel>
#include <QNetworkAccessManager>


/** Defines a single orbital element, enabling the tracking of a single satellite. This dataset
 * does not contain any transponder information.
 * @ingroup sat */
class OrbitalElement
{
public:
  /** Represents a Julien day epoch since a specified year. */
  struct Epoch {
    /** The year of the epoch. */
    unsigned int year;
    /** The month. */
    unsigned int month;
    /** The day. */
    unsigned int day;
    /** The hour. */
    unsigned int hour;
    /** The minute. */
    unsigned int minute;
    /** The second. */
    unsigned int second;
    /** The microsecond. */
    unsigned int microsecond;

    /** Default constructor. */
    Epoch();
    /** Constructor */
    Epoch(unsigned int year, unsigned int month, unsigned int day,
          unsigned int hour, unsigned int minute, unsigned int second, unsigned int microsecond);
    /** Copy constructor. */
    Epoch(const Epoch &other) = default;
    /** Copy assignment. */
    Epoch &operator =(const Epoch &other) = default;

    /** Parses a date-time string into the epoch. */
    static Epoch parse(const QString &datetime);
    /** Computes the decimal epoch as the day of year a  */
    double toEpoch() const;
    /** Encodes the Epoch as YYYY-MM-DDThh:mm:ss.uuuuuu. */
    QString toString() const;
  };

public:
  /** Default constructor. */
  OrbitalElement();
  /** Constructor from ID. */
  OrbitalElement(unsigned int id);
  /** Copy constructor. */
  OrbitalElement(const OrbitalElement &other) = default;
  /** Copy assignemnt. */
  OrbitalElement &operator=(const OrbitalElement &other) = default;

  /** Returns @c true, if this represents a valid satellite information. */
  bool isValid() const;
  /** Returns the NORAD catalog id. */
  unsigned int id() const;
  /** Retunrs the name of the satellite. */
  const QString &name() const;

  /** Epoch of the orbital elements. */
  const Epoch &epoch() const;
  /** Returns the mean motion. */
  double meanMotion() const;
  /** Returns the first derivative of the mean motion. */
  double meanMotionDerivative() const;
  /** Returns the inclination. */
  double inclination() const;
  /** Returns the right ascension of the ascending node. */
  double ascension() const;
  /** Returns the eccentricity. */
  double eccentricity() const;
  /** Returns the argument of perigee. */
  double perigee() const;
  /** Returns the mean anomaly. */
  double meanAnomaly() const;
  /** Returns the revolution number. */
  unsigned int revolutionNumber() const;

public:
  /** Constructs a orbital element from a CelesTrak JSON object. */
  static OrbitalElement fromCelesTrak(const QJsonObject &obj);

protected:
  /** NORAD id of the satellite. */
  unsigned int _id;
  /** Desriptive name of the satellite. */
  QString _name;
  /** The epoch. */
  Epoch _epoch;
  /** Mean motion. */
  double _meanMotion;
  /** First derivative of the mean motion. */
  double _meanMotionDerivative;
  /** Inclination. */
  double _inclination;
  /** Right ascension of the ascending node. */
  double _ascension;
  /** Eccentricity. */
  double _eccentricity;
  /** Argument of perigee. */
  double _perigee;
  /** Mean anomaly. */
  double _meanAnomaly;
  /** The revolution number. */
  unsigned int _revolutionNumber;
};



/** Downloads and updates a database of orbital elements from CelesTrak.
 * @ingroup sat */
class OrbitalElementsDatabase: public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructs a orbital element database.
   * @param autoLoad [in] If @c true, the database gets downloaded and loaded automatically.
   * @param updatePeriodDays [in] Specifies the max age of the local database cache in days.
   * @param parent [in] Specifies the QObject parent. */
  explicit OrbitalElementsDatabase(bool autoLoad, unsigned int updatePeriodDays=7, QObject *parent=nullptr);

  /** @c returns @c true if the database contains a satellite with the given NORAD id. */
  bool contains(unsigned int id) const;
  /** Retunrs the orbital elements for the satellite with the given NORAD id. */
  OrbitalElement getById(unsigned int id) const;
  /** Returns the i-th orbital element. */
  const OrbitalElement &getAt(unsigned int idx) const;
  /** Returns the i-th orbital element. */
  OrbitalElement &getAt(unsigned int idx);

  /** Returns the current age of the cache. */
  unsigned int dbAge() const;
  /** If needed, downloads the database and loads all received orbital elements. */
  void load();

  /** Returns the number of elements in the database. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  /** Returns the number of columns of the database table. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  /** Returns a single cell of the database table. */
  QVariant data(const QModelIndex &index, int role) const;

signals:
  /** Gets emitted once the satellite orbitals has been loaded. */
  void loaded();
  /** Gets emitted if the loading one of the sources fails. */
  void error(const QString &msg);

public slots:
  /** Starts the download of the orbital elements. */
  void download();

private slots:
  /** Gets called whenever the orbital elements download is complete. */
  void downloadFinished(QNetworkReply *reply);

protected:
  /** Loads a database from the given filename. */
  bool load(const QString &filename);

private:
  /** Update period in days. */
  unsigned int _updatePeriod;
  /** Holds all sattellites sorted by their catalog number. */
  QVector<OrbitalElement>  _elements;
  /** Maps NORAD id to element. */
  QHash<unsigned int, unsigned int> _idIndexMap;
  /** The network access used for downloading. */
  QNetworkAccessManager _network;
};

#endif // ORBITALELEMENTSDATABASE_HH
