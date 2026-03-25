#ifndef CHANNEL_EXTENSION_HH
#define CHANNEL_EXTENSION_HH

#include "configobject.hh"


/** Common extended settings for all channels.
 * @ingroup conf */
class ChannelExtension : public ConfigExtension
{
  Q_OBJECT

  /** If @c true, talkaround is enabled. */
  Q_PROPERTY(bool talkaround READ talkaround WRITE enableTalkaround)

protected:
  /** Hidden constructor. */
  explicit ChannelExtension(QObject *parent = nullptr);


public:
  /** Returns @c true, if talkaround is enabled. */
  bool talkaround() const;
  /** Enables/disables talkaround. */
  void enableTalkaround(bool enable);

protected:
  /** If @c true, talkaround is enabled. */
  bool _talkaround;
};



/** Common FM channel extended settings.
 * @ingroup conf */
class FMChannelExtension: public ChannelExtension
{
  Q_OBJECT

  /** If @c true, the CTCSS phase-reverse burst at the end of transmission is enabled. */
  Q_PROPERTY(bool reverseBurst READ reverseBurst WRITE enableReverseBurst)

public:
  /** Default constructor. */
  explicit FMChannelExtension(QObject *parent = nullptr);

  ConfigItem *clone() const override;


  /** Returns @c true, if the CTCSS phase-reverse burst is enabled. */
  bool reverseBurst() const;
  /** Enables/disables the CTCSS phase-reverse burst. */
  void enableReverseBurst(bool enable);

protected:
  /** If @c true, the CTCSS phase-reverse burst at the end of transmission is enabled. */
  bool _reverseBurst;
};


/** Common DMR channel extended settings.
 * @ingroup conf */
class DMRChannelExtension: public ChannelExtension
{
  Q_OBJECT

  /** If @c true, the call confirmation is enabled. */
  Q_PROPERTY(bool callConfirm READ callConfirm WRITE enableCallConfirm)
  /** If @c true, SMS reception is enabled. */
  Q_PROPERTY(bool sms READ sms WRITE enableSMS)
  /** If @c true, the SMS confirmation is enabled. */
  Q_PROPERTY(bool smsConfirm READ smsConfirm WRITE enableSMSConfirm)
  /** If @c true, the radio will response to received data packages. Should be enabled. */
  Q_PROPERTY(bool dataConfirm READ dataConfirm WRITE enableDataConfirm)
  /** If @c true, the simplex DCDM mode is enabled. */
  Q_PROPERTY(bool dcdm READ dcdm WRITE enableDCDM)
  /** If @c true, the lone-worker feature is enabled for this channel. */
  Q_PROPERTY(bool loneWorker READ loneWorker WRITE enableLoneWorker)

public:
  /** Default constructor. */
  explicit DMRChannelExtension(QObject *parent=nullptr);

  ConfigItem *clone() const override;

  /** Returns @c true if the call confirmation is enabled. */
  bool callConfirm() const;
  /** Enables/disables the call confirmation. */
  void enableCallConfirm(bool enabled);

  /** Returns @c true if SMS reception is enabled. */
  bool sms() const;
  /** Enables/disables SMS reception. */
  void enableSMS(bool enable);

  /** Returns @c true if the SMS confirmation is enabled. */
  bool smsConfirm() const;
  /** Enables/disables the SMS confirmation. */
  void enableSMSConfirm(bool enabled);

  /** Returns @c true if the data acknowledgement is enabled. */
  bool dataConfirm() const;
  /** Enables/disables the data acknowledgement. */
  void enableDataConfirm(bool enable);

  /** Returns @c true if the simplex DCDM mode is enabled. */
  bool dcdm() const;
  /** Enables/disables the simplex DCDM mode. */
  void enableDCDM(bool enable);

  /** Returns @c true if the lone-worker feature is enabled. */
  bool loneWorker() const;
  /** Enables the lone-worker feature for this channel. */
  void enableLoneWorker(bool enable);

protected:
  /** If @c true, the call confirmation is enabled. */
  bool _callConfirm;
  /** If @c true, the SMS reception is enabled. */
  bool _sms;
  /** If @c true, the SMS confirmation is enabled. */
  bool _smsConfirm;
  /** If @c true, the data acknowledgement is enabled. */
  bool _dataConfirm;
  /** If @c true, the simplex TDMA mode is enabled. */
  bool _dcdm;
  /** If @c true the lone-worker feature is enabled. */
  bool _loneWorker;

};


#endif // CHANNEL_EXTENSION_HH
