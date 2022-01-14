#include "radiolimits.hh"

/* ********************************************************************************************* *
 * Implementation of RadioLimitContext
 * ********************************************************************************************* */
RadioLimitContext::RadioLimitContext(const RadioLimits &limits)
  : _limits(limits), _stack()
{
  // pass...
}

/* ********************************************************************************************* *
 * Implementation of RadioLimitProperty
 * ********************************************************************************************* */
RadioLimitValue::RadioLimitValue(QObject *parent)
  : QObject()

/* ********************************************************************************************* *
 * Implementation of RadioLimits
 * ********************************************************************************************* */
RadioLimits::RadioLimits(QObject *parent) : QObject(parent)
{

}
