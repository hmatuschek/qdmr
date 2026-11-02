#include "anytone_preserve.hh"
#include "anytone_codeplug.hh"
#include "config.hh"
#include "logger.hh"

#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <cstring>


bool
AnytonePreservingWrite::readMissing(AnytoneCodeplug &cp, const Reader &read, Memory &mem,
                                    Stats &stats, const ErrorStack &err)
{
  for (int n=0; n<cp.image(0).numElements(); n++) {
    unsigned addr = cp.image(0).element(n).address();
    unsigned size = cp.image(0).element(n).data().size();
    if ((addr % BlockSize) || (size % BlockSize)) {
      errMsg(err) << "Codeplug element at 0x" << QString::number(addr, 16)
                  << " (size " << size << ") is not aligned to " << BlockSize << " bytes.";
      return false;
    }
    // Read contiguous runs of blocks we do not hold yet
    unsigned off = 0;
    while (off < size) {
      if (mem.contains(addr+off)) { off += BlockSize; continue; }
      unsigned start = off;
      while ((off < size) && (! mem.contains(addr+off)))
        off += BlockSize;
      QByteArray buffer(off-start, 0);
      if (! read(addr+start, reinterpret_cast<uint8_t *>(buffer.data()), off-start, err)) {
        errMsg(err) << "Cannot read radio memory at 0x" << QString::number(addr+start, 16) << ".";
        return false;
      }
      for (unsigned b=0; b<(off-start); b+=BlockSize) {
        mem[addr+start+b] = buffer.mid(b, BlockSize);
        stats.blocksRead++;
      }
    }
  }
  fill(cp, mem);
  return true;
}


void
AnytonePreservingWrite::fill(AnytoneCodeplug &cp, const Memory &mem) {
  for (int n=0; n<cp.image(0).numElements(); n++) {
    unsigned addr = cp.image(0).element(n).address();
    unsigned size = cp.image(0).element(n).data().size();
    for (unsigned off=0; off<size; off+=BlockSize) {
      auto it = mem.find(addr+off);
      if (it != mem.end())
        memcpy(cp.data(addr)+off, it->constData(), BlockSize);
    }
  }
}


AnytonePreservingWrite::Memory
AnytonePreservingWrite::dump(AnytoneCodeplug &cp) {
  Memory out;
  cp.image(0).sort();
  for (int n=0; n<cp.image(0).numElements(); n++) {
    unsigned addr = cp.image(0).element(n).address();
    unsigned size = cp.image(0).element(n).data().size();
    for (unsigned off=0; off<size; off+=BlockSize)
      out[addr+off] = QByteArray(reinterpret_cast<const char *>(cp.data(addr)+off), BlockSize);
  }
  return out;
}


bool
AnytonePreservingWrite::encodeOn(AnytoneCodeplug &cp, const Memory &mem, Config *config,
                                 const Codeplug::Flags &flags, Memory &out, const ErrorStack &err)
{
  // Same sequence as AnytoneRadio::upload(): bitmaps, then the update areas, then encode.
  cp.clear();
  fill(cp, mem);
  cp.allocateUpdated();
  fill(cp, mem);
  Codeplug::Flags f(flags);
  f.setUpdateCodeplug(true);
  if (! cp.encode(config, f, err)) {
    errMsg(err) << "Cannot encode codeplug.";
    return false;
  }
  out = dump(cp);
  return true;
}


bool
AnytonePreservingWrite::plan(AnytoneCodeplug &cp, const Reader &read, Config *edited,
                             const Codeplug::Flags &flags, Plan &plan, Stats &stats,
                             const QString &currentYAML, Memory *radioMemory, const ErrorStack &err)
{
  plan.clear();
  stats = Stats();
  Memory orig;

  // 1. Read what an update reads (bitmaps, then update areas) and what a decode reads.
  cp.clear();
  if (! readMissing(cp, read, orig, stats, err)) return false;
  cp.allocateUpdated();
  if (! readMissing(cp, read, orig, stats, err)) return false;
  cp.clear();
  fill(cp, orig);
  cp.allocateForDecoding();
  if (! readMissing(cp, read, orig, stats, err)) return false;
  if (radioMemory)
    *radioMemory = orig;
  logDebug() << "Preserving write: read " << stats.blocksRead << " blocks from the radio.";

  // 2. Decode C0 from a codeplug holding exactly the decode areas, as a download does.
  cp.clear();
  fill(cp, orig);
  cp.allocateForDecoding();
  fill(cp, orig);
  Config decoded;
  if (! cp.decode(&decoded, err)) {
    errMsg(err) << "Cannot decode the radio's current codeplug.";
    return false;
  }
  if (! cp.postprocess(&decoded, err)) {
    errMsg(err) << "Cannot post-process the radio's current codeplug.";
    return false;
  }

  // C0 goes through YAML exactly like the edited file does.
  QTemporaryDir tmp;
  QString c0path = currentYAML.isEmpty() ? tmp.filePath("current.yaml") : currentYAML;
  {
    QFile file(c0path);
    if (! file.open(QIODevice::WriteOnly)) {
      errMsg(err) << "Cannot write '" << c0path << "': " << file.errorString();
      return false;
    }
    QTextStream stream(&file);
    if (! decoded.toYAML(stream, err)) {
      errMsg(err) << "Cannot serialize the radio's current codeplug.";
      return false;
    }
    stream.flush();
    file.close();
  }
  Config c0;
  if (! c0.readYAML(c0path, err)) {
    errMsg(err) << "Cannot re-read the radio's current codeplug.";
    return false;
  }
  Config *c0i = cp.preprocess(&c0, err);
  if (nullptr == c0i) {
    errMsg(err) << "Cannot pre-process the radio's current codeplug.";
    return false;
  }

  // 3. Encode C0 and C1 the same way, on top of what the radio holds.
  Memory e0, e1;
  bool ok = encodeOn(cp, orig, c0i, flags, e0, err);
  delete c0i;
  if (! ok) {
    errMsg(err) << "Cannot encode the radio's current codeplug.";
    return false;
  }
  if (! encodeOn(cp, orig, edited, flags, e1, err)) {
    errMsg(err) << "Cannot encode the edited codeplug.";
    return false;
  }

  // 4. and 5. Merge bit by bit; keep only blocks that change.
  for (auto it=e1.cbegin(); it!=e1.cend(); it++) {
    unsigned addr = it.key();
    const QByteArray &n1 = it.value();
    stats.blocksConsidered++;
    auto o = orig.constFind(addr);
    auto z = e0.constFind(addr);
    bool held = (o != orig.cend());
    QByteArray fin(BlockSize, 0);
    bool anyEdit = false;
    for (unsigned i=0; i<BlockSize; i++) {
      uint8_t e1b = uint8_t(n1.at(i));
      uint8_t d = (z == e0.cend()) ? 0xff : uint8_t(uint8_t(z->at(i)) ^ e1b);
      anyEdit |= (0 != d);
      if (! held)
        fin[i] = char(e1b);
      else
        fin[i] = char((uint8_t(o->at(i)) & ~d) | (e1b & d));
    }
    if ((! held) && (! anyEdit))
      continue;
    if (held && (o.value() == fin))
      continue;
    Block blk;
    blk.after = fin;
    if (held) {
      blk.before = o.value();
      for (unsigned i=0; i<BlockSize; i++)
        if (blk.before.at(i) != fin.at(i)) stats.bytesChanged++;
    } else {
      stats.blocksNew++;
      stats.bytesChanged += BlockSize;
      logWarn() << "Preserving write: the edit writes block 0x" << QString::number(addr, 16)
                << ", which was not read from the radio.";
    }
    plan[addr] = blk;
  }
  stats.blocksToWrite = plan.size();
  logDebug() << "Preserving write: " << stats.blocksConsidered << " blocks considered, "
             << stats.blocksToWrite << " to write.";
  return true;
}


bool
AnytonePreservingWrite::writePlanFile(const QString &filename, const Plan &plan, const Stats &stats,
                                      const QString &header, const ErrorStack &err)
{
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    errMsg(err) << "Cannot write plan file '" << filename << "': " << file.errorString();
    return false;
  }
  QTextStream out(&file);
  out << "# Preserving write plan (dmrconf, gsquared3d fork)\n";
  if (! header.isEmpty())
    out << "# " << header << "\n";
  out << "# blocks read: " << stats.blocksRead << "\n"
      << "# blocks considered: " << stats.blocksConsidered << "\n"
      << "# blocks to write: " << stats.blocksToWrite << " (new: " << stats.blocksNew << ")\n"
      << "# bytes changed: " << stats.bytesChanged << "\n"
      << "# address before after\n";
  for (auto it=plan.cbegin(); it!=plan.cend(); it++) {
    out << QString("%1").arg(it.key(), 8, 16, QChar('0')) << " "
        << (it->before.isEmpty() ? QString("--") : QString(it->before.toHex())) << " "
        << QString(it->after.toHex()) << "\n";
  }
  out.flush();
  return true;
}


bool
AnytonePreservingWrite::readMemoryFile(const QString &filename, Memory &mem, const ErrorStack &err) {
  QFile file(filename);
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    errMsg(err) << "Cannot read memory file '" << filename << "': " << file.errorString();
    return false;
  }
  mem.clear();
  QTextStream in(&file);
  unsigned lineNo = 0;
  while (! in.atEnd()) {
    QString line = in.readLine().trimmed();
    lineNo++;
    if (line.isEmpty() || line.startsWith('#'))
      continue;
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    bool ok = false;
    unsigned addr = (parts.size() >= 2) ? parts[0].toUInt(&ok, 16) : 0;
    QByteArray data = (parts.size() >= 2) ? QByteArray::fromHex(parts[1].toLatin1()) : QByteArray();
    if ((! ok) || (BlockSize != unsigned(data.size())) || (addr % BlockSize)) {
      errMsg(err) << "Memory file '" << filename << "', line " << lineNo << ": expected "
                  << "'address data' with " << BlockSize << " bytes of data.";
      return false;
    }
    mem[addr] = data;
  }
  return true;
}


bool
AnytonePreservingWrite::writeMemoryFile(const QString &filename, const Memory &mem, const ErrorStack &err) {
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    errMsg(err) << "Cannot write memory file '" << filename << "': " << file.errorString();
    return false;
  }
  QTextStream out(&file);
  for (auto it=mem.cbegin(); it!=mem.cend(); it++)
    out << QString("%1").arg(it.key(), 8, 16, QChar('0')) << " " << QString(it->toHex()) << "\n";
  out.flush();
  return true;
}


AnytonePreservingWrite::Reader
AnytonePreservingWrite::memoryReader(const Memory &mem) {
  return [mem](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &err) -> bool {
    for (unsigned off=0; off<size; off+=BlockSize) {
      auto it = mem.constFind(addr+off);
      if (it == mem.cend()) {
        errMsg(err) << "Memory file lacks block 0x" << QString::number(addr+off, 16) << ".";
        return false;
      }
      memcpy(dst+off, it->constData(), BlockSize);
    }
    return true;
  };
}
