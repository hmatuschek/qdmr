/** @mainpage libdmrconf -- A Library to program DMR radios.
 * Libdmrconf is a Qt based library, providing methods to assemble, read and program codeplugs for
 * cheap Chinese DMR radios. To this end, it aims at providing a common way to describe codeplugs
 * irrespective of the actual radio being used.
 *
 * The aim of this project is, to define a minimal but sufficient generic configuration for all
 * radios, such that all relevant ham-radio features of the radios can be used. This is a challenge,
 * as there are many manufacturers, each selling several different models, each having
 * a variety of different features. In fact, manufacturers provide a separate application to program
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
 * To this end, @c libdmrconf defines a configuration language in text format, that represents the
 * configuration of the radio and from which the actual binary code-plug for each radio is derived.
 * If a radio does not support a particular feature within the configuration, it simply gets
 * ignored. The configuration language as well as classes for reading and writing config files are
 * documented in the <a href="https://dm3mat.darc.de/qdmr/manual/#conf">manual</a>.
 *
 * @section usage Using libdmrconf
 *   - Using cmake to find libdmrconf
 *   - setting paths
 *   - linking
 *   - include files
 *
 * @section interface Interfacing radios
 * In a first step, detect all USB devices that could be connected radios. Some radios use generic
 * USB CDC-AMC (USB-to-serial) chips. In these cases be careful, that you do not confuse them with
 * other hardware connected to the host. So, to detect USB interfaces use the device specific
 * interfaces, e.g. @c AnytoneInterface::detect(), @c RadioddityInterface::detect() or
 * @c TyTInterface::detect(). For example
 * @code
 *  #include <libdmrconf.hh>
 *
 *  int main(void) {
 *    // Find any supported devices connected
 *    QList<USBDeviceDescriptor> devices = AnytoneInterface::detect() +
 *                                         TyTInterface::detect() +
 *                                         RadioddityInterface::detect() +
 *                                         OpenGD77Interface::detect();
 *    if (0 == devices.size()) {
 *      std::cerr << "No matching device found.";
 *      return -1;
 *    }
 *
 *    // As the connected radio may use a generic USB CDC-ACM chip, check if the interface is safe.
 *    if (! devices.first().isSave()) {
 *      std::cerr << "Sorry, the detected device is not save to talk to automatically, "
 *                   "check if the chosen device is the correct one.";
 *      return -1;
 *    }
 *
 *    // Then try to connect to the first found.
 *    // The radio interface is common to all devices. So the remaining code is device independent.
 *    Radio *radio = Radio::detect(devices.first(), RadioInfo(), err);
 *    if (nullptr == radio) {
 *      std::cerr << "Opps:" << err.format();
 *      return -1;
 *    }
 *
 *    // Download codeplug
 *    if (! radio->startDownload(true, err)) {
 *      std::cerr << "Error while downloading codeplug:" << err.format();
 *      delete radio;
 *      return -1;
 *    }
 *
 *    // Decode codeplug to device independent config
 *    Config config;
 *    if (! radio->codeplug().decode(&config, err)) {
 *      std::cerr << "Cannot decode codeplug:" << err.format();
 *      delete radio;
 *      return -1;
 *    }
 *
 *    // Now, manipulate the generic codeplug in config.
 *
 *    // Verify codeplug with radio
 *    RadioLimitContext issues;
 *    if (! radio->limits().verifyConfig(config, issues)) {
 *      // Dump issues...
 *      delete radio;
 *      return -1;
 *    }
 *
 *    // Encode and upload codeplug back to the device
 *    if (! radio->startUpload(config, true, Codeplug::Flags(), err)) {
 *      std::err << "Cannot write codeplug: " << err.format();
 *      delete radio;
 *      return -1;
 *    }
 *
 *    // Done.
 *    delete radio;
 *    return 0;
 *  }
 * @endcode
 *
 * @section codeplug Reading and writing codeplug files
 *   - Using YAML for reading and writing codeplug files
 *
 * @section config Manipulating codeplugs
 *   - Some examples for basic manipulations of the @c Config object.
 *
 */

#ifndef __LIBDMRCONF_HH__
#define __LIBDMRCONF_HH__

#include "config.h"
#include "config.hh"
#include "csvreader.hh"

#include "radio.hh"

#include "anytone_interface.hh"
#include "tyt_interface.hh"
#include "opengd77_interface.hh"
#include "radioddity_interface.hh"

#endif // __LIBDMRCONF_HH__
