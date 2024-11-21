#ifndef REPEATERDATABASE_HH
#define REPEATERDATABASE_HH

#include <QAbstractListModel>
#include <QGeoCoordinate>
#include <QDateTime>
#include <QFile>
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QNetworkAccessManager>

#include "frequency.hh"
#include "signaling.hh"


class QNetworkReply;



/** Repeater database entry.
 * Just a collection of some meaningful information for FM and DMR repeater. */
class RepeaterDatabaseEntry
{
public:
  /** The possible repeater types. */
  enum class Type {
    Invalid, FM, DMR, M17
  };

protected:
  /** FM Constructor. */
  RepeaterDatabaseEntry(const QString &call,
                        const Frequency &rxFrequency, const Frequency &txFrequency,
                        const QGeoCoordinate &location,
                        const SelectiveCall &rxTone,
                        const SelectiveCall &txTone,
                        const QDateTime &updated,
                        const QDateTime &loaded);

  /** DMR Constructor. */
  RepeaterDatabaseEntry(const QString &call,
                        const Frequency &rxFrequency, const Frequency &txFrequency,
                        const QGeoCoordinate &location,
                        unsigned int colorCode,
                        const QDateTime &updated,
                        const QDateTime &loaded);

public:
  /** Default constructor, constructs an invalid entry. */
  RepeaterDatabaseEntry();
  /** Copy constructor. */
  RepeaterDatabaseEntry(const RepeaterDatabaseEntry &other) = default;
  /** Assignment operator. */
  RepeaterDatabaseEntry &operator=(const RepeaterDatabaseEntry &other) = default;
  /** Comparison. */
  bool operator==(const RepeaterDatabaseEntry &other) const;
  /** Comparison. */
  bool operator<(const RepeaterDatabaseEntry &other) const;

  QJsonValue toJson() const;

  bool isValid() const;

  Type type() const;
  const QString &call() const;
  const Frequency &rxFrequency() const;
  const Frequency &txFrequency() const;

  const QGeoCoordinate &location() const;
  QString locator() const;

  const SelectiveCall &rxTone() const;
  const SelectiveCall &txTone() const;

  unsigned int colorCode() const;

  const QDateTime &updated() const;
  const QDateTime &loaded() const;

public:
  static RepeaterDatabaseEntry fm(const QString &call,
                                  const Frequency &rxFrequency, const Frequency &txFrequency,
                                  const QGeoCoordinate &location,
                                  const SelectiveCall &rxTone=SelectiveCall(),
                                  const SelectiveCall &txTone=SelectiveCall(),
                                  const QDateTime &updated = QDateTime(),
                                  const QDateTime &loaded = QDateTime::currentDateTime());

  static RepeaterDatabaseEntry dmr(const QString &call,
                                   const Frequency &rxFrequeny, const Frequency &txFrequency,
                                   const QGeoCoordinate &location,
                                   unsigned int colorCode = 0,
                                   const QDateTime &updated = QDateTime(),
                                   const QDateTime &loaded = QDateTime::currentDateTime());

  static RepeaterDatabaseEntry fromJson(const QJsonObject &obj);

protected:
  Type           _type;
  QString        _call;
  Frequency      _rxFrequency;
  Frequency      _txFrequency;
  QGeoCoordinate _location;
  SelectiveCall  _rxTone;
  SelectiveCall  _txTone;
  unsigned int   _colorCode;
  QDateTime      _updated;
  QDateTime      _loaded;
};



/** Base class for all database sources. */
class RepeaterDatabaseSource: public QObject
{
  Q_OBJECT

protected:
  explicit RepeaterDatabaseSource(QObject *parent = nullptr);

public:
  virtual bool query(const QString &call, const QGeoCoordinate &pos=QGeoCoordinate());

  /** Returns the number of stored entries. By default, none are stored. */
  virtual unsigned int count() const;
  /** Returns the i-th stored entry. */
  virtual RepeaterDatabaseEntry get(unsigned int idx) const;

signals:
  /** Gets emitted, once an entry gets updated or is added. */
  void updated(const RepeaterDatabaseEntry &entry);

protected:
  /** Needs to be implemented to query new entries. */
  virtual bool load(const QString &call, const QGeoCoordinate &pos) = 0;
};



/** Base class for all cached database sources. */
class CachedRepeaterDatabaseSource: public RepeaterDatabaseSource
{
  Q_OBJECT

protected:
  CachedRepeaterDatabaseSource(const QString &filename, QObject *parent = nullptr);

public:
  unsigned int count() const;
  RepeaterDatabaseEntry get(unsigned int idx) const;

  bool query(const QString &call, const QGeoCoordinate &pos=QGeoCoordinate());

protected:
  void loadCache();
  void cache(const RepeaterDatabaseEntry &entry);
  void saveCache();

protected:
  unsigned int _maxAge;
  QFile _cacheFile;
  QList<RepeaterDatabaseEntry> _cache;
};



class DownloadableRepeaterDatabaseSource: public CachedRepeaterDatabaseSource
{
  Q_OBJECT

protected:
  DownloadableRepeaterDatabaseSource(const QString &filename, const QUrl &source,
                                     unsigned int maxAge=5, QObject *parent=nullptr);

public:
  bool needsUpdate() const;

protected:
  virtual bool parse(const QByteArray &doc) = 0;
  bool load(const QString &call, const QGeoCoordinate &pos);

protected slots:
  void onRequestFinished(QNetworkReply *reply);
  void download();

protected:
  QUrl _url;
  QNetworkAccessManager _network;
  QNetworkReply *_currentReply;
};



/** Base class of all repeater databases. */
class RepeaterDatabase : public QAbstractListModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit RepeaterDatabase(QObject *parent = nullptr);

  RepeaterDatabaseEntry get(unsigned int idx) const;
  void addSource(RepeaterDatabaseSource *source);

  virtual bool query(const QString &call, const QGeoCoordinate &pos=QGeoCoordinate());

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role) const;


protected slots:
  void merge(const RepeaterDatabaseEntry &entry);


protected:
  QList<RepeaterDatabaseSource *> _sources;
  QMap<RepeaterDatabaseEntry, unsigned int> _indices;
  QVector<RepeaterDatabaseEntry> _entries;
};




#endif // REPEATERDATABASE_HH
