#ifndef CHANNEL_TYPE_EDIT_HH
#define CHANNEL_TYPE_EDIT_HH

#include <QDialog>
#include "channel.hh"

class QLabel;
class QListWidget;


class ChannelTypeEdit: public QWidget
{
  Q_OBJECT

public:
  explicit ChannelTypeEdit(QWidget *parent = nullptr);

  Channel::Types types() const;

public slots:
  void setTypes(Channel::Types types);

signals:
  void typesChanged(Channel::Types types);

protected slots:
  void onEditClicked();

protected:
  QLabel *_label;
  Channel::Types _types;
};


class ChannelTypeSelectionDialog: public QDialog
{
  Q_OBJECT

public:
  ChannelTypeSelectionDialog(Channel::Types types, QWidget *parent = nullptr);

  Channel::Types types() const;

protected:
  QListWidget *_list;
};


#endif //CHANNEL_TYPE_EDIT_HH
