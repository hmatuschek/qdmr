#include "task_progress.hh"

#include <QNetworkReply>

#include "radio.hh"
#include "radio.hh"



/* ********************************************************************************************** *
 * TaskProgress
 * ********************************************************************************************** */
TaskProgress::TaskProgress(const QString &label, QObject *parent)
  : QObject{parent}, _label{label}, _selfDestructTimer() {
  _selfDestructTimer.setSingleShot(true);
  _selfDestructTimer.setInterval(10000);
  connect(&_selfDestructTimer, &QTimer::timeout,
    this, &TaskProgress::deleteLater);
}

const QString &
TaskProgress::label() const {
  return _label;
}

void
TaskProgress::setProgress(int prog) {
  prog = std::min(std::max(prog, 0), 100);
  emit progress(prog);
}

void
TaskProgress::finish(bool succ, const QString &message) {
  if (succ)
    emit success(message);
  else
    emit failed(message);
  _selfDestructTimer.start();
}



/* ********************************************************************************************** *
 * DownloadTaskProgress
 * ********************************************************************************************** */
DownloadTaskProgress::DownloadTaskProgress(const QString &label, QNetworkReply *reply, QObject *parent)
  : TaskProgress{label, parent} {
  connect(reply, &QNetworkReply::downloadProgress, [this](qint64 loaded, qint64 total) {
    if (total > 0)
      this->progress((100*loaded)/total);
    else
      this->progress(0);
  });
  connect(reply, &QNetworkReply::finished, [this, reply]() {
    if (reply->error() != QNetworkReply::NoError)
      this->finish(false, tr("Download failed."));
    else
      this->finish(true, tr("Download complete"));
  });
}



/* ********************************************************************************************** *
 * RadioTransferTaskProgress
 * ********************************************************************************************** */
RadioTransferTaskProgress::RadioTransferTaskProgress(const QString &label, Radio *radio, QObject *parent)
  : TaskProgress{label, parent} {
  connect(radio, &Radio::downloadProgress, this, &TaskProgress::progress);
  connect(radio, &Radio::downloadError, [this](Radio *radio) {
    Q_UNUSED(radio);
    finish(false, tr("Read failed."));
  });
  connect(radio, &Radio::downloadFinished, [this](Radio *radio) {
    Q_UNUSED(radio);
    finish(true, tr("Read complete."));
  });
  connect(radio, &Radio::uploadProgress, this, &TaskProgress::progress);
  connect(radio, &Radio::uploadError, [this](Radio *radio) {
    Q_UNUSED(radio);
    finish(false, tr("Write failed."));
  });
  connect(radio, &Radio::uploadComplete, [this](Radio *radio) {
    Q_UNUSED(radio);
    finish(true, tr("Write complete."));
  });
}