#ifndef CONFIGOBJECT_HH
#define CONFIGOBJECT_HH

#include <QObject>
#include <QString>
#include <QHash>


class ConfigObject : public QObject
{
  Q_OBJECT

public:
  class Context
  {
  public:
    Context();
    virtual ~Context();

    const QString &version() const;
    void setVersion(const QString &ver);

    virtual bool contains(ConfigObject *obj) const;
    virtual bool contains(const QString &id) const;

    virtual QString getId(ConfigObject *obj) const;
    virtual ConfigObject *getObj(const QString &id) const;

    virtual bool add(const QString &id, ConfigObject *);

  protected:
    QString _version;
    QHash<QString, ConfigObject*> _objects;
    QHash<ConfigObject*, QString> _ids;
  };

protected:
  /** Hidden constructor. */
  ConfigObject(const QString &idBase="id", QObject *parent = nullptr);

public:
  virtual bool label(Context &context);

  virtual bool hasExtension(const QString &name) const;
  virtual QList<QString> extensionNames() const;
  virtual const ConfigObject *extension(const QString &name) const;
  virtual ConfigObject *extension(const QString &name);
  virtual void addExtension(const QString &name, ConfigObject *ext);

  template <class Object>
  bool is() const {
    return nullptr != qobject_cast<const Object*>(this);
  }

  template <class Object>
  const Object *as() const {
    return qobject_cast<const Object*>(this);
  }

  template <class Object>
  Object *as() {
    return qobject_cast<Object *>(this);
  }

signals:
  void modified(ConfigObject *obj);

protected:
  QString _idBase;
  QHash<QString, ConfigObject *> _extensions;
};


class AbstractConfigObjectList: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractConfigObjectList(QObject *parent = nullptr);

public:
  virtual int count() const = 0;
  virtual int indexOf(ConfigObject *obj) const = 0;

  virtual bool add(ConfigObject *obj);

protected:
  virtual bool take(ConfigObject *obj);

signals:
  void modified(ConfigObject *obj);
  void elementModified(int idx);
  void elementDeleted(int idx);

private slots:
  void onElementModified(ConfigObject *obj);
  void onElementDeleted(QObject *obj);
};


class ConfigObjectList: public AbstractConfigObjectList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigObjectList(QObject *parent = nullptr);

  bool add(ConfigObject *obj);

protected:
  bool take(ConfigObject *obj);
};


class ConfigObjectRefList: public AbstractConfigObjectList
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit ConfigObjectRefList(QObject *parent = nullptr);
};


#endif // CONFIGOBJECT_HH
