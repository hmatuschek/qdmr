#include "readaddresses.hh"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>

#include "logger.hh"
#include "radio.hh"
#include "progressbar.hh"
#include "autodetect.hh"
#include "anytone_radio.hh"
#include "anytone_preserve.hh"


static bool
readRanges(const QString &filename, QList<QPair<unsigned, unsigned>> &ranges) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    logError() << "Cannot read ranges file '" << filename << "': " << file.errorString();
    return false;
  }
  QTextStream in(&file);
  unsigned lineNo = 0;
  while (! in.atEnd()) {
    QString line = in.readLine().trimmed();
    lineNo++;
    if (line.isEmpty() || line.startsWith('#'))
      continue;
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    bool ok1 = false, ok2 = false;
    unsigned start = (parts.size() >= 2) ? parts[0].toUInt(&ok1, 16) : 0;
    unsigned end   = (parts.size() >= 2) ? parts[1].toUInt(&ok2, 16) : 0;
    if ((! ok1) || (! ok2) || (start >= end) || (start % 16) || (end % 16)) {
      logError() << "Ranges file '" << filename << "', line " << lineNo
                 << ": expected 'START END' in hex, 16-byte aligned, START < END.";
      return false;
    }
    ranges.append({start, end});
  }
  if (ranges.isEmpty()) {
    logError() << "Ranges file '" << filename << "' lists no ranges.";
    return false;
  }
  return true;
}


int
readAddresses(QCommandLineParser &parser, QCoreApplication &app) {
  if (3 > parser.positionalArguments().size()) {
    logError() << "Usage: dmrconf read-addr RANGES-FILE MEMORY-FILE";
    return -1;
  }
  QList<QPair<unsigned, unsigned>> ranges;
  if (! readRanges(parser.positionalArguments().at(1), ranges))
    return -1;
  QString outFile = parser.positionalArguments().at(2);

  ErrorStack err;
  Radio *radio = autoDetect(parser, app, err);
  if (nullptr == radio) {
    logError() << "Cannot detect radio: " << err.format();
    return -1;
  }
  AnytoneRadio *anytone = qobject_cast<AnytoneRadio *>(radio);
  if (nullptr == anytone) {
    logError() << "read-addr is only available for AnyTone radios.";
    delete radio;
    return -1;
  }

  if (! parser.isSet("verbose")) {
    showProgress();
    QObject::connect(radio, &Radio::downloadProgress, updateProgress);
  }

  AnytonePreservingWrite::Memory mem;
  unsigned failedAddr = 0;
  bool complete = anytone->readAddresses(ranges, mem, failedAddr, err);

  // Save whatever was read, even after a failure, so it can be inspected.
  ErrorStack saveErr;
  bool saved = mem.isEmpty() ? true
                             : AnytonePreservingWrite::writeMemoryFile(outFile, mem, saveErr);
  if (! saved)
    logError() << "Cannot save memory file: " << saveErr.format();

  if (! complete) {
    logError() << err.format();
    logError() << "Stopped at block 0x" << QString::number(failedAddr, 16) << "; "
               << mem.size() << " blocks read and saved before it. Nothing was written.";
    delete radio;
    return 2;
  }
  logInfo() << "Read " << mem.size() << " blocks into '" << outFile << "'. Nothing was written.";
  delete radio;
  return saved ? 0 : -1;
}
