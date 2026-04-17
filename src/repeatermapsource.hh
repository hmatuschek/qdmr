#ifndef REPEATERMAPSOURCE_HH
#define REPEATERMAPSOURCE_HH

#include "repeaterdatabase.hh"


class RepeaterMapSource : public DownloadableRepeaterDatabaseSource
{
  Q_OBJECT

public:
  explicit RepeaterMapSource(const QByteArray &apiToken, QObject *parent = nullptr);

protected:
  bool parse(const QByteArray &doc) override;
};

#endif // REPEATERMAPSOURCE_HH
