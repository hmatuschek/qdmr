#include "userdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <algorithm>
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of User
 * ********************************************************************************************* */
UserDatabase::User::User()
  : id(0)
{
  // pass...
}

UserDatabase::User::User(const QJsonObject &obj)
  : id(obj.value("id").toInt()), call(obj.value("callsign").toString()),
    name(obj.value("name").toString()), surname(obj.value("surname").toString()),
    country(obj.value("country").toString())
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of UserDatabase
 * ********************************************************************************************* */
UserDatabase::UserDatabase(uint updatePeriodDays, QObject *parent)
  : QAbstractTableModel(parent), _user(), _network()
{
  connect(&_network, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));

  if ((! load()) || (updatePeriodDays < dbAge()))
    download();
}

bool
UserDatabase::load() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return load(path+"/user.json");
}

const UserDatabase::User &
UserDatabase::user(int idx) const {
  return _user[idx];
}

bool
UserDatabase::load(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open user list '" << filename << "'.";
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (! doc.isObject()) {
    logError() << "Failed to load user DB: JSON document is not an object!";
    return false;
  }
  if (! doc.object().contains("users")) {
    logError() << "Failed to load user DB: JSON object does not contain 'users' item.";
    return false;
  }
  if (! doc.object()["users"].isArray()) {
    logError() << "Failed to load user DB: 'users' item is not an array.";
    return false;
  }

  beginResetModel();
  _user.clear();

  QJsonArray array = doc.object()["users"].toArray();
  _user.reserve(array.size());
  for (int i=0; i<array.size(); i++) {
    User user(array.at(i).toObject());
    if (user.isValid())
      _user.append(user);
  }
  // Sort repeater w.r.t. distance to me
  std::stable_sort(_user.begin(), _user.end(), [](const User &a, const User &b){ return a.id < b.id; });
  // Done.
  endResetModel();

  logDebug() << "Loaded user database with " << _user.size() << " entries.";

  return true;
}

void
UserDatabase::download() {
  QUrl url("https://www.radioid.net/static/users.json");
  QNetworkRequest request(url);
  _network.get(request);
}

void
UserDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    logError() << "Cannot download repeater list: " << reply->errorString();
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/user.json");
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    logError() << "Cannot create path '" << path << "'.";
    return;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    logError() << "Cannot save user list at '" << (path+"/user.json") << "'.";
    return;
  }

  file.write(reply->readAll());
  file.flush();
  file.close();

  load();
  reply->deleteLater();
}

uint
UserDatabase::dbAge() const {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/user.json";
  QFileInfo info(path);
  if (! info.exists())
    return -1;
  return info.lastModified().daysTo(QDateTime::currentDateTime());
}

int
UserDatabase::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _user.size();
}

int
UserDatabase::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 3;
}

QVariant
UserDatabase::data(const QModelIndex &index, int role) const {
  if ((Qt::EditRole != role) && ((Qt::DisplayRole != role)))
    return QVariant();

  if (index.row() >= _user.size())
    return QVariant();

  if (0 == index.column()) {
    // Call
    if (Qt::DisplayRole == role)
      return tr("%1 (%2, %3)")
          .arg(_user[index.row()].call)
          .arg(_user[index.row()].name)
          .arg(_user[index.row()].surname);
    else
      return _user[index.row()].call;
  } else if (1 == index.column()) {
    // ID
    return _user[index.row()].id;
  } else if (2 == index.column()) {
    // Country
    return _user[index.column()].country;
  }

  return QVariant();
}

