#ifndef DMRCHANNELDIALOG_H
#define DMRCHANNELDIALOG_H

#include "channeldialog.hh"

class DMRChannel;
class DMRIdSelect;
class DMRAdmitSelect;
class QSpinBox;
class TimeslotSelect;
class RXGroupListBox;
class DMRContactSelect;
class APRSSelect;
class RoamingZoneSelect;


class DMRChannelDialog: public ChannelDialog
{
  Q_OBJECT

public:
  DMRChannelDialog(Config *config, QWidget *parent=nullptr);

  void setChannel(DMRChannel *ch);

public slots:
  void accept() override;

protected slots:
  void onRepeaterSelected(const QModelIndex &index);

protected:
  DMRChannel *_clone;
  QPointer<DMRChannel> _orig;
  DMRIdSelect *_dmrId;
  DMRAdmitSelect *_admit;
  QSpinBox *_colorcode;
  TimeslotSelect *_timeSlot;
  RXGroupListBox *_groupList;
  DMRContactSelect *_contact;
  APRSSelect *_aprs;
  RoamingZoneSelect *_roam;
};

#endif // DMRCHANNELDIALOG_H
