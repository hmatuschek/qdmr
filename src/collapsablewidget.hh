#ifndef COLLAPSABLEWIDGET_HH
#define COLLAPSABLEWIDGET_HH

#include <QToolButton>
#include <QParallelAnimationGroup>

class CollapsableWidget : public QToolButton
{
  Q_OBJECT

public:
  explicit CollapsableWidget(QWidget *parent=nullptr);

  void setContent(QWidget *content);

public slots:
  void hideContent();
  void showContent();

private:
  QWidget *_content;
  QParallelAnimationGroup _animator;
};

#endif // COLLAPSABLEWIDGET_HH
