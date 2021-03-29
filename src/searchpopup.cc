#include "searchpopup.hh"
#include <QAction>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include "logger.hh"


SearchPopup::SearchPopup(QAbstractItemView *parent)
  : QFrame(parent)
{
  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  setFrameStyle(QFrame::Panel);
  setWindowModality(Qt::ApplicationModal);

  QAction *search = new QAction(this);
  search->setShortcut(QKeySequence(tr("Ctrl+F")));
  parent->addAction(search);
  connect(search, SIGNAL(triggered(bool)), this, SLOT(showPopup()));

  _search = new QLineEdit();
  connect(_search, SIGNAL(textChanged(QString)), this, SLOT(onSearchChanged(QString)));

  QAction *hide = new QAction(_search);
  hide->setShortcut(QKeySequence(Qt::Key_Escape));
  _search->addAction(hide, QLineEdit::TrailingPosition);
  connect(hide, SIGNAL(triggered(bool)), this, SLOT(hide()));

  _label = new QLabel();
  QToolButton *up = new QToolButton(this);
  up->setIcon(QIcon(":/icons/up.png"));
  connect(up, SIGNAL(clicked(bool)), this, SLOT(onPrevious()));
  QToolButton *down = new QToolButton(this);
  down->setIcon(QIcon(":/icons/down.png"));
  connect(down, SIGNAL(clicked(bool)), this, SLOT(onNext()));
  QToolButton *close = new QToolButton(this);
  close->setIcon(QIcon(":/icons/save.png"));
  connect(close, SIGNAL(clicked(bool)), this, SLOT(hide()));

  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(_search);
  layout->addWidget(_label);
  layout->addWidget(up);
  layout->addWidget(down);
  layout->addWidget(close);
  setLayout(layout);

  this->hide();
}

void
SearchPopup::showPopup() {
  show();
  QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(parent());
  QPoint parentPos = itemView->mapToGlobal(itemView->pos());
  int x = parentPos.x();
  int y = parentPos.y()+itemView->height()-height();
  move(x,y);
  _search->clear();
  _search->setFocus();
}

void
SearchPopup::onSearchChanged(const QString &text) {
  QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(parent());
  QAbstractItemModel *model = itemView->model();

  if (text.isEmpty()) {
    _currentMatch = 0;
    _matches.clear();
    itemView->selectionModel()->clear();
    _label->setText("");
    return;
  }

  _currentMatch = 0;
  itemView->selectionModel()->clear();
  _matches.clear();
  for (int i=0; i<model->columnCount(); i++)
    _matches.append(model->match(model->index(0,i), Qt::DisplayRole, text, -1,
                                 Qt::MatchContains|Qt::MatchWrap));
  std::sort(
        _matches.begin(), _matches.end(),
        [](const QModelIndex &a, const QModelIndex &b) {
    if (a.row() < b.row())
      return true;
    if (a.row() > b.row())
      return false;
    return a.column() < b.column();
  });

  if (_matches.count()) {
    _label->setText(tr("%1/%2").arg(_currentMatch+1).arg(_matches.count()));
    itemView->setCurrentIndex(_matches.at(_currentMatch));
  } else {
    _label->setText("");
  }
}

void
SearchPopup::onNext() {
  if (0 == _matches.count())
    return;

  QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(parent());
  if ((++_currentMatch) >= _matches.count())
    _currentMatch = 0;
  _label->setText(tr("%1/%2").arg(_currentMatch+1).arg(_matches.count()));
  itemView->setCurrentIndex(_matches.at(_currentMatch));
}

void
SearchPopup::onPrevious() {
  if (0 == _matches.count())
    return;

  QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(parent());
  if (0 == _currentMatch)
    _currentMatch = _matches.count()-1;
  _label->setText(tr("%1/%2").arg(_currentMatch+1).arg(_matches.count()));
  itemView->setCurrentIndex(_matches.at(_currentMatch));
}


void
SearchPopup::attach(QAbstractItemView *itemview) {
  if (nullptr == itemview)
    return;

  new SearchPopup(itemview);
}
