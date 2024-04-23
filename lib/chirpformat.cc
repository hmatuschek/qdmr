#include "chirpformat.hh"
#include "logger.hh"
#include "frequency.hh"
#include "signaling.hh"
#include "channel.hh"
#include "config.hh"

#include <QStringList>


/* ********************************************************************************************* *
 * Implementation of ChirpFormat
 * ********************************************************************************************* */
QSet<QString> const ChirpFormat::_mandatoryHeaders = {
  "Location", "Name", "Frequency", "Duplex", "Offset", "Mode"
};

QSet<QString> const ChirpFormat::_knownHeaders = {
  "Location", "Name", "Frequency", "Duplex", "Offset", "Mode"
  "Tone", "rToneFreq", "cToneFreq", "DtcsCode", "DtcsPolarity", "RxDtcsCode", "CrossMode",
  "TStep", "Skip", "Power", "Comment", "URCALL", "RPT1CALL", "RPT2CALL", "DVCODE"
};

QHash<QString, ChirpFormat::Duplex> const ChirpFormat::_duplexCodes = {
  {"", ChirpFormat::Duplex::None},
  {"+", ChirpFormat::Duplex::Positive},
  {"-", ChirpFormat::Duplex::Negative},
  {"Off", ChirpFormat::Duplex::Off},
};

QHash<QString, ChirpFormat::Mode> const ChirpFormat::_modeCodes = {
  {"FM", ChirpReader::Mode::FM},
  {"NFM", ChirpReader::Mode::NFM},
  {"WFM", ChirpReader::Mode::WFM},
  {"AM", ChirpReader::Mode::AM},
  {"DV", ChirpReader::Mode::DV},
  {"DN", ChirpReader::Mode::DN},
};

QHash<QString, ChirpFormat::ToneMode> const ChirpFormat::_toneModeCodes = {
  {"", ChirpFormat::ToneMode::None},
  {"Tone", ChirpFormat::ToneMode::Tone},
  {"TSQL", ChirpFormat::ToneMode::TSQL},
  {"TSQL-R", ChirpFormat::ToneMode::TSQL_R},
  {"DTCS", ChirpFormat::ToneMode::DTCS},
  {"DTCS-R", ChirpFormat::ToneMode::DTCS_R},
  {"Cross", ChirpFormat::ToneMode::Cross},
};

QHash<QString, ChirpFormat::CrossMode> const ChirpFormat::_crossModes = {
  {"", ChirpFormat::CrossMode::ToneNone},
  {"->Tone", ChirpFormat::CrossMode::NoneTone},
  {"->DTCS", ChirpFormat::CrossMode::NoneDTCS},
  {"Tone->", ChirpFormat::CrossMode::ToneNone},
  {"Tone->Tone", ChirpFormat::CrossMode::ToneTone},
  {"Tone->DTCS", ChirpFormat::CrossMode::ToneDTCS},
  {"DTCS->", ChirpFormat::CrossMode::DTCSNone},
  {"DTCS->Tone", ChirpFormat::CrossMode::DTCSTone},
  {"DTCS->DTCS", ChirpFormat::CrossMode::DTCSDTCS},
};


/* ********************************************************************************************* *
 * Implementation of ChirpReader
 * ********************************************************************************************* */
bool
ChirpReader::read(QTextStream &stream, Config *config, const ErrorStack &err) {
  // First read header
  QStringList header;
  if (! readLine(stream, header, err)) {
    errMsg(err) << "Cannot read CSV header.";
    return false;
  }

  // Some trivial sanity checks for the header
  if (0 == header.size()) {
    errMsg(err) << "Invalid CSV file header: Got empty header.";
    return false;
  }

  if ("Location" != header.at(0)) {
    errMsg(err) << "Invalid CSV file header: 'Location' is not first column!";
    return false;
  }

  // check fields
  foreach (QString field, _mandatoryHeaders) {
    if (! header.contains(field)) {
      errMsg(err) << "Mandatory column '" << field << "' is missing.";
      return false;
    }
  }

  foreach(QString field, header) {
    if (! _knownHeaders.contains(field)) {
      logInfo() << "Unknown header field '" << field << "'.";
    }
  }

  int line=2;
  while (! stream.atEnd()) {
    QStringList entry;
    if (! readLine(stream, entry, err)) {
      errMsg(err) << "In CSV file line " << line << ": Cannot read line.";
      return false;
    }
    if (! processLine(header, entry, config, err)) {
      errMsg(err) << "In CSV file line " << line << ": Cannot read line.";
      return false;
    }
    line++;
  }

  return true;
}


bool
ChirpReader::readLine(QTextStream &stream, QStringList &list, const ErrorStack &err) {
  Q_UNUSED(err)

  list.clear();
  if (stream.atEnd())
    return true;

  QString token;
  QChar ch; stream >> ch;
  bool string = false;
  while ((! stream.atEnd()) && (QChar('\n') != ch)) {
    if ((!string) && (QChar(',') == ch)) {
      list.append(token); token.clear();
    } else if ((! string) && (QChar('"') == ch)) {
      string = true;
    } else if (string && (QChar('"') == ch)) {
      string = false;
    } else {
      token.append(ch);
    }
    stream >> ch;
  }

  list.append(token);

  return true;
}


bool
ChirpReader::processLine(const QStringList &header, const QStringList &line, Config *config, const ErrorStack &err) {
  if (header.size() != line.size()) {
    errMsg(err) << "Malformed line. Expected " << header.size() << " entries, got " << line.size() << ".";
    return false;
  }

  bool ok;
  QString name;
  Frequency rxFrequency, txFrequency;
  Duplex duplex = Duplex::None;
  Mode mode = Mode::FM;
  ToneMode toneMode = ToneMode::None;
  CrossMode crossMode;
  double txTone = 67.0, rxTone = 67.0;
  int txDTCSCode = 000, rxDTCSCode = 000;
  Polarity txPol = Polarity::Normal, rxPol = Polarity::Normal;

  for (int i=1; i<header.size(); i++) {
    if ("Name" == header.at(i)) {
      name = line.at(i).simplified();
    } else if ("Frequency" == header.at(i)) {
      rxFrequency = Frequency::fromMHz(line.at(i).toDouble(&ok));
      if (! ok) {
        errMsg(err) << "Cannot parse frequency '" << line.at(i) << "': Malformed frequency.";
        return false;
      }
    } else if (("Offset" == header.at(i)) && (! line.at(i).isEmpty())) {
      txFrequency = Frequency::fromMHz(line.at(i).toDouble(&ok));
      if (! ok) {
        errMsg(err) << "Cannot parse offset frequency '" << line.at(i) << "': Malformed frequency.";
        return false;
      }
    } else if ("Duplex" == header.at(i)) {
      if (! processDuplex(line.at(i), duplex, err))
        return false;
    } else if ("Mode" == header.at(i)) {
      if (! processMode(line.at(i), mode, err))
        return false;
    } else if ("Tone" == header.at(i)) {
      if (! processToneMode(line.at(i), toneMode, err))
        return false;
    } else if (("rToneFreq" == header.at(i)) && (! line.at(i).isEmpty())) {
      txTone = line.at(i).toDouble(&ok);
      if (! ok) {
        errMsg(err) << "Cannot parse TX CTCSS tone frequency '" << line.at(i) << "'.";
        return false;
      }
    } else if (("cToneFreq" == header.at(i)) && (! line.at(i).isEmpty())) {
      rxTone = line.at(i).toDouble(&ok);
      if (! ok) {
        errMsg(err) << "Cannot parse RX CTCSS tone frequency '" << line.at(i) << "'.";
        return false;
      }
    } else if (("DtcsCode" == header.at(i)) && (! line.at(i).isEmpty())) {
      txDTCSCode = line.at(i).toUInt(&ok);
      if (! ok) {
        errMsg(err) << "Cannot decode TX DCS code '" << line.at(i) <<"': invalid format.";
        return false;
      }
    } else if (("RxDtcsCode" == header.at(i)) && (! line.at(i).isEmpty())) {
      rxDTCSCode = line.at(i).toUInt(&ok);
      if (! ok) {
        errMsg(err) << "Cannot decode RX DCS code '" << line.at(i) <<"': invalid format.";
        return false;
      }
    } else if ("DtcsPolarity" == header.at(i)) {
      if (! processPolarity(line.at(i), txPol, rxPol, err))
        return false;
    } else if ("CrossMode" == header.at(i)) {
      if (! processCrossMode(line.at(i), crossMode, err))
        return false;
    }
  }

  // Some more sanity checks:
  if (name.isEmpty()) {
    errMsg(err) << "Invalid empty name.";
    return false;
  }

  if ((Mode::FM == mode) || (Mode::NFM == mode)) {
    FMChannel *fm = new FMChannel();

    fm->setName(name);
    fm->setRXFrequency(rxFrequency.inMHz());

    switch (duplex) {
    case Duplex::None:
      fm->setTXFrequency(fm->rxFrequency());
      break;
    case Duplex::Off:
      fm->setTXFrequency(fm->rxFrequency());
      fm->setRXOnly(true);
      break;
    case Duplex::Split:
      fm->setTXFrequency(txFrequency.inMHz());
      break;
    case Duplex::Negative:
      fm->setTXFrequency(Frequency::fromHz(rxFrequency.inHz()-txFrequency.inHz()).inMHz());
      break;
    case Duplex::Positive:
      fm->setTXFrequency(Frequency::fromHz(rxFrequency.inHz()+txFrequency.inHz()).inMHz());
      break;
    }

    switch (toneMode) {
    case ToneMode::None: break;
    case ToneMode::Tone:
      fm->setTXTone(Signaling::fromCTCSSFrequency(txTone));
      fm->setRXTone(Signaling::SIGNALING_NONE);
      break;
    case ToneMode::TSQL:
      fm->setTXTone(Signaling::fromCTCSSFrequency(rxTone));
      fm->setRXTone(Signaling::fromCTCSSFrequency(rxTone));
      break;
    case ToneMode::TSQL_R:
      errMsg(err) << "Reversed CTCSS not supported.";
      return false;
    case ToneMode::DTCS:
      fm->setTXTone(Signaling::fromDCSNumber(txDTCSCode, Polarity::Reversed == txPol));
      fm->setRXTone(Signaling::fromDCSNumber(txDTCSCode, Polarity::Reversed == rxPol));
      break;
    case ToneMode::DTCS_R:
      errMsg(err) << "Reversed DCS not supported.";
      return false;
    case ToneMode::Cross:
      switch (crossMode) {
      case CrossMode::NoneTone:
        fm->setTXTone(Signaling::SIGNALING_NONE);
        fm->setRXTone(Signaling::fromCTCSSFrequency(rxTone));
        break;
      case CrossMode::NoneDTCS:
        fm->setTXTone(Signaling::SIGNALING_NONE);
        fm->setRXTone(Signaling::fromDCSNumber(rxDTCSCode, Polarity::Reversed == rxPol));
        break;
      case CrossMode::ToneNone:
        fm->setTXTone(Signaling::fromCTCSSFrequency(txTone));
        fm->setRXTone(Signaling::SIGNALING_NONE);
        break;
      case CrossMode::ToneTone:
        fm->setTXTone(Signaling::fromCTCSSFrequency(txTone));
        fm->setRXTone(Signaling::fromCTCSSFrequency(rxTone));
        break;
      case CrossMode::ToneDTCS:
        fm->setTXTone(Signaling::fromCTCSSFrequency(txTone));
        fm->setRXTone(Signaling::fromDCSNumber(rxDTCSCode, Polarity::Reversed == rxPol));
        break;
      case CrossMode::DTCSNone:
        fm->setTXTone(Signaling::fromDCSNumber(txDTCSCode, Polarity::Reversed == txPol));
        fm->setRXTone(Signaling::SIGNALING_NONE);
        break;
      case CrossMode::DTCSTone:
        fm->setTXTone(Signaling::fromDCSNumber(txDTCSCode, Polarity::Reversed == txPol));
        fm->setRXTone(Signaling::fromCTCSSFrequency(rxTone));
        break;
      case CrossMode::DTCSDTCS:
        fm->setTXTone(Signaling::fromDCSNumber(txDTCSCode, Polarity::Reversed == txPol));
        fm->setRXTone(Signaling::fromDCSNumber(rxDTCSCode, Polarity::Reversed == rxPol));
        break;
      }
    }

    config->channelList()->add(fm);
    return true;
  }

  errMsg(err) << "Unhandled channel format.";

  return false;
}


bool
ChirpReader::processDuplex(const QString &code, Duplex &duplex, const ErrorStack &err) {
  if (! _duplexCodes.contains(code.simplified())) {
    errMsg(err) << "Cannot decode duplex '" << code << "': Unknown setting.";
    return false;
  }
  duplex = _duplexCodes[code.simplified()];
  return true;
}

bool
ChirpReader::processMode(const QString &code, Mode &mode, const ErrorStack &err) {
  if (! _modeCodes.contains(code.simplified())) {
    errMsg(err) << "Cannot decode mode '" << code << "': Unknown setting.";
    return false;
  }
  mode = _modeCodes[code.simplified()];
  return true;
}

bool
ChirpReader::processToneMode(const QString &code, ToneMode &mode, const ErrorStack &err) {
  if (! _toneModeCodes.contains(code.simplified())) {
    errMsg(err) << "Cannot decode tone mode '" << code << "': Unknown setting.";
    return false;
  }
  mode = _toneModeCodes[code.simplified()];
  return true;
}

bool
ChirpReader::processPolarity(const QString &code, Polarity &txPol, Polarity &rxPol, const ErrorStack &err) {
  if (2 != code.simplified().size()) {
    errMsg(err) << "Cannot parse polarity code '" << code << "': invalid format.";
    return false;
  }

  QChar tx = code.simplified().at(0), rx = code.simplified().at(1);
  if ('N' == tx) {
    txPol = Polarity::Normal;
  } else if ('R' == tx) {
    txPol = Polarity::Reversed;
  } else {
    errMsg(err) << "Invalid polarity code: '" << tx << "': expected 'N' or 'R'.";
    return false;
  }

  if ('N' == rx) {
    rxPol = Polarity::Normal;
  } else if ('R' == rx) {
    rxPol = Polarity::Reversed;
  } else {
    errMsg(err) << "Invalid polarity code: '" << rx << "': expected 'N' or 'R'.";
    return false;
  }

  return true;
}

bool
ChirpReader::processCrossMode(const QString &code, CrossMode &crossMode, const ErrorStack &err) {
  if (! _crossModes.contains(code.simplified())) {
    errMsg(err) << "Cannot decode cross-mode '" << code << "': unknown mode.";
    return false;
  }
  crossMode = _crossModes[code.simplified()];
  return true;
}



/* ********************************************************************************************* *
 * Implementation of ChirpWriter
 * ********************************************************************************************* */
bool
ChirpWriter::write(QTextStream &stream, Config *config, const ErrorStack &err) {
  if (! writeHeader(stream, err)) {
    errMsg(err) << "Cannot write CHIRP CSV file.";
    return false;
  }

  for (int i=0; i<config->channelList()->count(); i++) {
    if (! config->channelList()->channel(i)->is<FMChannel>())
      continue;
    if (! writeChannel(stream, i, config->channelList()->channel(i)->as<FMChannel>(), err)) {
      errMsg(err) << "Cannot encode FM channel '" << config->channelList()->channel(i)->name()
                  << "'.";
      return false;
    }
  }

  return true;
}

bool
ChirpWriter::writeHeader(QTextStream &stream, const ErrorStack &err) {
  Q_UNUSED(err)

  stream << "Location" << "," << "Name" << "," << "Frequency" << "," << "Duplex" << ","
         << "Offset" << "," << "Tone" << "," << "rToneFreq" << "," << "cToneFreq" << ","
         << "DtcsCode" << "," << "DtcsPolarity" << "," << "Mode" << "\n";

  return true;
}

bool
ChirpWriter::writeChannel(QTextStream &stream, int i, FMChannel *channel, const ErrorStack &err) {
  stream << i << "," << '"' << channel->name() << '"';

  if (! encodeFrequency(stream, channel, err)) {
    errMsg(err) << "Cannot encode frequencies of channel '" << channel->name() << "'.";
    return false;
  }

  if (! encodeSubTone(stream, channel, err)) {
    errMsg(err) << "Cannot encode sub-tone setting for channel '" << channel->name() << "'.";
    return false;
  }

  if (! encodeBandwidth(stream, channel, err)) {
    errMsg(err) << "Cannot encode sub-tone setting for channel '" << channel->name() << "'.";
    return false;
  }
  stream << "\n";

  return true;
}

bool
ChirpWriter::encodeFrequency(QTextStream &stream, FMChannel *channel, const ErrorStack &err) {
  Q_UNUSED(err)

  stream << "," << channel->rxFrequency();

  if (channel->rxOnly())
    stream << "," << "Off" << "," << 0.0;
  else if (channel->rxFrequency() == channel->txFrequency())
    stream << "," << "" << "," << 0.0;
  else if (channel->rxFrequency() > channel->txFrequency())
    stream << "," << "-" << "," << channel->rxFrequency()-channel->txFrequency();
  else
    stream << "," << "+" << "," << channel->txFrequency()-channel->rxFrequency();

  return true;
}

bool
ChirpWriter::encodeSubTone(QTextStream &stream, FMChannel *channel, const ErrorStack &err) {
  Q_UNUSED(err)

  if (Signaling::SIGNALING_NONE == channel->txTone())
    stream << "," << "" << "," << 65.0 << "," << 65.0 << "," << "023" << "," << "NN";
  else if (Signaling::isCTCSS(channel->txTone()) && (Signaling::SIGNALING_NONE == channel->rxTone()))
    stream << "," << "Tone" << "," << 65.0 << "," << Signaling::toCTCSSFrequency(channel->txTone())
           << "," << "023" << "," << "NN";
  else if (Signaling::isCTCSS(channel->txTone()) && (channel->txTone() == channel->rxTone()))
    stream << "," << "TSQL" << "," << 65.0 << "," << Signaling::toCTCSSFrequency(channel->txTone())
           << "," << "023" << "," << "NN";
  else if (Signaling::isDCSNumber(channel->txTone()))
    stream << "," << "DTCS" << "," << 65.0 << "," << 65.0 << "," <<
           Signaling::toDCSNumber(channel->txTone()) << ","
           << (Signaling::isDCSNumber(channel->txTone()) && Signaling::isDCSInverted(channel->txTone()) ? 'I' : 'N')
           << (Signaling::isDCSNumber(channel->rxTone()) && Signaling::isDCSInverted(channel->rxTone()) ? 'I' : 'N');
  else
    stream << "," << "" << "," << 65.0 << "," << 65.0 << "," << "023" << "," << "NN";

  return true;
}

bool
ChirpWriter::encodeBandwidth(QTextStream &stream, FMChannel *channel, const ErrorStack &err) {
  Q_UNUSED(err)

  if (FMChannel::Bandwidth::Narrow == channel->bandwidth())
    stream << "," << "NFM";
  else
    stream << "," << "FM";

  return true;
}
