#include "anytone_preserve_test.hh"
#include "anytone_preserve.hh"
#include "anytone_codeplug.hh"
#include "d868uv_codeplug.hh"
#include "d878uv_codeplug.hh"
#include "d878uv2_codeplug.hh"
#include "d578uv_codeplug.hh"
#include "d168uv_codeplug.hh"
#include "config.hh"
#include "errorstack.hh"
#include <QTest>
#include <QFile>
#include <QTemporaryDir>
#include <QRegularExpression>
#include <memory>

typedef AnytonePreservingWrite APW;

static AnytoneCodeplug *makeCodeplug(const QString &key) {
  if ("d868uve" == key) return new D868UVCodeplug();
  if ("d878uv" == key)  return new D878UVCodeplug();
  if ("d878uv2" == key) return new D878UV2Codeplug();
  if ("d578uv" == key)  return new D578UVCodeplug();
  if ("d168uv" == key)  return new D168UVCodeplug();
  return nullptr;
}

static void radios() {
  QTest::addColumn<QString>("radio");
  for (const char *r : {"d868uve", "d878uv", "d878uv2", "d578uv", "d168uv"})
    QTest::newRow(r) << QString(r);
}

/** A simulated radio: blocks from @c mem, anything else reads as 0x00. */
static APW::Reader simulatedRadio(const APW::Memory &mem) {
  return [mem](unsigned addr, uint8_t *dst, unsigned size, const ErrorStack &) -> bool {
    for (unsigned off=0; off<size; off+=APW::BlockSize) {
      auto it = mem.constFind(addr+off);
      if (it == mem.cend()) memset(dst+off, 0, APW::BlockSize);
      else memcpy(dst+off, it->constData(), APW::BlockSize);
    }
    return true;
  };
}

/** Builds the simulated radio's memory: the test codeplug encoded from scratch. */
static bool makeRadio(AnytoneCodeplug &cp, APW::Memory &mem, QString &msg) {
  ErrorStack err;
  Config config;
  if (! config.readYAML(":/data/anytone_preserve_test.yaml", err)) { msg = err.format(); return false; }
  Config *in = cp.preprocess(&config, err);
  if (! in) { msg = err.format(); return false; }
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  bool ok = cp.encode(in, flags, err);
  delete in;
  if (! ok) { msg = err.format(); return false; }
  mem.clear();
  cp.image(0).sort();
  for (int n=0; n<cp.image(0).numElements(); n++) {
    unsigned addr = cp.image(0).element(n).address();
    unsigned size = cp.image(0).element(n).data().size();
    for (unsigned off=0; off<size; off+=APW::BlockSize)
      mem[addr+off] = QByteArray(reinterpret_cast<const char *>(cp.data(addr)+off), APW::BlockSize);
  }
  return true;
}

/** Plans a write of the YAML text @c yaml onto @c mem; returns the plan and the radio's
 * current set-up (as YAML text) before the write. */
static bool planYAML(AnytoneCodeplug &cp, const APW::Memory &mem, const QString &yaml,
                     APW::Plan &plan, QString &current, QString &msg) {
  QTemporaryDir tmp;
  QString in = tmp.filePath("edited.yaml"), cur = tmp.filePath("current.yaml");
  { QFile f(in); f.open(QIODevice::WriteOnly); f.write(yaml.toUtf8()); }
  ErrorStack err;
  Config config;
  if (! config.readYAML(in, err)) { msg = "read edited: " + err.format(); return false; }
  Config *pre = cp.preprocess(&config, err);
  if (! pre) { msg = err.format(); return false; }
  Codeplug::Flags flags;
  APW::Stats stats;
  bool ok = APW::plan(cp, simulatedRadio(mem), pre, flags, plan, stats, cur, nullptr, err);
  delete pre;
  if (! ok) { msg = err.format(); return false; }
  QFile f(cur); f.open(QIODevice::ReadOnly); current = QString::fromUtf8(f.readAll());
  return true;
}

/** Reads what the radio holds now (C0 as YAML), by planning a write of the test file. */
static bool readRadio(AnytoneCodeplug &cp, const APW::Memory &mem, QString &yaml, QString &msg) {
  QFile f(":/data/anytone_preserve_test.yaml"); f.open(QIODevice::ReadOnly);
  APW::Plan plan;
  return planYAML(cp, mem, QString::fromUtf8(f.readAll()), plan, yaml, msg);
}

/** Planned blocks whose content changes (padding blocks are written as the radio has them). */
static int changed(const APW::Plan &plan) {
  int n = 0;
  for (auto it=plan.cbegin(); it!=plan.cend(); it++) if (it->before != it->after) n++;
  return n;
}

static APW::Memory apply(const APW::Memory &mem, const APW::Plan &plan) {
  APW::Memory out(mem);
  for (auto it=plan.cbegin(); it!=plan.cend(); it++) out[it.key()] = it->after;
  return out;
}

static bool load(const QString &yaml, Config &config, QString &msg) {
  QTemporaryDir tmp; QString fn = tmp.filePath("c.yaml");
  { QFile f(fn); f.open(QIODevice::WriteOnly); f.write(yaml.toUtf8()); }
  ErrorStack err;
  if (! config.readYAML(fn, err)) { msg = err.format(); return false; }
  return true;
}

/** Everything about the digital channels that D-112 says must never change. */
static QStringList digitalFingerprint(const Config &c) {
  QStringList out;
  for (int i=0; i<c.channelList()->count(); i++) {
    Channel *ch = c.channelList()->channel(i);
    if (! ch->is<DMRChannel>()) continue;
    DMRChannel *d = ch->as<DMRChannel>();
    out << QString("%1|%2|%3|%4|%5|%6|slot %7").arg(d->name(), d->rxFrequency().format(),
          d->txFrequency().format()).arg(d->colorCode()).arg(int(d->timeSlot()))
          .arg(int(d->admit())).arg(AnytoneCodeplug::channelSlot(d));
  }
  return out;
}

/** Zone members (names, in order), leaving out the named channels. */
static QStringList zoneFingerprint(const Config &c, const QStringList &skip=QStringList()) {
  QStringList out;
  for (int z=0; z<c.zones()->count(); z++) {
    Zone *zone = c.zones()->zone(z);
    QStringList a, b;
    for (int i=0; i<zone->A()->count(); i++) {
      QString n = zone->A()->get(i)->name(); if (! skip.contains(n)) a << n; }
    for (int i=0; i<zone->B()->count(); i++) {
      QString n = zone->B()->get(i)->name(); if (! skip.contains(n)) b << n; }
    out << zone->name() + " A: " + a.join(", ") + " B: " + b.join(", ");
  }
  return out;
}

static Channel *byName(const Config &c, const QString &name) {
  for (int i=0; i<c.channelList()->count(); i++)
    if (c.channelList()->channel(i)->name() == name) return c.channelList()->channel(i);
  return nullptr;
}


AnytonePreserveTest::AnytonePreserveTest(QObject *parent) : QObject(parent) { }

void AnytonePreserveTest::testNoEditWritesNothing_data() { radios(); }
void AnytonePreserveTest::testNoEditWritesNothing() {
  QFETCH(QString, radio);
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(radio));
  APW::Memory mem; QString msg, c0;
  QVERIFY2(makeRadio(*cp, mem, msg), qPrintable(msg));
  QVERIFY2(readRadio(*cp, mem, c0, msg), qPrintable(msg));
  APW::Plan plan; QString again;
  QVERIFY2(planYAML(*cp, mem, c0, plan, again, msg), qPrintable(msg));
  QCOMPARE(changed(plan), 0);
  QCOMPARE(plan.size(), 0); // nothing changed, so no area is padded either
}

void AnytonePreserveTest::testAnalogEditLeavesDigitalAlone_data() { radios(); }
void AnytonePreserveTest::testAnalogEditLeavesDigitalAlone() {
  QFETCH(QString, radio);
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(radio));
  APW::Memory mem; QString msg, c0;
  QVERIFY2(makeRadio(*cp, mem, msg), qPrintable(msg));
  QVERIFY2(readRadio(*cp, mem, c0, msg), qPrintable(msg));

  // Edit: FM B 145.6125 -> 145.6375 MHz (receive and transmit)
  QString edited = c0;
  QCOMPARE(edited.count("145.6125 MHz"), 2);
  edited.replace("145.6125 MHz", "145.6375 MHz");
  APW::Plan plan; QString cur;
  QVERIFY2(planYAML(*cp, mem, edited, plan, cur, msg), qPrintable(msg));
  QVERIFY2((changed(plan) >= 1) && (changed(plan) <= 2),
           qPrintable(QString("expected 1-2 changed blocks, got %1").arg(changed(plan))));
  // Whole-area padding: more blocks written, but only with the radio's own bytes.
  QVERIFY(plan.size() > changed(plan));
  for (auto it=plan.cbegin(); it!=plan.cend(); it++)
    if (it->before == it->after)
      QCOMPARE(it->after, mem.value(it.key(), QByteArray(APW::BlockSize, 0)));

  // What the radio holds afterwards
  APW::Memory after = apply(mem, plan);
  QString c2;
  QVERIFY2(readRadio(*cp, after, c2, msg), qPrintable(msg));
  Config before, now;
  QVERIFY2(load(c0, before, msg), qPrintable(msg));
  QVERIFY2(load(c2, now, msg), qPrintable(msg));
  QVERIFY(byName(now, "FM B"));
  QCOMPARE(byName(now, "FM B")->rxFrequency(), Frequency::fromMHz(145.6375));
  QCOMPARE(byName(now, "FM B")->txFrequency(), Frequency::fromMHz(145.6375));
  QCOMPARE(byName(now, "FM A")->rxFrequency(), byName(before, "FM A")->rxFrequency());
  QCOMPARE(digitalFingerprint(now), digitalFingerprint(before));
  QCOMPARE(zoneFingerprint(now), zoneFingerprint(before));

  // Idempotent: writing the same edit again changes nothing.
  APW::Plan again;
  QVERIFY2(planYAML(*cp, after, edited, again, cur, msg), qPrintable(msg));
  QCOMPARE(again.size(), 0);
}

void AnytonePreserveTest::testAddAnalogChannelLeavesDigitalAlone_data() { radios(); }
void AnytonePreserveTest::testAddAnalogChannelLeavesDigitalAlone() {
  QFETCH(QString, radio);
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(radio));
  APW::Memory mem; QString msg, c0;
  QVERIFY2(makeRadio(*cp, mem, msg), qPrintable(msg));
  QVERIFY2(readRadio(*cp, mem, c0, msg), qPrintable(msg));

  // Add an FM channel "FM NEW" and append it to the first zone's A list.
  QString edited = c0;
  int z = edited.indexOf("\nzones:");
  QVERIFY(z > 0);
  edited.insert(z+1, QString("  - fm:\n      id: chnew\n      name: FM NEW\n      rxFrequency: 146.52 MHz\n"
                             "      txFrequency: 146.52 MHz\n      bandwidth: Narrow\n      power: High\n"));
  QRegularExpression firstA("(\n    A: \\[[^\\]\n]*)\\]");
  QRegularExpressionMatch m = firstA.match(edited, edited.indexOf("\nzones:"));
  QVERIFY2(m.hasMatch(), "zone A list not found");
  edited.replace(m.capturedStart(0), m.capturedLength(0), m.captured(1) + ", chnew]");

  APW::Plan plan; QString cur;
  QVERIFY2(planYAML(*cp, mem, edited, plan, cur, msg), qPrintable(msg));
  QVERIFY(changed(plan) >= 1);

  APW::Memory after = apply(mem, plan);
  QString c2;
  QVERIFY2(readRadio(*cp, after, c2, msg), qPrintable(msg));
  Config before, now;
  QVERIFY2(load(c0, before, msg), qPrintable(msg));
  QVERIFY2(load(c2, now, msg), qPrintable(msg));
  QVERIFY2(byName(now, "FM NEW"), "new channel missing after write");
  QCOMPARE(digitalFingerprint(now), digitalFingerprint(before));
  QCOMPARE(zoneFingerprint(now, {"FM NEW"}), zoneFingerprint(before));
  QCOMPARE(now.channelList()->count(), before.channelList()->count()+1);
}

/** Zone names in the order of their places in the radio's zone list, with their places. */
static QStringList zonePlaces(const Config &c) {
  QStringList out;
  for (int z=0; z<c.zones()->count(); z++)
    out << QString("%1@%2").arg(c.zones()->zone(z)->name()).arg(AnytoneCodeplug::zoneSlot(c.zones()->zone(z)));
  return out;
}

/** Appends an FM channel and a zone holding it to a set-up given as YAML text. */
static bool addChannelAndZone(QString &yaml, const QString &chId, const QString &chName,
                              const QString &zoneName) {
  int z = yaml.indexOf("\nzones:");
  if (z < 0) return false;
  yaml.insert(z+1, QString("  - fm:\n      id: %1\n      name: %2\n      rxFrequency: 146.52 MHz\n"
                           "      txFrequency: 146.52 MHz\n      bandwidth: Wide\n      power: Low\n")
              .arg(chId, chName));
  z = yaml.indexOf("\nzones:");
  int end = yaml.indexOf(QRegularExpression("\n[a-zA-Z]"), z+1);
  if (end < 0) end = yaml.size();
  yaml.insert(end, QString("\n  - id: znew_%1\n    name: %2\n    A: [%1]\n    B: []").arg(chId, zoneName));
  return true;
}

void AnytonePreserveTest::testZonesKeepTheirPlaces_data() { radios(); }
void AnytonePreserveTest::testZonesKeepTheirPlaces() {
  // W-06 case 3: a radio whose zone list has a gap. qdmr used to close the gap, so a new zone
  // was written over the zone after it. Zones must keep their places; new ones fill the gap.
  QFETCH(QString, radio);
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(radio));
  APW::Memory mem; QString msg, c0;
  QVERIFY2(makeRadio(*cp, mem, msg), qPrintable(msg));
  QVERIFY2(readRadio(*cp, mem, c0, msg), qPrintable(msg));
  Config before;
  QVERIFY2(load(c0, before, msg), qPrintable(msg));
  QVERIFY(before.zones()->count() >= 2);
  QStringList places0 = zonePlaces(before);
  for (int z=0; z<before.zones()->count(); z++)
    QVERIFY(AnytoneCodeplug::zoneSlot(before.zones()->zone(z)) >= 0); // every zone has a place

  // 1. Make a gap: remove the first zone. The others must stay where they are.
  QString gap = c0;
  QRegularExpression firstZone("\n  - id: [^\n]+\n    name: [^\n]+\n(    [^\n]*\n)*?(?=  - id: |[a-zA-Z])");
  int zs = gap.indexOf("\nzones:");
  QRegularExpressionMatch m = firstZone.match(gap, zs+7);
  QVERIFY2(m.hasMatch() && (m.capturedStart(0) == zs+7), "first zone not found");
  QString removed = before.zones()->zone(0)->name();
  QRegularExpression idRe("\n  - id: ([^\n]+)\n");
  QString zid = idRe.match(m.captured(0)).captured(1).trimmed();
  QVERIFY(! zid.isEmpty());
  gap.remove(m.capturedStart(0)+1, m.capturedLength(0)-1);
  // Settings may point at the removed zone (e.g. the zone shown on start-up): drop those lines.
  gap.remove(QRegularExpression(QString("\n[ ]+[A-Za-z]+: %1(?=\n)").arg(QRegularExpression::escape(zid))));
  APW::Plan plan; QString cur;
  QVERIFY2(planYAML(*cp, mem, gap, plan, cur, msg), qPrintable(msg));
  APW::Memory holed = apply(mem, plan);
  QString c1;
  QVERIFY2(readRadio(*cp, holed, c1, msg), qPrintable(msg));
  Config mid;
  QVERIFY2(load(c1, mid, msg), qPrintable(msg));
  QStringList expect = places0.mid(1);
  QCOMPARE(zonePlaces(mid), expect);

  // 2. Add a channel and a new zone holding it: the zone takes the free place 0, nothing moves.
  QString added = c1;
  QVERIFY(addChannelAndZone(added, "chw06", "W06 NEW", "NEW CHANNELS"));
  APW::Plan plan2;
  QVERIFY2(planYAML(*cp, holed, added, plan2, cur, msg), qPrintable(msg));
  APW::Memory after = apply(holed, plan2);
  QString c2;
  QVERIFY2(readRadio(*cp, after, c2, msg), qPrintable(msg));
  Config now;
  QVERIFY2(load(c2, now, msg), qPrintable(msg));
  // The first zone used place 0 (and place 1 for its B half, if it had one); the new zone
  // takes place 0 and every other zone stays where it was.
  QCOMPARE(zonePlaces(now), QStringList({"NEW CHANNELS@0"}) + expect);
  QVERIFY2(byName(now, "W06 NEW"), "new channel not in the radio (slot not marked in use)");
  QCOMPARE(zoneFingerprint(now, {"W06 NEW"}).mid(1), zoneFingerprint(mid));
  QCOMPARE(digitalFingerprint(now), digitalFingerprint(before));
  Q_UNUSED(removed);

  // Idempotent.
  APW::Plan again;
  QVERIFY2(planYAML(*cp, after, added, again, cur, msg), qPrintable(msg));
  QCOMPARE(again.size(), 0);
}

void AnytonePreserveTest::testAPRSOnlyChannelTakesNoSlot_data() { radios(); }
void AnytonePreserveTest::testAPRSOnlyChannelTakesNoSlot() {
  // W-06 case 3: when the radio's FM APRS frequency matches no channel, decoding creates an
  // "APRS Channel" that has no slot and is in no zone. It used to take the radio's lowest free
  // slot on every write, so a new channel pushed it into another free slot, written as a real
  // channel, and the new channel's slot was never marked in use.
  QFETCH(QString, radio);
  std::unique_ptr<AnytoneCodeplug> cp(makeCodeplug(radio));
  QFile tf(":/data/anytone_preserve_test.yaml"); tf.open(QIODevice::ReadOnly);
  QString base = QString::fromUtf8(tf.readAll());
  base.replace("\r\n", "\n"); // a Windows checkout may give the test file CRLF line ends
  // An FM APRS system whose revert channel is in no zone (so the radio holds it only in its
  // APRS settings).
  int cs = base.indexOf("\nchannels:");
  QVERIFY(cs >= 0);
  int z = base.indexOf(QRegularExpression("\n[a-zA-Z]"), cs+1);
  QVERIFY(z > 0);
  base.insert(z+1, "  - fm:\n      id: aprsch\n      name: APRS TX\n      rxFrequency: 144.39 MHz\n"
                   "      txFrequency: 144.39 MHz\n      bandwidth: Wide\n      power: Low\n");
  int p = base.indexOf("\npositioning:");
  QVERIFY(p > 0);
  base.insert(p+14, "  - aprs:\n      id: fmaprs\n      name: APRS APAT81\n      period: 300\n"
                    "      revert: aprsch\n      icon: Jogger\n      message: test\n"
                    "      destination: APAT81-0\n      source: DM3MAT-7\n      path: [WIDE1-1, WIDE2-1]\n");
  ErrorStack err; Config config;
  QTemporaryDir tmp; QString fn = tmp.filePath("b.yaml");
  { QFile f(fn); f.open(QIODevice::WriteOnly); f.write(base.toUtf8()); }
  QVERIFY2(config.readYAML(fn, err), qPrintable(err.format()));
  Config *in = cp->preprocess(&config, err);
  QVERIFY2(in, qPrintable(err.format()));
  Codeplug::Flags flags; flags.setUpdateCodeplug(false);
  QVERIFY2(cp->encode(in, flags, err), qPrintable(err.format()));
  delete in;
  APW::Memory mem;
  cp->image(0).sort();
  for (int n=0; n<cp->image(0).numElements(); n++) {
    unsigned addr = cp->image(0).element(n).address();
    unsigned size = cp->image(0).element(n).data().size();
    for (unsigned off=0; off<size; off+=APW::BlockSize)
      mem[addr+off] = QByteArray(reinterpret_cast<const char *>(cp->data(addr)+off), APW::BlockSize);
  }

  QString msg, c0; APW::Plan plan0;
  QVERIFY2(planYAML(*cp, mem, base, plan0, c0, msg), qPrintable(msg));
  Config before;
  QVERIFY2(load(c0, before, msg), qPrintable(msg));
  int nBefore = before.channelList()->count();

  // An unedited write changes nothing.
  APW::Plan none; QString cur;
  QVERIFY2(planYAML(*cp, mem, c0, none, cur, msg), qPrintable(msg));
  QCOMPARE(none.size(), 0);

  // Add a channel in a zone.
  QString added = c0;
  QVERIFY(addChannelAndZone(added, "chw06", "W06 NEW", "NEW CHANNELS"));
  APW::Plan plan;
  QVERIFY2(planYAML(*cp, mem, added, plan, cur, msg), qPrintable(msg));
  APW::Memory after = apply(mem, plan);
  QString c2;
  QVERIFY2(readRadio(*cp, after, c2, msg), qPrintable(msg));
  Config now;
  QVERIFY2(load(c2, now, msg), qPrintable(msg));
  QVERIFY2(byName(now, "W06 NEW"), "new channel not in the radio (slot not marked in use)");
  QVERIFY(AnytoneCodeplug::channelSlot(byName(now, "W06 NEW")) >= 0);
  // Exactly one channel more: no APRS channel written into a free slot.
  QCOMPARE(now.channelList()->count(), nBefore+1);
  for (int i=0; i<now.channelList()->count(); i++) {
    Channel *ch = now.channelList()->channel(i);
    if (ch->name() == "APRS TX")
      QCOMPARE(AnytoneCodeplug::channelSlot(ch), -1);
  }
}

void AnytonePreserveTest::testMemoryFileRoundTrip() {
  APW::Memory mem;
  mem[0x00000000] = QByteArray(16, char(0x01));
  mem[0x02500010] = QByteArray::fromHex("00112233445566778899aabbccddeeff");
  QTemporaryDir tmp; QString fn = tmp.filePath("m.txt");
  QVERIFY(APW::writeMemoryFile(fn, mem));
  APW::Memory back;
  QVERIFY(APW::readMemoryFile(fn, back));
  QCOMPARE(back, mem);
}

QTEST_GUILESS_MAIN(AnytonePreserveTest)
