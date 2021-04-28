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


QPair<int, int>
getSelectionRowRange(const QModelIndexList &indices) {
  int rmin=-1, rmax=-1;
  foreach (QModelIndex idx, indices) {
    if ((-1==rmin) || (rmin>idx.row()))
      rmin = idx.row();
    if ((-1==rmax) || (rmax<idx.row()))
      rmax = idx.row();
  }
  return QPair<int,int>(rmin, rmax);
}

QList<int>
getSelectionRows(const QModelIndexList &indices) {
  QList<int> rows;
  foreach (QModelIndex idx, indices) {
    if (rows.contains(idx.row()))
      continue;
    rows.append(idx.row());
  }
  return rows;
}


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
    QFile file(argv[1]);
    if (! file.open(QIODevice::ReadOnly)) {

      return;
    }
    QString errorMessage;
    QTextStream stream(&file);
    if (! _config->readCSV(stream, errorMessage))
      logError() << errorMessage;
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
  connect(_config, SIGNAL(modified()), this, SLOT(onConfigModifed()));
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

RepeaterDatabase *Application::repeater() const{
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

  connect(findDev, SIGNAL(triggered()), this, SLOT(detectRadio()));
  connect(verCP, SIGNAL(triggered()), this, SLOT(verifyCodeplug()));
  connect(downCP, SIGNAL(triggered()), this, SLOT(downloadCodeplug()));
  connect(upCP, SIGNAL(triggered()), this, SLOT(uploadCodeplug()));
  connect(upCDB, SIGNAL(triggered()), this, SLOT(uploadCallsignDB()));

  // Wire-up "General Settings" view
  QComboBox *dmrID  = _mainWindow->findChild<QComboBox*>("dmrID");
  QPushButton *addID = _mainWindow->findChild<QPushButton*>("addID");
  QPushButton *remID = _mainWindow->findChild<QPushButton*>("remID");
  QLineEdit *rname  = _mainWindow->findChild<QLineEdit*>("radioName");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");
  QSpinBox  *mic    = _mainWindow->findChild<QSpinBox *>("mic");
  QCheckBox *speech = _mainWindow->findChild<QCheckBox*>("speech");

  dmrID->setModel(_config->radioIDs());
  dmrID->setCurrentIndex(0);

  rname->setText(_config->name());
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());
  mic->setValue(_config->micLevel());
  speech->setChecked(_config->speech());

  connect(dmrID->lineEdit(), SIGNAL(editingFinished()), this, SLOT(onDMRIDChanged()));
  connect(dmrID, SIGNAL(currentIndexChanged(int)), this, SLOT(onDMRIDSelected(int)));
  connect(addID, SIGNAL(clicked(bool)), this, SLOT(onAddDMRID()));
  connect(remID, SIGNAL(clicked(bool)), this, SLOT(onRemDMRID()));
  connect(rname, SIGNAL(editingFinished()), this, SLOT(onNameChanged()));
  connect(intro1, SIGNAL(editingFinished()), this, SLOT(onIntroLine1Changed()));
  connect(intro2, SIGNAL(editingFinished()), this, SLOT(onIntroLine2Changed()));
  connect(mic, SIGNAL(valueChanged(int)), this, SLOT(onMicLevelChanged()));
  connect(speech, SIGNAL(toggled(bool)), this, SLOT(onSpeechChanged()));

  // Wire-up "Contact List" view
  QTableView *contacts = _mainWindow->findChild<QTableView *>("contactsView");
  SearchPopup::attach(contacts);
  QPushButton *cntUp   = _mainWindow->findChild<QPushButton *>("contactUp");
  QPushButton *cntDown = _mainWindow->findChild<QPushButton *>("contactDown");
  QPushButton *addCnt  = _mainWindow->findChild<QPushButton *>("addContact");
  QPushButton *remCnt  = _mainWindow->findChild<QPushButton *>("remContact");
  connect(contacts->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadContactListSectionState()));
  connect(contacts->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeContactListSectionState()));
  contacts->setModel(_config->contacts());
  connect(addCnt, SIGNAL(clicked()), this, SLOT(onAddContact()));
  connect(remCnt, SIGNAL(clicked()), this, SLOT(onRemContact()));
  connect(cntUp, SIGNAL(clicked()), this, SLOT(onContactUp()));
  connect(cntDown, SIGNAL(clicked()), this, SLOT(onContactDown()));
  connect(contacts, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditContact(const QModelIndex &)));

  // Wire-up "RX Group List" view
  QListView *rxgroups  = _mainWindow->findChild<QListView *>("groupListView");
  SearchPopup::attach(rxgroups);
  QPushButton *grpUp   = _mainWindow->findChild<QPushButton *>("rxGroupUp");
  QPushButton *grpDown = _mainWindow->findChild<QPushButton *>("rxGroupDown");
  QPushButton *addGrp  = _mainWindow->findChild<QPushButton *>("addRXGroup");
  QPushButton *remGrp  = _mainWindow->findChild<QPushButton *>("remRXGroup");
  rxgroups->setModel(_config->rxGroupLists());
  connect(addGrp, SIGNAL(clicked()), this, SLOT(onAddRxGroup()));
  connect(remGrp, SIGNAL(clicked()), this, SLOT(onRemRxGroup()));
  connect(grpUp, SIGNAL(clicked()), this, SLOT(onRxGroupUp()));
  connect(grpDown, SIGNAL(clicked()), this, SLOT(onRxGroupDown()));
  connect(rxgroups, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditRxGroup(const QModelIndex &)));

  // Wire-up "Channel List" view
  QTableView *channels = _mainWindow->findChild<QTableView *>("channelView");
  SearchPopup::attach(channels);
  QPushButton *chUp    = _mainWindow->findChild<QPushButton *>("channelUp");
  QPushButton *chDown  = _mainWindow->findChild<QPushButton *>("channelDown");
  QPushButton *addACh  = _mainWindow->findChild<QPushButton *>("addAnalogChannel");
  QPushButton *addDCh  = _mainWindow->findChild<QPushButton *>("addDigitalChannel");
  QPushButton *cloneCh = _mainWindow->findChild<QPushButton *>("cloneChannel");
  QPushButton *remCh   = _mainWindow->findChild<QPushButton *>("remChannel");
  connect(channels->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadChannelListSectionState()));
  connect(channels->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storeChannelListSectionState()));
  channels->setModel(_config->channelList());
  connect(addACh, SIGNAL(clicked()), this, SLOT(onAddAnalogChannel()));
  connect(addDCh, SIGNAL(clicked()), this, SLOT(onAddDigitalChannel()));
  connect(cloneCh, SIGNAL(clicked()), this, SLOT(onCloneChannel()));
  connect(remCh, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(chUp, SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(chDown, SIGNAL(clicked()), this, SLOT(onChannelDown()));
  connect(channels, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditChannel(const QModelIndex &)));

  // Wire-up "Zone List" view
  QListView *zones     = _mainWindow->findChild<QListView *>("zoneView");
  SearchPopup::attach(zones);
  QPushButton *zoneUp   = _mainWindow->findChild<QPushButton *>("zoneUp");
  QPushButton *zoneDown = _mainWindow->findChild<QPushButton *>("zoneDown");
  QPushButton *addZone  = _mainWindow->findChild<QPushButton *>("addZone");
  QPushButton *remZone  = _mainWindow->findChild<QPushButton *>("remZone");
  zones->setModel(_config->zones());
  connect(addZone, SIGNAL(clicked()), this, SLOT(onAddZone()));
  connect(remZone, SIGNAL(clicked()), this, SLOT(onRemZone()));
  connect(zoneUp, SIGNAL(clicked()), this, SLOT(onZoneUp()));
  connect(zoneDown, SIGNAL(clicked()), this, SLOT(onZoneDown()));
  connect(zones, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditZone(const QModelIndex &)));

  // Wire-up "Scan List" view
  QListView *scanLists = _mainWindow->findChild<QListView *>("scanListView");
  SearchPopup::attach(scanLists);
  QPushButton *slUp   = _mainWindow->findChild<QPushButton *>("scanListUp");
  QPushButton *slDown = _mainWindow->findChild<QPushButton *>("scanListDown");
  QPushButton *addSl  = _mainWindow->findChild<QPushButton *>("addScanList");
  QPushButton *remSl  = _mainWindow->findChild<QPushButton *>("remScanList");
  scanLists->setModel(_config->scanlists());
  connect(addSl, SIGNAL(clicked()), this, SLOT(onAddScanList()));
  connect(remSl, SIGNAL(clicked()), this, SLOT(onRemScanList()));
  connect(slUp, SIGNAL(clicked()), this, SLOT(onScanListUp()));
  connect(slDown, SIGNAL(clicked()), this, SLOT(onScanListDown()));
  connect(scanLists, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditScanList(const QModelIndex &)));

  // Wire-up "GPS System List" view
  QTableView *gpsList = _mainWindow->findChild<QTableView *>("gpsView");
  SearchPopup::attach(gpsList);
  QPushButton *gpsUp   = _mainWindow->findChild<QPushButton *>("gpsUp");
  QPushButton *gpsDown = _mainWindow->findChild<QPushButton *>("gpsDown");
  QPushButton *addGPS  = _mainWindow->findChild<QPushButton *>("addGPS");
  QPushButton *addAPRS = _mainWindow->findChild<QPushButton *>("addAPRS");
  QPushButton *remGPS  = _mainWindow->findChild<QPushButton *>("remGPS");
  QLabel *gpsNote = _mainWindow->findChild<QLabel*>("gpsNote");
  connect(gpsList->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)),
          this, SLOT(loadPositioningSectionState()));
  connect(gpsList->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(storePositioningSectionState()));
  gpsList->setModel(_config->posSystems());
  if (settings.hideGSPNote())
    gpsNote->setVisible(false);
  connect(addGPS, SIGNAL(clicked()), this, SLOT(onAddGPS()));
  connect(addAPRS, SIGNAL(clicked()), this, SLOT(onAddAPRS()));
  connect(remGPS, SIGNAL(clicked()), this, SLOT(onRemGPS()));
  connect(gpsUp, SIGNAL(clicked()), this, SLOT(onGPSUp()));
  connect(gpsDown, SIGNAL(clicked()), this, SLOT(onGPSDown()));
  connect(gpsList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditGPS(const QModelIndex &)));
  connect(gpsNote, SIGNAL(linkActivated(QString)), this, SLOT(onHideGPSNote()));

  // Wire-up "Roaming Zone List" view
  QListView *roamingZones      = _mainWindow->findChild<QListView *>("roamingZoneList");
  SearchPopup::attach(roamingZones);
  QPushButton *roamingZoneUp   = _mainWindow->findChild<QPushButton *>("roamingZoneUp");
  QPushButton *roamingZoneDown = _mainWindow->findChild<QPushButton *>("roamingZoneDown");
  QPushButton *addRoamingZone  = _mainWindow->findChild<QPushButton *>("addRoamingZone");
  QPushButton *genRoamingZone  = _mainWindow->findChild<QPushButton *>("genRoamingZone");
  QPushButton *remRoamingZone  = _mainWindow->findChild<QPushButton *>("remRoamingZone");
  QLabel *roamingNote          = _mainWindow->findChild<QLabel*>("roamingNote");
  roamingZones->setModel(_config->roaming());
  connect(addRoamingZone, SIGNAL(clicked()), this, SLOT(onAddRoamingZone()));
  connect(genRoamingZone, SIGNAL(clicked(bool)), this, SLOT(onGenRoamingZone()));
  connect(remRoamingZone, SIGNAL(clicked()), this, SLOT(onRemRoamingZone()));
  connect(roamingZoneUp, SIGNAL(clicked()), this, SLOT(onRoamingZoneUp()));
  connect(roamingZoneDown, SIGNAL(clicked()), this, SLOT(onRoamingZoneDown()));
  connect(roamingZones, SIGNAL(doubleClicked(const QModelIndex &)),
          this, SLOT(onEditRoamingZone(const QModelIndex &)));
  connect(roamingNote, SIGNAL(linkActivated(QString)), this, SLOT(onHideRoamingNote()));
  if (settings.hideRoamingNote())
    roamingNote->setVisible(false);

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
  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open codeplug"), settings.lastDirectory().absolutePath(),
                                                  tr("Codeplug Files (*.conf *.csv *.txt);;All Files (*)"));
  if (filename.isEmpty())
    return;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  logDebug() << "Load codeplug from '" << filename << "'.";
  QFileInfo info(filename);
  settings.setLastDirectoryDir(info.absoluteDir());

  QString errorMessage;
  QTextStream stream(&file);
  if (_config->readCSV(stream, errorMessage)) {
    _mainWindow->setWindowModified(false);
  } else {
    QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(errorMessage));
    _config->reset();
  }
}


void
Application::saveCodeplug() {
  if (! _mainWindow)
    return;

  Settings settings;
  QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save codeplug"), settings.lastDirectory().absolutePath(),
                                                  tr("Codeplug Files (*.conf *.csv *.txt)"));
  if (filename.isEmpty())
    return;

  // check for file suffix
  QFileInfo info(filename);
  if (("conf" != info.suffix()) && ("csv" != info.suffix()) && ("txt" != info.suffix()))
    filename = filename + ".conf";

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  QString errorMessage;
  QTextStream stream(&file);
  if (_config->writeCSV(stream, errorMessage))
    _mainWindow->setWindowModified(false);
  else
    QMessageBox::critical(nullptr, tr("Cannot save codeplug"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(errorMessage));

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
                             tr("No known radio detected. Check connection?\nError:")+errorMessage);
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
  VerifyIssue::Type maxIssue = myRadio->verifyConfig(_config, issues);
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
  radio->startDownload(false);
  _mainWindow->statusBar()->showMessage(tr("Download ..."));
  _mainWindow->setEnabled(false);
}

void
Application::onCodeplugDownloadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Download error"));
  QMessageBox::critical(0, tr("Download error"),
                        tr("Cannot download codeplug from device. "
                           "An error occurred during download: %1").arg(radio->errorMessage()));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

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
  radio->startUpload(_config, false, settings.codePlugFlags());

  _mainWindow->statusBar()->showMessage(tr("Upload ..."));
  _mainWindow->setEnabled(false);
}

void
Application::uploadCallsignDB() {
  // Start upload
  QString errorMessage;

  Radio *radio = Radio::detect(errorMessage);
  if (nullptr == radio) {
    QMessageBox::critical(nullptr, tr("No Radio found."),
                          tr("Can not upload call-sign DB to device: No radio found.\nError: ")
                          + errorMessage);
    return;
  }

  if (! radio->features().hasCallsignDB) {
    QMessageBox::information(nullptr, tr("Cannot upload call-sign DB."),
                             tr("The detected radio '%1' does not support "
                                "the upload of a call-sign DB.")
                             .arg(radio->name()));
    return;
  }
  if (! radio->features().callsignDBImplemented) {
    QMessageBox::critical(nullptr, tr("Cannot upload call-sign DB."),
                          tr("The detected radio '%1' does support the upload of a call-sign DB. "
                             "This feature, however, is not implemented yet.").arg(radio->name()));
    return;
  }

  // Sort call-sign DB w.r.t. the current DMR ID in _config
  // this is part of the "auto-selection" of calls-signs for upload
  _users->sortUsers(_config->radioIDs()->getId(0)->id());

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0);
  progress->setMaximum(100);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));
  radio->startUploadCallsignDB(_users, false);

  _mainWindow->statusBar()->showMessage(tr("Upload User DB ..."));
  _mainWindow->setEnabled(false);
}


void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload error"));
  QMessageBox::critical(
        nullptr, tr("Upload error"),
        tr("Cannot upload codeplug or user DB to device. "
           "An error occurred during upload: %1").arg(radio->errorMessage()));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  radio->deleteLater();
}


void
Application::onCodeplugUploaded(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload complete"));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  radio->deleteLater();
}


void
Application::showSettings() {
  SettingsDialog dialog;
  if (QDialog::Accepted == dialog.exec()) {
    Settings settings;
    if (! settings.queryPosition()) {
      if (_source)
        _source->stopUpdates();
      _currentPosition = settings.position();
    } else {
      if (_source)
        _source->startUpdates();
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

  QLineEdit *rname  = _mainWindow->findChild<QLineEdit*>("radioName");
  QComboBox *dmrID  = _mainWindow->findChild<QComboBox*>("dmrID");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");
  QSpinBox  *mic    = _mainWindow->findChild<QSpinBox *>("mic");
  QCheckBox *speech = _mainWindow->findChild<QCheckBox*>("speech");

  rname->setText(_config->name());
  dmrID->setCurrentIndex(0);
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());
  mic->setValue(_config->micLevel());
  speech->setChecked(_config->speech());

  _mainWindow->setWindowModified(true);
}

void
Application::onDMRIDChanged() {
  QComboBox *dmrID  = _mainWindow->findChild<QComboBox *>("dmrID");
  _config->radioIDs()->getId(0)->setId(dmrID->currentText().toUInt());
}

void
Application::onDMRIDSelected(int idx) {
  _config->radioIDs()->setDefault(idx);
}

void
Application::onAddDMRID() {
  int idx = _config->radioIDs()->addId(uint32_t(0));
  _config->radioIDs()->setDefault(idx);
  QComboBox *dmrID  = _mainWindow->findChild<QComboBox *>("dmrID");
  dmrID->setCurrentIndex(0);
}

void
Application::onRemDMRID() {
  if (1 == _config->radioIDs()->count()) {
    QMessageBox::information(
          nullptr, tr("Cannot remove last DMR ID"),
          tr("The codeplug needs at least one valid DMR ID."));
    return;
  }
  _config->radioIDs()->delId(_config->radioIDs()->getId(0));
}

void
Application::onNameChanged() {
  QLineEdit *rname = _mainWindow->findChild<QLineEdit*>("radioName");
  _config->setName(rname->text().simplified());
}

void
Application::onIntroLine1Changed() {
  QLineEdit *line  = _mainWindow->findChild<QLineEdit*>("introLine1");
  _config->setIntroLine1(line->text().simplified());
}

void
Application::onMicLevelChanged() {
  QSpinBox *mic = _mainWindow->findChild<QSpinBox *>("mic");
  _config->setMicLevel(mic->value());
}

void
Application::onSpeechChanged() {
  QCheckBox *speech = _mainWindow->findChild<QCheckBox *>("speech");
  _config->setSpeech(speech->isChecked());
}

void
Application::onIntroLine2Changed() {
  QLineEdit *line  = _mainWindow->findChild<QLineEdit*>("introLine2");
  _config->setIntroLine2(line->text().simplified());
}


void
Application::onAddContact() {
  ContactDialog dialog(_users, _talkgroups);
  if (QDialog::Accepted != dialog.exec())
    return;

  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  int row=-1;
  if (table->selectionModel()->hasSelection())
    row = table->selectionModel()->selection().back().bottom()+1;
  _config->contacts()->addContact(dialog.contact(), row);
}

void
Application::onRemContact() {
  // Get table
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  // Check if there is any contacts seleced
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete contact"),
          tr("Cannot delete contact: You have to select a contact first."));
    return;
  }
  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(table->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->contacts()->contact(rows.front())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete contact?"), tr("Delete contact %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete contacts?"), tr("Delete %1 contacts?").arg(rows.count())))
      return;
  }
  // collect all selected contacts
  // need to collect them first as rows change when deleting contacts
  QList<Contact *> contacts; contacts.reserve(rows.count());
  foreach (int row, rows)
    contacts.push_back(_config->contacts()->contact(row));
  // remove contacts
  foreach (Contact *contact, contacts)
    _config->contacts()->remContact(contact);
}

void
Application::onEditContact(const QModelIndex &idx) {
  if (idx.row() >= _config->contacts()->count())
    return;
  ContactDialog dialog(_config->contacts()->contact(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.contact();
}

void
Application::onContactUp() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move contacts"),
          tr("Cannot move contacts: You have to select at least one contact first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->contacts()->moveUp(rows.first, rows.second);
}

void
Application::onContactDown() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move contacts"),
          tr("Cannot move contacts: You have to select at least one contact first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->contacts()->moveDown(rows.first, rows.second);
}

void
Application::loadContactListSectionState() {
  Settings settings;
  QTableView *contacts = _mainWindow->findChild<QTableView *>("contactsView");
  contacts->horizontalHeader()->restoreState(settings.contactListHeaderState());
}
void
Application::storeContactListSectionState() {
  Settings settings;
  QTableView *contacts = _mainWindow->findChild<QTableView *>("contactsView");
  settings.setContactListHeaderState(contacts->horizontalHeader()->saveState());
}


void
Application::onAddRxGroup() {
  RXGroupListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  int row=-1;
  if (list->selectionModel()->hasSelection())
    row = list->selectionModel()->selection().back().bottom()+1;
  _config->rxGroupLists()->addList(dialog.groupList(), row);
}

void
Application::onRemRxGroup() {
  if (! _mainWindow)
    return;

  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  // Check if there is any groups seleced
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete RX group list"),
          tr("Cannot delete RX group lists: You have to select a group list first."));
    return;
  }
  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(list->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->rxGroupLists()->list(rows.first())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete RX group list?"), tr("Delete RX group list %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete RX group list?"), tr("Delete %1 RX group lists?").arg(rows.count())))
      return;
  }
  // collect all selected group lists
  // need to collect them first as rows change when deleting
  QList<RXGroupList *> lists; lists.reserve(rows.count());
  foreach (int row, rows)
    lists.push_back(_config->rxGroupLists()->list(row));
  // remove list
  foreach (RXGroupList *list, lists)
    _config->rxGroupLists()->remList(list);
}

void
Application::onEditRxGroup(const QModelIndex &index) {
  if (index.row() >= _config->rxGroupLists()->count())
    return;

  RXGroupListDialog dialog(_config, _config->rxGroupLists()->list(index.row()));

  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.groupList();

  emit _mainWindow->findChild<QListView *>("groupListView")->model()->dataChanged(index,index);
}

void
Application::onRxGroupUp() {
  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move RX group lists."),
          tr("Cannot move RX group lists: You have to select at least one list first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->rxGroupLists()->moveUp(rows.first, rows.second);
}

void
Application::onRxGroupDown() {
  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move RX group lists."),
          tr("Cannot move RX group lists: You have to select at least one list first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->rxGroupLists()->moveDown(rows.first, rows.second);
}

void
Application::onAddAnalogChannel() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  AnalogChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (table->selectionModel()->hasSelection())
    row = table->selectionModel()->selection().back().bottom()+1;
  _config->channelList()->addChannel(dialog.channel(), row);
}

void
Application::onAddDigitalChannel() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  DigitalChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  int row=-1;
  if (table->selectionModel()->hasSelection())
    row = table->selectionModel()->selection().back().bottom()+1;
  _config->channelList()->addChannel(dialog.channel(), row);
}

void
Application::onCloneChannel() {
  // get selection
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(nullptr, tr("Select a single channel first"),
                             tr("To clone a channel, please select a single channel to clone."),
                             QMessageBox::Close);
    return;
  }
  // Get selected channel
  Channel *channel = _config->channelList()->channel(selected.row());
  if (! channel)
    return;
  // Dispatch by type
  if (channel->is<AnalogChannel>()) {
    AnalogChannel *selch = channel->as<AnalogChannel>();
    // clone channel
    AnalogChannel *clone = new AnalogChannel(
          selch->name()+" clone", selch->rxFrequency(), selch->txFrequency(), selch->power(),
          selch->txTimeout(), selch->rxOnly(), selch->admit(), selch->squelch(),
          selch->rxTone(), selch->txTone(), selch->bandwidth(), selch->scanList(),
          selch->aprsSystem());
    // open editor
    AnalogChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      // if rejected -> destroy clone
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->addChannel(clone, selected.row()+1);
  } else {
    DigitalChannel *selch = channel->as<DigitalChannel>();
    // clone channel
    DigitalChannel *clone = new DigitalChannel(
          selch->name()+" clone", selch->rxFrequency(), selch->txFrequency(), selch->power(),
          selch->txTimeout(), selch->rxOnly(), selch->admit(), selch->colorCode(),
          selch->timeslot(), selch->rxGroupList(), selch->txContact(), selch->posSystem(),
          selch->scanList(), selch->roaming(), nullptr);
    // open editor
    DigitalChannelDialog dialog(_config, clone);
    if (QDialog::Accepted != dialog.exec()) {
      clone->deleteLater();
      return;
    }
    // update channel
    dialog.channel();
    // add to list (below selected one)
    _config->channelList()->addChannel(clone, selected.row()+1);
  }
}

void
Application::onRemChannel() {
  QTableView *table =_mainWindow->findChild<QTableView*>("channelView");
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete channel"),
          tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(table->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->channelList()->channel(rows.front())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete channel?"), tr("Delete channel %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete channel?"), tr("Delete %1 channels?").arg(rows.count())))
      return;
  }

  // collect all selected channels
  // need to collect them first as rows change when deleting channels
  QList<Channel *> channels; channels.reserve(rows.count());
  foreach (int row, rows)
    channels.push_back(_config->channelList()->channel(row));
  // remove channels
  foreach (Channel *channel, channels)
    _config->channelList()->remChannel(channel);
}

void
Application::onEditChannel(const QModelIndex &idx) {
  Channel *channel = _config->channelList()->channel(idx.row());
  if (! channel)
    return;
  if (channel->is<AnalogChannel>()) {
    AnalogChannelDialog dialog(_config, channel->as<AnalogChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  } else {
    DigitalChannelDialog dialog(_config, channel->as<DigitalChannel>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.channel();
  }
}

void
Application::onChannelUp() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move channels"),
          tr("Cannot move channels: You have to select at least one channel first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->channelList()->moveUp(rows.first, rows.second);
}

void
Application::onChannelDown() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move channels"),
          tr("Cannot move channels: You have to select at least one channel first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->channelList()->moveDown(rows.first, rows.second);
}

void
Application::loadChannelListSectionState() {
  Settings settings;
  QTableView *channels = _mainWindow->findChild<QTableView *>("channelView");
  channels->horizontalHeader()->restoreState(settings.channelListHeaderState());
}
void
Application::storeChannelListSectionState() {
  Settings settings;
  QTableView *channels = _mainWindow->findChild<QTableView *>("channelView");
  settings.setChannelListHeaderState(channels->horizontalHeader()->saveState());
}


void
Application::onAddZone() {
  ZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("zoneView");

  int row=-1;
  if (list->selectionModel()->hasSelection())
    row = list->selectionModel()->selection().back().bottom()+1;
  _config->zones()->addZone(dialog.zone(), row);
}

void
Application::onRemZone() {
  QListView *list = _mainWindow->findChild<QListView *>("zoneView");

  // Check if there is any zones seleced
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete zone"),
          tr("Cannot delete zone: You have to select a zone first."));
    return;
  }
  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(list->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->zones()->zone(rows.first())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete zone?"), tr("Delete zone %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete zones?"), tr("Delete %1 zones?").arg(rows.count())))
      return;
  }
  // collect all selected zones
  // need to collect them first as rows change when deleting
  QList<Zone *> lists; lists.reserve(rows.count());
  foreach (int row, rows)
    lists.push_back(_config->zones()->zone(row));
  // remove
  foreach (Zone *zone, lists)
    _config->zones()->remZone(zone);
}

void
Application::onZoneUp() {
  QListView *list = _mainWindow->findChild<QListView *>("zoneView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move zones."),
          tr("Cannot move zones: You have to select at least one zone first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->zones()->moveUp(rows.first, rows.second);
}

void
Application::onZoneDown() {
  QListView *list = _mainWindow->findChild<QListView *>("zoneView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move zones."),
          tr("Cannot move zones: You have to select at least one zone first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->zones()->moveDown(rows.first, rows.second);
}

void
Application::onEditZone(const QModelIndex &idx) {
  if (idx.row() >= _config->zones()->rowCount(QModelIndex()))
    return;

  ZoneDialog dialog(_config, _config->zones()->zone(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.zone();

  emit _mainWindow->findChild<QListView *>("zoneView")->model()->dataChanged(idx,idx);
}


void
Application::onAddScanList() {
  ScanListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("scanListView");

  int row=-1;
  if (list->selectionModel()->hasSelection())
    row = list->selectionModel()->selection().back().bottom()+1;
  _config->scanlists()->addScanList(dialog.scanlist(), row);
}

void
Application::onRemScanList() {
  QListView *list = _mainWindow->findChild<QListView *>("scanListView");
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete scanlist"),
          tr("Cannot delete scanlist: You have to select a scanlist first."));
    return;
  }
  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(list->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->scanlists()->scanlist(rows.first())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete scan list?"), tr("Delete scan list %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete scan lists?"), tr("Delete %1 scan lists?").arg(rows.count())))
      return;
  }
  // collect all selected scan lists
  // need to collect them first as rows change when deleting
  QList<ScanList *> lists; lists.reserve(rows.count());
  foreach (int row, rows)
    lists.push_back(_config->scanlists()->scanlist(row));
  // remove
  foreach (ScanList *list, lists)
    _config->scanlists()->remScanList(list);
}

void
Application::onScanListUp() {
  QListView *list = _mainWindow->findChild<QListView *>("scanListView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move scan lists."),
          tr("Cannot move scan lists: You have to select at least one scan list first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->scanlists()->moveUp(rows.first, rows.second);
}

void
Application::onScanListDown() {
  QListView *list = _mainWindow->findChild<QListView *>("scanListView");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move scan lists."),
          tr("Cannot move scan lists: You have to select at least one scan list first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->scanlists()->moveDown(rows.first, rows.second);
}

void
Application::onEditScanList(const QModelIndex &idx) {
  if (idx.row()>=_config->scanlists()->count())
    return;

  ScanListDialog dialog(_config, _config->scanlists()->scanlist(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.scanlist();

  emit _mainWindow->findChild<QListView *>("scanListView")->model()->dataChanged(idx,idx);
}

void
Application::onAddGPS() {
  GPSSystemDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QTableView *table = _mainWindow->findChild<QTableView *>("gpsView");
  int row=-1;
  if (table->selectionModel()->hasSelection())
    row = table->selectionModel()->selection().back().bottom()+1;
  _config->posSystems()->addSystem(dialog.gpsSystem(), row);
}

void
Application::onAddAPRS() {
  APRSSystemDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QTableView *table = _mainWindow->findChild<QTableView *>("gpsView");
  int row=-1;
  if (table->selectionModel()->hasSelection())
    row = table->selectionModel()->selection().back().bottom()+1;
  _config->posSystems()->addSystem(dialog.aprsSystem(), row);
}

void
Application::onRemGPS() {
  QTableView *table = _mainWindow->findChild<QTableView *>("gpsView");
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete GPS system"),
          tr("Cannot delete GPS system: You have to select a GPS system first."));
    return;
  }
  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(table->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->posSystems()->system(rows.front())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete positioning system?"), tr("Delete positioning system %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete positioning system?"), tr("Delete %1 positioning systems?").arg(rows.count())))
      return;
  }
  // collect all selected systems
  // need to collect them first as rows change when deleting systems
  QList<PositioningSystem *> systems; systems.reserve(rows.count());
  foreach (int row, rows)
    systems.push_back(_config->posSystems()->system(row));
  // remove systems
  foreach (PositioningSystem *system, systems)
    _config->posSystems()->remSystem(system);
}

void
Application::onGPSUp() {
  QTableView *table = _mainWindow->findChild<QTableView *>("gpsView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move positioning systems"),
          tr("Cannot move positioning systems: You have to select at least one positioning system first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->posSystems()->moveUp(rows.first, rows.second);
}

void
Application::onGPSDown() {
  QTableView *table = _mainWindow->findChild<QTableView *>("gpsView");
  // Check if there is a selection
  if (! table->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move positioning systems"),
          tr("Cannot move positioning systems: You have to select at least one positioning system first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(table->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->posSystems()->moveDown(rows.first, rows.second);
}

void
Application::onEditGPS(const QModelIndex &idx) {
  if (idx.row()>=_config->posSystems()->count())
    return;
  PositioningSystem *sys = _config->posSystems()->system(idx.row());
  if (sys->is<GPSSystem>()) {
    GPSSystemDialog dialog(_config, sys->as<GPSSystem>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.gpsSystem();
  } else if (sys->is<APRSSystem>()) {
    APRSSystemDialog dialog(_config, sys->as<APRSSystem>());
    if (QDialog::Accepted != dialog.exec())
      return;
    dialog.aprsSystem();
  }
  emit _mainWindow->findChild<QTableView *>("gpsView")->model()->dataChanged(idx,idx);
}

void
Application::onHideGPSNote() {
  Settings setting; setting.setHideGPSNote(true);
  QLabel *gpsNote = _mainWindow->findChild<QLabel*>("gpsNote");
  gpsNote->setVisible(false);
}

void
Application::loadPositioningSectionState() {
  Settings settings;
  QTableView *gps = _mainWindow->findChild<QTableView *>("gpsView");
  gps->horizontalHeader()->restoreState(settings.positioningHeaderState());
}
void
Application::storePositioningSectionState() {
  Settings settings;
  QTableView *gps = _mainWindow->findChild<QTableView *>("gpsView");
  settings.setPositioningHeaderState(gps->horizontalHeader()->saveState());
}


void
Application::onAddRoamingZone() {
  RoamingZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("roamingZoneList");
  int row=-1;
  if (list->selectionModel()->hasSelection())
    row = list->selectionModel()->selection().back().bottom()+1;
  _config->roaming()->addZone(dialog.zone(), row);
}

void
Application::onGenRoamingZone() {
  MultiGroupCallSelectionDialog contSel(_config->contacts());
  contSel.setWindowTitle(tr("Generate roaming zone"));
  contSel.setLabel(tr("Create a roaming zone by collecting all channels with these group calls."));
  if (QDialog::Accepted != contSel.exec())
    return;
  QList<DigitalContact *> contacts = contSel.contacts();
  RoamingZone *zone = new RoamingZone("Name");
  for (int i=0; i<_config->channelList()->count(); i++) {
    DigitalChannel *dch = _config->channelList()->channel(i)->as<DigitalChannel>();
    if (nullptr == dch)
      continue;
    if (contacts.contains(dch->txContact()))
      zone->addChannel(dch);
  }

  RoamingZoneDialog dialog(_config, zone);
  if (QDialog::Accepted != dialog.exec()) {
    zone->deleteLater();
    return;
  }

  QListView *list = _mainWindow->findChild<QListView *>("roamingZoneList");
  int row=-1;
  if (list->selectionModel()->hasSelection())
    row = list->selectionModel()->selection().back().bottom()+1;
  _config->roaming()->addZone(dialog.zone(), row);
}

void
Application::onRemRoamingZone() {
  QListView *list = _mainWindow->findChild<QListView *>("roamingZoneList");
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete roaming zone"),
          tr("Cannot delete roaming zone: You have to select a zone first."));
    return;
  }

  // Get selection and ask for deletion
  QList<int> rows = getSelectionRows(list->selectionModel()->selection().indexes());
  if (1 == rows.count()) {
    QString name = _config->roaming()->zone(rows.first())->name();
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zone?"), tr("Delete roaming zone %1?").arg(name)))
      return;
  } else {
    if (QMessageBox::No == QMessageBox::question(
          nullptr, tr("Delete roaming zones?"), tr("Delete %1 roaming zones?").arg(rows.count())))
      return;
  }
  // collect all selected zones
  // need to collect them first as rows change when deleting
  QList<RoamingZone *> lists; lists.reserve(rows.count());
  foreach (int row, rows)
    lists.push_back(_config->roaming()->zone(row));
  // remove
  foreach (RoamingZone *zone, lists)
    _config->roaming()->remZone(zone);
}

void
Application::onRoamingZoneUp() {
  QListView *list = _mainWindow->findChild<QListView *>("roamingZoneList");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move roaming zones."),
          tr("Cannot move roaming zones: You have to select at least one roaming zone first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->roaming()->moveUp(rows.first, rows.second);
}

void
Application::onRoamingZoneDown() {
  QListView *list = _mainWindow->findChild<QListView *>("roamingZoneList");
  // Check if there is a selection
  if (! list->selectionModel()->hasSelection()) {
    QMessageBox::information(
          nullptr, tr("Cannot move roaming zones."),
          tr("Cannot move roaming zones: You have to select at least one roaming zone first."));
    return;
  }
  // Get selection range assuming only continious selection mode
  QPair<int, int> rows = getSelectionRowRange(list->selectionModel()->selection().indexes());
  if ((0>rows.first) || (0>rows.second))
    return;
  // Then move rows
  _config->roaming()->moveDown(rows.first, rows.second);
}

void
Application::onEditRoamingZone(const QModelIndex &idx) {
  if (idx.row() >= _config->roaming()->count())
    return;

  RoamingZoneDialog dialog(_config, _config->roaming()->zone(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.zone();

  emit _mainWindow->findChild<QListView *>("roamingZoneList")->model()->dataChanged(idx,idx);
}

void
Application::onHideRoamingNote() {
  Settings setting; setting.setHideRoamingNote(true);
  QLabel *roamingNote = _mainWindow->findChild<QLabel*>("roamingNote");
  roamingNote->setVisible(false);
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



