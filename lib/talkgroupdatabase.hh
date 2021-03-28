#ifndef TALKGROUPDATABASE_HH
#define TALKGROUPDATABASE_HH

#include <QAbstractTableModel>
#include <QNetworkAccessManager>

class TalkGroupDatabase : public QAbstractTableModel
{
  Q_OBJECT

  class TalkGroup {
  public:
    TalkGroup();
    TalkGroup(const QString &name, uint number);

    uint id;
    QString name;
  };

public:
  TalkGroupDatabase(uint updatePeriodDays=30, QObject *parent=nullptr);

  /** Returns the number of talk groups. */
  qint64 count() const;
  /** Returns the age of the database in days. */
  uint dbAge() const;

  TalkGroup talkgroup(int index) const;

  /** Loads all entries from the downloaded talk group db. */
  bool load();
  /** Loads all entries from the talk group db at the specified location. */
  bool load(const QString &filename);

  /** Implements the QAbstractTableModel interface, returns the number of rows (number of entries). */
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  /** Implements the QAbstractTableModel interface, returns the number of columns. */
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  /** Implements the QAbstractTableModel interface, return the entry data. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

signals:
  /** Gets emitted once the talk group database has been loaded. */
  void loaded();
  /** Gets emitted if the loading of the talk group database fails. */
  void error(const QString &msg);

public slots:
  /** Starts the download of the talk group database. */
  void download();

private slots:
  /** Gets called whenever the download is complete. */
  void downloadFinished(QNetworkReply *reply);

protected:
  /** Holds all talk groups as id->name table. */
  QVector<TalkGroup>    _talkgroups;
  /** The network access used for downloading. */
  QNetworkAccessManager _network;

};

#endif // TALKGROUPDATABASE_HH
