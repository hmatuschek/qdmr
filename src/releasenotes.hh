#ifndef RELEASENOTES_HH
#define RELEASENOTES_HH

#include <QObject>
#include <QNetworkAccessManager>


class ReleaseNotes : public QObject
{
  Q_OBJECT

public:
  explicit ReleaseNotes(QObject *parent = nullptr);

  void checkForUpdate();

public slots:
  void show(const QString &notes);

protected slots:
  void onResponse(QNetworkReply *reply);

protected:
  QNetworkAccessManager _net;
};

#endif // RELEASENOTES_HH
