#include "melody.hh"
#include <QRegularExpression>


/* ********************************************************************************************* *
 * Implementation of Melody
 * ********************************************************************************************* */
Melody::Melody(unsigned int bpm, QObject *parent)
  : ConfigItem(parent), _bpm(bpm), _melody()
{
  // pass...
}

ConfigItem *
Melody::clone() const {
  Melody *melody = new Melody();
  if (! melody->copy(*this)) {
    melody->deleteLater();
    return nullptr;
  }
  return melody;
}

bool
Melody::copy(const ConfigItem &other) {
  if (! ConfigItem::copy(other))
    return false;

  const Melody *omelody = other.as<Melody>();

  _bpm = omelody->_bpm;
  _melody = omelody->_melody;

  return true;
}

Melody::iterator
Melody::begin() {
  return _melody.begin();
}
Melody::iterator
Melody::end() {
  return _melody.end();
}

Melody::const_iterator
Melody::begin() const {
  return _melody.begin();
}
Melody::const_iterator
Melody::end() const {
  return _melody.end();
}

size_t
Melody::count() const {
  return _melody.count();
}

const Melody::Note &
Melody::operator [](size_t index) const {
  return _melody[index];
}

Melody::Note &
Melody::operator [](size_t index) {
  return _melody[index];
}

unsigned int
Melody::bpm() const {
  return _bpm;
}
void
Melody::setBPM(unsigned int bpm) {
  if (_bpm == bpm)
    return;
  _bpm = bpm;
  emit modified(this);
}

bool
Melody::fromLilypond(const QString &melody) {
  _melody.clear();
  Note::Duration currentDuration = Note::Duration::Whole;
  for (QString note : melody.split(" ", Qt::SkipEmptyParts)) {
    Note n;
    if (! n.fromLilypond(note, currentDuration))
      return false;
    currentDuration = n.duration;
    _melody.append(n);
  }

  emit modified(this);
  return true;
}

QString
Melody::toLilypond() const {
  QStringList res;
  Note::Duration currentDuration = Note::Duration::Whole;
  for (Note note : _melody) {
    res.append(note.toLilypond(currentDuration));
    currentDuration = note.duration;
  }
  return res.join(" ");
}

QVector<QPair<double, unsigned int>>
Melody::toTones() const {
  QVector<QPair<double, unsigned int>> res;
  for (const Note &note: _melody) {
    res.append(note.toTone(_bpm));
  }
  return res;
}

bool
Melody::infer(const QVector<QPair<double, unsigned int> > &tones) {
  // Find minimum quantization error -> infer also BPM
  unsigned int best_bpm = 30;
  unsigned int best_cost = quantizationTimingError(tones, best_bpm) + std::abs(100-(int)best_bpm);
  for (unsigned int bpm=31; bpm<200; bpm++) {
    unsigned int cost = quantizationTimingError(tones, best_bpm);
    if (cost < best_cost) {
      best_bpm = bpm;
      best_cost = cost + std::abs(100 - (int)bpm);
    }
  }
  _bpm = best_bpm;
  _melody.clear();
  for(const QPair<double,unsigned int> &tone: tones) {
    Note note; note.infer(tone.first, tone.second, _bpm);
    _melody.append(note);
  }

  emit modified(this);
  return true;
}

unsigned int
Melody::quantizationTimingError(const QVector<QPair<double, unsigned int> > &tones, unsigned int bpm) {
  if (0 == tones.count())
    return 0;
  unsigned int error=0;
  for (const QPair<double,unsigned int> &tone: tones) {
    error += Note::quantizationTimingError(tone.second, bpm);
  }
  return error / tones.count();
}


/* ********************************************************************************************* *
 * Implementation of Melody::Note
 * ********************************************************************************************* */
Melody::Note::Note()
  : tone(Tone::C), duration(Duration::Quater), dotted(false), octave(0)
{
  // pass...
}

bool
Melody::Note::fromLilypond(const QString &note, Duration currentDuration) {
  QRegularExpression note_pattern("^(c|cis|des|d|dis|ees|e|f|fis|ges|g|gis|aes|a|ais|bes|b)([,]+|[']+|)(1|2|4|8|16|)(\\.|)$");
  QRegularExpression rest_pattern("^r(1|2|4|8|16|)(\\.|)$");
  QRegularExpressionMatch note_match = note_pattern.match(note);

  if (note_match.isValid()) {
    if ("c" == note_match.captured(1)) tone=Tone::C;
    else if (("cis" == note_match.captured(1)) || ("des" == note_match.captured(1))) tone=Tone::Cis;
    else if ("d" == note_match.captured(1)) tone=Tone::D;
    else if (("dis" == note_match.captured(1)) || ("ees" == note_match.captured(1))) tone=Tone::Dis;
    else if ("e" == note_match.captured(1)) tone=Tone::E;
    else if ("f" == note_match.captured(1)) tone=Tone::F;
    else if (("fis" == note_match.captured(1)) || ("ges" == note_match.captured(1))) tone=Tone::Fis;
    else if ("g" == note_match.captured(1)) tone=Tone::G;
    else if (("gis" == note_match.captured(1)) || ("aes" == note_match.captured(1))) tone=Tone::Gis;
    else if ("a" == note_match.captured(1)) tone=Tone::A;
    else if (("ais" == note_match.captured(1)) || ("bes" == note_match.captured(1))) tone=Tone::Ais;
    else if ("b" == note_match.captured(1)) tone=Tone::B;
    else return false;

    if (0 == note_match.capturedLength(2)) octave = 0;
    else if ('\'' == note_match.captured(2).at(0)) octave = note_match.capturedLength(3);
    else if (','  == note_match.captured(2).at(0)) octave = -note_match.capturedLength(3);
    else return false;

    if (0 == note_match.capturedLength(3)) duration = currentDuration;
    else if ("1" == note_match.captured(3)) duration = Duration::Whole;
    else if ("2" == note_match.captured(3)) duration = Duration::Half;
    else if ("4" == note_match.captured(3)) duration = Duration::Quater;
    else if ("8" == note_match.captured(3)) duration = Duration::Eighth;
    else if ("16" == note_match.captured(3)) duration = Duration::Sixteenth;
    else return false;

    dotted = (1 == note_match.capturedLength(4));

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
    dotted = (1 == note_match.capturedLength(2));
    return true;
  }

  return false;
}

QString
Melody::Note::toLilypond(Duration currentDuration) const {
  QString res; res.reserve(10);
  switch (tone) {
  case Tone::C: res.append("c"); break;
  case Tone::Cis: res.append("cis"); break;
  case Tone::D: res.append("d"); break;
  case Tone::Dis: res.append("dis"); break;
  case Tone::E: res.append("e"); break;
  case Tone::F: res.append("f"); break;
  case Tone::Fis: res.append("fis"); break;
  case Tone::G: res.append("g"); break;
  case Tone::Gis: res.append("gis"); break;
  case Tone::A: res.append("a"); break;
  case Tone::Ais: res.append("ais"); break;
  case Tone::B: res.append("b"); break;
  case Tone::Rest: res.append("r"); break;
  }
  if (Tone::Rest != tone) {
    if (0 > octave) {
      for (int i=0; i<-octave; i++)
        res.append(",");
    } else if (0 < octave) {
      for (int i=0; i<octave; i++)
        res.append("'");
    }
  }
  if (duration != currentDuration) {
    switch (duration) {
    case Duration::Whole: res.append("1"); break;
    case Duration::Half: res.append("2"); break;
    case Duration::Quater: res.append("4"); break;
    case Duration::Eighth: res.append("8"); break;
    case Duration::Sixteenth: res.append("16"); break;
    }
  }
  if (dotted) res.append(".");
  return res;
}

QPair<double, unsigned int>
Melody::Note::toTone(unsigned int bpm) const {
  static const unsigned int bar = (4*60000)/bpm;
  unsigned int dur = bar;

  if (Duration::Whole == duration) {
    dur = bar; if (dotted) dur += dur/2;
  } else if (Duration::Half == duration) {
    dur = bar/2; if (dotted) dur += dur/2;
  } else if (Duration::Quater == duration) {
    dur = bar/4; if (dotted) dur += dur/2;
  } else if (Duration::Eighth == duration) {
    dur = bar/8; if (dotted) dur += dur/2;
  } else if (Duration::Sixteenth == duration) {
    dur = bar/16; if (dotted) dur += dur/2;
  }

  if (Tone::Rest == tone) {
    return QPair<double, unsigned int>(0.0, dur);
  }

  int halfTones = 12*octave;
  switch (tone) {
  case Tone::C:   halfTones -=  9; break;
  case Tone::Cis: halfTones -=  8; break;
  case Tone::D:   halfTones -=  7; break;
  case Tone::Dis: halfTones -=  6; break;
  case Tone::E:   halfTones -=  5; break;
  case Tone::F:   halfTones -=  4; break;
  case Tone::Fis: halfTones -=  3; break;
  case Tone::G:   halfTones -=  2; break;
  case Tone::Gis: halfTones -=  1; break;
  case Tone::A: break;
  case Tone::Ais: halfTones += 1; break;
  case Tone::B:   halfTones += 2; break;
  case Tone::Rest: break;
  }
  double freq = 440.*std::pow(2.0,halfTones/12.0);
  return QPair<double, unsigned int>(freq, dur);
}

unsigned int
Melody::Note::infer(double frequency, unsigned int ms, unsigned int bpm) {
  // Try to infer half-tones from A4
  int halfTones = std::round(std::log2(frequency/440.)*12.0);
  int halfTonesC4 = halfTones+9;
  octave = halfTonesC4/12;
  halfTonesC4 = std::abs(halfTonesC4)%12;
  switch(halfTonesC4) {
  case 0: tone = Tone::C; break;
  case 1: tone = Tone::Cis; break;
  case 2: tone = Tone::D; break;
  case 3: tone = Tone::Dis; break;
  case 4: tone = Tone::E; break;
  case 5: tone = Tone::F; break;
  case 6: tone = Tone::Fis; break;
  case 7: tone = Tone::G; break;
  case 8: tone = Tone::Gis; break;
  case 9: tone = Tone::A; break;
  case 10: tone = Tone::Ais; break;
  case 11: tone = Tone::B; break;
  }

  // Try to infer note duration from duration and BPM
  static const unsigned int bar = (4*60000)/bpm;
  int fraction = std::round(std::log2(bar)-std::log2(ms));
  int diff = 0;
  if (1 > fraction) {
    duration = Duration::Whole;
    diff = std::abs((int)ms-(int)bar);
  } else if (2 > fraction) {
    duration = Duration::Half;
    diff = std::abs((int)ms-(int)bar/2);
  } else if (3 > fraction) {
    duration = Duration::Quater;
    diff = std::abs((int)ms-(int)bar/4);
  } else if (4 > fraction) {
    duration = Duration::Eighth;
    diff = std::abs((int)ms-(int)bar/8);
  } else {
    duration = Duration::Sixteenth;
    diff = std::abs((int)ms-(int)bar/16);
  }

  return diff;
}

unsigned int
Melody::Note::quantizationTimingError(unsigned int ms, unsigned int bpm) {
  static const unsigned int bar = (4*60000)/bpm;
  int fraction = std::round(std::log2(bar)-std::log2(ms));
  // Limit onto [2^0, 2^4] fractions == [1/1, 1/16]
  fraction = std::max(0, std::min(4, fraction));
  return std::abs(ms - bar*std::pow(2, fraction));
}
