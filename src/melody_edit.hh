#ifndef QDMR_MELODY_EDIT_HH
#define QDMR_MELODY_EDIT_HH

#include <QWidget>
#include <QPointer>

class Melody;
class QSpinBox;
class QLineEdit;
class ConfigItem;

class MelodyEdit: public QWidget
{
Q_OBJECT

public:
  explicit MelodyEdit(QWidget *parent = nullptr);

public slots:
  /** Sets the melody to edit. */
  void setMelody(Melody *melody);

protected slots:
  /** Applies the changes to the wrapped melody object. */
  void onMelodyChanged(ConfigItem *item);

private:
  QPointer<Melody> _melody;
  QSpinBox *_bpm;
  QLineEdit *_melodyEdit;
};



#endif //QDMR_MELODY_EDIT_HH
