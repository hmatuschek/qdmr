#ifndef HD1TEST_HH
#define HD1TEST_HH

#include "libdmrconfigtest.hh"

class HD1Test : public UnitTestBase
{
  Q_OBJECT

public:
  explicit HD1Test(QObject *parent = nullptr);

private slots:
  void testBasicConfigEncoding();
  void testBasicConfigDecoding();
  void testBasicConfigReencoding();
  void testPostProcessingOfEmptyCodeplug();
  /** A channel's member list is the members of its RX group list. */
  void testGroupListMembers();
  /** A custom member set (no matching list) becomes a list named after the channel. */
  void testCustomMemberSet();
  /** More than 250 lists still encode; channels keep their members. */
  void testManyGroupLists();
  /** CTCSS tones are BCD in 0.1 Hz, up to 254.1 Hz. */
  void testCTCSS();
  /** DCS codes: 0x8000 | 0x4000 (inverted) | octal digits as nibbles. */
  void testDCS();
  /** Bits 4 and 5 of 0xa1 hold the offset direction (TX above / below RX). */
  void testOffsetDirection();
  /** A default radio ID that is not the first one. */
  void testDefaultIdNotFirst();
  /** Zone B survives through zone splitting and merging. */
  void testDualZone();
  /** AM channels are dropped by preprocessing. */
  void testAMChannelFiltered();
  /** Unmodelled bytes stay with their channel when channels move. */
  void testUnmodelledBytesMove();
  /** A renamed channel keeps the unmodelled bytes of its slot. */
  void testRenamedChannelKeepsBytes();
  /** A TX contact reference qdmr cannot represent is kept. */
  void testDanglingContactKept();
  /** The channel usage bitmap follows the channel list. */
  void testChannelBitmap();
  /** A new channel's TX contact is what the manufacturer's software writes for none. */
  void testNewChannelTXContact();
};

#endif // HD1TEST_HH
