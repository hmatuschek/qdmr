#include "userdatabase.hh"
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <algorithm>
#include "logger.hh"
#include <cmath>


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
    name(obj.value("fname").toString()), surname(obj.value("surname").toString()),
    city(obj.value("city").toString()), state(obj.value("state").toString()),
    country(obj.value("country").toString()), comment(obj.value("remarks").toString())
{
  // pass...
}

uint
UserDatabase::User::distance(uint id) const {
  // Fix number of digits
  int a = this->id, b = id;
  int ad = std::ceil(std::log10(a));
  int bd = std::ceil(std::log10(b));
  if (ad > bd)
    b *= std::pow(10u, (ad-bd));
  else if (bd > ad)
    a *= std::pow(10u, (bd-ad));
  // Distance is just the difference between these two numbers
  // this ensures a small distance between two numbers with the same
  // prefix.
  return std::abs(a-b);
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

qint64
UserDatabase::count() const {
  return _user.size();
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
    QString msg = QString("Cannot open user list '%1': %2").arg(filename).arg(file.errorString());
    logError() << msg;
    emit error(msg);
    return false;
  }
  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (! doc.isObject()) {
    QString msg = "Failed to load user DB: JSON document is not an object!";
    logError() << msg;
    emit error(msg);
    return false;
  }
  if (! doc.object().contains("users")) {
    QString msg = "Failed to load user DB: JSON object does not contain 'users' item.";
    logError() << msg;
    emit error(msg);
    return false;
  }
  if (! doc.object()["users"].isArray()) {
    QString msg = "Failed to load user DB: 'users' item is not an array.";
    logError() << msg;
    emit error(msg);
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
  // Sort repeater w.r.t. their IDs
  std::stable_sort(_user.begin(), _user.end(), [](const User &a, const User &b){ return a.id < b.id; });
  // Done.
  endResetModel();

  logDebug() << "Loaded user database with " << _user.size() << " entries from " << filename << ".";

  emit loaded();
  return true;
}

void
UserDatabase::sortUsers(uint id) {
  // Sort repeater w.r.t. distance to ID
  std::stable_sort(_user.begin(), _user.end(), [id](const User &a, const User &b){
    return a.distance(id) < b.distance(id);
  });
}

void
UserDatabase::download() {
  QUrl url("https://database.radioid.net/static/users.json");
  QNetworkRequest request(url);
  _network.get(request);
}

void
UserDatabase::downloadFinished(QNetworkReply *reply) {
  if (reply->error()) {
    QString msg = QString("Cannot download user database: %1").arg(reply->errorString());
    logError() << msg;
    emit error(msg);
    return;
  }

  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(path+"/user.json");
  QDir directory;
  if ((! directory.exists(path)) && (!directory.mkpath(path))) {
    QString msg = QString("Cannot create path '%1'.").arg(path);
    logError() << msg;
    emit error(msg);
    return;
  }
  if (! file.open(QIODevice::WriteOnly)) {
    QString msg = QString("Cannot save user database at '%1'.").arg(path+"/user.json");
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
    if (Qt::DisplayRole == role) {
      if (_user[index.row()].surname.isEmpty()) {
        if (_user[index.row()].name.isEmpty()) {
          return _user[index.row()].call;
        } else {
          return tr("%1 (%2)")
              .arg(_user[index.row()].call)
              .arg(_user[index.row()].name);
        }
      } else {
        return tr("%1 (%2, %3)")
            .arg(_user[index.row()].call)
            .arg(_user[index.row()].name)
            .arg(_user[index.row()].surname);
      }
    } else {
      return _user[index.row()].call;
    }
  } else if (1 == index.column()) {
    // ID
    return _user[index.row()].id;
  } else if (2 == index.column()) {
    // Country
    return _user[index.column()].country;
  }

  return QVariant();
}

