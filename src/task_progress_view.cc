#include "task_progress_view.hh"

#include <QHBoxLayout>
#include <QLabel>

#include "task_progress.hh"
#include <QProgressBar>


/* ********************************************************************************************** *
 * View of a single task.
 * ********************************************************************************************** */
TaskProgressView::TaskProgressView(TaskProgress *task, QWidget *parent)
  : QWidget{parent}, _task{task} {
  _progressBar = new QProgressBar(this);
  _progressBar->setRange(0,0);
  _progressBar->setValue(0);
  _label = new QLabel();
  _label->setText(_task->label());

  auto layout = new QHBoxLayout();
  layout->addWidget(_label);
  layout->addWidget(_progressBar);
  setLayout(layout);

  connect(_task, &TaskProgress::progress, [this](int progress) {
    if ((0 != progress) && (0 == _progressBar->maximum())) {
      _progressBar->setRange(0, 100);
    }
    _progressBar->setValue(progress);
  });
  connect(_task, &TaskProgress::success, [this](const QString &message) {
    this->_label->setText(QString("%1 : %2").arg(this->_task->label()).arg(message));
    this->_progressBar->setRange(0,100);
    this->_progressBar->setValue(100);
  });
  connect(_task, &TaskProgress::failed, [this](const QString &message) {
    this->_label->setText(QString("%1: %2").arg(this->_task->label()).arg(message));
    this->_progressBar->setVisible(false);
  });
  connect(_task, &QObject::destroyed, this, &QObject::deleteLater, Qt::QueuedConnection);
}




/* ********************************************************************************************** *
 * View of several tasks
 * ********************************************************************************************** */
TaskProgressListView::TaskProgressListView(QWidget *parent)
  : QStackedWidget{parent} {
  setVisible(false);
  connect(this, &QStackedWidget::widgetRemoved, [this](int index) {
    this->setVisible(0 != this->count());
  });
}

void TaskProgressListView::addTask(TaskProgress *task) {
  setVisible(true);
  insertWidget(0, new TaskProgressView(task, this));
}