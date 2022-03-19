#ifndef REPEATERBOOKCOMPLETER_HH
#define REPEATERBOOKCOMPLETER_HH

#include <QCompleter>
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QGeoCoordinate>
#include <QDateTime>
#include "signaling.hh"
#include "channel.hh"


class RepeaterBookEntry: public QObject
{
public:
  explicit RepeaterBookEntry(const QJsonObject &obj, QObject *parent=nullptr);
  RepeaterBookEntry(const RepeaterBookEntry &other);

  RepeaterBookEntry &operator =(const RepeaterBookEntry &other);

  bool isValid() const;

  const QString &id() const;
  const QString &call() const;
  const QGeoCoordinate &location() const;
  double rxFrequency() const;
  double txFrequency() const;
  bool isFM() const;
  bool isDMR() const;
  Signaling::Code rxTone() const;
  Signaling::Code txTone() const;
  unsigned int colorCode() const;
  unsigned int age() const;

  QJsonObject toJson() const;

protected:
  QString _id;
  QString _call;
  QGeoCoordinate _location;
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

public slots:
  /** Searches the repeater book for the given call (or part of it). */
  void search(const QString &call);
  bool load();
  bool store() const;

protected slots:
  void onRequestFinished(QNetworkReply *reply);

protected:
  QString cachePath() const;
  bool updateEntry(const RepeaterBookEntry &entry);

protected:
  QNetworkAccessManager _network;
  QNetworkReply *_currentReply;
  QList<RepeaterBookEntry> _items;
};


class RepeaterBookCompleter: public QCompleter
{
  Q_OBJECT

public:
  explicit RepeaterBookCompleter(int minPrefixLength=2, QObject *parent=nullptr);

  QStringList splitPath(const QString &path) const;

protected:
  RepeaterBookList *_repeaterList;
  int _minPrefixLength;
};

#endif // REPEATERBOOKCOMPLETER_HH
