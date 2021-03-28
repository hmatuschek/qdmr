#include "talkgroupdatabase.hh"
#include <QStandardPaths>
#include <QFileInfo>
#include "logger.hh"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDir>


/* ********************************************************************************************* *
 * Implementation of TalkGroupDatabase::TalkGroup
 * ********************************************************************************************* */
TalkGroupDatabase::TalkGroup::TalkGroup()
  : id(0), name()
{
  // pass...
}

TalkGroupDatabase::TalkGroup::TalkGroup(const QString &name, uint number)
{
  this->id   = number;
  this->name = name;
}


/* ********************************************************************************************* *
 * Implementation of TalkGroupDatabase
 * ********************************************************************************************* */
TalkGroupDatabase::TalkGroupDatabase(uint updatePeriodDays, QObject *parent)
  : QAbstractTableModel(parent), _talkgroups(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if ((! load()) || (updatePeriodDays < dbAge()))
    download();
}

qint64
TalkGroupDatabase::count() const {
  return _talkgroups.count();
}

uint
TalkGroupDatabase::dbAge() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/talkgroups.json";
  QFileInfo info(path);
  if (! info.exists())
    return -1;
  return info.lastModified().daysTo(QDateTime::currentDateTime());
}

TalkGroupDatabase::TalkGroup
TalkGroupDatabase::talkgroup(int index) const {
  if ((0 > index) || (index >= count()))
    return TalkGroup();
  return _talkgroups[index];
}

void
TalkGroupDatabase::download() {
  QUrl url("https://api.brandmeister.network/v1.0/groups/");
  QNetworkRequest request(url);
  _network.get(request);
}

void
TalkGroupDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    QString msg = QString("Cannot download user database: %1").arg(reply->errorString());
    logError() << msg;
    emit error(msg);
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/talkgroups.json");
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    QString msg = QString("Cannot create path '%1'.").arg(path);
    logError() << msg;
    emit error(msg);
    return;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    QString msg = QString("Cannot save user database at '%1'.").arg(path+"/talkgroups.json");
    logError() << msg;
    emit error(msg);
    return;
  }

  file.write(reply->readAll());
  file.flush();
  file.close();

  load();
  reply->deleteLater();
}

bool
TalkGroupDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return load(path+"/talkgroups.json");
}

bool
TalkGroupDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    QString msg = QString("Cannot open talk group list '%1': ").arg(filename).arg(file.errorString());
    logError() << msg;
    emit error(msg);
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (! doc.isObject()) {
    QString msg = "Failed to load talk groups: JSON document is not an object!";
    logError() << msg;
    emit error(msg);
    return false;
  }

  beginResetModel();
  QJsonObject tgs = doc.object();
  _talkgroups.clear();
  _talkgroups.reserve(tgs.count());
  for (QJsonObject::const_iterator tg = tgs.begin(); tg!=tgs.end(); tg++) {
    _talkgroups.append(TalkGroup(tg.value().toString(), tg.key().toUInt()));
  }
  // Sort repeater w.r.t. their IDs
  std::stable_sort(_talkgroups.begin(), _talkgroups.end(),
                   [](const TalkGroup &a, const TalkGroup &b){ return a.id < b.id; });
  // Done.
  endResetModel();

  logDebug() << "Loaded talk group database with " << _talkgroups.size()
             << " entries from " << filename << ".";

  emit loaded();
  return true;
}


int
TalkGroupDatabase::rowCount(const QModelIndex &parent) const {
  return _talkgroups.count();
}

int
TalkGroupDatabase::columnCount(const QModelIndex &parent) const {
  return 2;
}

QVariant
TalkGroupDatabase::data(const QModelIndex &index, int role) const {
  if ((Qt::EditRole != role) && ((Qt::DisplayRole != role)))
    return QVariant();

  if (index.row() >= _talkgroups.size())
    return QVariant();

  if (0 == index.column()) {
    // Call
    if (Qt::DisplayRole == role) {
      return tr("%1 (%2)").arg(_talkgroups[index.row()].name).arg(_talkgroups[index.row()].name);
    } else {
      return _talkgroups[index.row()].name;
    }
  } else if (1 == index.column()) {
    // ID
    return _talkgroups[index.row()].id;
  }

  return QVariant();
}


