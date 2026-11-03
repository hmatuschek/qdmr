#include "planwrite.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <memory>

#include "logger.hh"
#include "config.hh"
#include "radioinfo.hh"
#include "anytone_preserve.hh"
#include "anytone_writeall.hh"
#include <cstring>
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

  const AnytoneWriteAll::Map *map = AnytoneWriteAll::forCodeplug(*cp);
  if (nullptr == map) {
    logError() << "No write map for '" << key << "' yet (INV-020).";
    delete intermediate;
    return -1;
  }
  AnytonePreservingWrite::Reader reader = AnytonePreservingWrite::memoryReader(mem);
  unsigned missing = 0;
  if (parser.isSet("missing-blank")) {
    reader = [&mem, &missing](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &) -> bool {
      for (unsigned off = 0; off < size; off += AnytonePreservingWrite::BlockSize) {
        auto it = mem.constFind(addr + off);
        if (it == mem.cend()) { memset(dst + off, 0xff, AnytonePreservingWrite::BlockSize); missing++; }
        else memcpy(dst + off, it->constData(), AnytonePreservingWrite::BlockSize);
      }
      return true;
    };
  }

  AnytoneWriteAll::Result res;
  bool ok = AnytoneWriteAll::build(*cp, *map, reader, intermediate, flags, res,
                                   parser.value("save-current"), nullptr, err);
  delete intermediate;
  if (! ok) {
    logError() << err.format();
    return -1;
  }
  AnytonePreservingWrite::Stats s = res.planStats;
  s.blocksToWrite = res.send.size();
  s.blocksChanged = res.blocksChanged;
  s.bytesChanged = res.bytesChanged;
  s.blocksPadding = res.send.size() - res.blocksChanged;
  QString header = QString("radio: %1, offline plan from memory file %2 (nothing written); write "
                           "everything: %3 pages, %4 blocks sent, %5 changed (only changed blocks are listed)")
      .arg(key, QFileInfo(parser.value("memory")).fileName()).arg(res.pages.size())
      .arg(res.send.size()).arg(res.blocksChanged);
  if (! AnytonePreservingWrite::writePlanFile(parser.value("plan"), res.plan, s, header, err)) {
    logError() << err.format();
    return -1;
  }
  if (parser.isSet("expect")) {
    QString f = parser.value("expect");
    if ((! AnytonePreservingWrite::writeMemoryFile(f, res.expected, err))
        || (! AnytoneWriteAll::writeRangesFile(f + ".ranges", res.pages, map->pageSize, header, err))
        || (! AnytonePreservingWrite::writeMemoryFile(f + ".send", res.send, err))
        || (! AnytonePreservingWrite::writeMemoryFile(f + ".before", res.image, err))) {
      logError() << err.format();
      return -1;
    }
  }
  QTextStream out(stdout);
  out << "pages " << res.pages.size() << " (" << res.pagesAdded << " added by the edit), blocks read "
      << res.blocksRead << (parser.isSet("missing-blank") ? QString(" (%1 missing, taken as blank)").arg(missing) : QString())
      << ", changed " << res.blocksChanged << " (" << res.bytesChanged << " bytes), sent "
      << res.send.size() << " (" << res.blocksSentNotCps << " beyond the CPS's own)\n";
  out.flush();
  return 0;
}
