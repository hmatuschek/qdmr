#include "anytone_limits.hh"

AnytoneLimits::AnytoneLimits(const QString &hardwareRevision, const QString &supportedRevision, bool betaWarning, QObject *parent)
  : RadioLimits(betaWarning, parent), _hardwareRevision(hardwareRevision), _supportedRevision(supportedRevision)
{
  // pass...
}

bool
AnytoneLimits::verifyConfig(const Config *config, RadioLimitContext &context) const {
  bool success = RadioLimits::verifyConfig(config, context);

  if (_hardwareRevision.isEmpty())
    return success;

  if (_supportedRevision < _hardwareRevision) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg = tr("You are likely using a newer radio reversion (%1) than supported (%2) by qdmr. "
             "The codeplug might be incompatible. "
             "Notify the developers of qdmr about the new reversion.").arg(_hardwareRevision, _supportedRevision);
  } else if (_supportedRevision > _hardwareRevision) {
    auto &msg = context.newMessage(RadioLimitIssue::Warning);
    msg = tr("You are likely using an older hardware reversion (%1) than supported (%2) by qdmr. "
             "The codeplug might be incompatible.").arg(_hardwareRevision, _supportedRevision);
  }

  return success;
}
