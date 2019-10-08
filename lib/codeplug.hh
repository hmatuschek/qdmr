#ifndef CODEPLUG_HH
#define CODEPLUG_HH

#include <QObject>

class Config;


class CodePlug: public QObject
{
	Q_OBJECT

protected:
	CodePlug(QObject *parent=nullptr);

public:
	virtual ~CodePlug();

	unsigned char *data();

	virtual bool decode(Config *config) = 0;
	virtual bool encode(Config *config) = 0;

protected:
	unsigned char _radio_mem[1024*1024*2];
};

#endif // CODEPLUG_HH
