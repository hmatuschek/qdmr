#ifndef GPSSYSTEM_H
#define GPSSYSTEM_H

#include <QObject>
#include <QAbstractTableModel>


class Config;
class DigitalContact;
class DigitalChannel;



/** This class represents a GPS signalling system.
 * @ingroup conf */
class GPSSystem : public QObject
{
  Q_OBJECT

public:
  GPSSystem(const QString &name, DigitalContact *contact=nullptr,
            DigitalChannel *revertChannel = nullptr, uint period=300,
            QObject *parent = nullptr);

  const QString &name() const;
  void setName(const QString &name);

  bool hasContact() const;
  DigitalContact *contact() const;
  void setContact(DigitalContact *contact);

  bool hasRevertChannel() const;
  DigitalChannel *revertChannel() const;
  void setRevertChannel(DigitalChannel *channel);

  uint period() const;
  void setPeriod(uint period);

signals:
  void modified();

protected slots:
  void onContactDeleted();
  void onRevertChannelDeleted();

protected:
  QString _name;
  DigitalContact *_contact;
  DigitalChannel *_revertChannel;
  uint _period;
};



class GPSSystems: public QAbstractTableModel
{
Q_OBJECT

public:
  explicit GPSSystems(QObject *parent=nullptr);

  /** Returns the number of GPS systems in this list. */
  int count() const;
  /** Clears the list. */
  void clear();
  /** Returns the index of the GPS System. */
  int indexOf(GPSSystem *gps) const;
  /** Gets the GPS system at the specified index. */
  GPSSystem *gpsSystem(int idx) const;
  /** Adds a GPS system to the list at the specified row.
   * If row<0 the system gets appendet to the list.*/
  int addGPSSystem(GPSSystem *gps, int row=-1);
  /** Removes the given GPS system from the list. */
  bool remGPSSystem(GPSSystem *gps);
  /** Removes the GPS system at the given index from the list. */
  bool remGPSSystem(int idx);
  /** Moves the GPS system at index @c idx one step up. */
  bool moveUp(int idx);
  /** Moves the GPS system at index @c idx one step up. */
  bool moveDown(int idx);

  // QAbstractTableModel interface
  /** Implements QAbstractTableModel, returns number of rows. */
  int rowCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns number of colums. */
  int columnCount(const QModelIndex &index) const;
  /** Implements QAbstractTableModel, returns data at cell. */
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
  /** Implements QAbstractTableModel, returns header at section. */
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

signals:
  /** Gets emitted once the table has been changed. */
  void modified();

protected slots:
  /** Internal callback on deleted GPS systems. */
  void onGPSSystemDeleted(QObject *obj);
  /** Internal callback on modified GPS systems. */
  void onGPSSystemEdited();

protected:
  /** Just the vector of GPS systems. */
  QVector<GPSSystem *> _gpsSystems;
};


#endif // GPSSYSTEM_H
