#ifndef RADIOID_HH
#define RADIOID_HH

#include <QAbstractListModel>


class RadioID : public QObject
{
  Q_OBJECT

public:
  explicit RadioID(uint32_t id, QObject *parent = nullptr);

  uint32_t id() const;
  void setId(uint32_t id);

signals:
  void modified();

protected:
  uint32_t _id;
};


class RadioIDList: public QAbstractListModel
{
  Q_OBJECT

public:
  explicit RadioIDList(QObject *parent=nullptr);

  void clear();

  int count() const;
  RadioID * getId(uint idx) const;
  RadioID *find(uint32_t id) const;
  int indexOf(RadioID *id) const;

  bool setDefault(uint idx);

  int addId(RadioID *id);
  int addId(uint32_t id);

  bool remId(RadioID *id);
  bool remId(uint32_t id);

  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

signals:
  void modified();

protected slots:
  void onIdDeleted(QObject *obj);

protected:
  QList<RadioID *> _ids;
};

#endif // RADIOID_HH
