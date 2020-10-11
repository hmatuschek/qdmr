#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>

#include "logger.hh"
#include "config.h"
#include "detect.hh"
#include "verify.hh"
#include "readcodeplug.hh"
#include "writecodeplug.hh"
#include "encodecodeplug.hh"
#include "decodecodeplug.hh"
#include "infofile.hh"

#include "uv390_codeplug.hh"

int main(int argc, char *argv[])
{
  // Install log handler to stderr.
  QTextStream out(stderr);
  StreamLogHandler *handler = new StreamLogHandler(out, LogMessage::WARNING);
  Logger::get().addHandler(handler);

  // Instantiate core application
  QCoreApplication app(argc, argv);
  app.setApplicationName("dmrconf");
  app.setOrganizationName("dm3mat");
  app.setOrganizationDomain("dm3mat.darc.de");
  app.setApplicationVersion(VERSION_STRING);

  QCommandLineParser parser;
  parser.setApplicationDescription(
        QCoreApplication::translate("main", "Up- and Download codeplugs for cheap Chineese radios."));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(
  {{"V","verbose"}, QCoreApplication::translate("main", "Verbose output.")});
  parser.addOption(
   {{"c", "csv"}, QCoreApplication::translate("main", "Up- and download codeplugs in CSV format.")});
  parser.addOption(
   {{"b", "bin"}, QCoreApplication::translate("main", "Up- and download codeplugs in binary format.")});
  parser.addOption(
   {{"R", "radio"}, QCoreApplication::translate("main", "Specifies the radio."), QCoreApplication::translate("main", "RADIO")});

  parser.addPositionalArgument(
        "command", QCoreApplication::translate("main", "Specifies the command to perform."),
        QCoreApplication::translate("main", "[command]"));

  parser.addPositionalArgument(
        "file", QCoreApplication::translate("main", "Codeplug file."),
        QCoreApplication::translate("main", "[filename]"));

  parser.process(app);

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
  if ("encode" == command)
    return encodeCodeplug(parser, app);
  if ("decode" == command)
    return decodeCodeplug(parser, app);
  if ("info" == command)
    return infoFile(parser, app);

  parser.showHelp(-1);
  return -1;
}
