#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "config.h"
#include "detect.hh"
#include "verify.hh"
#include "readcodeplug.hh"
#include "writecodeplug.hh"
#include "encodecodeplug.hh"
#include "decodecodeplug.hh"
#include "infofile.hh"


int main(int argc, char *argv[])
{
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
   {{"d","detect"}, QCoreApplication::translate("main", "Detect radio.")});
  parser.addOption(
   {{"c", "csv"}, QCoreApplication::translate("main", "Up- and download codeplugs in CSV format")});
  parser.addOption(
   {{"b", "bin"}, QCoreApplication::translate("main", "Up- and download codeplugs in binary format")});
  parser.addOption(
   {{"w","write"}, QCoreApplication::translate("main", "Upload codeplug to radio.")});
  parser.addOption(
   {{"r", "read"}, QCoreApplication::translate("main", "Download codeplug from radio.")});
  parser.addOption(
   {{"V", "verify"}, QCoreApplication::translate("main", "Verify codeplug.")});
  parser.addOption(
   {{"E", "encode"}, QCoreApplication::translate("main", "Encodes a CSV codeplug to a binary one.")});
  parser.addOption(
   {{"D", "decode"}, QCoreApplication::translate("main", "Decodes a binary codeplug to a CSV one.")});
  parser.addOption(
   {{"I", "info"}, QCoreApplication::translate("main", "Drops some information about the given file.")});
  parser.addOption(
   {{"R", "radio"}, QCoreApplication::translate("main", "Specifies the radio."), QCoreApplication::translate("main", "directory")});

  parser.addPositionalArgument(
        "file", QCoreApplication::translate("main", "Codeplug file."),
        QCoreApplication::translate("main", "[filename]"));
  parser.process(app);

  if (parser.isSet("detect"))
    return detect(parser, app);
  else if (parser.isSet("verify"))
    return verify(parser, app);
  else if (parser.isSet("read"))
    return readCodeplug(parser, app);
  else if (parser.isSet("write"))
    return writeCodeplug(parser, app);
  else if (parser.isSet("encode"))
    return encodeCodeplug(parser, app);
  else if (parser.isSet("decode"))
    return decodeCodeplug(parser, app);
  else if (parser.isSet("info"))
    return infoFile(parser, app);

  return -1;
}
