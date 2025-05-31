#include "repeatercompleter.hh"
#include "repeaterdatabase.hh"


/* ********************************************************************************************* *
 * RepeaterCompleter
 * ********************************************************************************************* */
RepeaterCompleter::RepeaterCompleter(int minPrefixLength, RepeaterDatabase *repeater, QObject *parent)
  : QCompleter(parent), _repeaters(repeater),
    _minPrefixLength(minPrefixLength)
{
  setModel(_repeaters);
  setCaseSensitivity(Qt::CaseInsensitive);
}

QStringList
RepeaterCompleter::splitPath(const QString &path) const {
  if (path.length() >= _minPrefixLength)
    _repeaters->query(path);
  return QCompleter::splitPath(path);
}



/* ********************************************************************************************* *
 * NearestRepeaterFilter
 * ********************************************************************************************* */
NearestRepeaterFilter::NearestRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent)
  : QSortFilterProxyModel(parent), _repeater(repeater), _location(location)
{
  setSourceModel(repeater);
  sort(0);
}

bool
NearestRepeaterFilter::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
  double ldist = _location.distanceTo(_repeater->get(source_left.row()).location());
  double rdist = _location.distanceTo(_repeater->get(source_right.row()).location());
  return ldist < rdist;
}


/* ********************************************************************************************* *
 * DMRRepeaterFilter
 * ********************************************************************************************* */
DMRRepeaterFilter::DMRRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent)
  : NearestRepeaterFilter(repeater, location, parent)
{
  invalidateFilter();
}

bool
DMRRepeaterFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const  {
  Q_UNUSED(source_parent);
  if (source_row >= _repeater->rowCount(QModelIndex()))
    return false;
  return RepeaterDatabaseEntry::Type::DMR == _repeater->get(source_row).type();
}


/* ********************************************************************************************* *
 * FMRepeaterFilter
 * ********************************************************************************************* */
FMRepeaterFilter::FMRepeaterFilter(RepeaterDatabase *repeater, const QGeoCoordinate &location, QObject *parent)
  : NearestRepeaterFilter(repeater, location, parent)
{
  invalidateFilter();
}

bool
FMRepeaterFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const  {
  Q_UNUSED(source_parent);
  if (source_row >= _repeater->rowCount(QModelIndex()))
    return false;
  return RepeaterDatabaseEntry::Type::FM == _repeater->get(source_row).type();
}

