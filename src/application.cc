#include "application.hh"
#include <QMainWindow>
#include <QtUiTools>
#include <QDesktopServices>

#include "logger.hh"
#include "radio.hh"
#include "codeplug.hh"
#include "config.h"
#include "settings.hh"
#include "verifydialog.hh"
#include "analogchanneldialog.hh"
#include "digitalchanneldialog.hh"
#include "contactdialog.hh"
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
#include "configreader.hh"
#include "generalsettingsview.hh"
#include "radioidlistview.hh"
#include "contactlistview.hh"
#include "grouplistsview.hh"
#include "channellistview.hh"
#include "zonelistview.hh"
#include "scanlistsview.hh"
#include "positioningsystemlistview.hh"
#include "roamingzonelistview.hh"


Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _config(nullptr), _mainWindow(nullptr), _repeater(nullptr)
{
  setApplicationName("qdmr");
  setOrganizationName("DM3MAT");
  setOrganizationDomain("hmatuschek.github.io");

  // open logfile
  QString logdir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  Logger::get().addHandler(new FileLogHandler(logdir+"/qdmr.log"));

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
      ConfigReader reader;
      if (! reader.read(_config, argv[1])) {
        logError() << "Cannot read yaml codeplug file '" << argv[1]
                   << "': " << reader.errorMessage();
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
  connect(_config, SIGNAL(modified(ConfigObject*)), this, SLOT(onConfigModifed()));
}

Application::~Application() {
  if (_mainWindow)
    delete _mainWindow;
  _mainWindow = nullptr;
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

  if (! settings.showCommercialFeatures()) {
    tabs->removeTab(tabs->indexOf(_radioIdTab));
    _radioIdTab->setHidden(true);
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

  _config->reset();
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
        tr("Codeplug Files (*.yaml *.conf *.csv *.txt);;All Files (*)"));
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

  QString errorMessage;
  if ("yaml" == info.suffix()){
    ConfigReader reader;
    if (reader.read(_config, filename)) {
      _mainWindow->setWindowModified(false);
    } else {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(reader.errorMessage()));
      _config->reset();
    }
  } else {
    QTextStream stream(&file);
    if (_config->readCSV(stream, errorMessage)) {
      _mainWindow->setWindowModified(false);
    } else {
      QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                            tr("Cannot read codeplug from file '%1': %2")
                            .arg(filename).arg(errorMessage));
      _config->reset();
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
        tr("Codeplug Files (*.yaml);;Codeplug Files (*.conf *.csv *.txt)"));

  if (filename.isEmpty())
    return;

  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  QTextStream stream(&file);

  // check for file suffix
  QFileInfo info(filename);
  if (("conf" == info.suffix()) || ("csv" == info.suffix())) {
    QString errorMessage;
    if (_config->writeCSV(stream, errorMessage))
      _mainWindow->setWindowModified(false);
    else
      QMessageBox::critical(nullptr, tr("Cannot save codeplug"),
                            tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(errorMessage));
  } else if ("yaml" == info.suffix()) {
    if (_config->toYAML(stream)) {
      _mainWindow->setWindowModified(false);
    } else {
      QMessageBox::critical(nullptr, tr("Cannot save codeplug"),
                            tr("Cannot save codeplug to file '%1'.").arg(filename));
    }
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


void
Application::detectRadio() {
  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (radio) {
    QMessageBox::information(nullptr, tr("Radio found"), tr("Found device '%1'.").arg(radio->name()));
    radio->deleteLater();
  } else {
    QMessageBox::information(nullptr, tr("No Radio found."),
                             tr("No known radio detected. Check connection?\nError: %1").arg(errorMessage));
  }
  radio->deleteLater();
}


bool
Application::verifyCodeplug(Radio *radio, bool showSuccess, const VerifyFlags &flags) {
  Radio *myRadio = radio;
  QString errorMessage;

  // If no radio is given -> try to detect the radio
  if (nullptr == myRadio)
    myRadio = Radio::detect(errorMessage);
  if (nullptr == myRadio) {
    QMessageBox::information(nullptr, tr("No Radio found."),
                             tr("Cannot verify codeplug: No known radio detected.\nError: ")
                             + errorMessage);
    return false;
  }

  bool verified = true;
  QList<VerifyIssue> issues;
  VerifyIssue::Type maxIssue = myRadio->verifyConfig(_config, issues, flags);
  if ( (flags.ignoreWarnings && (maxIssue>VerifyIssue::WARNING)) ||
       ((!flags.ignoreWarnings) && (maxIssue>=VerifyIssue::WARNING)) ) {
    VerifyDialog dialog(issues);
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

  QString errorMessage;
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    QMessageBox::critical(nullptr, tr("No Radio found."),
                          tr("Can not download codeplug from device: No radio found.\nError: ")
                          + errorMessage);
    return;
  }

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0); progress->setMaximum(100); progress->setVisible(true);
  connect(radio, SIGNAL(downloadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(downloadError(Radio *)), this, SLOT(onCodeplugDownloadError(Radio *)));
  connect(radio, SIGNAL(downloadFinished(Radio *, CodePlug *)), this, SLOT(onCodeplugDownloaded(Radio *, CodePlug *)));
  if (radio->startDownload(false)) {
    _mainWindow->statusBar()->showMessage(tr("Download ..."));
    _mainWindow->setEnabled(false);
  } else {
    QMessageBox::critical(nullptr, tr("Cannot download codeplug."),
                          tr("Cannot download codeplug: %1").arg(radio->errorMessage()));
    progress->setVisible(false);
  }
}

void
Application::onCodeplugDownloadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Download error"));
  QMessageBox::critical(0, tr("Download error"),
                        tr("Cannot download codeplug from device. "
                           "An error occurred during download: %1").arg(radio->errorMessage()));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();

  _mainWindow->setWindowModified(false);
}


void
Application::onCodeplugDownloaded(Radio *radio, CodePlug *codeplug) {
  _config->reset();
  _mainWindow->setWindowModified(false);
  if (codeplug->decode(_config)) {
    _mainWindow->statusBar()->showMessage(tr("Download complete"));
    _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);

    _config->setModified(false);
  } else {
    QMessageBox::critical(
          nullptr, tr("Cannot decode code-plug"),
          tr("Cannot decode code-plug: %2").arg(codeplug->errorMessage()));
  }
  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();
}

void
Application::uploadCodeplug() {
  // Start upload
  Settings settings;
  QString errorMessage;

  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    QMessageBox::critical(nullptr, tr("No Radio found."),
                          tr("Can not upload codeplug to device: No radio found.\nError: ")
                          + errorMessage);
    return;
  }

  // Verify codeplug against the detected radio before uploading,
  // but do not show a message on success.
  VerifyFlags flags = {
    settings.ignoreVerificationWarning(),
    settings.ignoreFrequencyLimits()
  };

  if (! verifyCodeplug(radio, false, flags))
    return;

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0);
  progress->setMaximum(100);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));

  if (radio->startUpload(_config, false, settings.codePlugFlags())) {
     _mainWindow->statusBar()->showMessage(tr("Upload ..."));
     _mainWindow->setEnabled(false);
  } else {
    QMessageBox::critical(nullptr, tr("Cannot upload codeplug."),
                          tr("Cannot upload codeplug: %1").arg(radio->errorMessage()));
    progress->setVisible(false);
  }
}

void
Application::uploadCallsignDB() {
  // Start upload
  QString errorMessage;

  logDebug() << "Detect radio...";
  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    logDebug() << "No matching radio found.";
    QMessageBox::critical(nullptr, tr("No Radio found."),
                          tr("Can not upload call-sign DB to device: No radio found.\nError: ")
                          + errorMessage);
    return;
  }
  logDebug() << "Found radio " << radio->name() << ".";

  if (! radio->features().hasCallsignDB) {
    logDebug() << "Radio " << radio->name() << " does not support call-sign DB.";
    QMessageBox::information(nullptr, tr("Cannot upload call-sign DB."),
                             tr("The detected radio '%1' does not support "
                                "the upload of a call-sign DB.")
                             .arg(radio->name()));
    radio->deleteLater();
    return;
  }
  if (! radio->features().callsignDBImplemented) {
    logDebug() << "Radio " << radio->name()
               << " does support call-sign DB but it is not implemented yet.";
    QMessageBox::critical(nullptr, tr("Cannot upload call-sign DB."),
                          tr("The detected radio '%1' does support the upload of a call-sign DB. "
                             "This feature, however, is not implemented yet.").arg(radio->name()));
    radio->deleteLater();
    return;
  }

  // Sort call-sign DB w.r.t. the current DMR ID in _config
  // this is part of the "auto-selection" of calls-signs for upload
  Settings settings;
  if (settings.selectUsingUserDMRID()) {
    // Sort w.r.t users DMR ID
    uint id = _config->radioIDs()->defaultId()->number();
    logDebug() << "Sort call-signs closest to ID=" << id << ".";
    _users->sortUsers(id);
  } else {
    // sort w.r.t. chosen prefixes
    QSet<uint> ids=settings.callSignDBPrefixes(); QStringList prefs;
    foreach (uint pref, ids)
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

  if (radio->startUploadCallsignDB(_users, false, css)) {
    logDebug() << "Start call-sign DB upload...";
    _mainWindow->statusBar()->showMessage(tr("Upload call-sign DB ..."));
    _mainWindow->setEnabled(false);
  } else {
    logDebug() << "Cannot start call-sign DB upload.";
    QMessageBox::critical(nullptr, tr("Cannot upload call-sign DB."),
                          tr("Cannot upload call-sign DB: %1").arg(radio->errorMessage()));
    progress->setVisible(false);
  }
}


void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload error"));
  QMessageBox::critical(
        nullptr, tr("Upload error"),
        tr("Cannot upload codeplug or call-sign DB to device. "
           "An error occurred during upload: %1").arg(radio->errorMessage()));
  logError() << "Upload error: " << radio->errorMessage() << ".";
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  if (radio->wait(250))
    radio->deleteLater();
}


void
Application::onCodeplugUploaded(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload complete"));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  logError() << "Upload complete.";

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
        QWidget *genSet = _mainWindow->findChild<QWidget*>("GeneralSettingsView");
        tabs->insertTab(tabs->indexOf(genSet)+1, _radioIdTab, tr("Radio IDs"));
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



