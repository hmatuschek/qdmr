#ifndef HIDINTERFACE_HH
#define HIDINTERFACE_HH

#include <QtGlobal>
#include <QObject>

#ifdef Q_OS_MACOS
#include "hid_macos.hh"
#endif


class HID: public HIDevice
{
	Q_OBJECT

public:
	explicit HID(int vid, int pid, QObject *parent = nullptr);
	virtual ~HID();

	QString identify();

	bool readBlock(int bno, unsigned char *data, int nbytes);
	bool readFinish();
	bool writeBlock(int bno, unsigned char *data, int nbytes);
	bool writeFinish();
};

#endif // HIDINTERFACE_HH
