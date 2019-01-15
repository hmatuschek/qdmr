#ifndef RD5R_HH
#define RD5R_HH

#include "radio.hh"
#include "hidinterface.hh"
#include "rd5r_codeplug.hh"


class RD5R: public Radio
{
	Q_OBJECT

public:
	typedef enum { TaskNone, TaskDownload, TaskUpload } Task;

public:
	RD5R(QObject *parent=nullptr );

	const QString &name() const;
	const Radio::Features &features() const;

public slots:
	bool startDownload(Config *config);
	bool startUpload(Config *config);

protected:
	void run();

protected slots:
	void onDonwloadFinished();

protected:
	QString _name;
	Task _task;
	HID *_dev;
	Config *_config;
	RD5RCodePlug _codeplug;
};

#endif // RD5R_HH
