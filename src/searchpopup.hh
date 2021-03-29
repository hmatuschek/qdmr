#ifndef SEARCHPOPUP_HH
#define SEARCHPOPUP_HH

#include <QFrame>
#include <QAbstractItemView>

class QLabel;


class SearchPopup : public QFrame
{
  Q_OBJECT

protected:
  explicit SearchPopup(QAbstractItemView *parent);

public:
  static void attach(QAbstractItemView *itemview);

public slots:
  void showPopup();

protected slots:
  void onSearchChanged(const QString &text);
  void onNext();
  void onPrevious();

protected:
  QLineEdit *_search;
  QLabel *_label;
  int _currentMatch;
  QModelIndexList _matches;
};

#endif // SEARCHPOPUP_HH
