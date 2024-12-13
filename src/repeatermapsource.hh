#ifndef REPEATERMAPSOURCE_HH
#define REPEATERMAPSOURCE_HH

#include "repeaterdatabase.hh"


class RepeaterMapSource : public DownloadableRepeaterDatabaseSource
{
  Q_OBJECT

public:
  explicit RepeaterMapSource(QObject *parent = nullptr);

protected:
  bool parse(const QByteArray &doc);
};

#endif // REPEATERMAPSOURCE_HH
