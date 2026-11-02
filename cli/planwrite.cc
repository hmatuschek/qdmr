#include "planwrite.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <memory>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "anytone_preserve.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"
#include "d168uv_codeplug.hh"

static AnytoneCodeplug *makeCodeplug(RadioInfo::Radio radio) {
  switch (radio) {
  case RadioInfo::D868UVE:  return new D868UVCodeplug();
  case RadioInfo::D878UV:   return new D878UVCodeplug();
  case RadioInfo::D878UVII: return new D878UV2Codeplug();
  case RadioInfo::D578UV:   return new D578UVCodeplug();
  case RadioInfo::D168UV:   return new D168UVCodeplug();
  default: break;
  }
  return nullptr;
}

int planWrite(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(app);
  if (2 > parser.positionalArguments().size())
    parser.showHelp(-1);
  if ((! parser.isSet("radio")) || (! parser.isSet("memory")) || (! parser.isSet("plan"))) {
    logError() << "plan-write needs --radio=<key>, --memory=<file> and --plan=<file>.";
    return -1;
  }
  QString key = parser.value("radio").toLower();
  if (! RadioInfo::hasRadioKey(key)) {
    logError() << "Unknown radio '" << key << "'.";
    return -1;
  }
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(RadioInfo::byKey(key).id()));
  if (! cp) {
    logError() << "plan-write supports the AnyTone D868UVE, D878UV, D878UVII, D578UV and D168UV only.";
    return -1;
  }

  ErrorStack err;
  AnytonePreservingWrite::Memory mem;
  if (! AnytonePreservingWrite::readMemoryFile(parser.value("memory"), mem, err)) {
    logError() << err.format();
    return -1;
  }

  QFileInfo fileinfo(parser.positionalArguments().at(1));
  Config config;
  if (! config.readYAML(fileinfo.canonicalFilePath(), err)) {
    logError() << "Cannot parse YAML codeplug '" << fileinfo.fileName() << "': " << err.format();
    return -1;
  }
  Config *intermediate = cp->preprocess(&config, err);
  if (nullptr == intermediate) {
    logError() << "Cannot pre-process codeplug: " << err.format();
    return -1;
  }

  Codeplug::Flags flags;
  flags.setUpdateCodeplug(true);
  flags.setAutoEnableGPS(parser.isSet("auto-enable-gps"));
  flags.setAutoEnableRoaming(parser.isSet("auto-enable-roaming"));

  AnytonePreservingWrite::Plan plan;
  AnytonePreservingWrite::Stats stats;
  bool ok = AnytonePreservingWrite::plan(*cp, AnytonePreservingWrite::memoryReader(mem), intermediate,
                                         flags, plan, stats, parser.value("save-current"), nullptr, err);
  delete intermediate;
  if (! ok) {
    logError() << err.format();
    return -1;
  }
  QString header = QString("radio: %1, offline plan from memory file %2 (nothing written)")
      .arg(key, QFileInfo(parser.value("memory")).fileName());
  if (! AnytonePreservingWrite::writePlanFile(parser.value("plan"), plan, stats, header, err)) {
    logError() << err.format();
    return -1;
  }
  QTextStream out(stdout);
  out << "blocks read " << stats.blocksRead << ", considered " << stats.blocksConsidered
      << ", to write " << stats.blocksToWrite << " (new " << stats.blocksNew << "), bytes changed "
      << stats.bytesChanged << "\n";
  out.flush();
  return 0;
}
