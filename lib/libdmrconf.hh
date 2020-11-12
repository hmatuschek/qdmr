/** @mainpage libdmrconf -- A Library to program DMR radios.
 * Libdmrconf is a Qt based library, providing methods to assemble, read and program codeplugs for
 * cheap Chineese DMR radios. To this end, it aims at providing a common way to describe codeplugs
 * irrespective of the actual radio being used. This project relies heavily on the work by
 * Serge Vakulenko (https://github.com/sergev/dmrconfig).
 *
 * The aim of this project is, to define a minimal but sufficient generic configuration for all
 * radios, such that all relevant ham-radio features of the radios can be used. This is a challange,
 * as there are many manufacturers, each selling serveral different models, each having
 * a varity of different features. In fact, manufacturers provide a separate application to programm
 * each model. The resulting codeplugs are usually stored in a binary format,
 * similar to the actual codeplug being loaded onto the radio.
 *
 * This makes absolutely sense for commercial applications, like radios used by staff at large
 * events. In these cases, the organizer of the event would buy large quantities of identical radios and
 * assemble a single codeplug for all radios.
 *
 * For ham-radio purposes, however, this incompatibility between models and manufactures is an
 * issue. Assembling a decent codeplug for a region with several repeaters is cumbersome. As these
 * applications cannot share codeplugs between models let alone between manufactures, this
 * cumbersome work has to be repeated for each model, although the basic configuration of these
 * models like contact, channels & zones remains identical. In fact, many features of these radios
 * are specific to the commercial application and are not used in ham-radio context.
 *
 * A common minimalistic configuration of these radios for ham-radio use would allow for sharing
 * codeplugs between different models and even manufactures. Hence, the hard work to
 * assemble a decent codeplug for a particular region only needs to be done once.
 *
 * To this end, @c libdmrconf defines a configuarion language in text format, that represents the
 * configuration of the radio and from which the actual binary code-plug for each radio is derived.
 * If a radio does not support a particular feature within the configuration, it simply gets
 * ignored. The configuration language as well as classes for reading and writing config files are
 * documented in the @link conffile config-file module@endlink.
 */

#ifndef __LIBDMRCONF_HH__
#define __LIBDMRCONF_HH__

#include "config.h"
#include "config.hh"
#include "codeplug.hh"
#include "csvreader.hh"
#include "csvreader.hh"

#include "radio.hh"
#include "uv390.hh"
#include "rd5r.hh"
#include "opengd77.hh"
#include "gd77.hh"
#include "d878uv.hh"

#endif // __LIBDMRCONF_HH__
