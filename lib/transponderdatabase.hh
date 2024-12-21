#ifndef TRANSPONDERDATABASE_HH
#define TRANSPONDERDATABASE_HH

#include <QAbstractTableModel>
#include <QNetworkAccessManager>

#include "frequency.hh"



struct Transponder
{

public:
  enum class Type {
    Transponder,
    Transmitter
  };

  enum class Mode {
    FM, CW, APRS, BPSK
  };

public:
  Transponder();

  bool isValid() const;

  unsigned int satellite() const;
  Type type() const;
  Mode mode() const;

  const QString &name() const;

  const Frequency &uplink() const;
  const Frequency &downlink() const;

public:
  static Transponder fromSATNOGS(const QJsonObject &obj);

protected:
  unsigned int _satellite;
  Type _type;
  Mode _mode;
  QString _name;
  Frequency _downlink;
  Frequency _uplink;
};



class TransponderDatabase : public QAbstractTableModel
{
  Q_OBJECT

public:
  typedef QVector<Transponder>::const_iterator const_iterator;

public:
  explicit TransponderDatabase(bool autoLoad, unsigned int updatePeriod = 7, QObject *parent = nullptr);

  unsigned int dbAge() const;

  const Transponder &getAt(unsigned int idx) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

  const_iterator begin() const;
  const_iterator end() const;

public slots:
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
