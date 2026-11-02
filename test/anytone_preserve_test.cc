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
  QCOMPARE(plan.size(), 0);
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
  QVERIFY2((plan.size() >= 1) && (plan.size() <= 2),
           qPrintable(QString("expected 1-2 blocks, got %1").arg(plan.size())));

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
  QVERIFY(plan.size() >= 1);

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
