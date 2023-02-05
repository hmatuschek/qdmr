#include "melody.hh"
#include <QRegularExpression>

/* ********************************************************************************************* *
 * Implementation of Melody
 * ********************************************************************************************* */
Melody::Melody(QObject *parent)
  : ConfigItem(parent), _melody()
{
  // pass...
}

bool
Melody::fromLilipond(const QString &melody) {
  _melody.clear();
  Note::Duration currentDuration = Note::Duration::Whole;
  foreach (QString note, melody.split(" ", Qt::SkipEmptyParts)) {
    Note n;
    if (! n.fromLilipond(note, currentDuration))
      return false;
    currentDuration = n.duration;
    _melody.append(n);
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of Melody::Note
 * ********************************************************************************************* */
Melody::Note::Note()
  : tone(Tone::Rest), duration(Duration::Whole), variant(Variant::None), octave(0)
{
  // pass...
}

bool
Melody::Note::fromLilipond(const QString &note, Duration currentDuration) {
  QRegularExpression note_pattern("^([cdefgab])(is|es|)([,]+|[']+|)(1|2|4|8|16|)$");
  QRegularExpression rest_pattern("^r(1|2|4|8|16|)$");
  QRegularExpressionMatch note_match = note_pattern.match(note);

  if (note_match.isValid()) {
    if ("c" == note_match.captured(1)) tone=Tone::C;
    else if ("d" == note_match.captured(1)) tone=Tone::D;
    else if ("e" == note_match.captured(1)) tone=Tone::E;
    else if ("f" == note_match.captured(1)) tone=Tone::F;
    else if ("g" == note_match.captured(1)) tone=Tone::G;
    else if ("a" == note_match.captured(1)) tone=Tone::A;
    else if ("b" == note_match.captured(1)) tone=Tone::B;
    else return false;

    if ("is" == note_match.captured(2)) variant = Variant::Sharp;
    else if ("es" == note_match.captured(2)) variant = Variant::Flat;
    else variant = Variant::None;

    if (0 == note_match.capturedLength(3)) octave = 0;
    else if ('\'' == note_match.captured(3).at(0)) octave = note_match.capturedLength(3);
    else if (','  == note_match.captured(3).at(0)) octave = -note_match.capturedLength(3);
    else return false;

    if (0 == note_match.capturedLength(4)) duration = currentDuration;
    else if ("1" == note_match.captured(4)) duration = Duration::Whole;
    else if ("2" == note_match.captured(4)) duration = Duration::Half;
    else if ("4" == note_match.captured(4)) duration = Duration::Quater;
    else if ("8" == note_match.captured(4)) duration = Duration::Eighth;
    else if ("16" == note_match.captured(4)) duration = Duration::Sixteenth;
    else return false;
    return true;
  }

  QRegularExpressionMatch rest_match = rest_pattern.match(note);
  if (rest_match.isValid()) {
    tone = Tone::Rest;
    if (0 == note_match.capturedLength(1)) duration = currentDuration;
    else if ("1" == note_match.captured(1)) duration = Duration::Whole;
    else if ("2" == note_match.captured(1)) duration = Duration::Half;
    else if ("4" == note_match.captured(1)) duration = Duration::Quater;
    else if ("8" == note_match.captured(1)) duration = Duration::Eighth;
    else if ("16" == note_match.captured(1)) duration = Duration::Sixteenth;
    else return false;
    return true;
  }

  return false;
}
