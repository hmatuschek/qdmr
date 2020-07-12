#ifndef USBSERIAL_HH
#define USBSERIAL_HH

#include <QObject>
#include <QSerialPort>
#include "radiointerface.hh"

class USBSerial : public QSerialPort, public RadioInterface
{
  Q_OBJECT

protected:
  explicit USBSerial(unsigned vid, unsigned pid, QObject *parent=nullptr);

public:
  virtual ~USBSerial();

  bool isOpen() const;
  void close();

  const QString &errorMessage() const;

protected:
  /** Holds the last error message. */
  QString _errorMessage;
};

#endif // USBSERIAL_HH
