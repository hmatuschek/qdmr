#ifndef QDMR_TASK_PROGRESS_VIEW_HH
#define QDMR_TASK_PROGRESS_VIEW_HH

#include <QProgressBar>
#include <QWidget>

class TaskProgress;

class TaskProgressView: public QWidget
{
  Q_OBJECT

public:
  explicit TaskProgressView(TaskProgress *task, QWidget *parent = nullptr);

protected:
  QProgressBar *_progressBar;
  TaskProgress *_task;
};

#endif //QDMR_TASK_PROGRESS_VIEW_HH
