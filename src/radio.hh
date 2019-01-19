#ifndef RADIO_HH
#define RADIO_HH

#include <QThread>
#include "verifydialog.hh"

class Config;


class Radio : public QThread
{
	Q_OBJECT

public:
	typedef struct {
		bool hasDigital;
		bool hasAnalog;

		int maxNameLength;
		int maxIntroLineLength;

		int maxChannels;
		int maxChannelNameLength;

		int maxZones;
		int maxZoneNameLength;
		int maxChannelsInZone;

		int maxScanlists;
		int maxScanlistNameLength;
		int maxChannelsInScanlist;

		int maxContacts;
		int maxContactNameLength;

		int maxGrouplists;
		int maxGrouplistNameLength;
		int maxContactsInGrouplist;
	} Features;

public:
	explicit Radio(QObject *parent = nullptr);

	virtual const QString &name() const = 0;
	virtual const Features &features() const = 0;

	bool verifyConfig(Config *config, QList<VerifyIssue> &issues);

public:
	static Radio *detect();

public slots:
	virtual bool startDownload(Config *config) = 0;
    virtual bool startUpload(Config *config) = 0;

signals:
	void downloadStarted();
	void downloadProgress(int percent);
	void downloadFinished();
	void downloadError(Radio *radio);
	void downloadComplete(Radio *radio, Config *config);

	void uploadStarted();
	void uploadProgress(int percent);
	void uploadError(Radio *radio);
	void uploadComplete(Radio *radio);
};

#endif // RADIO_HH
