#ifndef ORBITALELEMENTSDATABASE_HH
#define ORBITALELEMENTSDATABASE_HH

#include <QAbstractTableModel>
#include <QNetworkAccessManager>


class OrbitalElement
{
public:
  struct Epoch {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
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

    static Epoch parse(const QString &datetime);
    double toEpoch() const;
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
  static OrbitalElement fromCelesTrak(const QJsonObject &obj);

protected:
  unsigned int _id;
  QString _name;
  Epoch _epoch;
  double _meanMotion;
  double _meanMotionDerivative;
  double _inclination;
  double _ascension;
  double _eccentricity;
  double _perigee;
  double _meanAnomaly;
  unsigned int _revolutionNumber;
};



class OrbitalElementsDatabase: public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit OrbitalElementsDatabase(bool autoLoad, unsigned int updatePeriodDays=7, QObject *parent=nullptr);

  bool contains(unsigned int id) const;
  OrbitalElement getById(unsigned int id) const;
  const OrbitalElement &getAt(unsigned int idx) const;
  OrbitalElement &getAt(unsigned int idx);

  unsigned int dbAge() const;
  void load();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
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
