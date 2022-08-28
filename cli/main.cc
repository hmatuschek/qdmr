#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>

#include "logger.hh"
#include "config.h"
#include "detect.hh"
#include "verify.hh"
#include "radioinfo.hh"
#include "readcodeplug.hh"
#include "writecodeplug.hh"
#include "writecallsigndb.hh"
#include "encodecodeplug.hh"
#include "encodecallsigndb.hh"
#include "decodecodeplug.hh"
#include "infofile.hh"

#include "uv390_codeplug.hh"

int main(int argc, char *argv[])
{
  // Install log handler to stderr.
  QTextStream out(stderr);
  StreamLogHandler *handler = new StreamLogHandler(out, LogMessage::WARNING, true);
  Logger::get().addHandler(handler);

  // Instantiate core application
  QCoreApplication app(argc, argv);
  app.setApplicationName("dmrconf");
  app.setOrganizationName("DM3MAT");
  app.setOrganizationDomain("dm3mat.darc.de");
  app.setApplicationVersion(VERSION_STRING);

  QCommandLineParser parser;
  parser.setApplicationDescription(
        QCoreApplication::translate(
          "main", "Up- and download codeplugs for cheap Chinese DMR radios."));

  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption({
                     {"V","verbose"},
                     QCoreApplication::translate("main", "Verbose output.")
                   });
  parser.addOption({
                     {"c", "csv"},
                     QCoreApplication::translate("main", "Up- and download codeplugs in CSV format.")
                   });
  parser.addOption({
                     {"y", "yaml"},
                     QCoreApplication::translate("main", "Up- and download codeplugs in extensible YAML format.")
                   });
  parser.addOption({
                     {"b", "bin"},
                     QCoreApplication::translate("main", "Up- and download codeplugs in binary format.")
                   });
  parser.addOption({
                     {"m", "manufacturer"},
                     QCoreApplication::translate("main", "Given file is manufacturer codeplug file. "
                     " Can be used with 'decode'.")
                   });
  parser.addOption({
                     {"D","device"},
                     QCoreApplication::translate("main", "Specifies the device to use to talk to "
                     "the radio. If not specified, the dmrconf will try to detect the radio "
                     "automatically. Please note, that for some radios the device must be specified."),
                     QCoreApplication::translate("main", "DEVICE")
                   });
  parser.addOption({
                     {"R", "radio"},
                     QCoreApplication::translate("main", "Specifies the radio. This option can also "
                     "be used to override the auto-detection of radios. Be careful using this "
                     "option when writing to the device. A incompatible code-plug might be written."),
                     QCoreApplication::translate("main", "RADIO")
                   });
  parser.addOption({
                     {"i", "id"},
                     QCoreApplication::translate("main", "Specifies the DMR id."),
                     QCoreApplication::translate("main", "ID")
                   });
  parser.addOption({
                     {"n", "limit"},
                     QCoreApplication::translate("main", "Limits several amonuts, depending on the "
                     "context. When encoding/writing the callsign db, this option specifies the "
                     "maximum number of callsigns to encode."),
                     QCoreApplication::translate("main", "N")
                   });
  parser.addOption(QCommandLineOption(
                     "init-codeplug",
                     QCoreApplication::translate(
                       "main", "Initializes the code-plug in the radio. If not present (default) "
                               "the code-plug gets updated, maintining all settings made earlier.")));
  parser.addOption(QCommandLineOption(
                     "auto-enable-gps",
                     QCoreApplication::translate("main", "Automatically enables GPS if there is a "
                                                         "GPS/APRS system used by any channel.")));
  parser.addOption(QCommandLineOption(
                     "auto-enable-roaming",
                     QCoreApplication::translate("main", "Automatically enables roaming if there is a "
                                                         "roaming zone used by any channel.")));
  parser.addOption(QCommandLineOption(
                     "ignore-limits",
                     QCoreApplication::translate("main", "Disables some limit checks.")));
  parser.addOption(QCommandLineOption(
                     "list-radios",
                     QCoreApplication::translate("main", "Lists all supported radios including the "
                                                 "keys to be used with the --radio option.")));
  parser.addPositionalArgument(
        "command", QCoreApplication::translate(
          "main", "Specifies the command to perform. Either detect, verify, read, write, "
          "write-db, encode, encode-db, decode or info. Consult the man-page of dmrconf for a "
          "detailed description of these commands."),
        QCoreApplication::translate("main", "[command]"));

  parser.addPositionalArgument(
        "file", QCoreApplication::translate(
          "main", "The code-plug file. Either binary (extension .dfu), text/csv (extension .conf "
          "or .csv) or YAML format (extension .yaml). The format can be forced using the --csv, "
          "--yaml or --binary options."),
        QCoreApplication::translate("main", "[filename]"));

  parser.process(app);

  if (parser.isSet("list-radios")) {
    QList<RadioInfo> radios = RadioInfo::allRadios();
    QTextStream out(stdout);
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << " Key";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << "Name";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(60); out << "Manufacturer";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(60); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
    foreach (RadioInfo radio, radios) {
      out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << (" " +radio.key());
      out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
      out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << radio.name();
      out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
      out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << radio.manufactuer() << "\n";
    }
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(60); out << "-";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
    out.flush();
    return 0;
  }

  if (1 > parser.positionalArguments().size())
    parser.showHelp(-1);

  if (parser.isSet("verbose"))
    handler->setMinLevel(LogMessage::DEBUG);

  QString command = parser.positionalArguments().at(0);
  if ("detect" == command)
    return detect(parser, app);
  if ("verify" == command)
    return verify(parser, app);
  if ("read" == command)
    return readCodeplug(parser, app);
  if ("write" == command)
    return writeCodeplug(parser, app);
  if ("write-db" == command)
    return writeCallsignDB(parser, app);
  if ("encode" == command)
    return encodeCodeplug(parser, app);
  if ("encode-db" == command)
    return encodeCallsignDB(parser, app);
  if ("decode" == command)
    return decodeCodeplug(parser, app);
  if ("info" == command)
    return infoFile(parser, app);

  parser.showHelp(-1);
  return -1;
}
