#ifndef ANYTONELIMITS_HH
#define ANYTONELIMITS_HH

#include "radiolimits.hh"

/** Base class of limtis for all AnyTone radios.
 * This class extends @c RadioLimits and implements the hardware revision check.
 * @ingroup anytone */
class AnytoneLimits: public RadioLimits
{
  Q_OBJECT

protected:
  /** Constructor.
   * @param hardwareRevision Specifies the hardware revision as reported by the radio. If empty,
   *        no check is perfored.
   * @param supportedRevision Specifies the supported hardware revision.
   * @param betaWarning If @c true, a warning is issued that the radio is still under development.
   * @param parent Specifies the QObject parent. */
  AnytoneLimits(const QString &hardwareRevision, const QString &supportedRevision, bool betaWarning,
                QObject *parent=nullptr);

public:
  bool verifyConfig(const Config *config, RadioLimitContext &context) const;

protected:
  /** Holds the hardware revision of the radio. */
  QString _hardwareRevision;
  /** Holds the supported hardware revision of the radio. */
  QString _supportedRevision;
};

#endif // ANYTONELIMITS_HH
