#include "application.hh"
#include <QMainWindow>
#include <QtUiTools>
#include <QDesktopServices>
#include <QTranslator>
#include <QStandardPaths>

#include "logger.hh"
#include "radio.hh"
#include "codeplug.hh"
#include "config.h"
#include "settings.hh"
#include "radiolimits.hh"
#include "verifydialog.hh"
#include "rxgrouplistdialog.hh"
#include "zonedialog.hh"
#include "scanlistdialog.hh"
#include "roamingzonedialog.hh"
#include "repeaterdatabase.hh"
#include "repeaterbooksource.hh"
#include "repeatermapsource.hh"
#include "hearhamrepeatersource.hh"
#include "radioidrepeatersource.hh"
#include "userdatabase.hh"
#include "talkgroupdatabase.hh"
#include "satellitedatabase.hh"
#include "generalsettingsview.hh"
#include "radioidlistview.hh"
#include "contactlistview.hh"
#include "grouplistsview.hh"
#include "channellistview.hh"
#include "zonelistview.hh"
#include "scanlistsview.hh"
#include "positioningsystemlistview.hh"
#include "roamingchannellistview.hh"
#include "roamingzonelistview.hh"
#include "errormessageview.hh"
#include "deviceselectiondialog.hh"
#include "radioselectiondialog.hh"
#include "chirpformat.hh"
#include "configmergedialog.hh"
#include "configmergevisitor.hh"
#include "satellitedatabasedialog.hh"
#include "mainwindow.hh"



inline QStringList getLanguages() {
  QStringList languages = {QLocale::system().name()};
  if (languages.last().contains("_")) {
    languages.append(languages.last().split("_").first());
  }
  return languages;
}

Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _config(nullptr), _mainWindow(nullptr), _translator(nullptr),
    _repeater(nullptr), _satellites(nullptr), _lastDevice()
{
  setApplicationName("qdmr");
  setOrganizationName("DM3MAT");
  setOrganizationDomain("hmatuschek.github.io");

  // open logfile
  QString logdir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  Logger::get().addHandler(new FileLogHandler(logdir+"/qdmr.log"));

  // register icon themes
  QStringList iconPaths = QIcon::themeSearchPaths();
  iconPaths.prepend(":/icons");
  QIcon::setThemeSearchPaths(iconPaths);
  onPaletteChanged(palette());

  // handle translations
  _translator = new QTranslator(this);
  foreach (QString language, getLanguages()) {
    logDebug() << "Search for translation in ':/i18n/" << language << ".qm'.";
    if (_translator->load(language, ":/i18n/", "", ".qm")) {
      this->installTranslator(_translator);
      logDebug() << "Installed translator for locale '" << QLocale::system().name() << "'.";
      break;
    }
  }

  // load settings
  Settings settings;
  // load databases
  _repeater   = new RepeaterDatabase(this);
  if (settings.repeaterBookSourceEnabled())
    _repeater->addSource(new RepeaterBookSource());
  if (settings.repeaterMapSourceEnabled())
    _repeater->addSource(new RepeaterMapSource());
  if (settings.hearhamSourceEnabled())
    _repeater->addSource(new HearhamRepeaterSource());
  if (settings.radioIdRepeaterSourceEnabled())
    _repeater->addSource(new RadioidRepeaterSource());
  _users      = new UserDatabase(true, 30, this);
  _talkgroups = new TalkGroupDatabase(30, this);
  _satellites = new SatelliteDatabase(7, this);

  // create empty codeplug
  _config     = new Config(this);

  // Handle args (if there are some)
  if (argc > 1) {
    QFileInfo info(argv[1]);
    QFile file(argv[1]);
    if (! file.open(QIODevice::ReadOnly)) {
      logError() << "Cannot open codeplug file '" << argv[1] << "': " << file.errorString();
      return;
    }
    if (("conf" == info.suffix()) || ("csv" == info.suffix())) {
      QString errorMessage; QTextStream stream(&file);
      if (! _config->readCSV(stream, errorMessage)) {
        logError() << errorMessage;
        return;
      }
    } else if ("yaml" == info.suffix()) {
      ErrorStack err;
      if (! _config->readYAML(argv[1], err)) {
        logError() << "Cannot read yaml codeplug file '" << argv[1]
                   << "': " << err.format();
        return;
      }
    }
  }

  // load position
  _currentPosition = settings.position();
  _source = QGeoPositionInfoSource::createDefaultSource(this);
  if (_source) {    
    connect(_source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));
    if (settings.queryPosition()) {
      _source->startUpdates();
      _currentPosition = _source->lastKnownPosition().coordinate();
    }
  }

  // Check if updated
  _releaseNotes.checkForUpdate();

  logDebug() << "Last known position: " << _currentPosition.toString();
  connect(_config, SIGNAL(modified(ConfigItem*)), this, SLOT(onConfigModifed()));
}

Application::~Application() {
  if (_mainWindow)
    delete _mainWindow;
  _mainWindow = nullptr;
}

bool
Application::isDarkMode() const {
  return isDarkMode(palette());
}

bool
Application::isDarkMode(const QPalette &palette) const {
  int text_hsv_value = palette.color(QPalette::WindowText).value(),
      bg_hsv_value = palette.color(QPalette::Base).value();
  return text_hsv_value > bg_hsv_value;
}


bool
Application::isModified() const {
  return _config->isModified();
}

MainWindow *
Application::mainWindow() {
  if (_mainWindow)
    return _mainWindow;
  return (_mainWindow = new MainWindow(_config));
}

UserDatabase *
Application::user() const {
  return _users;
}

TalkGroupDatabase *
Application::talkgroup() const {
  return _talkgroups;
}

RepeaterDatabase *
Application::repeater() const{
  return _repeater;
}

SatelliteDatabase *
Application::satellite() const {
  return _satellites;
}


void
Application::newCodeplug() {
  if (_config->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(0, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  _config->clear();
  _config->setModified(false);
}


void
Application::loadCodeplug() {
  if (! _mainWindow)
    return;

  if (_config->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(nullptr, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  Settings settings;
  QString filename = QFileDialog::getOpenFileName(
        nullptr, tr("Open codeplug"),
        settings.lastDirectory().absolutePath(),
        tr("Codeplug Files (*.yaml);;Codeplug Files, old format (*.conf *.csv *.txt);;All Files (*)"));
  if (filename.isEmpty())
    return;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(
          nullptr, tr("Cannot open file"),
          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  logDebug() << "Load codeplug from '" << filename << "'.";
  QFileInfo info(filename);
  settings.setLastDirectoryDir(info.absoluteDir());

  if ("yaml" == info.suffix()){
    ErrorStack err;
    if (! _config->readYAML(filename, err)) {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(err.format()));
      _config->clear();
    }
  } else {
    QString errorMessage;
    QTextStream stream(&file);
    if (!_config->readCSV(stream, errorMessage)) {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(errorMessage));
      _config->clear();
    }
  }

  processEvents();
  _config->setModified(false);
}


void
Application::saveCodeplug() {
  if (! _mainWindow)
    return;

  Settings settings;
  QString filename = QFileDialog::getSaveFileName(
        nullptr, tr("Save codeplug"), settings.lastDirectory().absolutePath(),
        tr("Codeplug Files (*.yaml *.yml)"));

  if (filename.isEmpty())
    return;

  if (filename.endsWith(".conf") || filename.endsWith("csv")){
    QMessageBox::critical(nullptr, tr("Please use new YAML format."),
                          tr("Saving in the old table-based conf format was disabled with 0.9.0. "
                             "Reading these files still works."));
    return;
  }

  // append .yaml if missing
  if ((!filename.endsWith(".yaml")) && (!filename.endsWith(".yml")))
    filename.append(".yaml");

  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  QTextStream stream(&file);
  QFileInfo info(filename);
  if (_config->toYAML(stream)) {
    _config->setModified(false);
  } else {
    QMessageBox::critical(nullptr, tr("Cannot save codeplug"),
                          tr("Cannot save codeplug to file '%1'.").arg(filename));
  }

  file.flush();
  file.close();

  settings.setLastDirectoryDir(info.absoluteDir());
}


void
Application::exportCodeplugToChirp() {
  if (! _mainWindow)
    return;

  Settings settings;
  QString filename = QFileDialog::getSaveFileName(
        nullptr, tr("Export codeplug"), settings.lastDirectory().absolutePath(),
        tr("CHIRP CSV Files (*.csv)"));

  if (filename.isEmpty())
    return;


  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  QTextStream stream(&file);
  QFileInfo info(filename);
  ErrorStack err;
  if (! ChirpWriter::write(stream, _config, err))
    QMessageBox::critical(nullptr, tr("Cannot export codeplug"),
                          tr("Cannot export codeplug to file '%1':\n%2").arg(filename).arg(err.format()));

  file.flush();
  file.close();

  settings.setLastDirectoryDir(info.absoluteDir());
}


void
Application::importCodeplug() {
  if (! _mainWindow)
    return;

  Settings settings;
  QString filename = QFileDialog::getOpenFileName(
        nullptr, tr("Import codeplug"), settings.lastDirectory().absolutePath(),
        tr("CHIRP CSV Files (*.csv);;YAML Files (*.yaml *.yml)"));

  if (filename.isEmpty())
    return;

  Config merging;
  ErrorStack err;

  if (filename.endsWith(".csv")) {
    // import CHIRP CSV file
    QFile file(filename);
    if (! file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(nullptr, tr("Cannot open file"),
                            tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(file.errorString()));
      return;
    }

    QTextStream stream(&file);
    if (! ChirpReader::read(stream, &merging, err)) {
      QMessageBox::critical(nullptr, tr("Cannot import codeplug"),
                            tr("Cannot import codeplug from '%1': %2")
                            .arg(filename).arg(err.format()));
      return;
    }
  } else if (filename.endsWith(".yaml") || filename.endsWith(".yml")) {
    // import QDMR YAML codeplug
    if (! merging.readYAML(filename, err)) {
      QMessageBox::critical(nullptr, tr("Cannot import codeplug"),
                            tr("Cannot import codeplug from '%1': %2")
                            .arg(filename).arg(err.format()));
      return;
    }
  } else {
    QMessageBox::critical(nullptr, tr("Cannot import codeplug"),
                          tr("Do not know, how to handle file '%1'.")
                          .arg(filename));
    return;
  }

  ConfigMergeDialog mergeDialog;
  if (QDialog::Accepted != mergeDialog.exec())
    return;

  logDebug() << "Merging codeplugs ...";
  if (! ConfigMerge::mergeInto(_config, &merging, mergeDialog.itemStrategy(),
                               mergeDialog.setStrategy(), err)) {
    QMessageBox::critical(nullptr, tr("Cannot import codeplug"),
                          tr("Cannot import codeplug from '%1': %2")
                          .arg(filename).arg(err.format()));
    return;
  }
}


Radio *
Application::autoDetect(const ErrorStack &err) {
  Settings settings;
  // If the last detected device is still valid
  //  -> skip interface detection and selection
  if ((! _lastDevice.isValid()) || settings.disableAutoDetect()) {
    logDebug() << "Last device is invalid, search for new one.";
    // First get all devices that are known by VID/PID
    QList<USBDeviceDescriptor> interfaces = USBDeviceDescriptor::detect();
    if (interfaces.isEmpty()) {
      logDebug() << "No save device found, continue searching for unsave ones.";
      interfaces = USBDeviceDescriptor::detect(false);
    }
    if (interfaces.isEmpty()) {
      errMsg(err) << tr("No matching devices found.");
      return nullptr;
    } else if ((1 != interfaces.count()) || (! interfaces.first().isSave()) || settings.disableAutoDetect()) {
      // More than one device found, or device not save -> select by user
      DeviceSelectionDialog dialog(interfaces);
      if (QDialog::Accepted != dialog.exec()) {
        return nullptr;
      }
      _lastDevice = dialog.device();
    } else {
      _lastDevice = interfaces.first();
    }
  }

  // Check if device supports identification
  RadioInfo radioInfo;
  if ((! _lastDevice.isSave()) || settings.disableAutoDetect()){
    RadioSelectionDialog dialog(_lastDevice);
    if (QDialog::Accepted != dialog.exec()) {
      return nullptr;
    }
    radioInfo = dialog.radioInfo();
  }

  Radio *radio = Radio::detect(_lastDevice, radioInfo, err);
  if (nullptr == radio) {
    QMessageBox::critical(nullptr, tr("Cannot connect to radio"),
                          tr("Cannot connect to radio: %1").arg(err.format()));
    return nullptr;
  }

  return radio;
}

void
Application::detectRadio() {
  if (Radio *radio = autoDetect()) {
    QMessageBox::information(nullptr, tr("Radio found"), tr("Found device '%1'.").arg(radio->name()));
    delete radio;
  } else {
    QMessageBox::information(nullptr, tr("No radio found"),
                             tr("No matching device was found."));
  }
}


bool
Application::verifyCodeplug(Radio *radio, bool showSuccess) {
  Radio *myRadio = radio;

  // If no radio is given -> try to detect the radio
  if (nullptr == myRadio)
    myRadio = autoDetect();

  if (nullptr == myRadio) {
    if (showSuccess) {
      QMessageBox::warning(nullptr, tr("No radio found"),
                           tr("No matching device was found."));
    }
    return false;
  }

  ErrorStack err;
  Config *intermediate = myRadio->codeplug().preprocess(_config, err);
  if (nullptr == intermediate) {
    ErrorMessageView(err).exec();
    return false;
  }

  Settings settings;
  RadioLimitContext ctx(settings.ignoreFrequencyLimits());
  myRadio->limits().verifyConfig(intermediate, ctx);

  bool verified = true;
  if ( (settings.ignoreVerificationWarning() && (ctx.maxSeverity()>RadioLimitIssue::Warning)) ||
       ((!settings.ignoreVerificationWarning()) && (ctx.maxSeverity()>=RadioLimitIssue::Warning)) ) {
    VerifyDialog dialog(ctx, (nullptr != radio));
    if (QDialog::Accepted != dialog.exec())
      verified = false;
  } else if (showSuccess) {
    QMessageBox::information(
          nullptr, tr("Verification success"),
          tr("The codeplug was successfully verified with the radio '%1'").arg(myRadio->name()));
  }

  // Delete intermediate representation
  delete intermediate;

  // If no radio was given -> close connection to radio again
  if (nullptr == radio)
    myRadio->deleteLater();

  return verified;
}


void
Application::downloadCodeplug() {
  if (! _mainWindow)
    return;

  if (_config->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(nullptr, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  Radio *radio = autoDetect();
  if (nullptr == radio) {
    QMessageBox::warning(nullptr, tr("No radio found"),
                         tr("No matching device was found."));
    return;
  }

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0); progress->setMaximum(100); progress->setVisible(true);
  connect(radio, SIGNAL(downloadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(downloadError(Radio *)), this, SLOT(onCodeplugDownloadError(Radio *)));
  connect(radio, SIGNAL(downloadFinished(Radio *, Codeplug *)), this, SLOT(onCodeplugDownloaded(Radio *, Codeplug *)));

  TransferFlags flags; flags.setBlocking(false);

  ErrorStack err;
  if (radio->startDownload(flags, err)) {
    _mainWindow->statusBar()->showMessage(tr("Read ..."));
    _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).exec();
    progress->setVisible(false);
  }
}

void
Application::onCodeplugDownloadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Read error"));
  ErrorMessageView(radio->errorStack()).exec();
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();

  _mainWindow->setWindowModified(false);
}


void
Application::onCodeplugDownloaded(Radio *radio, Codeplug *codeplug) {
  _config->clear();
  _mainWindow->setWindowModified(false);

  ErrorStack err;
  if (codeplug->decode(_config, err)) {
    _mainWindow->statusBar()->showMessage(tr("Read complete"));
    _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
    _config->setModified(false);
  } else {
    ErrorMessageView(err).exec();
    _config->clear();
  }

  if (! codeplug->postprocess(_config, err)) {
    ErrorMessageView(err).exec();
    _config->clear();
  }

  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();
}

void
Application::uploadCodeplug() {
  // Start upload
  Settings settings;

  Radio *radio = autoDetect();
  if (nullptr == radio) {
    QMessageBox::warning(nullptr, tr("No radio found"),
                         tr("No matching device was found."));
    return;
  }

  if (! verifyCodeplug(radio, false)) {
    radio->deleteLater();
    return;
  }

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0);
  progress->setMaximum(100);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));

  ErrorStack err;
  Config *intermediate = radio->codeplug().preprocess(_config, err);
  if (nullptr == intermediate) {
    ErrorMessageView(err).exec();
    progress->setVisible(false);
    return;
  }

  Codeplug::Flags flags = settings.codePlugFlags();
  flags.setBlocking(false);

  if (radio->startUpload(intermediate, flags, err)) {
     _mainWindow->statusBar()->showMessage(tr("Upload ..."));
     _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).exec();
    progress->setVisible(false);
  }
}

void
Application::uploadCallsignDB() {
  // Start upload
  Radio *radio = autoDetect();
  if (nullptr == radio) {
    QMessageBox::warning(nullptr, tr("No radio found"),
                         tr("No matching device was found."));
    return;
  }

  if (! radio->limits().hasCallSignDB()) {
    logDebug() << "Radio " << radio->name() << " does not support call-sign DB.";
    QMessageBox::information(nullptr, tr("Cannot write call-sign DB."),
                             tr("The detected radio '%1' does not support "
                                "a call-sign DB.")
                             .arg(radio->name()));
    radio->deleteLater();
    return;
  }
  if (! radio->limits().callSignDBImplemented()) {
    logDebug() << "Radio " << radio->name()
               << " does support call-sign DB but it is not implemented yet.";
    QMessageBox::critical(nullptr, tr("Cannot write call-sign DB."),
                          tr("The detected radio '%1' does support a call-sign DB. "
                             "This feature, however, is not implemented yet.").arg(radio->name()));
    radio->deleteLater();
    return;
  }

  // Sort call-sign DB w.r.t. the current DMR ID in _config
  // this is part of the "auto-selection" of calls-signs for upload
  Settings settings;
  if (settings.selectUsingUserDMRID()) {
    if (nullptr == _config->settings()->defaultId()) {
      QMessageBox::critical(nullptr, tr("Cannot write call-sign DB."),
                            tr("QDMR selects the call-signs to be written based on the default DMR "
                               "ID of the radio. No default ID set."));
      radio->deleteLater();
      return;
    }
    // Sort w.r.t users DMR ID
    unsigned id = _config->settings()->defaultId()->number();
    logDebug() << "Sort call-signs closest to ID=" << id << ".";
    _users->sortUsers(id);
  } else {
    // sort w.r.t. chosen prefixes
    QSet<unsigned> ids=settings.callSignDBPrefixes(); QStringList prefs;
    foreach (unsigned pref, ids)
      prefs.append(QString::number(pref));
    logDebug() << "Sort call-signs closest to IDs={" << prefs.join(", ") << "}.";
    _users->sortUsers(ids);
  }

  // Assemble flags for callsign DB encoding
  CallsignDB::Flags css;
  css.setUpdateDeviceClock(settings.updateDeviceClock());
  if (settings.limitCallSignDBEntries()) {
    logDebug() << "Limit callsign DB entries to " << settings.maxCallSignDBEntries() << ".";
    css.setCountLimit(settings.maxCallSignDBEntries());
  }

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setRange(0, 100); progress->setValue(0);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));

  ErrorStack err;
  css.setBlocking(false);
  if (radio->startUploadCallsignDB(_users, css, err)) {
    logDebug() << "Start call-sign DB write...";
    _mainWindow->statusBar()->showMessage(tr("Write call-sign DB ..."));
    _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).exec();
    progress->setVisible(false);
  }
}


void
Application::uploadSatellites() {
  // Start upload satellites
  Radio *radio = autoDetect();
  if (nullptr == radio) {
    QMessageBox::warning(nullptr, tr("No radio found"),
                         tr("No matching device was found."));
    return;
  }

  if (! radio->limits().hasSatelliteConfig()) {
    logDebug() << "Radio " << radio->name() << " does not support satellite tracking.";
    QMessageBox::information(nullptr, tr("Cannot write satellite config."),
                             tr("The detected radio '%1' does not support satellite tracking.")
                             .arg(radio->name()));
    radio->deleteLater();
    return;
  }

  if (! radio->limits().satelliteConfigImplemented()) {
    logDebug() << "Radio " << radio->name()
               << " does support satellite tracking but it is not implemented yet.";
    QMessageBox::critical(nullptr, tr("Cannot write satellite config."),
                          tr("The detected radio '%1' does support satellite tracking. "
                             "This feature, however, is not implemented yet.").arg(radio->name()));
    radio->deleteLater();
    return;
  }

  Settings settings;
  TransferFlags flags;
  flags.setUpdateDeviceClock(settings.updateDeviceClock());

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setRange(0, 100); progress->setValue(0);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));

  ErrorStack err;  
  flags.setBlocking(false);
  if (radio->startUploadSatelliteConfig(_satellites, flags, err)) {
    logDebug() << "Start satellite config write...";
    _mainWindow->statusBar()->showMessage(tr("Write satellite config ..."));
    _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).exec();
    progress->setVisible(false);
  }
}

void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Write error"));
  ErrorMessageView(radio->errorStack()).exec();
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();
}


void
Application::onCodeplugUploaded(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Write complete"));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  logDebug() << "Write complete.";

  if (radio->wait(250))
    radio->deleteLater();
}


void
Application::showSettings() {
  SettingsDialog dialog;
  if (QDialog::Accepted != dialog.exec())
    return;


  Settings settings;
  // Handle positioning
  if (! settings.queryPosition()) {
    if (_source)
      _source->stopUpdates();
    _currentPosition = settings.position();
  } else {
    if (_source)
      _source->startUpdates();
  }

  _mainWindow->applySettings();
}


void
Application::showAbout() {

  QUiLoader loader;
  QFile uiFile("://ui/aboutdialog.ui");
  uiFile.open(QIODevice::ReadOnly);
  auto obj = loader.load(&uiFile);
  if (nullptr == obj) return;
  QDialog *dialog = qobject_cast<QDialog *>(obj);
  if (nullptr == dialog) return;

  QTextEdit *text = dialog->findChild<QTextEdit *>("textEdit");
  text->setHtml(text->toHtml().arg(VERSION_STRING));

  QTreeWidget *radioTab = dialog->findChild<QTreeWidget *>("radioTable");
  radioTab->setColumnCount(1);

  QHash<QString, QTreeWidgetItem*> items;
  foreach (RadioInfo radio, RadioInfo::allRadios(false)) {
    if (! items.contains(radio.manufacturer()))
      items.insert(radio.manufacturer(),
                   new QTreeWidgetItem(QStringList(radio.manufacturer())));
    items[radio.manufacturer()]->addChild(
          new QTreeWidgetItem(QStringList(radio.name())));
    foreach (RadioInfo alias, radio.alias()) {
      if (! items.contains(alias.manufacturer()))
        items.insert(alias.manufacturer(),
                     new QTreeWidgetItem(QStringList(alias.manufacturer())));
      items[alias.manufacturer()]->addChild(
            new QTreeWidgetItem(QStringList(tr("%1 (alias for %2 %3)").arg(alias.name())
                                            .arg(radio.manufacturer()).arg(radio.name()))));
    }
  }
  radioTab->insertTopLevelItems(0, items.values());
  radioTab->sortByColumn(0, Qt::AscendingOrder);

  connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
  dialog->open();
}

void
Application::showHelp() {
  QDesktopServices::openUrl(QUrl("https://dm3mat.darc.de/qdmr/manual"));
}


void
Application::editSatellites() {
  SatelliteDatabaseDialog dialog(_satellites);
  if (QDialog::Accepted == dialog.exec()) {
    _satellites->save();
  } else {
    _satellites->load();
  }
}


void
Application::onConfigModifed() {
  if (! _mainWindow)
    return;

  _mainWindow->setWindowModified(true);
}

void
Application::positionUpdated(const QGeoPositionInfo &info) {
  if (info.isValid())
    _currentPosition = info.coordinate();
}

bool
Application::hasPosition() const {
  return _currentPosition.isValid();
}

QGeoCoordinate
Application::position() const {
  return _currentPosition;
}

void
Application::onPaletteChanged(const QPalette &palette) {
  // Set theme based on UI mode (light vs. dark).
  if (isDarkMode(palette)) {
    QIcon::setThemeName("dark");
    logDebug() << "Set icon theme to 'dark'.";
  } else {
    QIcon::setThemeName("light");
    logDebug() << "Set icon theme to 'light'.";
  }
}


