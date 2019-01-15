#ifndef CHANNEL_HH
#define CHANNEL_HH

#include <QObject>
#include <QAbstractTableModel>
#include <QWidget>
#include <QDialog>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>


class Config;
class RXGroupList;
class DigitalContact;
class ScanList;

class Channel: public QObject
{
	Q_OBJECT

public:
	typedef enum {
		HighPower, LowPower
	} Power;

protected:
	Channel(const QString &name, float rx, float tx, Power power, uint txTimeout, bool rxOnly,
	        ScanList *scanlist, QObject *parent=nullptr);

public:
  template<class T>
  bool is() const {
	  return 0 != dynamic_cast<const T *>(this);
  }

  template<class T>
  T *as() {
	  return dynamic_cast<T *>(this);
  }

  template<class T>
  const T *as() const{
	  return dynamic_cast<const T *>(this);
  }

  const QString &name() const;
  bool setName(const QString &name);

  float rxFrequency() const;
  bool setRXFrequency(float freq);
  float txFrequency() const;
  bool setTXFrequency(float freq);

  Power power() const;
  void setPower(Power power);

  uint txTimeout() const;
  bool setTimeout(uint dur);

  bool rxOnly() const;
  bool setRXOnly(bool enable);

  ScanList *scanList() const;
  bool setScanList(ScanList *list);

signals:
  void modified();

protected slots:
  void onScanListDeleted(QObject *obj);

protected:
  QString _name;
  float _rxFreq;
  float _txFreq;
  Power _power;
  uint  _txTimeOut;
  bool  _rxOnly;
  ScanList *_scanlist;
};


class AnalogChannel: public Channel
{
public:
	typedef enum {
		AdmitNone, AdmitFree, AdmitTone
	} Admit;

	typedef enum {
		BWNarrow, BWWide
	} Bandwidth;

public:
	AnalogChannel(const QString &name, float rxFreq, float txFreq, Power power, uint txTimout,
	              bool txOnly, Admit admit, uint squelch, float rxTone, float txTone, Bandwidth bw,
                ScanList *list, QObject *parent=nullptr);

	Admit admit() const;
	void setAdmit(Admit admit);

	uint squelch() const;
	bool setSquelch(uint squelch);

	float rxTone() const;
	bool setRXTone(float freq);
	float txTone() const;
	bool setTXTone(float freq);

	Bandwidth bandwidth() const;
	bool setBandwidth(Bandwidth bw);

protected:
	Admit _admit;
	uint  _squelch;
	float _rxTone;
	float _txTone;
	Bandwidth _bw;
};


class DigitalChannel: public Channel
{
	Q_OBJECT

public:
	typedef enum {
		AdmitNone, AdmitFree, AdmitColorCode
	} Admit;
	typedef enum {
		TimeSlot1, TimeSlot2
	} TimeSlot;

public:
	DigitalChannel(const QString &name, float rxFreq, float txFreq, Power power, uint txTimout,
	               bool rxOnly, Admit admit, uint colorCode, TimeSlot timeslot, RXGroupList *rxGroup,
	               DigitalContact *txContact, ScanList *list, QObject *parent=nullptr);

	Admit admit() const;
	void setAdmit(Admit admit);

	uint colorCode() const;
	bool setColorCode(uint cc);

	TimeSlot timeslot() const;
	bool setTimeSlot(TimeSlot ts);

	RXGroupList *rxGroupList() const;
	bool setRXGroupList(RXGroupList *rxg);

	DigitalContact *txContact() const;
	bool setTXContact(DigitalContact *c);

protected slots:
	void onRxGroupDeleted();
	void onTxContactDeleted();

protected:
	Admit _admit;
	uint _colorCode;
	TimeSlot _timeSlot;
	RXGroupList *_rxGroup;
	DigitalContact *_txContact;
};


class ChannelList: public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit ChannelList(QObject *parent=nullptr);

  int count() const;

  void clear();

  int indexOf(Channel *channel) const;
	Channel *channel(int idx) const;
	int addChannel(Channel *channel);
	bool remChannel(Channel *channel);
	bool remChannel(int idx);

  bool moveUp(int idx);
  bool moveDown(int idx);

	// QAbstractTableModel interface
	int rowCount(const QModelIndex &index) const;
	int columnCount(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

signals:
	void modified();

protected slots:
  void onChannelDeleted(QObject *obj);
  void onChannelEdited();

protected:
	QVector<Channel *> _channels;
};


class ChannelListView: public QWidget
{
	Q_OBJECT

public:
	ChannelListView(Config *config, QWidget *parent=nullptr);

protected slots:
  void onChannelUp();
  void onChannelDown();
  void onAddAnalogChannel();
  void onAddDigitalChannel();
  void onRemChannel();
  void onEditChannel(const QModelIndex &index);

protected:
	Config *_config;
	ChannelList *_list;
	QTableView *_view;
};


class AnalogChannelDialog: public QDialog
{
  Q_OBJECT

public:
  AnalogChannelDialog(Config *config, QWidget *parent=nullptr);
  AnalogChannelDialog(Config *config, AnalogChannel *channel, QWidget *parent=nullptr);

  AnalogChannel *channel();

protected:
  void construct();

protected:
  Config *_config;
  AnalogChannel *_channel;
  QLineEdit *_name;
  QLineEdit *_rx;
  QLineEdit *_tx;
  QComboBox *_power;
  QSpinBox *_timeout;
  QCheckBox *_rxOnly;
  QComboBox *_scanlist;
  QComboBox *_admit;
  QSpinBox *_squelch;
  QComboBox *_rxTone;
  QComboBox *_txTone;
  QComboBox *_bw;
};


class DigitalChannelDialog: public QDialog
{
  Q_OBJECT

public:
  DigitalChannelDialog(Config *config, QWidget *parent=nullptr);
  DigitalChannelDialog(Config *config, DigitalChannel *channel, QWidget *parent=nullptr);

  DigitalChannel *channel();

protected:
  void construct();

protected:
  Config *_config;
  DigitalChannel *_channel;
  QLineEdit *_name;
  QLineEdit *_rx;
  QLineEdit *_tx;
  QComboBox *_power;
  QSpinBox *_timeout;
  QCheckBox *_rxOnly;
  QComboBox *_scanlist;
  QComboBox *_admit;
  QSpinBox  *_colorCode;
  QComboBox *_slot;
  QComboBox *_rxGroup;
  QComboBox *_contact;
};


class ChannelComboBox: public QComboBox
{
  Q_OBJECT

public:
  ChannelComboBox(ChannelList *lst, QWidget *parent=nullptr);

  Channel *channel() const;
};

#endif // CHANNEL_HH
