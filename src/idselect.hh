#ifndef IDSELECT_HH
#define IDSELECT_HH

#include "config.hh"
#include <QComboBox>

class Config;
class DMRRadioID;

class DMRIdSelect : public QComboBox
{
  Q_OBJECT

public:
  DMRIdSelect(Config *config, QWidget *parent=nullptr);

  void setRadioId(DMRRadioID *id);
  DMRRadioID *radioId() const;

};

#endif // IDSELECT_HH
