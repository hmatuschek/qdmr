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
  "Tone", "rToneFreq", "cToneFreq", "DtcsCode", "DtcsPolarity", "TStep", "Skip",
  "Comment", "URCALL", "RPT1CALL", "RPT2CALL"
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
  {"DTCS", ChirpFormat::ToneMode::DTCS},
  {"Cross", ChirpFormat::ToneMode::Cross},
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
  double rxTone = 67.0, txTone = 67.0;
  int dctsCode = 000;
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
    } else if ("Offset" == header.at(i)) {
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
    } else if ("rToneFreq" == header.at(i)) {
      rxTone = line.at(i).toDouble(&ok);
      if (! ok) {
        errMsg(err) << "Cannot parse RX CTCSS tone frequency '" << line.at(i) << "'.";
        return false;
      }
    } else if ("cToneFreq" == header.at(i)) {
      txTone = line.at(i).toDouble(&ok);
      if (! ok) {
        errMsg(err) << "Cannot parse TX CTCSS tone frequency '" << line.at(i) << "'.";
        return false;
      }
    } else if ("DtcsCode" == line.at(i)) {
      dctsCode = line.at(i).toUInt(&ok);
      if (! ok) {
        errMsg(err) << "Cannot decode DCS code '" << line.at(i) <<"': invalid format.";
        return false;
      }
    } else if ("DtcsPolarity" == line.at(i)) {
      if (! processPolarity(line.at(i), txPol, rxPol, err))
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
      break;
    case ToneMode::TSQL:
      fm->setTXTone(Signaling::fromCTCSSFrequency(txTone));
      fm->setRXTone(Signaling::fromCTCSSFrequency(txTone));
      break;
    case ToneMode::DTCS:
      fm->setTXTone(Signaling::fromDCSNumber(dctsCode, Polarity::Reversed == txPol));
      fm->setRXTone(Signaling::fromDCSNumber(dctsCode, Polarity::Reversed == rxPol));
      break;
    case ToneMode::Cross:
      errMsg(err) << "'CrossMode' not yet implemented.";
      return false;
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



/* ********************************************************************************************* *
 * Implementation of ChirpWriter
 * ********************************************************************************************* */
