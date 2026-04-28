#ifndef RADIOSETTINGS_HH
#define RADIOSETTINGS_HH

#include "configobject.hh"
#include "channel.hh"

#include "bootsettings.hh"
#include "audiosettings.hh"
#include "tonesettings.hh"
#include "gnsssettings.hh"
#include "dmrsettings.hh"
#include "radioddity_extensions.hh"
#include "anytone_settingsextension.hh"
#include "tyt_extensions.hh"


/** Represents the common radio-global settings.
 * @ingroup conf */
class RadioSettings : public ConfigItem
{
  Q_OBJECT
  /** The default channel power */
  Q_PROPERTY(Channel::Power power READ power WRITE setPower)
  /** The default transmit timeout */
  Q_PROPERTY(Interval tot READ tot WRITE setTOT SCRIPTABLE false)
  /** The default DMR radio ID. */
  Q_PROPERTY(DMRRadioIDReference *defaultID READ defaultIdRef)
  /** Common boot settings. */
  Q_PROPERTY(BootSettings* boot READ boot);
  /** Common audio and tone settings. */
  Q_PROPERTY(AudioSettings *audio READ audio);
  /** Common tone settings. */
  Q_PROPERTY(ToneSettings *tone READ tone);
  /** The GNSS settings. */
  Q_PROPERTY(GNSSSettings *gnss READ gnss);
  /** The common DMR settings. */
  Q_PROPERTY(DMRSettings *dmr READ dmr);
  /** The settings extension for TyT devices. */
  Q_PROPERTY(TyTSettingsExtension* tyt READ tytExtension WRITE setTyTExtension)
  /** The settings extension for Radioddity devices. */
  Q_PROPERTY(RadiodditySettingsExtension * radioddity READ radioddityExtension WRITE setRadioddityExtension)
  /** Settings for AnyTone devices. */
  Q_PROPERTY(AnytoneSettingsExtension *anytone READ anytoneExtension WRITE setAnytoneExtension)

public:
  /** Default constructor. */
  explicit RadioSettings(QObject *parent=nullptr);

  bool copy(const ConfigItem &other);
  ConfigItem *clone() const;

  /** Resets the settings. */
  void clear();

  /** Returns the default channel power. */
  Channel::Power power() const;
  /** Sets the default channel power. */
  void setPower(Channel::Power power);

  /** Returns @c true if the transmit timeout (TOT) is disabled. */
  bool totDisabled() const;
  /** Returns the default transmit timeout (TOT) in seconds, 0=disabled. */
  Interval tot() const;
  /** Sets the default transmit timeout (TOT) in seconds, 0=disabled. */
  void setTOT(const Interval &sec);
  /** Disables the transmit timeout (TOT). */
  void disableTOT();

  /** Returns a reference to the default DMR radio Id. */
  DMRRadioIDReference *defaultIdRef() const;
  /** Returns the default DMR ID or nullptr, if non is set. */
  DMRRadioID *defaultId() const;
  /** Sets the default DMR ID. */
  void setDefaultId(DMRRadioID *id);

  /** Returns the boot settings. */
  BootSettings *boot() const;
  /** Returns the audio/tone settings. */
  AudioSettings *audio() const;
  /** Returns the tone settings. */
  ToneSettings *tone() const;
  /** Returns the GNSS settings. */
  GNSSSettings *gnss() const;
  /** Returns the DMR settings. */
  DMRSettings *dmr() const;

  /** Returns the TyT device specific radio settings. */
  TyTSettingsExtension *tytExtension() const;
  /** Sets the TyT device specific radio settings. */
  void setTyTExtension(TyTSettingsExtension *ext);

  /** Returns the Radioddity device specific radio settings. */
  RadiodditySettingsExtension *radioddityExtension() const;
  /** Sets the Radioddity device specific radio settings. */
  void setRadioddityExtension(RadiodditySettingsExtension *ext);

  /** Returns the AnyTone device specific radio settings. */
  AnytoneSettingsExtension *anytoneExtension() const;
  /** Sets the AnyTone device specific radio settings. */
  void setAnytoneExtension(AnytoneSettingsExtension *ext);

  bool parse(const YAML::Node &node, Context &ctx, const ErrorStack &err=ErrorStack());

protected:
  bool populate(YAML::Node &node, const Context &context, const ErrorStack &err=ErrorStack());

protected slots:
  /** Internal used callback whenever an extension is modified. */
  void onExtensionModified();

protected:
  /** Holds the global power setting. */
  Channel::Power _power;
  /** Holds the global transmit timeout. */
  Interval _transmitTimeOut;
  /** Reference to the default DMR radio ID. */
  DMRRadioIDReference *_defaultId;
  /** The boot settings. */
  BootSettings *_boot;
  /** The audio/tone settings. */
  AudioSettings *_audio;
  /** The tone settings. */
  ToneSettings *_tone;
  /** The GNSS settings. */
  GNSSSettings *_gnss;
  /** The DMR settings. */
  DMRSettings *_dmr;
  /** Device specific settings extension for TyT devices. */
  TyTSettingsExtension *_tytExtension;
  /** Device specific settings extension for Radioddity devices. */
  RadiodditySettingsExtension *_radioddityExtension;
  /** Device specific settings extension for AnyTone devices. */
  AnytoneSettingsExtension *_anytoneExtension;
};

#endif // RADIOCONFIG_HH
