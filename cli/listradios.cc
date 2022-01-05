#include "listradios.hh"
#include <QTextStream>
#include "radioinfo.hh"


int
listRadios(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser); Q_UNUSED(app);

  QList<RadioInfo> radios = RadioInfo::allRadios(false);
  QTextStream out(stdout);
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << " Key";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << "Name";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << "Manufacturer";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(40); out << "Remark";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(40); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
  foreach (RadioInfo radio, radios) {
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << (" " +radio.key());
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << radio.name();
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << radio.manufactuer();
    out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| \n";
    if (! radio.alias().isEmpty()) {
      foreach (RadioInfo alias, radio.alias()) {
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << (" " +alias.key());
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << alias.name();
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(18); out << alias.manufactuer();
        out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "| ";
        //out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(40);
        out << " Alias for " << radio.manufactuer() << " " << radio.name() << "\n";
      }
    }
  }
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(18); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "+-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar('-'); out.setFieldWidth(40); out << "-";
  out.setFieldAlignment(QTextStream::AlignLeft); out.setPadChar(' '); out.setFieldWidth(0); out << "\n";
  out.flush();

  return 0;
}
