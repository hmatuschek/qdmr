#ifndef REPEATERBOOKCOMPLETER_HH
#define REPEATERBOOKCOMPLETER_HH

#include <QCompleter>
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QGeoCoordinate>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include "signaling.hh"
#include "channel.hh"


class RepeaterBookEntry: public QObject
{
public:
  explicit RepeaterBookEntry(QObject *parent=nullptr);
  RepeaterBookEntry(const RepeaterBookEntry &other);

  RepeaterBookEntry &operator =(const RepeaterBookEntry &other);

  bool isValid() const;

  const QString &id() const;
  const QString &call() const;
  const QGeoCoordinate &location() const;
  QString locator() const;
  const QString &qth() const;
  double rxFrequency() const;
  double txFrequency() const;
  bool isFM() const;
  bool isDMR() const;
  Signaling::Code rxTone() const;
  Signaling::Code txTone() const;
  unsigned int colorCode() const;
  qint64 age() const;

  bool fromRepeaterBook(const QJsonObject &obj);
  bool fromCache(const QJsonObject &obj);
  QJsonObject toCache() const;

protected:
  QString _id;
  QString _call;
  QGeoCoordinate _location;
  QString _qth;
  double _rxFrequency;
  double _txFrequency;
  bool _isFM;
  bool _isDMR;
  Signaling::Code _rxTone;
  Signaling::Code _txTone;
  unsigned int _colorCode;
  QDateTime _timestamp;
};


class RepeaterBookList: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit RepeaterBookList(QObject *parent=nullptr);

  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

  const RepeaterBookEntry *repeater(int row) const;

public slots:
  /** Searches the repeater book for the given call (or part of it). */
  void search(const QString &call);
  bool load();
  bool store() const;

protected slots:
  void onRequestFinished(QNetworkReply *reply);

protected:
  QString cachePath() const;
  QString queryPath() const;
  bool updateEntry(const RepeaterBookEntry &entry);

protected:
  QNetworkAccessManager _network;
  QNetworkReply *_currentReply;
  QList<RepeaterBookEntry> _items;
  QHash<QString, QDateTime> _queries;
};


class RepeaterBookCompleter: public QCompleter
{
  Q_OBJECT

public:
  explicit RepeaterBookCompleter(int minPrefixLength, RepeaterBookList *repeater, QObject *parent=nullptr);

  QStringList splitPath(const QString &path) const;

protected:
  RepeaterBookList *_repeaterList;
  int _minPrefixLength;
};


class NearestRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit NearestRepeaterFilter(RepeaterBookList *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

protected:
  bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

protected:
  RepeaterBookList *_repeater;
  QGeoCoordinate _location;
};


/** A filter proxy for DMR repeaters.
 * @ingroup util */
class DMRRepeaterFilter: public NearestRepeaterFilter
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRRepeaterFilter(RepeaterBookList *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};


/** A filter proxy for analog FM repeaters.
 * @ingroup util */
class FMRepeaterFilter: public NearestRepeaterFilter
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit FMRepeaterFilter(RepeaterBookList *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // REPEATERBOOKCOMPLETER_HH
