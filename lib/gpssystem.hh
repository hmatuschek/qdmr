#ifndef GPSSYSTEM_H
#define GPSSYSTEM_H

#include <QObject>
#include <QAbstractTableModel>


class Config;
class DigitalContact;
class DigitalChannel;



/** This class represents a GPS signalling system within the codeplug.
 * @ingroup conf */
class GPSSystem : public QObject
{
  Q_OBJECT

public:
  /** Constructor.
   *
   * Please note, that a contact needs to be set in order for the GPS system to work properly.
   *
   * @param name Specifies the name of the GPS system.
   * @param contact Specifies the contact, the GPS position is send to.
   * @param revertChannel Specifies the channel on which the GPS is send on. If @c nullptr, the GPS
   * data is send on the current channel.
   * @param period Specifies the update period in seconds.
   * @param parent Specifies the QObject parent object. */
  GPSSystem(const QString &name, DigitalContact *contact=nullptr,
            DigitalChannel *revertChannel = nullptr, uint period=300,
            QObject *parent = nullptr);

  /** Returns the name of the GPS system. */
  const QString &name() const;
  /** Sets the name of the GPS system. */
  void setName(const QString &name);

  /** Returns @c true if a contact is set for the GPS system. */
  bool hasContact() const;
  /** Returns the destination contact for the GPS information or @c nullptr if not set. */
  DigitalContact *contact() const;
  /** Sets the destination contact for the GPS information. */
  void setContact(DigitalContact *contact);

  /** Returns @c true if the GPS system has a revert channel set. If not, the GPS information will
   * be send on the current channel. */
  bool hasRevertChannel() const;
  /** Returns the revert channel for the GPS information or @c nullptr if not set. */
  DigitalChannel *revertChannel() const;
  /** Sets the revert channel for the GPS information to be send on. */
  void setRevertChannel(DigitalChannel *channel);

  /** Returns the update period in seconds. */
  uint period() const;
  /** Sets the update period in seconds. */
  void setPeriod(uint period);

signals:
  /** Gets emitted if the GPS system is modified. */
  void modified();

protected slots:
  /** Internal used callback to get notified if the destination contact is deleted. */
  void onContactDeleted();
  /** Internal used callback to get notified if the revert channel is deleted. */
  void onRevertChannelDeleted();

protected:
  /** Holds the name of the GPS system. */
  QString _name;
  /** Holds the destination contact for the GPS information. */
  DigitalContact *_contact;
  /** Holds the revert channel on which the GPS information is send on. */
  DigitalChannel *_revertChannel;
  /** Holds the update period in seconds. */
  uint _period;
};


/** The list of GPS systems.
 * @ingroup conf */
class GPSSystems: public QAbstractTableModel
{
Q_OBJECT

public:
  /** Constructs an empty list of GPS systems. */
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
