#ifndef DMRSETTINGS_HH
#define DMRSETTINGS_HH

#include "interval.hh"
#include "configobject.hh"

/** Implements some common DMR settings present in many devices.
 * @ingroup config */
class DMRSettings : public ConfigExtension
{
  Q_OBJECT

  Q_CLASSINFO("description", "Common DMR settings.")
  Q_CLASSINFO("privateCallMatchDescription", "If enabled, private calls are only received, "
                                             "if they are for you.")
  Q_PROPERTY(bool privateCallMatch READ privateCallMatchEnabled WRITE enablePrivateCallMatch FINAL)
  Q_CLASSINFO("groupCallMatchDescription", "If enabled, group calls are only received, "
                                           "if they are on the group list.")
  Q_PROPERTY(bool groupCallMatch READ groupCallMatchEnabled WRITE enableGroupCallMatch FINAL)
  Q_CLASSINFO("privateCallHangTimeDescription", "Time span during which a direct answer for a "
                                                "private call is possible.")
  Q_PROPERTY(Interval privateCallHangTime READ privateCallHangTime WRITE setPrivateCallHangTime FINAL)
  Q_CLASSINFO("groupCallHangTimeDescription", "Time span during which a direct answer for a "
                                              "group call is possible.")
  Q_PROPERTY(Interval groupCallHangTime READ groupCallHangTime WRITE setGroupCallHangTime FINAL)
  Q_CLASSINFO("sendTalkerAliasDescription", "If enabled, the talker alias is send.")
  Q_PROPERTY(bool sendTalkerAlias READ sendTalkerAliasEnabled WRITE enableSendTalkerAlias FINAL)
  Q_CLASSINFO("talkerAliasEncodingDescription", "Specifies the encoding of the talker alias.")
  Q_PROPERTY(TalkerAliasEncoding talkerAliasEncoding READ talkerAliasEncoding WRITE setTalkerAliasEncoding FINAL)
  Q_CLASSINFO("preambleDescription", "Specifies preamble duration. Usually 100ms.")
  Q_PROPERTY(Interval preamble READ preamble WRITE setPreamble FINAL)

public:
  /** Possible talker alias encodings. */
  enum class TalkerAliasEncoding {
    Iso7, Iso8, Unicode
  };
  Q_ENUM(TalkerAliasEncoding)

public:
  /** Default constructor. */
  explicit DMRSettings(QObject *parent = nullptr);

  void clear() override;
  ConfigItem *clone() const override;

  /** Returns @c true if the private call must match. */
  bool privateCallMatchEnabled() const;
  /** Enables private call match. */
  void enablePrivateCallMatch(bool enable);

  /** Returns @c true if the group call must match. */
  bool groupCallMatchEnabled() const;
  /** Enables group call match. */
  void enableGroupCallMatch(bool enable);

  /** Returns the private call hang time. */
  Interval privateCallHangTime() const;
  /** Sets the private call hang time. */
  void setPrivateCallHangTime(const Interval &dur);

  /** Returns the group call hang time. */
  Interval groupCallHangTime() const;
  /** Sets the group call hang time. */
  void setGroupCallHangTime(const Interval &dur);

  /** Retunrs @c true if the talker alias is send. */
  bool sendTalkerAliasEnabled() const;
  /** Enables sending talker alias. */
  void enableSendTalkerAlias(bool enable);

  /** Returns the talker alias encoding. */
  TalkerAliasEncoding talkerAliasEncoding() const;
  /** Sets the talker alias encoding. */
  void setTalkerAliasEncoding(TalkerAliasEncoding encoding);

  /** Returns the preamble duration. */
  Interval preamble() const;
  /** Sets the preamble duration. */
  void setPreamble(const Interval &dur);

protected:
  /** Enables private call match. */
  bool _privateCallMatch;
  /** Enables group call match. */
  bool _groupCallMatch;
  /** The private call hang time. */
  Interval _privateCallHangTime;
  /** The group call hang time. */
  Interval _groupCallHangTime;
  /** Enables sending the talker alias. */
  bool _sendTalkerAlias;
  /** The talker alias encoding. */
  TalkerAliasEncoding _talkerAliasEncoding;
  /** The preamble duration. */
  Interval _preamble;
};

#endif // DMRSETTINGS_HH
