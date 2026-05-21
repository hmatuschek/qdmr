#ifndef QDMR_TASK_PROGRESS_VIEW_HH
#define QDMR_TASK_PROGRESS_VIEW_HH

#include <QStackedWidget>
#include <QWidget>

class QLabel;
class QProgressBar;
class TaskProgress;

class TaskProgressView: public QWidget
{
  Q_OBJECT

public:
  explicit TaskProgressView(TaskProgress *task, QWidget *parent = nullptr);

protected:
  QLabel *_label;
  QProgressBar *_progressBar;
  TaskProgress *_task;
};


class TaskProgressListView : public QStackedWidget
{
  Q_OBJECT

public:
  explicit TaskProgressListView(QWidget *parent = nullptr);

  void addTask(TaskProgress *task);
};

#endif //QDMR_TASK_PROGRESS_VIEW_HH
