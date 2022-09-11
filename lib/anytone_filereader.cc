#include "anytone_filereader.hh"
#include <QFile>
#include <QFileInfo>
#include <QtEndian>

#include "d868uv_filereader.hh"
#include "d878uv_filereader.hh"

/** Internal used struct to read a CPS file header. */
struct __attribute__((packed)) file_header {
  char version[5];         ///< The version number.
  uint32_t payload_size;   ///< The content size.
  char modelname[7];       ///< The model name.
  uint8_t _unused0010[3];  ///< Unused/reserved.
  char hw_version[4];      ///< The hardware version.
};


/* ********************************************************************************************* *
 * Implementation of AnytoneFileReader::Element
 * ********************************************************************************************* */
AnytoneFileReader::Element::Element(const uint8_t *ptr)
  : _data(ptr)
{
  // pass...
}

AnytoneFileReader::Element::~Element() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneFileReader
 * ********************************************************************************************* */
AnytoneFileReader::AnytoneFileReader(Config *config, const uint8_t *data, size_t size, QString &message)
  : _context(config), _start(data), _data(data), _size(size), _message(message)
{
  // pass...
}

AnytoneFileReader::~AnytoneFileReader() {
  // pass...
}

bool
AnytoneFileReader::read() {
  // Set pointer to start
  _data = _start;

  if (! this->readHeader())
    return false;
  if (! this->readChannels())
    return false;
  if (! this->readRadioIDs())
    return false;
  if (! this->readZones())
    return false;
  if (! this->readScanLists())
    return false;
  if (! this->readAnalogContacts())
    return false;

  // 2nd pass, link.
  _data = _start;
  if (! this->linkHeader())
    return false;
  if (! this->linkChannels())
    return false;
  if (! this->linkRadioIDs())
    return false;
  if (! this->linkZones())
    return false;
  if (! this->linkScanLists())
    return false;
  if (! this->linkAnalogContacts())
    return false;

  return true;
}


bool
AnytoneFileReader::read(const QString &filename, Config *config, QString &message)
{
  QFileInfo info(filename);
  if (! info.exists()) {
    message = QObject::tr("Cannot open file '%1': File does not exist.").arg(filename);
    return false;
  }

  QFile file(filename);
  if (! file.open(QFile::ReadOnly)) {
    message = QObject::tr("Cannot open file '%1': %2.").arg(filename).arg(file.errorString());
    return false;
  }

  file_header head;
  if (sizeof(file_header) != file.read((char *)&head, sizeof(file_header))) {
    message = QObject::tr("Cannot read header from file '%1': %2.")
        .arg(filename).arg(file.errorString());
    file.close();
    return false;
  }

  size_t size = qFromLittleEndian(head.payload_size)+14;
  if (size != (size_t)info.size()) {
    message = QObject::tr("Malformed header in file '%1': Mismatching content size. Expected %2, got %3.")
        .arg(filename).arg(info.size()-14).arg(size-14);
    file.close();
    return false;
  }

  uint8_t *data = file.map(0, size);
  if (nullptr == data) {
    message = QObject::tr("Cannot mmap file '%1': %2.")
        .arg(filename).arg(file.errorString());
    file.close();
    return false;
  }

  QString cps_version = QString::fromLocal8Bit(head.version, strnlen(head.version,5));
  QString model = QString::fromLocal8Bit(head.modelname, strnlen(head.modelname,7));

  // Dispatch by model name
  AnytoneFileReader *reader = nullptr;
  if ("D868UVE" == model) {
    reader = new D868UVFileReader(config, data, size, message);
  } else if ("D878UV" == model) {
    reader = new D878UVFileReader(config, data, size, message);
  } else if ("D878UV2" == model) {

  } else if ("D578UV" == model) {

  } else {
    message = QObject::tr("Cannot read codeplug file '%1': Uknown model '%2'.")
        .arg(filename).arg(model);
    file.unmap(data);
    file.close();
    return false;
  }

  if (nullptr == reader) {
    message = QObject::tr("Cannot read codeplug file '%1': Model '%2' not implemented yet.")
        .arg(filename).arg(model);
    file.unmap(data);
    file.close();
    return false;
  }

  // Clear config
  config->reset();

  if (! reader->read()) {
    message = QObject::tr("Cannot read codeplug file '%1': %2").arg(filename).arg(message);
    file.unmap(data);
    file.close();
    return false;
  }

  file.unmap(data);
  file.close();
  return true;
}
