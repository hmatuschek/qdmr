#ifndef HEARHAMREPEATERSOURCE_HH
#define HEARHAMREPEATERSOURCE_HH

#include "repeaterdatabase.hh"


class HearhamRepeaterSource : public DownloadableRepeaterDatabaseSource
{
Q_OBJECT

public:
  explicit HearhamRepeaterSource(QObject *parent = nullptr);

protected:
  bool parse(const QByteArray &doc);
};

#endif // HEARHAMREPEATERSOURCE_HH
