#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include "radio.hh"
#include "radiointerface.hh"


int detect(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser);
  Q_UNUSED(app);

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    qDebug() << "No compatible radio found:" << errorMessage;
    return -1;
  }

  qDebug() << "Found:" << radio->name();
  delete  radio;

  return 0;
}


