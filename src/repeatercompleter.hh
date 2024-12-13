#ifndef REPEATERCOMPLETER_HH
#define REPEATERCOMPLETER_HH

#include <QCompleter>
#include <QGeoCoordinate>
#include <QSortFilterProxyModel>

class RepeaterDatabase;


class RepeaterCompleter: public QCompleter
{
  Q_OBJECT

public:
  explicit RepeaterCompleter(int minPrefixLength, RepeaterDatabase *repeater, QObject *parent=nullptr);

  QStringList splitPath(const QString &path) const;

protected:
  RepeaterDatabase *_repeaters;
  int _minPrefixLength;
};



class NearestRepeaterFilter: public QSortFilterProxyModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit NearestRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

protected:
  bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

protected:
  RepeaterDatabase *_repeater;
  QGeoCoordinate _location;
};



/** A filter proxy for DMR repeaters.
 * @ingroup util */
class DMRRepeaterFilter: public NearestRepeaterFilter
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit DMRRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

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
  explicit FMRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent=nullptr);

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};


#endif // REPEATERCOMPLETER_HH
