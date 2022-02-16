#include "application.hh"
#include <QMainWindow>
#include <QtUiTools>
#include <QDesktopServices>

#include "logger.hh"
#include "radio.hh"
#include "codeplug.hh"
#include "config.h"
#include "settings.hh"
#include "radiolimits.hh"
#include "verifydialog.hh"
#include "analogchanneldialog.hh"
#include "digitalchanneldialog.hh"
#include "rxgrouplistdialog.hh"
#include "zonedialog.hh"
#include "scanlistdialog.hh"
#include "gpssystemdialog.hh"
#include "roamingzonedialog.hh"
#include "aprssystemdialog.hh"
#include "repeaterdatabase.hh"
#include "userdatabase.hh"
#include "talkgroupdatabase.hh"
#include "searchpopup.hh"
#include "contactselectiondialog.hh"
#include "configitemwrapper.hh"
#include "generalsettingsview.hh"
#include "radioidlistview.hh"
#include "contactlistview.hh"
#include "grouplistsview.hh"
#include "channellistview.hh"
#include "zonelistview.hh"
#include "scanlistsview.hh"
#include "positioningsystemlistview.hh"
#include "roamingzonelistview.hh"
#include "errormessageview.hh"
#include "extensionview.hh"
#include "deviceselectiondialog.hh"
#include "radioselectiondialog.hh"


Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _config(nullptr), _mainWindow(nullptr), _repeater(nullptr),
    _lastDevice()
{
  setApplicationName("qdmr");
  setOrganizationName("DM3MAT");
  setOrganizationDomain("hmatuschek.github.io");

  // open logfile
  QString logdir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  Logger::get().addHandler(new FileLogHandler(logdir+"/qdmr.log"));

  // Register icon themes
  QStringList iconPaths = QIcon::themeSearchPaths();
  iconPaths.prepend(":/icons");
  QIcon::setThemeSearchPaths(iconPaths);
  onPaletteChanged(palette());

  Settings settings;
  _repeater   = new RepeaterDatabase(settings.position(), 7, this);
  _users      = new UserDatabase(30, this);
  _talkgroups = new TalkGroupDatabase(30, this);
  _config = new Config(this);

  if (argc>1) {
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
      bg_hsv_value = palette.color(QPalette::Background).value();
  return text_hsv_value > bg_hsv_value;
}


QMainWindow *
Application::mainWindow() {
  if (0 == _mainWindow)
    return createMainWindow();

  return _mainWindow;
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


QMainWindow *
Application::createMainWindow() {
  if (_mainWindow)
    return _mainWindow;

  Settings settings;
  logDebug() << "Create main window using icon theme '" << QIcon::themeName() << "'.";

  QUiLoader loader;
  QFile uiFile("://ui/mainwindow.ui");
  uiFile.open(QIODevice::ReadOnly);

  QWidget *window = loader.load(&uiFile);
  _mainWindow = qobject_cast<QMainWindow *>(window);

  QProgressBar *progress = new QProgressBar();
  progress->setObjectName("progress");
  _mainWindow->statusBar()->addPermanentWidget(progress);
  progress->setVisible(false);

  QAction *newCP   = _mainWindow->findChild<QAction*>("actionNewCodeplug");
  QAction *loadCP  = _mainWindow->findChild<QAction*>("actionOpenCodeplug");
  QAction *saveCP  = _mainWindow->findChild<QAction*>("actionSaveCodeplug");

  QAction *findDev = _mainWindow->findChild<QAction*>("actionDetectDevice");
  QAction *verCP   = _mainWindow->findChild<QAction*>("actionVerifyCodeplug");
  QAction *downCP  = _mainWindow->findChild<QAction*>("actionDownload");
  QAction *upCP    = _mainWindow->findChild<QAction*>("actionUpload");
  QAction *upCDB   = _mainWindow->findChild<QAction*>("actionUploadCallsignDB");

  QAction *refreshCallsignDB  = _mainWindow->findChild<QAction*>("actionRefreshCallsignDB");
  QAction *refreshTalkgroupDB  = _mainWindow->findChild<QAction*>("actionRefreshTalkgroupDB");
  QAction *refreshRepeaterDB  = _mainWindow->findChild<QAction*>("actionRefreshRepeaterDB");

  QAction *about   = _mainWindow->findChild<QAction*>("actionAbout");
  QAction *sett    = _mainWindow->findChild<QAction*>("actionSettings");
  QAction *help    = _mainWindow->findChild<QAction*>("actionHelp");
  QAction *quit    = _mainWindow->findChild<QAction*>("actionQuit");

  connect(newCP, SIGNAL(triggered()), this, SLOT(newCodeplug()));
  connect(loadCP, SIGNAL(triggered()), this, SLOT(loadCodeplug()));
  connect(saveCP, SIGNAL(triggered()), this, SLOT(saveCodeplug()));
  connect(quit, SIGNAL(triggered()), this, SLOT(quitApplication()));
  connect(about, SIGNAL(triggered()), this, SLOT(showAbout()));
  connect(sett, SIGNAL(triggered()), this, SLOT(showSettings()));
  connect(help, SIGNAL(triggered()), this, SLOT(showHelp()));

  connect(refreshCallsignDB, SIGNAL(triggered()), _users, SLOT(download()));
  connect(refreshTalkgroupDB, SIGNAL(triggered()), _talkgroups, SLOT(download()));
  connect(refreshRepeaterDB, SIGNAL(triggered()), _repeater, SLOT(download()));

  connect(findDev, SIGNAL(triggered()), this, SLOT(detectRadio()));
  connect(verCP, SIGNAL(triggered()), this, SLOT(verifyCodeplug()));
  connect(downCP, SIGNAL(triggered()), this, SLOT(downloadCodeplug()));
  connect(upCP, SIGNAL(triggered()), this, SLOT(uploadCodeplug()));
  connect(upCDB, SIGNAL(triggered()), this, SLOT(uploadCallsignDB()));

  QTabWidget *tabs = _mainWindow->findChild<QTabWidget*>("tabs");

  // Wire-up "General Settings" view
  _generalSettings = new GeneralSettingsView(_config);
  tabs->addTab(_generalSettings, tr("Settings"));
  if (settings.showCommercialFeatures()) {
    _generalSettings->hideDMRID(true);
  } else {
    _generalSettings->hideDMRID(false);
  }
  if (settings.showExtensions()) {
    _generalSettings->hideExtensions(false);
  } else {
    _generalSettings->hideExtensions(true);
  }

  // Wire-up "Radio IDs" view
  _radioIdTab = new RadioIDListView(_config);
  tabs->addTab(_radioIdTab, tr("Radio IDs"));

  // Wire-up "Contact List" view
  _contactList = new ContactListView(_config);
  tabs->addTab(_contactList, tr("Contacts"));

  // Wire-up "RX Group List" view
  _groupLists = new GroupListsView(_config);
  tabs->addTab(_groupLists, tr("Group Lists"));

  // Wire-up "Channel List" view
  _channelList = new ChannelListView(_config);
  tabs->addTab(_channelList, tr("Channels"));

  // Wire-up "Zone List" view
  _zoneList = new ZoneListView(_config);
  tabs->addTab(_zoneList, tr("Zones"));

  // Wire-up "Scan List" view
  _scanLists = new ScanListsView(_config);
  tabs->addTab(_scanLists, tr("Scan Lists"));

  // Wire-up "GPS System List" view
  _posSysList = new PositioningSystemListView(_config);
  tabs->addTab(_posSysList, tr("GPS/APRS"));

  // Wire-up "Roaming Zone List" view
  _roamingZoneList = new RoamingZoneListView(_config);
  tabs->addTab(_roamingZoneList, tr("Roaming"));

  // Wire-up "extension view"
  _extensionView = new ExtensionView();
  _extensionView->setObject(_config, _config);
  tabs->addTab(_extensionView, tr("Extensions"));

  if (! settings.showCommercialFeatures()) {
    tabs->removeTab(tabs->indexOf(_radioIdTab));
    _radioIdTab->setHidden(true);
  }
  if (! settings.showExtensions()) {
    tabs->removeTab(tabs->indexOf(_extensionView));
    _extensionView->setHidden(true);
  }

  _mainWindow->restoreGeometry(settings.mainWindowState());
  return _mainWindow;
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
    if (_config->readYAML(filename, err)) {
      _mainWindow->setWindowModified(false);
    } else {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(err.format()));
      _config->clear();
    }
  } else {
    QString errorMessage;
    QTextStream stream(&file);
    if (_config->readCSV(stream, errorMessage)) {
      _mainWindow->setWindowModified(false);
    } else {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(errorMessage));
      _config->clear();
    }
  }
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
                          tr("Saveing in the old table-based conf format was disabled with 0.9.0. "
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
    _mainWindow->setWindowModified(false);
  } else {
    QMessageBox::critical(nullptr, tr("Cannot save codeplug"),
                          tr("Cannot save codeplug to file '%1'.").arg(filename));
  }

  file.flush();
  file.close();

  settings.setLastDirectoryDir(info.absoluteDir());
}


void
Application::quitApplication() {
  if (_config->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(nullptr, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  Settings settings;
  if (_mainWindow)
    settings.setMainWindowState(_mainWindow->saveGeometry());

  quit();
}


Radio *
Application::autoDetect(const ErrorStack &err) {
  // If the last detected device is still valid
  //  -> skip interface detection and selection
  if (! _lastDevice.isValid()) {
    logDebug() << "Last device is invalid, search for new one.";
    // First get all devices that are known by VID/PID
    QList<USBDeviceDescriptor> interfaces = USBDeviceDescriptor::detect();
    if (interfaces.isEmpty()) {
      errMsg(err) << tr("No matching devices found.");
      return nullptr;
    } else if ((1 != interfaces.count()) || (! interfaces.first().isSave())) {
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
  if (! _lastDevice.isIdentifiable()) {
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
    radio->deleteLater();
  }
}


bool
Application::verifyCodeplug(Radio *radio, bool showSuccess) {
  Radio *myRadio = radio;

  // If no radio is given -> try to detect the radio
  if (nullptr == myRadio)
    myRadio = autoDetect();
  if (nullptr == myRadio)
    return false;

  Settings settings;
  RadioLimitContext ctx(settings.ignoreFrequencyLimits());
  myRadio->limits().verifyConfig(_config, ctx);
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
  if (nullptr == radio)
    return;

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0); progress->setMaximum(100); progress->setVisible(true);
  connect(radio, SIGNAL(downloadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(downloadError(Radio *)), this, SLOT(onCodeplugDownloadError(Radio *)));
  connect(radio, SIGNAL(downloadFinished(Radio *, Codeplug *)), this, SLOT(onCodeplugDownloaded(Radio *, Codeplug *)));

  ErrorStack err;
  if (radio->startDownload(false, err)) {
    _mainWindow->statusBar()->showMessage(tr("Read ..."));
    _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).show();
    progress->setVisible(false);
  }
}

void
Application::onCodeplugDownloadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Read error"));
  ErrorMessageView(radio->errorStack()).show();
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
    ErrorMessageView(err).show();
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
  if (nullptr == radio)
    return;

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
  if (radio->startUpload(_config, false, settings.codePlugFlags(), err)) {
     _mainWindow->statusBar()->showMessage(tr("Upload ..."));
     _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).show();
    progress->setVisible(false);
  }
}

void
Application::uploadCallsignDB() {
  // Start upload
  Radio *radio = autoDetect();
  if (nullptr == radio)
    return;

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
    if (nullptr == _config->radioIDs()->defaultId()) {
      QMessageBox::critical(nullptr, tr("Cannot write call-sign DB."),
                            tr("QDMR selects the call-signs to be written based on the default DMR "
                               "ID of the radio. No default ID set."));
      radio->deleteLater();
      return;
    }
    // Sort w.r.t users DMR ID
    unsigned id = _config->radioIDs()->defaultId()->number();
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
  CallsignDB::Selection css;
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
  if (radio->startUploadCallsignDB(_users, false, css, err)) {
    logDebug() << "Start call-sign DB write...";
    _mainWindow->statusBar()->showMessage(tr("Write call-sign DB ..."));
    _mainWindow->setEnabled(false);
  } else {
    ErrorMessageView(err).show();
    progress->setVisible(false);
  }
}


void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Write error"));
  ErrorMessageView(radio->errorStack()).show();
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

  logError() << "Write complete.";

  if (radio->wait(250))
    radio->deleteLater();
}


void
Application::showSettings() {
  SettingsDialog dialog;
  if (QDialog::Accepted == dialog.exec()) {
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
    // Handle commercial features
    QTabWidget *tabs = _mainWindow->findChild<QTabWidget*>("tabs");
    if (settings.showCommercialFeatures()) {
      if (-1 == tabs->indexOf(_radioIdTab)) {
        tabs->insertTab(tabs->indexOf(_generalSettings)+1, _radioIdTab, tr("Radio IDs"));
        _mainWindow->update();
      }
      _generalSettings->hideDMRID(true);
    } else if (! settings.showCommercialFeatures()) {
      if (-1 != tabs->indexOf(_radioIdTab)) {
        tabs->removeTab(tabs->indexOf(_radioIdTab));
        _mainWindow->update();
      }
      _generalSettings->hideDMRID(false);
    }
    // Handle extensions
    if (settings.showExtensions()) {
      if (-1 == tabs->indexOf(_extensionView)) {
        tabs->insertTab(tabs->indexOf(_roamingZoneList)+1, _extensionView, tr("Extensions"));
        _mainWindow->update();
      }
      _generalSettings->hideExtensions(false);
    } else {
      if (-1 != tabs->indexOf(_extensionView)) {
        tabs->removeTab(tabs->indexOf(_extensionView));
        _mainWindow->update();
      }
      _generalSettings->hideExtensions(true);
    }
  }
}

void
Application::showAbout() {
  QUiLoader loader;
  QFile uiFile("://ui/aboutdialog.ui");
  uiFile.open(QIODevice::ReadOnly);
  QDialog *dialog = qobject_cast<QDialog *>(loader.load(&uiFile));
  QTextEdit *text = dialog->findChild<QTextEdit *>("textEdit");
  text->setHtml(text->toHtml().arg(VERSION_STRING));

  QTreeWidget *radioTab = dialog->findChild<QTreeWidget *>("radioTable");
  radioTab->setColumnCount(1);
  QHash<QString, QTreeWidgetItem*> items;
  foreach (RadioInfo radio, RadioInfo::allRadios(false)) {
    if (! items.contains(radio.manufactuer()))
      items.insert(radio.manufactuer(),
                   new QTreeWidgetItem(QStringList(radio.manufactuer())));
    items[radio.manufactuer()]->addChild(
          new QTreeWidgetItem(QStringList(radio.name())));
    foreach (RadioInfo alias, radio.alias()) {
      if (! items.contains(alias.manufactuer()))
        items.insert(alias.manufactuer(),
                     new QTreeWidgetItem(QStringList(alias.manufactuer())));
      items[alias.manufactuer()]->addChild(
            new QTreeWidgetItem(QStringList(tr("%1 (alias for %2 %3)").arg(alias.name())
                                            .arg(radio.manufactuer()).arg(radio.name()))));
    }
  }
  radioTab->insertTopLevelItems(0, items.values());
  radioTab->sortByColumn(0,Qt::AscendingOrder);

  if (dialog) {
    dialog->exec();
    dialog->deleteLater();
  }
}

void
Application::showHelp() {
  QDesktopServices::openUrl(QUrl("https://dm3mat.darc.de/qdmr/manual"));
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


