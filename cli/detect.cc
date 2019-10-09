#include "detect.hh"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include "dfu_libusb.hh"


int detect(QCommandLineParser &parser, QCoreApplication &app) {
  Q_UNUSED(parser);

  DFUDevice device(0x0483, 0xdf11, &app);
  if (device.isOpen()) {
    qDebug() << "Device: " << device.identifier();
    return 0;
  }

  qDebug() << "Cannot detect device.";
  return -1;
}


