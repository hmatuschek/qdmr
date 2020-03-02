#include "csvreader.hh"
#include "config.hh"
#include "utils.hh"
#include "logger.hh"

#include <QRegExp>
#include <QDebug>

QVector< QPair<QRegExp, CSVLexer::Token::TokenType> > CSVLexer::_pattern = {
  { QRegExp("^([a-zA-Z_][a-zA-Z0-9_]*)"),      CSVLexer::Token::T_KEYWORD },
  { QRegExp("^\"([^\"\r\n]*)\""),              CSVLexer::Token::T_STRING },
  { QRegExp("^([+-]?[0-9]+(\\.[0-9]*)?)"),     CSVLexer::Token::T_NUMBER },
  { QRegExp("^(:)"),                           CSVLexer::Token::T_COLON },
  { QRegExp("^(-)"),                           CSVLexer::Token::T_NOT_SET },
  { QRegExp("^(\\+)"),                         CSVLexer::Token::T_ENABLED },
  { QRegExp("^(,)"),                           CSVLexer::Token::T_COMMA },
  { QRegExp("^([ \t]+)"),                      CSVLexer::Token::T_WHITESPACE },
  { QRegExp("^(\r?\n)"),                       CSVLexer::Token::T_NEWLINE},
  { QRegExp("^(#[^\n\r]*)"),                   CSVLexer::Token::T_COMMENT},
};


/* ********************************************************************************************* *
 * Implementation of CSVLexer
 * ********************************************************************************************* */
CSVLexer::CSVLexer(QTextStream &stream, QObject *parent)
  : QObject(parent), _errorMessage(), _stream(stream), _stack(), _currentLine()
{
  _stream.seek(0);
  _stack.reserve(10);
  _stack.push_back({0, 1, 1});
  _currentLine = _stream.readLine();
}

const QString &
CSVLexer::errorMessage() const {
  return _errorMessage;
}

CSVLexer::Token
CSVLexer::next() {
  Token token = lex();
  while ((Token::T_WHITESPACE == token.type) || (Token::T_COMMENT == token.type))
    token = lex();
  return token;
}

CSVLexer::Token
CSVLexer::lex() {
  if (_currentLine.isEmpty() && _stream.atEnd()) {
    return {Token::T_END_OF_STREAM, "", _stack.back().line, _stack.back().column };
  } else if (_currentLine.isEmpty()) {
    Token token = {Token::T_NEWLINE, "", _stack.back().line, _stack.back().column };
    _stack.back().offset = _stream.pos();
    _currentLine = _stream.readLine();
    _stack.back().line++;
    _stack.back().column = 1;
    return token;
  }
  foreach (auto pattern, _pattern) {
    if (0 == pattern.first.indexIn(_currentLine)) {
      Token token = {pattern.second, pattern.first.cap(1), _stack.back().line, _stack.back().column};
      _stack.back().offset += pattern.first.matchedLength();
      _stack.back().column += token.value.size();
      _currentLine = _currentLine.mid(pattern.first.matchedLength());
      return token;
    }
  }
  _errorMessage = tr("Lexer error %1,%2: Unexpected char '%3'.").arg(_stack.back().line)
      .arg(_stack.back().column).arg(_currentLine.at(0));
  return {Token::T_ERROR, _errorMessage, _stack.back().line, _stack.back().column};
}

void
CSVLexer::push() {
  _stack.push_back(_stack.back());
}

void
CSVLexer::pop() {
  if (_stack.size() < 2)
    return;
  _stack.pop_back();
  _stream.seek(_stack.back().offset);
  _currentLine = QString();
}

/* ********************************************************************************************* *
 * Implementation of CSVHandler
 * ********************************************************************************************* */
CSVHandler::CSVHandler(QObject *parent)
  : QObject(parent)
{
  // pass...
}

CSVHandler::~CSVHandler() {
  // pass...
}

bool
CSVHandler::handleRadioId(qint64 id, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(id);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage)
  return true;
}

bool
CSVHandler::handleRadioName(const QString &name, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(name);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleIntroLine1(const QString &text, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(text);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleIntroLine2(const QString &text, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(text);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleMicLevel(uint level, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(level);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleSpeech(bool speech, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(speech);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleDTMFContact(qint64 idx, const QString &name, const QString &num, bool rxTone,
                              qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(num);
  Q_UNUSED(rxTone);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleDigitalContact(qint64 idx, const QString &name, DigitalContact::Type type, qint64 id,
                                 bool rxTone, qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(type);
  Q_UNUSED(id);
  Q_UNUSED(rxTone);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                            qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(contacts);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleDigitalChannel(qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
    qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
    qint64 gl, qint64 contact, qint64 gps, qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(rx);
  Q_UNUSED(tx);
  Q_UNUSED(power);
  Q_UNUSED(scan);
  Q_UNUSED(tot);
  Q_UNUSED(ro);
  Q_UNUSED(admit);
  Q_UNUSED(color);
  Q_UNUSED(slot);
  Q_UNUSED(gl);
  Q_UNUSED(contact);
  Q_UNUSED(gps);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleAnalogChannel(qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
    qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
    AnalogChannel::Bandwidth bw, qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(rx);
  Q_UNUSED(tx);
  Q_UNUSED(power);
  Q_UNUSED(scan);
  Q_UNUSED(tot);
  Q_UNUSED(ro);
  Q_UNUSED(admit);
  Q_UNUSED(squelch);
  Q_UNUSED(rxTone);
  Q_UNUSED(txTone);
  Q_UNUSED(bw);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleZone(qint64 idx, const QString &name, bool a, const QList<qint64> &channels,
                       qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(a);
  Q_UNUSED(channels);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleGPSSystem(
    qint64 idx, const QString &name, qint64 contactIdx, qint64 period, qint64 revertChannelIdx,
    qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(contactIdx);
  Q_UNUSED(period);
  Q_UNUSED(revertChannelIdx);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}

bool
CSVHandler::handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                           const QList<qint64> &channels, qint64 line, qint64 column, QString &errorMessage)
{
  Q_UNUSED(idx);
  Q_UNUSED(name);
  Q_UNUSED(pch1);
  Q_UNUSED(pch2);
  Q_UNUSED(txch);
  Q_UNUSED(channels);
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of CSVParser
 * ********************************************************************************************* */
CSVParser::CSVParser(CSVHandler *handler, QObject *parent)
  : QObject(parent), _errorMessage(), _handler(handler)
{
  // pass...
}

const QString &
CSVParser::errorMessage() const {
  return _errorMessage;
}

bool
CSVParser::parse(QTextStream &stream) {
  if (! stream.seek(0))
    return false;

  CSVLexer lexer(stream);

  for (CSVLexer::Token token=lexer.next(); CSVLexer::Token::T_END_OF_STREAM != token.type; token = lexer.next()) {
    if (CSVLexer::Token::T_NEWLINE == token.type)
      continue;
    if ((CSVLexer::Token::T_KEYWORD == token.type) && ("id" == token.value.toLower())) {
      if (! _parse_radio_id(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("name" == token.value.toLower())) {
      if (! _parse_radio_name(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("introline1" == token.value.toLower())) {
      if (! _parse_introline1(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("introline2" == token.value.toLower())) {
      if (! _parse_introline2(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("miclevel" == token.value.toLower())) {
      if (! _parse_mic_level(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("speech" == token.value.toLower())) {
      if (! _parse_speech(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("contact" == token.value.toLower())) {
      if (! _parse_contacts(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("grouplist" == token.value.toLower())) {
      if (! _parse_rx_groups(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("digital" == token.value.toLower())) {
      if (! _parse_digital_channels(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("analog" == token.value.toLower())) {
      if (! _parse_analog_channels(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("zone" == token.value.toLower())) {
      if (! _parse_zones(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("gps" == token.value.toLower())) {
      if (! _parse_gps_systems(lexer))
        return false;
    } else if ((CSVLexer::Token::T_KEYWORD == token.type) && ("scanlist" == token.value.toLower())) {
      if (! _parse_scanlists(lexer))
        return false;
    } else if (CSVLexer::Token::T_ERROR == token.type) {
      _errorMessage = QString("Lexer error @ %1,%2 '%3': %4").arg(token.line).arg(token.column)
          .arg(token.value).arg(lexer.errorMessage());
      return false;
    } else {
      _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4'.").arg(token.line)
          .arg(token.column).arg(token.type).arg(token.value);
      return false;
    }
  }

  return true;
}

bool
CSVParser::_parse_radio_id(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 id = token.value.toInt();
  qint64 line=token.line, column=token.column;

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleRadioId(id, line, column, _errorMessage);
}

bool
CSVParser::_parse_radio_name(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  token = lexer.next();
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;
  qint64 line=token.line, column=token.column;

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleRadioName(name, line, column, _errorMessage);
}

bool
CSVParser::_parse_introline1(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  token = lexer.next();
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString text = token.value;
  qint64 line=token.line, column=token.column;

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleIntroLine1(text, line, column, _errorMessage);
}

bool
CSVParser::_parse_introline2(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  token = lexer.next();
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString text = token.value;
  qint64 line=token.line, column=token.column;

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleIntroLine2(text, line, column, _errorMessage);
}

bool
CSVParser::_parse_mic_level(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 level = token.value.toInt();
  qint64 line=token.line, column=token.column;

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleMicLevel(level, line, column, _errorMessage);
}

bool
CSVParser::_parse_speech(CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  if (CSVLexer::Token::T_COLON != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected ':'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_KEYWORD != token.type) ||
      (("on" != token.value.toLower()) && ("off" != token.value.toLower())))
  {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'On' or 'Off'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 line=token.line, column=token.column;
  bool speech = ("on" == token.value.toLower());

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleSpeech(speech, line, column, _errorMessage);
}

bool
CSVParser::_parse_contacts(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_contact(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_contact(qint64 idx, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;

  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  token = lexer.next();
  if (CSVLexer::Token::T_KEYWORD != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected keyword.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  bool dtmf = false;
  DigitalContact::Type type;
  if ("group" == token.value.toLower()) {
    type = DigitalContact::GroupCall;
  } else if ("private" == token.value.toLower()) {
    type = DigitalContact::PrivateCall;
  } else if ("all" == token.value.toLower()) {
    type = DigitalContact::AllCall;
  } else if ("dtmf" == token.value.toLower()) {
    dtmf = true;
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'Group', 'Private', 'All' or 'DTMF'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ( (!dtmf) && (CSVLexer::Token::T_NUMBER != token.type) ) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  } else if (dtmf && (CSVLexer::Token::T_NUMBER != token.type) && (CSVLexer::Token::T_STRING != token.type)) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString dtmf_num = token.value;
  if (dtmf && (! validDTMFNumber(dtmf_num))) {
    _errorMessage = QString("Parse error @ %1,%2: Invalid DTMF number '%3'.")
        .arg(token.line).arg(token.column).arg(token.value);
  }
  qint64 id = token.value.toInt();

  bool rxToneEnabled;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type)
    rxToneEnabled = false;
  else if (CSVLexer::Token::T_ENABLED == token.type)
    rxToneEnabled = true;
  else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected '+' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  if (dtmf)
    return _handler->handleDTMFContact(idx, name, dtmf_num, rxToneEnabled, line, column, _errorMessage);
  return _handler->handleDigitalContact(idx, name, type, id, rxToneEnabled, line, column, _errorMessage);
}


bool
CSVParser::_parse_rx_groups(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_rx_group(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);

  return false;
}

bool
CSVParser::_parse_rx_group(qint64 idx, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  QList<qint64> lst;
  token = lexer.next();
  while (CSVLexer::Token::T_NUMBER == token.type) {
    lst.append(token.value.toInt());
    token = lexer.next();
    if (CSVLexer::Token::T_COMMA == token.type)
      token = lexer.next();
  }

  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleGroupList(idx, name, lst, line, column, _errorMessage);
}

bool
CSVParser::_parse_digital_channels(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_digital_channel(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_digital_channel(qint64 idx, CSVLexer &lexer) {
  bool ok=false;
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  double rx = token.value.toDouble(&ok);
  if (! ok) {
    _errorMessage = QString("Parse error @ %1,%2: Cannot convert '%3' to double.")
        .arg(token.line).arg(token.column).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  double tx = token.value.toDouble(&ok);
  if (! ok) {
    _errorMessage = QString("Parse error @ %1,%2: Cannot convert '%3' to double.")
        .arg(token.line).arg(token.column).arg(token.value);
    return false;
  }
  if (token.value.startsWith('+') || token.value.startsWith('-'))
    tx = rx + tx;

  token = lexer.next();
  if (CSVLexer::Token::T_KEYWORD != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected keyword.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  Channel::Power pwr;
  if ("high" == token.value.toLower())
    pwr = Channel::HighPower;
  else if ("low" == token.value.toLower())
    pwr = Channel::LowPower;
  else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'High' or 'Low'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  qint64 scanlist;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    scanlist = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    scanlist = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_NUMBER != token.type) && (CSVLexer::Token::T_NOT_SET != token.type)) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 tot =  (CSVLexer::Token::T_NOT_SET == token.type) ? 0 : token.value.toInt();

  bool rxOnly;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type)
    rxOnly = false;
  else if (CSVLexer::Token::T_ENABLED == token.type)
    rxOnly = true;
  else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected '+' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  DigitalChannel::Admit admit;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    admit = DigitalChannel::AdmitNone;
  } else if (CSVLexer::Token::T_KEYWORD == token.type) {
    if ("free" == token.value.toLower())
      admit = DigitalChannel::AdmitFree;
    else if ("color" == token.value.toLower())
      admit = DigitalChannel::AdmitColorCode;
    else {
      _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'Free' or 'Color'.")
          .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
      return false;
    }
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'Free', 'Color' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 color = token.value.toInt();

  token = lexer.next();
  DigitalChannel::TimeSlot slot;
  if (CSVLexer::Token::T_NUMBER == token.type) {
    if (1 == token.value.toInt()) {
      slot = DigitalChannel::TimeSlot1;
    } else if (2 == token.value.toInt()) {
      slot = DigitalChannel::TimeSlot2;
    } else {
      _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected '1' or '2'.")
          .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
      return false;
    }
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  qint64 rxGroupList;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    rxGroupList = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    rxGroupList = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  qint64 txContact;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    txContact = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    txContact = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  qint64 gps;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    gps = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    gps = token.value.toInt(&ok);
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleDigitalChannel(idx, name, rx, tx, pwr, scanlist, tot, rxOnly, admit, color,
                                        slot, rxGroupList, txContact, gps, line, column, _errorMessage);
}

bool
CSVParser::_parse_analog_channels(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_analog_channel(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_analog_channel(qint64 idx, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  double rx = token.value.toDouble();

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  double tx = token.value.toDouble();
  if (token.value.startsWith('+') || token.value.startsWith('-'))
    tx = rx + tx;

  token = lexer.next();
  if (CSVLexer::Token::T_KEYWORD != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected keyword.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  Channel::Power pwr;
  if ("high" == token.value.toLower())
    pwr = Channel::HighPower;
  else if ("low" == token.value.toLower())
    pwr = Channel::LowPower;
  else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'High' or 'Low'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  qint64 scanlist;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    scanlist = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    scanlist = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_NUMBER != token.type) && (CSVLexer::Token::T_NOT_SET != token.type)) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 tot =  (CSVLexer::Token::T_NOT_SET == token.type) ? 0 : token.value.toInt();

  bool rxOnly;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type)
    rxOnly = false;
  else if (CSVLexer::Token::T_ENABLED == token.type)
    rxOnly = true;
  else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected '+' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  AnalogChannel::Admit admit;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    admit = AnalogChannel::AdmitNone;
  } else if (CSVLexer::Token::T_KEYWORD == token.type) {
    if ("free" == token.value.toLower())
      admit = AnalogChannel::AdmitFree;
    else if ("color" == token.value.toLower())
      admit = AnalogChannel::AdmitTone;
    else {
      _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'Free', 'Tone'.")
          .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
      return false;
    }
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'Free', 'Tone' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  qint64 squelch = token.value.toInt();

  token = lexer.next();
  float rxTone;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    rxTone = 0.0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    rxTone = token.value.toFloat();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  float txTone;
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    txTone = 0.0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    txTone = token.value.toFloat();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  AnalogChannel::Bandwidth bw;
  if (25 == token.value.toFloat()) {
    bw = AnalogChannel::BWWide;
  } else if (12.5 == token.value.toFloat()) {
    bw = AnalogChannel::BWNarrow;
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected '12.5' or '25'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleAnalogChannel(idx, name, rx, tx, pwr, scanlist, tot, rxOnly, admit, squelch,
                                       rxTone, txTone, bw, line, column, _errorMessage);
}

bool
CSVParser::_parse_zones(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_zone(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_zone(qint64 idx, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  token = lexer.next();
  if ((CSVLexer::Token::T_KEYWORD != token.type) || (("a" != token.value.toLower()) && ("b" != token.value.toLower())) ) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected 'A or 'B'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  bool a = ("a" == token.value.toLower());

  QList<qint64> lst;
  token = lexer.next();
  while (CSVLexer::Token::T_NUMBER == token.type) {
    lst.append(token.value.toInt());
    token = lexer.next();
    if (CSVLexer::Token::T_COMMA == token.type)
      token = lexer.next();
  }

  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleZone(idx, name, a, lst, line, column, _errorMessage);
}


bool
CSVParser::_parse_gps_systems(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_gps_system(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_gps_system(qint64 id, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  qint64 contact;
  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER == token.type) {
    contact = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  qint64 period;
  token = lexer.next();
  if (CSVLexer::Token::T_NUMBER == token.type) {
    period = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  qint64 chan;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    chan = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    chan = token.value.toInt();
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleGPSSystem(id, name, contact, period, chan, line, column, _errorMessage);
}


bool
CSVParser::_parse_scanlists(CSVLexer &lexer) {
  // skip rest of header
  CSVLexer::Token token = lexer.next();
  for (; CSVLexer::Token::T_KEYWORD==token.type; token=lexer.next()) {
    // skip
  }
  if (CSVLexer::Token::T_NEWLINE != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  token = lexer.next();
  for (; CSVLexer::Token::T_NUMBER == token.type; token=lexer.next()) {
    qint64 idx = token.value.toInt();
    if (! _parse_scanlist(idx, lexer))
      return false;
  }

  if ((CSVLexer::Token::T_NEWLINE == token.type) || (CSVLexer::Token::T_END_OF_STREAM == token.type))
    return true;

  _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
      .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
  return false;
}

bool
CSVParser::_parse_scanlist(qint64 idx, CSVLexer &lexer) {
  CSVLexer::Token token = lexer.next();
  qint64 line=token.line, column=token.column;
  if (CSVLexer::Token::T_STRING != token.type) {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected string.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }
  QString name = token.value;

  qint64 pch1;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    pch1 = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    pch1 = token.value.toInt() + 1;
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  qint64 pch2;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    pch2 = 0;
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    pch2 = token.value.toInt() + 1;
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  qint64 txch;
  token = lexer.next();
  if (CSVLexer::Token::T_NOT_SET == token.type) {
    txch = 0;
  } else if (CSVLexer::Token::T_KEYWORD == token.type) {
    if ("sel" == token.value.toLower()) {
      txch = 1;
    } else {
      _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number, 'Sel' or '-'.")
          .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
      return false;
    }
  } else if (CSVLexer::Token::T_NUMBER == token.type) {
    txch = token.value.toInt() + 1;
  } else {
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected number, 'Sel' or '-'.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  QList<qint64> lst;
  token = lexer.next();
  while (CSVLexer::Token::T_NUMBER == token.type) {
    lst.append(token.value.toInt());
    token = lexer.next();
    if (CSVLexer::Token::T_COMMA == token.type)
      token = lexer.next();
  }

  if ((CSVLexer::Token::T_NEWLINE != token.type) && (CSVLexer::Token::T_END_OF_STREAM != token.type)){
    _errorMessage = QString("Parse error @ %1,%2: Unexpected token %3 '%4' expected newline/EOS.")
        .arg(token.line).arg(token.column).arg(token.type).arg(token.value);
    return false;
  }

  return _handler->handleScanList(idx, name, pch1, pch2, txch, lst, line, column, _errorMessage);
}


/* ********************************************************************************************* *
 * Implementation of CSVReader
 * ********************************************************************************************* */
CSVReader::CSVReader(Config *config, QObject *parent)
  : CSVHandler(parent), _link(false), _config(config)
{
  // pass...
}


bool
CSVReader::read(Config *config, QTextStream &stream, QString &errorMessage) {
  // Seek to start-of-stream
  if (! stream.seek(0)) {
    errorMessage = "Cannot read CSV codeplug: Cannot seek within text stream. Abort import." ;
    return false;
  }

  config->reset();

  CSVReader reader(config);
  CSVParser parser(&reader);

  if (! parser.parse(stream)) {
    errorMessage = parser.errorMessage();
    return false;
  }
  reader._link = true;

  if (! parser.parse(stream)) {
    errorMessage = parser.errorMessage();
    return false;
  }

  return true;
}


bool
CSVReader::handleRadioId(qint64 id, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setId(id);
  }
  return true;
}

bool
CSVReader::handleRadioName(const QString &name, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setName(name);
  }
  return true;
}

bool
CSVReader::handleIntroLine1(const QString &text, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setIntroLine1(text);
  }
  return true;
}

bool
CSVReader::handleIntroLine2(const QString &text, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setIntroLine2(text);
  }
  return true;
}

bool
CSVReader::handleMicLevel(uint level, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setMicLevel(level);
  }
  return true;
}


bool
CSVReader::handleSpeech(bool speech, qint64 line, qint64 column, QString &errorMessage) {
  Q_UNUSED(line);
  Q_UNUSED(column);
  Q_UNUSED(errorMessage);

  if (_link) {
    _config->setSpeech(speech);
  }
  return true;
}


bool
CSVReader::handleDTMFContact(qint64 idx, const QString &name, const QString &num, bool rxTone,
                             qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // pass...
    return true;
  }

  if (_digital_contacts.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create contact '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }
  DTMFContact *contact = new DTMFContact(name, num, rxTone);
  //_contacts[idx] = contact;
  _config->contacts()->addContact(contact);

  return true;
}


bool
CSVReader::handleDigitalContact(qint64 idx, const QString &name, DigitalContact::Type type,
                                qint64 id, bool rxTone, qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // pass...
    return true;
  }

  if (_digital_contacts.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create contact '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }
  DigitalContact *contact = new DigitalContact(type, name, id, rxTone);
  _digital_contacts[idx] = contact;
  _config->contacts()->addContact(contact);

  return true;
}

bool
CSVReader::handleGroupList(qint64 idx, const QString &name, const QList<qint64> &contacts,
                           qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    foreach(qint64 i, contacts) {
      // Check contacts
      if (! _digital_contacts.contains(i)) {
        errorMessage = QString("Parse error @ %1,%2: Cannot create contact list '%3' with index %4, unknown index.")
            .arg(line).arg(column).arg(name).arg(i);
        return false;
      }
      // link contacts
      _rxgroups[idx]->addContact(_digital_contacts[i]);
    }
    // done
    return true;
  }

  // check index
  if (_rxgroups.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create RX-group list '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }
  // Create group list
  RXGroupList *lst = new RXGroupList(name);
  _config->rxGroupLists()->addList(lst);
  _rxgroups[idx] = lst;

  return true;
}

bool
CSVReader::handleDigitalChannel(qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
    qint64 tot, bool ro, DigitalChannel::Admit admit, qint64 color, DigitalChannel::TimeSlot slot,
    qint64 gl, qint64 contact, qint64 gps, qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // Check RX Grouplist
    if ((0 < gl) && (! _rxgroups.contains(gl))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot link digital channel '%3', unknown RX-group list %4.")
          .arg(line).arg(column).arg(name).arg(gl);
      return false;
    }
    _channels[idx]->as<DigitalChannel>()->setRXGroupList(_rxgroups[gl]);
    // Check TX Contact
    if ((0 < contact) && (! _digital_contacts.contains(contact))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot link digital channel '%3', unknown contact index %4.")
          .arg(line).arg(column).arg(name).arg(contact);
      return false;
    }
    _channels[idx]->as<DigitalChannel>()->setTXContact(_digital_contacts[contact]);
    // Check scanlist
    if ((0 < scan) && (! _scanlists.contains(scan))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot link digital channel '%3', unknown scan-list index %4.")
          .arg(line).arg(column).arg(name).arg(scan);
      return false;
    }
    _channels[idx]->as<DigitalChannel>()->setScanList(_scanlists[scan]);
    // Check GPS System
    if ((0<gps) && (! _gpsSystems.contains(gps))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot link digital channel '%3', unknown system index %4.")
          .arg(line).arg(column).arg(name).arg(gps);
      return false;
    }
    _channels[idx]->as<DigitalChannel>()->setGPSSystem(_gpsSystems[gps]);
    return true;
  }

  // check index
  if (_channels.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create digital channel '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }

  DigitalChannel *chan = new DigitalChannel(name, rx, tx, power, tot, ro, admit, color, slot,
                                            nullptr, nullptr, nullptr, nullptr);
  _config->channelList()->addChannel(chan);
  _channels[idx] = chan;

  return true;
}


bool
CSVReader::handleAnalogChannel(qint64 idx, const QString &name, double rx, double tx, Channel::Power power, qint64 scan,
    qint64 tot, bool ro, AnalogChannel::Admit admit, qint64 squelch, float rxTone, float txTone,
    AnalogChannel::Bandwidth bw, qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // Check scanlist
    if ((0 < scan) && (! _scanlists.contains(scan))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot link analog channel '%3', unknown scan-list index %4.")
          .arg(line).arg(column).arg(name).arg(scan);
      return false;
    }
    _channels[idx]->as<AnalogChannel>()->setScanList(_scanlists[scan]);
    return true;
  }

  // check index
  if (_channels.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create analog channel '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }

  AnalogChannel *chan = new AnalogChannel(name, rx, tx, power, tot, ro, admit, squelch, rxTone,
                                          txTone, bw, nullptr);
  _config->channelList()->addChannel(chan);
  _channels[idx] = chan;

  return true;
}

bool
CSVReader::handleZone(qint64 idx, const QString &name, bool a, const QList<qint64> &channels, qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    foreach(qint64 i, channels) {
      // Check channels
      if (! _channels.contains(i)) {
        errorMessage = QString("Parse error @ %1,%2: Cannot create zone '%3', unknown channel index %4.")
            .arg(line).arg(column).arg(name).arg(i);
        return false;
      }
      // link channels
      if (a)
        _zones[idx]->A()->addChannel(_channels[i]);
      else
        _zones[idx]->B()->addChannel(_channels[i]);
    }
    // done
    return true;
  }

  // check index
  if (! _zones.contains(idx)) {
    Zone *zone = new Zone(name);
    _zones[idx] = zone;
    _config->zones()->addZone(zone);
  }

  return true;
}

bool
CSVReader::handleGPSSystem(
    qint64 idx, const QString &name, qint64 contactIdx, qint64 period, qint64 revertChannelIdx,
    qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // Check contact ID
    if (! _digital_contacts.contains(contactIdx)) {
      errorMessage = QString("Parse error @ %1,%2: Cannot create GPS system '%3', unknown destination contact ID %4.")
          .arg(line).arg(column).arg(name).arg(contactIdx);
      logError() << errorMessage;
      return false;
    }
    _gpsSystems[idx]->setContact(_digital_contacts[contactIdx]);

    if (revertChannelIdx) {
      if (! _channels.contains(revertChannelIdx))  {
        errorMessage = QString("Parse error @ %1,%2: Cannot create GPS system '%3', unknown revert channel ID %4.")
            .arg(line).arg(column).arg(name).arg(revertChannelIdx);
        return false;
      }
      if (! _channels[revertChannelIdx]->is<DigitalChannel>()) {
        errorMessage = QString("Parse error @ %1,%2: Cannot create GPS system '%3', revert channel %4 is not a digital channel.")
            .arg(line).arg(column).arg(name).arg(revertChannelIdx);
        return false;
      }
      _gpsSystems[idx]->setRevertChannel(_channels[revertChannelIdx]->as<DigitalChannel>());
    }

    return true;
  }

  // check index
  if (_gpsSystems.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create GPS system '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }

  GPSSystem *gps = new GPSSystem(name, nullptr, nullptr, period);
  _gpsSystems[idx] = gps;
  _config->gpsSystems()->addGPSSystem(gps);

  return true;
}

bool
CSVReader::handleScanList(qint64 idx, const QString &name, qint64 pch1, qint64 pch2, qint64 txch,
                          const QList<qint64> &channels, qint64 line, qint64 column, QString &errorMessage)
{
  if (_link) {
    // Check PriChan 1
    if (pch1 && (! _channels.contains(pch1))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot create scanlist '%3', unknown priority channel 1 index %4.")
          .arg(line).arg(column).arg(name).arg(pch1);
      return false;
    }
    _scanlists[idx]->setPriorityChannel(_channels[pch1]);

    // Check PriChan 2
    if (pch2 && (! _channels.contains(pch2))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot create scanlist '%3', unknown priority channel 2 index %4.")
          .arg(line).arg(column).arg(name).arg(pch2);
      return false;
    }
    _scanlists[idx]->setSecPriorityChannel(_channels[pch2]);

    // Check Tx channel
    if ((txch>1) && (! _channels.contains(txch-1))) {
      errorMessage = QString("Parse error @ %1,%2: Cannot create scanlist '%3', unknown TX channel index %4.")
          .arg(line).arg(column).arg(name).arg(txch-1);
      return false;
    }
    /// @todo set TX Channel

    // Check channels
    foreach(qint64 i, channels) {
      if (! _channels.contains(i)) {
        errorMessage = QString("Parse error @ %1,%2: Cannot create scanlist '%3', unknown channel index %4.")
            .arg(line).arg(column).arg(name).arg(i);
        return false;
      }
      // link channels
      _scanlists[idx]->addChannel(_channels[i]);
    }
    // done
    return true;
  }

  // check index
  if (_scanlists.contains(idx)) {
    errorMessage = QString("Parse error @ %1,%2: Cannot create scan list '%3' with index %4, index already taken.")
        .arg(line).arg(column).arg(name).arg(idx);
    return false;
  }

  ScanList *lst = new ScanList(name);
  _scanlists[idx] = lst;
  _config->scanlists()->addScanList(lst);

  return true;
}
