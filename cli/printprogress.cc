#include "printprogress.hh"

#include <QTextStream>


void print_progress(int prog) {
  QTextStream out(stdout);
  out << "\r";
  out.setFieldWidth(2);
  out << prog;
  out.setFieldWidth(0);
  out << "%";
  prog = (prog*20)/100;
  out << "[";
  for (int i=0; i<prog; i++)
    out << "*";
  for (int i=prog; i<20; i++)
    out << " ";
  out << "]";
}

