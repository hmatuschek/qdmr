#include "task_progress_view.hh"
#include "task_progress.hh"
#include <QProgressBar>

TaskProgressView::TaskProgressView(TaskProgress *task, QWidget *parent)
  : QWidget{parent}, _task{task} {

  
  connect(_task, &QObject::destroyed, this, &QObject::deleteLater, Qt::QueuedConnection);

}
