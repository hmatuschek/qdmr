#include "application.hh"
#include <QMainWindow>
#include <QtUiTools>
#include "radio.hh"


Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _config(nullptr), _mainWindow(nullptr)
{
  _config = new Config(this);

  if (argc>1) {
    QFile file(argv[1]);
    if (! file.open(QIODevice::ReadOnly)) {

      return;
    }
    QTextStream stream(&file);
    _config->readCSV(stream);
  }

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


QMainWindow *
Application::createMainWindow() {
  if (_mainWindow)
    return _mainWindow;

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
  QAction *cpWiz   = _mainWindow->findChild<QAction*>("actionCodeplugWizzard");
  QAction *loadCP  = _mainWindow->findChild<QAction*>("actionOpenCodeplug");
  QAction *saveCP  = _mainWindow->findChild<QAction*>("actionSaveCodeplug");

  QAction *findDev = _mainWindow->findChild<QAction*>("actionDetectDevice");
  QAction *verCP   = _mainWindow->findChild<QAction*>("actionVerifyCodeplug");
  QAction *downCP  = _mainWindow->findChild<QAction*>("actionDownload");
  QAction *upCP    = _mainWindow->findChild<QAction*>("actionUpload");

  QAction *about   = _mainWindow->findChild<QAction*>("actionAbout");
  QAction *help    = _mainWindow->findChild<QAction*>("actionHelp");
  QAction *quit    = _mainWindow->findChild<QAction*>("actionQuit");

  connect(newCP, SIGNAL(triggered()), this, SLOT(newCodeplug()));
  connect(cpWiz, SIGNAL(triggered()), this, SLOT(codeplugWizzard()));
  connect(loadCP, SIGNAL(triggered()), this, SLOT(loadCodeplug()));
  connect(saveCP, SIGNAL(triggered()), this, SLOT(saveCodeplug()));
  connect(quit, SIGNAL(triggered()), this, SLOT(quit()));

  connect(findDev, SIGNAL(triggered()), this, SLOT(detectRadio()));
  connect(verCP, SIGNAL(triggered()), this, SLOT(verifyCodeplug()));
  connect(downCP, SIGNAL(triggered()), this, SLOT(downloadCodeplug()));
  connect(upCP, SIGNAL(triggered()), this, SLOT(uploadCodeplug()));

  // Wire-up "General Settings" view
  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");
  dmrID->setText(QString::number(_config->id()));
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());
  connect(dmrID, SIGNAL(editingFinished()), this, SLOT(onDMRIDChanged()));
  connect(intro1, SIGNAL(editingFinished()), this, SLOT(onIntroLine1Changed()));
  connect(intro2, SIGNAL(editingFinished()), this, SLOT(onIntroLine2Changed()));

  // Wire-up "Contact List" view
  QTableView *contacts = _mainWindow->findChild<QTableView *>("contactsView");
  QPushButton *cntUp   = _mainWindow->findChild<QPushButton *>("contactUp");
  QPushButton *cntDown = _mainWindow->findChild<QPushButton *>("contactUp");
  QPushButton *addCnt  = _mainWindow->findChild<QPushButton *>("addContact");
  QPushButton *remCnt  = _mainWindow->findChild<QPushButton *>("remContact");
  contacts->setModel(_config->contacts());
  connect(addCnt, SIGNAL(clicked()), this, SLOT(onAddContact()));
  connect(remCnt, SIGNAL(clicked()), this, SLOT(onRemContact()));
  connect(contacts, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditContact(const QModelIndex &)));

  // Wire-up "RX Group List" view
  QListView *rxgroups  = _mainWindow->findChild<QListView *>("groupListView");
  QPushButton *grpUp   = _mainWindow->findChild<QPushButton *>("rxGroupUp");
  QPushButton *grpDown = _mainWindow->findChild<QPushButton *>("rxGroupUp");
  QPushButton *addGrp  = _mainWindow->findChild<QPushButton *>("addRXGroup");
  QPushButton *remGrp  = _mainWindow->findChild<QPushButton *>("remRXGroup");
  rxgroups->setModel(_config->rxGroupLists());
  connect(addGrp, SIGNAL(clicked()), this, SLOT(onAddRxGroup()));
  connect(remGrp, SIGNAL(clicked()), this, SLOT(onRemRxGroup()));
  connect(rxgroups, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditRxGroup(const QModelIndex &)));

  // Wire-up "Channel List" view
  QTableView *channels = _mainWindow->findChild<QTableView *>("channelView");
  QPushButton *chUp    = _mainWindow->findChild<QPushButton *>("channelUp");
  QPushButton *chDown  = _mainWindow->findChild<QPushButton *>("channelUp");
  QPushButton *addCh   = _mainWindow->findChild<QPushButton *>("addChannel");
  QPushButton *remCh   = _mainWindow->findChild<QPushButton *>("remChannel");
  channels->setModel(_config->channelList());
  connect(addCh, SIGNAL(clicked()), this, SLOT(onAddChannel()));
  connect(remCh, SIGNAL(clicked()), this, SLOT(onRemChannel()));

  // Wire-up "Zone List" view
  QListView *zones     = _mainWindow->findChild<QListView *>("zoneView");
  QPushButton *zoneUp   = _mainWindow->findChild<QPushButton *>("zoneUp");
  QPushButton *zoneDown = _mainWindow->findChild<QPushButton *>("zoneUp");
  QPushButton *addZone  = _mainWindow->findChild<QPushButton *>("addZone");
  QPushButton *remZone  = _mainWindow->findChild<QPushButton *>("remZone");
  zones->setModel(_config->zones());
  connect(addZone, SIGNAL(clicked()), this, SLOT(onAddZone()));
  connect(remZone, SIGNAL(clicked()), this, SLOT(onRemZone()));

  // Wire-up "Scan List" view
  QListView *scanLists = _mainWindow->findChild<QListView *>("scanListView");
  QPushButton *slUp   = _mainWindow->findChild<QPushButton *>("scanListUp");
  QPushButton *slDown = _mainWindow->findChild<QPushButton *>("scanListUp");
  QPushButton *addSl  = _mainWindow->findChild<QPushButton *>("addScanList");
  QPushButton *remSl  = _mainWindow->findChild<QPushButton *>("remScanList");
  scanLists->setModel(_config->scanlists());
  connect(addSl, SIGNAL(clicked()), this, SLOT(onAddScanList()));
  connect(remSl, SIGNAL(clicked()), this, SLOT(onRemScanList()));

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
    if (QMessageBox::Ok != QMessageBox::question(0, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open codeplug"), QString(),
                                                  tr("CSV Files (*.csv *.conf)"));
  if (filename.isEmpty())
    return;
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot read codeplug from file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }
  QTextStream stream(&file);
  if (_config->readCSV(stream))
    _mainWindow->setWindowModified(false);
}


void
Application::saveCodeplug() {
  if (! _mainWindow)
    return;

  QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save codeplug"), QString(),
                                                  tr("CSV Files (*.csv *.conf)"));
  if (filename.isEmpty())
    return;

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, tr("Cannot open file"),
                          tr("Cannot save codeplug to file '%1': %2").arg(filename).arg(file.errorString()));
    return;
  }

  QTextStream stream(&file);
  if (_config->writeCSV(stream))
    _mainWindow->setWindowModified(false);

  file.flush();
  file.close();
}


void
Application::detectRadio() {
  Radio *radio = Radio::detect();
  if (radio) {
    QMessageBox::information(0, tr("Radio found"), tr("Found device '%1'.").arg(radio->name()));
    radio->deleteLater();
  } else {
    QMessageBox::information(0, tr("No Radio found."),
                             tr("No known radio detected. Check connection?"));
  }
  radio->deleteLater();
}


bool
Application::verifyCodeplug(Radio *radio) {
  Radio *myRadio = radio;
  if (nullptr == myRadio)
    myRadio = Radio::detect();

  if (! myRadio) {
    QMessageBox::information(0, tr("No Radio found."),
                             tr("Cannot verify codeplug: No known radio detected."));
    return false;
  }

  QStringList issues;
  if (! myRadio->verifyConfig(_config, issues)) {
    QMessageBox::information(0, tr("Codeplug verification."),
                             tr("Codeplug does not match device features!"));
  }

  if (nullptr == radio)
    myRadio->deleteLater();

  return (0 == issues.size());
}


void
Application::downloadCodeplug() {
  if (! _mainWindow)
    return;

  if (_config->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(0, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      return;
  }

  Radio *radio = Radio::detect();
  if (! radio) {
    QMessageBox::critical(0, tr("No Radio found."),
                          tr("Can not download codeplug from device: No radio found."));
    return;
  }

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0); progress->setMaximum(100); progress->setVisible(true);
  connect(radio, SIGNAL(downloadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(downloadError(Radio *)), this, SLOT(onCodeplugDownloadError(Radio *)));
  connect(radio, SIGNAL(downloadComplete(Radio *,Config *)), this, SLOT(onCodeplugDownloaded(Radio *, Config *)));
  radio->startDownload(_config);
  _mainWindow->statusBar()->showMessage(tr("Download ..."));
  _mainWindow->setEnabled(false);
}

void
Application::onCodeplugDownloadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Download error"));
  QMessageBox::critical(0, tr("Download error"), tr("Cannot download codeplug from device. "
                                                    "An error occurred during download."));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  radio->deleteLater();
  _mainWindow->setWindowModified(false);
}


void
Application::onCodeplugDownloaded(Radio *radio, Config *config) {
  Q_UNUSED(config)
  _mainWindow->statusBar()->showMessage(tr("Download complete"));
  _mainWindow->findChild<QProgressBar *>("progress")->setVisible(false);
  _mainWindow->setEnabled(true);

  radio->deleteLater();
  _mainWindow->setWindowModified(false);
  config->setModified(false);
}

void
Application::uploadCodeplug() {
  // Start upload
  Radio *radio = Radio::detect();
  if (! radio) {
    QMessageBox::critical(0, tr("No Radio found."), tr("Can not upload codeplug to device: No radio found."));
    return;
  }

  // Verify codeplug against the radio before uploading
  if (! verifyCodeplug(radio))
    return;

  QProgressBar *progress = _mainWindow->findChild<QProgressBar *>("progress");
  progress->setValue(0);
  progress->setMaximum(100);
  progress->setVisible(true);

  connect(radio, SIGNAL(uploadProgress(int)), progress, SLOT(setValue(int)));
  connect(radio, SIGNAL(uploadError(Radio *)), this, SLOT(onCodeplugUploadError(Radio *)));
  connect(radio, SIGNAL(uploadComplete(Radio *)), this, SLOT(onCodeplugUploaded(Radio *)));
  radio->startUpload(_config);

  _mainWindow->statusBar()->showMessage(tr("Upload ..."));
  _mainWindow->setEnabled(false);
}


void
Application::onCodeplugUploadError(Radio *radio) {
  _mainWindow->statusBar()->showMessage(tr("Upload error"));
  QMessageBox::critical(0, tr("Upload error"), tr("Cannot upload codeplug to device. "
                                                  "An error occurred during upload."));
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
Application::showAbout() {

}

void
Application::showHelp() {

}


void
Application::onConfigModifed() {
  if (! _mainWindow)
    return;

  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  QLineEdit *intro1 = _mainWindow->findChild<QLineEdit*>("introLine1");
  QLineEdit *intro2 = _mainWindow->findChild<QLineEdit*>("introLine2");

  dmrID->setText(QString::number(_config->id()));
  intro1->setText(_config->introLine1());
  intro2->setText(_config->introLine2());

  _mainWindow->setWindowModified(true);
}


void
Application::onDMRIDChanged() {
  QLineEdit *dmrID  = _mainWindow->findChild<QLineEdit*>("dmrID");
  _config->setId(dmrID->text().toUInt());
}

void
Application::onIntroLine1Changed() {
  QLineEdit *line  = _mainWindow->findChild<QLineEdit*>("introLine1");
  _config->setIntroLine1(line->text());
}

void
Application::onIntroLine2Changed() {
  QLineEdit *line  = _mainWindow->findChild<QLineEdit*>("introLine2");
  _config->setIntroLine2(line->text());
}


void
Application::onAddContact() {
  ContactDialog dialog;
  if (QDialog::Accepted != dialog.exec())
    return;
  _config->contacts()->addContact(dialog.contact());
}

void
Application::onRemContact() {
  QTableView *table = _mainWindow->findChild<QTableView *>("contactsView");
  QModelIndex selected = table->selectionModel()->currentIndex();
  if (! selected.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete contact"),
                             tr("Cannot delete contact: You have to select a contact first."));
    return;
  }

  QString name = _config->contacts()->contact(selected.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete contact?"), tr("Delete contact %1?").arg(name)))
    return;

  _config->contacts()->remContact(selected.row());
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
Application::onAddRxGroup() {
  RXGroupListDialog dialog(_config);

  if (QDialog::Accepted != dialog.exec())
    return;

  _config->rxGroupLists()->addList(dialog.groupList());
}

void
Application::onRemRxGroup() {
  if (! _mainWindow)
    return;

  QModelIndex idx = _mainWindow->findChild<QListView *>("groupListView")->selectionModel()->currentIndex();
  if (! idx.isValid()) {
    QMessageBox::information(nullptr, tr("Cannot delete group list"),
                             tr("Cannot delete group list: You have to select a group list first."));
    return;
  }

  QString name = _config->rxGroupLists()->list(idx.row())->name();
  if (QMessageBox::No == QMessageBox::question(nullptr, tr("Delete group list?"), tr("Delete group list %1?").arg(name)))
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


