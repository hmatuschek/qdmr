#include "releasenotes.hh"
#include "config.h"
#include "settings.hh"
#include "logger.hh"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QMessageBox>


ReleaseNotes::ReleaseNotes(QObject *parent)
  : QObject(parent)
{
  connect(&_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResponse(QNetworkReply*)));
}

void
ReleaseNotes::checkForUpdate() {
  Settings settings;
  if (! settings.isUpdated())
    return;

  QString url = QString("https://api.github.com/repos/hmatuschek/qdmr/releases/tags/v%1");
  _net.get(QNetworkRequest(QUrl(url.arg(VERSION_STRING))));
}

void
ReleaseNotes::onResponse(QNetworkReply *reply) {
  if (reply->error()) {
    show(tr("Cannot download release notes from https://github.com/hmatuschek/qdmr\n\t %1")
         .arg(reply->errorString()));
    return;
  }

  QByteArray data = reply->readAll();
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(data, &err);
  if (QJsonParseError::NoError != err.error) {
    logWarn() << "JSON error in release data: " << err.errorString();
  }
  if (! doc.isObject()) {
    show(tr("Cannot read release notes from https://github.com/hmatuschek/qdmr\n\t"
            "Release is not a JSON object!"));
    return;
  }
  if (! doc.object().contains("body")) {
    show(tr("Cannot read release notes from https://github.com/hmatuschek/qdmr\n\t"
            "Release does not contain a release note."));
    return;
  }

  show(doc.object()["body"].toString());
}

void
ReleaseNotes::show(const QString &notes) {
  QMessageBox::information(
        nullptr, tr("qDMR was updated to version %1").arg(VERSION_STRING),
        notes);
  Settings().markUpdated();
}
