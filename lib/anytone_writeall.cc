#include "anytone_writeall.hh"

#include <QFile>
#include <QSet>
#include <QTextStream>
#include <algorithm>
#include "logger.hh"
#include "anytone_codeplug.hh"
#include "d878uv2_codeplug.hh"

static constexpr unsigned BS = AnytonePreservingWrite::BlockSize;

/* D878UVII: every block AnyTone CPS 3.06 writes, from the USB capture of the CPS writing radio B
 * (ham-radio-coordinator W-01 golden capture, 2026-10-30; 12,986 blocks in 29 pages). Page size
 * 16 KB: a write of 0x00800000..0x0080003F erased exactly 0x00800040..0x00803FFF and left the
 * next page untouched (W-04 §4b). The CPS skips empty channel slots, so its blocks depend on the
 * set-up; whole pages are read and every block holding data is sent, so that doesn't matter. */
static const AnytoneWriteAll::Map d878uv2Map = {
  "AnyTone AT-D878UVII", 0x4000, {
    {0x00800000, 0x00800400}, {0x00800440, 0x00800480}, {0x008004c0, 0x00800a40},
    {0x00800a80, 0x00800b80}, {0x00800bc0, 0x00802400}, {0x00802440, 0x00802480},
    {0x008024c0, 0x00802a40}, {0x00802a80, 0x00802b80}, {0x00802bc0, 0x00804000},
    {0x00840000, 0x00844000}, {0x00880000, 0x00881200}, {0x00881240, 0x00883200},
    {0x00883240, 0x00884000}, {0x008c0000, 0x008c4000}, {0x00900000, 0x00904000},
    {0x00940000, 0x00940bc0}, {0x00940c00, 0x00942bc0}, {0x00942c00, 0x00944000},
    {0x00980000, 0x00984000}, {0x009c0000, 0x009c11c0}, {0x009c1200, 0x009c1780},
    {0x009c1a00, 0x009c1b40}, {0x009c1b80, 0x009c1d00}, {0x009c1d40, 0x009c1e40},
    {0x009c1e80, 0x009c31c0}, {0x009c3200, 0x009c3780}, {0x009c3a00, 0x009c3b40},
    {0x009c3b80, 0x009c3d00}, {0x009c3d40, 0x009c3e40}, {0x009c3e80, 0x009c4000},
    {0x00fc0800, 0x00fc0880}, {0x00fc2800, 0x00fc2880}, {0x01000000, 0x01001400},
    {0x01040000, 0x01040020}, {0x01042000, 0x01042020}, {0x01042080, 0x01042090},
    {0x01043000, 0x01043080}, {0x01080000, 0x01080090}, {0x01640800, 0x01640890},
    {0x02480000, 0x02480010}, {0x02480200, 0x02480230}, {0x024c0c80, 0x024c0c90},
    {0x024c0d00, 0x024c0f00}, {0x024c1000, 0x024c10d0}, {0x024c1280, 0x024c12a0},
    {0x024c1300, 0x024c1380}, {0x024c1400, 0x024c1420}, {0x024c1440, 0x024c1470},
    {0x024c1500, 0x024c1740}, {0x024c1800, 0x024c1d00}, {0x024c2000, 0x024c23f0},
    {0x024c2600, 0x024c2610}, {0x024c4000, 0x024c8020}, {0x02500000, 0x025000f0},
    {0x02500100, 0x02500630}, {0x02501000, 0x02501240}, {0x02501280, 0x025012b0},
    {0x02501400, 0x02501600}, {0x02501800, 0x02501900}, {0x02504000, 0x02504400},
    {0x02540000, 0x02540140}, {0x02580000, 0x02580020}, {0x02582000, 0x02582020},
    {0x025c0000, 0x025c0860}, {0x025c0b00, 0x025c0b30}, {0x025c0c00, 0x025c1bf0},
    {0x025c1c00, 0x025c1c20}, {0x02600000, 0x02609c40}, {0x02640000, 0x026404f0},
    {0x02680000, 0x02680320}, {0x02900000, 0x02900080}, {0x02900100, 0x02900180},
    {0x04800000, 0x04800040}
  },
  "CPS 3.06 write capture of radio B, 2026-10-30 (ham-radio-coordinator W-01, W-04 §4c)"
};


const AnytoneWriteAll::Map *
AnytoneWriteAll::forCodeplug(const AnytoneCodeplug &cp) {
  if (dynamic_cast<const D878UV2Codeplug *>(&cp))
    return &d878uv2Map;
  return nullptr;
}

bool
AnytoneWriteAll::blank(const QByteArray &block) {
  for (char c : block)
    if (char(0xff) != c)
      return false;
  return true;
}

QList<unsigned>
AnytoneWriteAll::pages(const Map &map) {
  QSet<unsigned> set;
  for (const auto &r : map.cps)
    for (unsigned a = r.first & ~(map.pageSize-1); a < r.second; a += map.pageSize)
      set.insert(a);
  QList<unsigned> res(set.begin(), set.end());
  std::sort(res.begin(), res.end());
  return res;
}

static bool
readPage(const AnytoneWriteAll::Reader &read, unsigned page, unsigned size,
         AnytoneWriteAll::Memory &img, unsigned &count, const ErrorStack &err) {
  for (unsigned a = page; a < page + size; a += BS) {
    if (img.contains(a))
      continue;
    QByteArray b(BS, '\0');
    if (! read(a, reinterpret_cast<uint8_t *>(b.data()), BS, err)) {
      errMsg(err) << "Cannot read block 0x" << QString::number(a, 16) << ".";
      return false;
    }
    img[a] = b;
    count++;
  }
  return true;
}

bool
AnytoneWriteAll::build(AnytoneCodeplug &cp, const Map &map, const Reader &read, Config *edited,
                       const Codeplug::Flags &flags, Result &res, const QString &currentYAML,
                       const std::function<void(int)> &progress, const ErrorStack &err) {
  const unsigned P = map.pageSize;
  res = Result();
  res.pages = pages(map);

  // 1. Read the CPS's pages whole.
  for (int i = 0; i < res.pages.size(); i++) {
    if (! readPage(read, res.pages[i], P, res.image, res.blocksRead, err)) {
      errMsg(err) << "Cannot read page 0x" << QString::number(res.pages[i], 16) << "; nothing was written.";
      return false;
    }
    if (progress)
      progress(int(80.0 * (i+1) / res.pages.size()));
  }

  // 2. Plan the edit; blocks the planner needs outside the pages are read too (not sent).
  Memory extra;
  Reader cached = [&res, &extra, &read](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &e) -> bool {
    for (unsigned off = 0; off < size; off += BS) {
      unsigned a = addr + off;
      auto it = res.image.constFind(a);
      if (it == res.image.cend()) {
        it = extra.constFind(a);
        if (it == extra.cend()) {
          QByteArray b(BS, '\0');
          if (! read(a, reinterpret_cast<uint8_t *>(b.data()), BS, e))
            return false;
          it = extra.insert(a, b);
        }
      }
      memcpy(dst + off, it->constData(), BS);
    }
    return true;
  };
  if (! AnytonePreservingWrite::plan(cp, cached, edited, flags, res.plan, res.planStats,
                                     currentYAML, nullptr, err, false)) {
    errMsg(err) << "Cannot plan the edit; nothing was written.";
    return false;
  }

  // 3. Keep only changed blocks; a page the edit touches outside the CPS's pages is read whole.
  for (auto it = res.plan.begin(); it != res.plan.end(); ) {
    if (it->before == it->after) { it = res.plan.erase(it); continue; }
    unsigned page = it.key() & ~(P-1);
    if (! res.pages.contains(page)) {
      for (unsigned a = page; a < page + P; a += BS)   // reuse what the planner already read
        if (extra.contains(a) && (! res.image.contains(a)))
          res.image[a] = extra[a];
      if (! readPage(read, page, P, res.image, res.blocksRead, err)) {
        errMsg(err) << "Cannot read page 0x" << QString::number(page, 16) << "; nothing was written.";
        return false;
      }
      res.pages.append(page);
      res.pagesAdded++;
    }
    res.blocksChanged++;
    for (int i = 0; i < int(BS); i++)
      if ((it->before.size() != int(BS)) || (it->before[i] != it->after[i]))
        res.bytesChanged++;
    ++it;
  }
  std::sort(res.pages.begin(), res.pages.end());

  // 4. What to send: every CPS block and every block holding data, with the edit applied.
  QSet<unsigned> cps;
  for (const auto &r : map.cps)
    for (unsigned a = r.first; a < r.second; a += BS)
      cps.insert(a);
  for (unsigned page : res.pages) {
    for (unsigned a = page; a < page + P; a += BS) {
      QByteArray b = res.plan.contains(a) ? res.plan[a].after : res.image.value(a);
      if (cps.contains(a) || (! blank(b))) {
        res.send[a] = b;
        if (! cps.contains(a))
          res.blocksSentNotCps++;
      }
      res.expected[a] = res.send.contains(a) ? res.send[a] : QByteArray(BS, char(0xff));
    }
  }

  // 5. Self-check: after the pages are erased and the blocks sent, every byte must equal the
  //    radio's own except the edit's.
  for (unsigned page : res.pages)
    for (unsigned a = page; a < page + P; a += BS) {
      QByteArray want = res.plan.contains(a) ? res.plan[a].after : res.image.value(a);
      if (res.expected.value(a) != want) {
        errMsg(err) << "Internal check failed at block 0x" << QString::number(a, 16)
                    << ": it would not survive the write; nothing was written.";
        return false;
      }
    }
  if (progress)
    progress(90);
  return true;
}

bool
AnytoneWriteAll::writeRangesFile(const QString &filename, const QList<unsigned> &pages,
                                 unsigned pageSize, const QString &header, const ErrorStack &err) {
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    errMsg(err) << "Cannot write ranges file '" << filename << "': " << file.errorString();
    return false;
  }
  QTextStream out(&file);
  out << "# " << header << "\n";
  for (unsigned p : pages)
    out << QString("%1 %2\n").arg(p, 8, 16, QChar('0')).arg(p + pageSize, 8, 16, QChar('0'));
  out.flush();
  return true;
}
