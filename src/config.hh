#ifndef CONFIG_HH
#define CONFIG_HH

#include "contact.hh"
#include "rxgrouplist.hh"
#include "channel.hh"
#include "zone.hh"
#include "scanlist.hh"
#include <QWidget>
#include <QLineEdit>


class Config : public QObject
{
	Q_OBJECT

public:
	explicit Config(QObject *parent = nullptr);

  bool isModified() const;
  void setModified(bool modified);

	ContactList *contacts() const;
  RXGroupLists *rxGroupLists() const;
  ChannelList *channelList() const;
  ZoneList *zones() const;
  ScanLists *scanlists() const;

  uint id() const;
  void setId(uint id);

  const QString &name() const;
  void setName(const QString &name);

  const QString &introLine1() const;
  void setIntroLine1(const QString &line);
  const QString &introLine2() const;
  void setIntroLine2(const QString &line);

  void reset();

  bool readCSV(QTextStream &stream);
  bool writeCSV(QTextStream &stream);

signals:
	void modified();

protected slots:
  void onConfigModified();

protected:
  bool readCSVDefine(QTextStream &stream, QMap<int, Channel *> &channels,
                     QMap<int, DigitalContact *> &contacts, QMap<int, RXGroupList *> &rxgroups,
                     QMap<int, Zone *> &zones, QMap<int, ScanList *> &scanlists);

  bool readCSVLink(QTextStream &stream, QMap<int, Channel *> &channels,
                   QMap<int, DigitalContact *> &contacts, QMap<int, RXGroupList *> &rxgroups,
                   QMap<int, Zone *> &zones, QMap<int, ScanList *> &scanlists);

protected:
  bool _modified;
	ContactList *_contacts;
  RXGroupLists *_rxGroupLists;
  ChannelList *_channels;
  ZoneList *_zones;
  ScanLists *_scanlists;

  uint _id;
  QString _name;
  QString _introLine1;
  QString _introLine2;
};


class ConfigView: public QWidget
{
  Q_OBJECT

public:
  ConfigView(Config *conf, QWidget *parent=nullptr);

protected slots:
  void onConfigModified();
  void onIdModified();
  void onNameModified();
  void onIntro1Modified();
  void onIntro2Modified();

protected:
  Config *_config;
  QLineEdit *_id;
  QLineEdit *_name;
  QLineEdit *_intro1;
  QLineEdit *_intro2;
};

#endif // CONFIG_HH
