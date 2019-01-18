#ifndef SETTINGS_HH
#define SETTINGS_HH

#include <QSettings>
#include <QDateTime>

class Settings : public QSettings
{
	Q_OBJECT

public:
	explicit Settings(QObject *parent=nullptr);

	QDateTime lastRepeaterUpdate() const;
	bool repeaterUpdateNeeded(uint period=7) const;
	void repeaterUpdated();
};

#endif // SETTINGS_HH
