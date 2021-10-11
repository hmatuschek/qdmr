#include "collapsablewidget.hh"

#include <QPropertyAnimation>
#include <QApplication>


CollapsableWidget::CollapsableWidget(QWidget *parent)
  : QToolButton(parent), _content(nullptr)
{
  setCheckable(true);
  setStyleSheet("background:none");
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setIconSize(QSize(8, 8));
  setFont(QApplication::font());
  hideContent();
  setArrowType(Qt::ArrowType::RightArrow);
  connect(this, &QToolButton::toggled, [=](bool checked) {
    setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    _content != nullptr && checked ? showContent() : hideContent();
  });
}


void
CollapsableWidget::setContent(QWidget *content) {
  assert(content != nullptr);
  _content = content;
  auto animation = new QPropertyAnimation(_content, "maximumHeight"); // QObject with auto delete
  animation->setStartValue(0);
  animation->setEasingCurve(QEasingCurve::InOutQuad);
  animation->setDuration(300);
  animation->setEndValue(content->geometry().height() + 10);
  _animator.addAnimation(animation);
  if (!isChecked())
    content->setMaximumHeight(0);
}

void
CollapsableWidget::hideContent() {
  _animator.setDirection(QAbstractAnimation::Backward);
  _animator.start();
}

void
CollapsableWidget::showContent() {
  _animator.setDirection(QAbstractAnimation::Forward);
  _animator.start();
}
