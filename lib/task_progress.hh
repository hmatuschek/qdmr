#ifndef QDMR_TASK_PROGRESS_HH
#define QDMR_TASK_PROGRESS_HH

#include <QObject>
#include <QTimer>

class QNetworkReply;
class Radio;


/** Represents the current progress of a task. */
class TaskProgress: public QObject
{
  Q_OBJECT

public:
  /** Constructs the progress for a task with the given label. */
  TaskProgress(const QString &label, QObject *parent = nullptr);

  /** Returns the label. */
  const QString &label() const;

signals:
  /** Emits the current progress in percent. */
  void progress(int progress);
  /** Gets emitted if the task is finished successfully. */
  void success(const QString &message);
  /** Gets emitted if the task failed. */
  void failed(const QString &message);

public slots:
  /** Sets the progress in percent. */
  void setProgress(int progress);
  /** Finishes the task with the given state and message. */
  void finish(bool success, const QString &message="");

protected:
  /** The label of the task. */
  QString _label;
  /** Self-destruction timer. */
  QTimer _selfDestructTimer;
};


/** A task progress for download tasks. */
class DownloadTaskProgress: public TaskProgress
{
  Q_OBJECT

public:
  DownloadTaskProgress(const QString &label, QNetworkReply *reply, QObject *parent = nullptr);
};


/** A radio transfer task progress. */
class RadioTransferTaskProgress: public TaskProgress
{
  Q_OBJECT

public:
  RadioTransferTaskProgress(const QString &label, Radio *radio, QObject *parent = nullptr);
};

#endif //QDMR_TASK_PROGRESS_HH
