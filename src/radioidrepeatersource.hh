#ifndef RADIOIDREPEATERSOURCE_HH
#define RADIOIDREPEATERSOURCE_HH

#include "repeaterdatabase.hh"


class RadioidRepeaterSource : public DownloadableRepeaterDatabaseSource
{
Q_OBJECT

public:
  explicit RadioidRepeaterSource(QObject *parent = nullptr);

protected:
  bool parse(const QByteArray &doc);
};

#endif // RADIOIDREPEATERSOURCE_HH
