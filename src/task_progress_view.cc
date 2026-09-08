#include "task_progress_view.hh"

#include <QHBoxLayout>
#include <QLabel>

#include "task_progress.hh"
#include <QProgressBar>
#include <QStackedWidget>
#include <QSpinBox>



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
  layout->setContentsMargins(0,0,0,0);
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
    this->_progressBar->setVisible(false);
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
  : QWidget{parent} {
  _stack = new QStackedWidget();
  _selector = new QSpinBox();
  auto layout = new QHBoxLayout();
  layout->addWidget(_stack);
  layout->addWidget(_selector);
  layout->setContentsMargins(6,0,6,0);
  setLayout(layout);

  setVisible(false);

  connect(this->_stack, &QStackedWidget::widgetRemoved, [this](int index) {
    this->setVisible(0 != this->_stack->count());
    this->_selector->setVisible(this->_stack->count() > 1);
    if (this->_stack->count()) {
      this->_selector->setValue(this->_stack->currentIndex()+1);
      this->_selector->setRange(1,this->_stack->count());
      this->_selector->setPrefix(QString("/%1").arg(this->_stack->count()));
    }
  });

  connect(_selector, &QSpinBox::valueChanged, [this](int value) {
    this->_stack->setCurrentIndex(value-1);
  });
}

void TaskProgressListView::addTask(TaskProgress *task) {
  setVisible(true);
  _stack->insertWidget(0, new TaskProgressView(task, this));
  if (this->_stack->count()) {
    this->_selector->setRange(1,this->_stack->count());
    this->_selector->setPrefix(QString("/%1").arg(this->_stack->count()));
    this->_selector->setValue(1);
  }
  _selector->setVisible(_stack->count() > 1);
}
