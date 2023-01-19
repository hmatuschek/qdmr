#ifndef MULTIROAMINGCHANNELSELECTIONDIALOG_HH
#define MULTIROAMINGCHANNELSELECTIONDIALOG_HH

#include <QDialog>
#include <QList>

// Forward declarations
class Config;
class RoamingChannel;
class QListWidget;

class MultiRoamingChannelSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  MultiRoamingChannelSelectionDialog(Config *config, QWidget *parent=nullptr);

  QList<RoamingChannel *> channels() const;

protected:
  QListWidget *_channels;
};

#endif // MULTIROAMINGCHANNELSELECTIONDIALOG_HH
