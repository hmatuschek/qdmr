#include "anytone_codeplug_gen2.hh"
#include "intermediaterepresentation.hh"
#include "config.hh"


/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen2::CTCSS
 * ********************************************************************************************* */
const SelectiveCall
AnytoneCodeplugGen2::CTCSS::_codeTable[] = {
  SelectiveCall(62.5),  SelectiveCall(67.0),  SelectiveCall(69.3),  SelectiveCall(71.9),
  SelectiveCall(74.4),  SelectiveCall(77.0),  SelectiveCall(79.7),  SelectiveCall(82.5),
  SelectiveCall(85.4),  SelectiveCall(88.5),  SelectiveCall(91.5),  SelectiveCall(94.8),
  SelectiveCall(97.4),  SelectiveCall(100.0), SelectiveCall(103.5), SelectiveCall(107.2),
  SelectiveCall(110.9), SelectiveCall(114.8), SelectiveCall(118.8), SelectiveCall(123.0),
  SelectiveCall(127.3), SelectiveCall(131.8), SelectiveCall(136.5), SelectiveCall(141.3),
  SelectiveCall(146.2), SelectiveCall(151.4), SelectiveCall(156.7), SelectiveCall(159.8),
  SelectiveCall(162.2), SelectiveCall(165.5), SelectiveCall(167.9), SelectiveCall(171.3),
  SelectiveCall(173.8), SelectiveCall(177.3), SelectiveCall(179.9), SelectiveCall(183.5),
  SelectiveCall(186.2), SelectiveCall(189.9), SelectiveCall(192.8), SelectiveCall(196.6),
  SelectiveCall(199.5), SelectiveCall(203.5), SelectiveCall(206.5), SelectiveCall(210.7),
  SelectiveCall(218.1), SelectiveCall(225.7), SelectiveCall(229.1), SelectiveCall(233.6),
  SelectiveCall(241.8), SelectiveCall(250.3), SelectiveCall(254.1)
};

uint8_t
AnytoneCodeplugGen2::CTCSS::encode(const SelectiveCall &tone) {
  for (uint8_t i=0; i<customIndex(); i++) {
    if (tone == _codeTable[i])
      return i;
  }
  return customIndex();
}

SelectiveCall
AnytoneCodeplugGen2::CTCSS::decode(uint8_t idx) {
  if (idx >= customIndex())
    return SelectiveCall();
  return _codeTable[idx];
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen2::BitmapElement and InvertedBitmapElement
 * ********************************************************************************************* */
AnytoneCodeplugGen2::BitmapElement::BitmapElement(uint8_t *ptr, size_t size)
  : Codeplug::BitmapElement(ptr, size)
{
  // pass...
}

AnytoneCodeplugGen2::InvertedBitmapElement::InvertedBitmapElement(uint8_t *ptr, size_t size)
  : Codeplug::InvertedBitmapElement(ptr, size)
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of AnytoneCodeplugGen2
 * ********************************************************************************************* */
AnytoneCodeplugGen2::AnytoneCodeplugGen2(const QString &label, QObject *parent)
  : AnytoneCodeplugBase(parent), _label(label)
{
  // pass...
}

void
AnytoneCodeplugGen2::clear() {
  while (this->numImages())
    remImage(0);
  addImage(_label);
}

Config *
AnytoneCodeplugGen2::preprocess(Config *config, const ErrorStack &err) const {
  Config *intermediate = AnytoneCodeplugBase::preprocess(config, err);
  if (nullptr == intermediate) {
    errMsg(err) << "Cannot pre-process codeplug for anytone device.";
    return nullptr;
  }

  ZoneSplitVisitor splitter;
  if (! splitter.process(intermediate, err)) {
    errMsg(err) << "Split multi-VFO zones.";
    delete intermediate;
    return nullptr;
  }

  return intermediate;
}

bool
AnytoneCodeplugGen2::postprocess(Config *config, const ErrorStack &err) const {
  if (! AnytoneCodeplugBase::postprocess(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for anytone device.";
    return false;
  }

  ZoneMergeVisitor merger;
  if (! merger.process(config, err)) {
    errMsg(err) << "Cannot post-process codeplug for anytone device.";
    return false;
  }

  return true;
}
