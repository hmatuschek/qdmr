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
#include "repeaterdatabase.hh"
#include "userdatabase.hh"


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
  _repeater = new RepeaterDatabase(settings.position(), 7, this);
  _users    = new UserDatabase(30, this);
  _config = new Config(_users, this);

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
  QAction *cpWiz   = _mainWindow->findChild<QAction*>("actionCodeplugWizard");
  cpWiz->setEnabled(false);
  QAction *loadCP  = _mainWindow->findChild<QAction*>("actionOpenCodeplug");
  QAction *saveCP  = _mainWindow->findChild<QAction*>("actionSaveCodeplug");

  QAction *findDev = _mainWindow->findChild<QAction*>("actionDetectDevice");
  QAction *verCP   = _mainWindow->findChild<QAction*>("actionVerifyCodeplug");
  QAction *downCP  = _mainWindow->findChild<QAction*>("actionDownload");
  QAction *upCP    = _mainWindow->findChild<QAction*>("actionUpload");

  QAction *about   = _mainWindow->findChild<QAction*>("actionAbout");
  QAction *sett    = _mainWindow->findChild<QAction*>("actionSettings");
  QAction *help    = _mainWindow->findChild<QAction*>("actionHelp");
  QAction *quit    = _mainWindow->findChild<QAction*>("actionQuit");

  connect(newCP, SIGNAL(triggered()), this, SLOT(newCodeplug()));
  connect(cpWiz, SIGNAL(triggered()), this, SLOT(codeplugWizzard()));
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

  // Wire-up "General Settings" view
  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  QLineEdit *rname  = _mainWindow->findChild<QLineEdit*>("radioName");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");
  QSpinBox  *mic    = _mainWindow->findChild<QSpinBox *>("mic");
  QCheckBox *speech = _mainWindow->findChild<QCheckBox*>("speech");
  QCheckBox *userdb = _mainWindow->findChild<QCheckBox*>("uploadUserDB");

  dmrID->setText(QString::number(_config->id()));
  rname->setText(_config->name());
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());
  mic->setValue(_config->micLevel());
  speech->setChecked(_config->speech());
  userdb->setChecked(_config->uploadUserDB());

  connect(dmrID, SIGNAL(editingFinished()), this, SLOT(onDMRIDChanged()));
  connect(rname, SIGNAL(editingFinished()), this, SLOT(onNameChanged()));
  connect(intro1, SIGNAL(editingFinished()), this, SLOT(onIntroLine1Changed()));
  connect(intro2, SIGNAL(editingFinished()), this, SLOT(onIntroLine2Changed()));
  connect(mic, SIGNAL(valueChanged(int)), this, SLOT(onMicLevelChanged()));
  connect(speech, SIGNAL(toggled(bool)), this, SLOT(onSpeechChanged()));
  connect(userdb, SIGNAL(toggled(bool)), this, SLOT(onUploadUserDBChanged()));

  // Wire-up "Contact List" view
  QTableView *contacts = _mainWindow->findChild<QTableView *>("contactsView");
  QPushButton *cntUp   = _mainWindow->findChild<QPushButton *>("contactUp");
  QPushButton *cntDown = _mainWindow->findChild<QPushButton *>("contactDown");
  QPushButton *addCnt  = _mainWindow->findChild<QPushButton *>("addContact");
  QPushButton *remCnt  = _mainWindow->findChild<QPushButton *>("remContact");
  contacts->setModel(_config->contacts());
  connect(addCnt, SIGNAL(clicked()), this, SLOT(onAddContact()));
  connect(remCnt, SIGNAL(clicked()), this, SLOT(onRemContact()));
  connect(cntUp, SIGNAL(clicked()), this, SLOT(onContactUp()));
  connect(cntDown, SIGNAL(clicked()), this, SLOT(onContactDown()));
  connect(contacts, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditContact(const QModelIndex &)));

  // Wire-up "RX Group List" view
  QListView *rxgroups  = _mainWindow->findChild<QListView *>("groupListView");
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
  QPushButton *chUp    = _mainWindow->findChild<QPushButton *>("channelUp");
  QPushButton *chDown  = _mainWindow->findChild<QPushButton *>("channelDown");
  QPushButton *addACh  = _mainWindow->findChild<QPushButton *>("addAnalogChannel");
  QPushButton *addDCh  = _mainWindow->findChild<QPushButton *>("addDigitalChannel");
  QPushButton *remCh   = _mainWindow->findChild<QPushButton *>("remChannel");
  channels->setModel(_config->channelList());
  channels->resizeColumnsToContents();
  connect(addACh, SIGNAL(clicked()), this, SLOT(onAddAnalogChannel()));
  connect(addDCh, SIGNAL(clicked()), this, SLOT(onAddDigitalChannel()));
  connect(remCh, SIGNAL(clicked()), this, SLOT(onRemChannel()));
  connect(chUp, SIGNAL(clicked()), this, SLOT(onChannelUp()));
  connect(chDown, SIGNAL(clicked()), this, SLOT(onChannelDown()));
  connect(channels, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditChannel(const QModelIndex &)));

  // Wire-up "Zone List" view
  QListView *zones     = _mainWindow->findChild<QListView *>("zoneView");
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
  QPushButton *gpsUp   = _mainWindow->findChild<QPushButton *>("gpsUp");
  QPushButton *gpsDown = _mainWindow->findChild<QPushButton *>("gpsDown");
  QPushButton *addGPS  = _mainWindow->findChild<QPushButton *>("addGPS");
  QPushButton *remGPS  = _mainWindow->findChild<QPushButton *>("remGPS");
  QLabel *gpsNote = _mainWindow->findChild<QLabel*>("gpsNote");
  gpsList->setModel(_config->gpsSystems());
  if (settings.hideGSPNote())
    gpsNote->setVisible(false);
  connect(addGPS, SIGNAL(clicked()), this, SLOT(onAddGPS()));
  connect(remGPS, SIGNAL(clicked()), this, SLOT(onRemGPS()));
  connect(gpsUp, SIGNAL(clicked()), this, SLOT(onGPSUp()));
  connect(gpsDown, SIGNAL(clicked()), this, SLOT(onGPSDown()));
  connect(gpsList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditGPS(const QModelIndex &)));
  connect(gpsNote, SIGNAL(linkActivated(QString)), this, SLOT(onHideGPSNote()));

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
Application::codeplugWizzard() {

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

  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open codeplug"), QString(),
                                                  tr("Codeplug Files (*.conf *.csv *.txt)"));
  if (filename.isEmpty())
    return;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  logDebug() << "Load codeplug from '" << filename << "'.";

  QString errorMessage;
  QTextStream stream(&file);
  if (_config->readCSV(stream, errorMessage))
    _mainWindow->setWindowModified(false);
  else
    QMessageBox::critical(nullptr, tr("Cannot read codeplug."),
                          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(errorMessage));
}


void
Application::saveCodeplug() {
  if (! _mainWindow)
    return;

  QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save codeplug"), QString(),
                                                  tr("CSV Files (*.conf)"));
  if (filename.isEmpty())
    return;

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
Application::verifyCodeplug(Radio *radio, bool showSuccess) {
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
  if (! myRadio->verifyConfig(_config, issues)) {
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
  if (codeplug->decode(_config)) {
    _mainWindow->statusBar()->showMessage(tr("Download complete"));
    _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
    _mainWindow->setEnabled(true);

    _mainWindow->setWindowModified(false);
    _config->setModified(false);
  } else {
    QMessageBox::critical(
          nullptr, tr("Cannot decode code-plug"),
          tr("Cannot decode code-plug: %2").arg(codeplug->errorMessage()));
  }

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
  if (! verifyCodeplug(radio, false))
    return;

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0);
  progress->setMaximum(100);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));
  radio->startUpload(_config, false, settings.updateCodeplug());

  _mainWindow->statusBar()->showMessage(tr("Upload ..."));
  _mainWindow->setEnabled(false);
}


void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload error"));
  QMessageBox::critical(
        nullptr, tr("Upload error"),
        tr("Cannot upload codeplug to device. "
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
    settings.setQueryPosition(dialog.systemLocationEnabled());
    settings.setLocator(dialog.locator());
    settings.setUpdateCodeplug(dialog.updateCodeplug());
    if (! settings.queryPosition()) {
      _source->stopUpdates();
      _currentPosition = settings.position();
    } else {
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

  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  QLineEdit *rname  = _mainWindow->findChild<QLineEdit*>("radioName");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");
  QSpinBox  *mic    = _mainWindow->findChild<QSpinBox *>("mic");
  QCheckBox *speech = _mainWindow->findChild<QCheckBox*>("speech");

  dmrID->setText(QString::number(_config->id()));
  rname->setText(_config->name());
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());
  mic->setValue(_config->micLevel());
  speech->setChecked(_config->speech());

  _mainWindow->setWindowModified(true);
}


void
Application::onDMRIDChanged() {
  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  _config->setId(dmrID->text().toUInt());
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
Application::onUploadUserDBChanged() {
  QCheckBox *userdb = _mainWindow->findChild<QCheckBox *>("uploadUserDB");
  _config->setUploadUserDB(userdb->isChecked());
}

void
Application::onIntroLine2Changed() {
  QLineEdit *line  = _mainWindow->findChild<QLineEdit*>("introLine2");
  _config->setIntroLine2(line->text().simplified());
}


void
Application::onAddContact() {
  ContactDialog dialog(_users);
  if (QDialog::Accepted != dialog.exec())
    return;

  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if (selected.isValid())
    _config->contacts()->addContact(dialog.contact(), selected.row()+1);
  else
    _config->contacts()->addContact(dialog.contact());
}

void
Application::onRemContact() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete contact"),
          tr("Cannot delete contact: You have to select a contact first."));
    return;
  }

  QString name = _config->contacts()->contact(selected.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete contact?"), tr("Delete contact %1?").arg(name)))
    return;

  _config->contacts()->remContact(selected.row());
}

void
Application::onEditContact(const QModelIndex &idx) {
  if (idx.row() >= _config->contacts()->count())
    return;
  ContactDialog dialog(_users, _config->contacts()->contact(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.contact();
}

void
Application::onContactUp() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0==selected.row()))
    return;
  if (_config->contacts()->moveUp(selected.row()))
    table->selectRow(selected.row()-1);
}

void
Application::onContactDown() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->contacts()->count()-1) <= selected.row()))
    return;
  if (_config->contacts()->moveDown(selected.row()))
    table->selectRow(selected.row()+1);
}

void
Application::onAddRxGroup() {
  RXGroupListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  QModelIndex selected = list->selectionModel()->currentIndex();

  if (selected.isValid())
    _config->rxGroupLists()->addList(dialog.groupList(), selected.row()+1);
  else
    _config->rxGroupLists()->addList(dialog.groupList());
}

void
Application::onRemRxGroup() {
  if (! _mainWindow)
    return;

  QModelIndex idx = _mainWindow->findChild<QListView *>("groupListView")->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete group list"),
          tr("Cannot delete group list: You have to select a group list first."));
    return;
  }

  QString name = _config->rxGroupLists()->list(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete group list?"), tr("Delete group list %1?").arg(name)))
    return;

  _config->rxGroupLists()->remList(idx.row());
}

void
Application::onEditRxGroup(const QModelIndex &index) {
  if (index.row() >= _config->rxGroupLists()->rowCount(QModelIndex()))
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
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0 >= selected.row()))
    return;
  if (_config->rxGroupLists()->moveUp(selected.row()))
    list->setCurrentIndex(_config->rxGroupLists()->index(selected.row()-1));
}

void
Application::onRxGroupDown() {
  QListView *list = _mainWindow->findChild<QListView *>("groupListView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->rxGroupLists()->count()-1) <= selected.row()))
    return;
  if (_config->rxGroupLists()->moveDown(selected.row()))
    list->setCurrentIndex(_config->rxGroupLists()->index(selected.row()+1));
}

void
Application::onAddAnalogChannel() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  QModelIndex selected = table->selectionModel()->currentIndex();

  AnalogChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;

  if (selected.isValid())
    _config->channelList()->addChannel(dialog.channel(), selected.row()+1);
  else
    _config->channelList()->addChannel(dialog.channel());

}

void
Application::onAddDigitalChannel() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  QModelIndex selected = table->selectionModel()->currentIndex();

  DigitalChannelDialog dialog(_config);
  if (QDialog::Accepted != dialog.exec())
    return;
  if (selected.isValid())
    _config->channelList()->addChannel(dialog.channel(), selected.row()+1);
  else
    _config->channelList()->addChannel(dialog.channel());
}

void
Application::onRemChannel() {
  QModelIndex selected =_mainWindow->findChild<QTableView*>("channelView")->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete channel"),
          tr("Cannot delete channel: You have to select a channel first."));
    return;
  }

  QString name = _config->channelList()->channel(selected.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete channel?"), tr("Delete channel %1?").arg(name)))
    return;

  _config->channelList()->remChannel(selected.row());
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
  QModelIndex selected = table->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0 == selected.row()))
    return;
  if (_config->channelList()->moveUp(selected.row()))
    table->selectRow(selected.row()-1);
}

void
Application::onChannelDown() {
  QTableView *table = _mainWindow->findChild<QTableView *>("channelView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->channelList()->count()-1) == selected.row()))
    return;
  if (_config->channelList()->moveDown(selected.row()))
    table->selectRow(selected.row()+1);
}


void
Application::onAddZone() {
  ZoneDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  QListView *list = _mainWindow->findChild<QListView *>("zoneView");
  QModelIndex selected = list->selectionModel()->currentIndex();

  Zone *zone = dialog.zone();
  if (selected.isValid())
    _config->zones()->addZone(zone, selected.row()+1);
  else
    _config->zones()->addZone(zone);
}

void
Application::onRemZone() {
  QModelIndex idx = _mainWindow->findChild<QListView *>("zoneView")->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete zone"),
          tr("Cannot delete zone: You have to select a zone first."));
    return;
  }

  QString name = _config->zones()->zone(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete zone?"), tr("Delete zone %1?").arg(name)))
    return;

  _config->zones()->remZone(idx.row());
}

void
Application::onZoneUp() {
  QListView *list = _mainWindow->findChild<QListView *>("zoneView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0 >= selected.row()))
    return;
  if (_config->zones()->moveUp(selected.row()))
    list->setCurrentIndex(_config->zones()->index(selected.row()-1));
}

void
Application::onZoneDown() {
  QListView *list = _mainWindow->findChild<QListView *>("zoneView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->zones()->count()-1) <= selected.row()))
    return;
  if (_config->zones()->moveDown(selected.row()))
    list->setCurrentIndex(_config->zones()->index(selected.row()+1));
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
  QModelIndex selected = list->selectionModel()->currentIndex();
  ScanList *scanlist = dialog.scanlist();
  if (selected.isValid())
    _config->scanlists()->addScanList(scanlist, selected.row()+1);
  else
    _config->scanlists()->addScanList(scanlist);
}

void
Application::onRemScanList() {
  QModelIndex idx = _mainWindow->findChild<QListView *>("scanListView")->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete scanlist"),
          tr("Cannot delete scanlist: You have to select a scanlist first."));
    return;
  }

  QString name = _config->scanlists()->scanlist(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete scanlist?"), tr("Delete scanlist %1?").arg(name)))
    return;

  _config->scanlists()->remScanList(idx.row());
}

void
Application::onScanListUp() {
  QListView *list = _mainWindow->findChild<QListView *>("scanListView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0 >= selected.row()))
    return;
  if (_config->scanlists()->moveUp(selected.row()))
    list->setCurrentIndex(_config->scanlists()->index(selected.row()-1));
}

void
Application::onScanListDown() {
  QListView *list = _mainWindow->findChild<QListView *>("scanListView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->scanlists()->count()-1) <= selected.row()))
    return;
  if (_config->scanlists()->moveDown(selected.row()))
    list->setCurrentIndex(_config->scanlists()->index(selected.row()+1));
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

  QTableView *list = _mainWindow->findChild<QTableView *>("gpsView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  GPSSystem *gps = dialog.gpsSystem();
  if (selected.isValid())
    _config->gpsSystems()->addGPSSystem(gps, selected.row()+1);
  else
    _config->gpsSystems()->addGPSSystem(gps);
}

void
Application::onRemGPS() {
  QModelIndex idx = _mainWindow->findChild<QTableView *>("gpsView")->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(
          nullptr, tr("Cannot delete GPS system"),
          tr("Cannot delete GPS system: You have to select a GPS system first."));
    return;
  }

  QString name = _config->gpsSystems()->gpsSystem(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(
        nullptr, tr("Delete GPS system?"), tr("Delete GPS system %1?").arg(name)))
    return;

  _config->gpsSystems()->remGPSSystem(idx.row());
}

void
Application::onGPSUp() {
  QTableView *list = _mainWindow->findChild<QTableView *>("gpsView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || (0 >= selected.row()))
    return;
  if (_config->gpsSystems()->moveUp(selected.row()))
    list->setCurrentIndex(_config->gpsSystems()->index(selected.row()-1,0));
}

void
Application::onGPSDown() {
  QTableView *list = _mainWindow->findChild<QTableView *>("gpsView");
  QModelIndex selected = list->selectionModel()->currentIndex();
  if ((! selected.isValid()) || ((_config->gpsSystems()->count()-1) <= selected.row()))
    return;
  if (_config->gpsSystems()->moveDown(selected.row()))
    list->setCurrentIndex(_config->gpsSystems()->index(selected.row()+1,0));
}

void
Application::onEditGPS(const QModelIndex &idx) {
  if (idx.row()>=_config->gpsSystems()->count())
    return;

  GPSSystemDialog dialog(_config, _config->gpsSystems()->gpsSystem(idx.row()));
  if (QDialog::Accepted != dialog.exec())
    return;

  dialog.gpsSystem();

  emit _mainWindow->findChild<QTableView *>("gpsView")->model()->dataChanged(idx,idx);
}

void
Application::onHideGPSNote() {
  Settings setting; setting.setHideGPSNote(true);
  QLabel *gpsNote = _mainWindow->findChild<QLabel*>("gpsNote");
  gpsNote->setVisible(false);
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



