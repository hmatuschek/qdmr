#ifndef AUTODETECT_HH
#define AUTODETECT_HH

#include "radio.hh"
#include <QCommandLineParser>
#include <QCoreApplication>


QVariant parseDeviceHandle(const QString &device);
void printDevices(QTextStream &out, const QList<USBDeviceDescriptor> &devices);
Radio *autoDetect(QCommandLineParser &parser, QCoreApplication &app);

#endif // AUTODETECT_HH
