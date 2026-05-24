#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QStyleHints>

#include "settings.hh"
#include "logger.hh"
#include "application.hh"

#include "channellistview.hh"
#include "contactlistview.hh"
#include "extensionview.hh"
#include "generalsettingsview.hh"
#include "grouplistsview.hh"
#include "positioningsystemlistview.hh"
#include "radioidlistview.hh"
#include "roamingchannellistview.hh"
#include "roamingzonelistview.hh"
#include "satellitedatabase.hh"
#include "scanlistsview.hh"
#include "talkgroupdatabase.hh"
#include "task_progress.hh"
#include "task_progress_view.hh"
#include "zonelistview.hh"

MainWindow::MainWindow(Config *config, QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  auto app = qobject_cast<Application*>(QApplication::instance());

  Settings settings;
  logDebug() << "Create main window using icon theme '" << QIcon::themeName() << "'.";

  auto progress = new TaskProgressListView();
  progress->setObjectName("progress");
  ui->statusbar->addPermanentWidget(progress);

  ui->actionExportToCHIRP->setIcon(QIcon::fromTheme("document-export"));
  ui->actionImport->setIcon(QIcon::fromTheme("document-import"));

  ui->actionRefreshCallsignDB->setIcon(QIcon::fromTheme("document-download"));
  ui->actionRefreshTalkgroupDB->setIcon(QIcon::fromTheme("document-download"));
  ui->actionRefreshOrbitalElements->setIcon(QIcon::fromTheme("document-download"));
  ui->actionWriteSatellites->setIcon(QIcon::fromTheme("device-write-satellites"));
  ui->actionEditSatellites->setIcon(QIcon::fromTheme("edit-satellites"));

#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
  connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [=](Qt::ColorScheme scheme) {
      bool isDarkTheme = scheme == Qt::ColorScheme::Dark ? true : false;
      QIcon::setThemeName(isDarkTheme ? "dark" : "light");
  });
#endif

  connect(ui->actionNewCodeplug, SIGNAL(triggered()), app, SLOT(newCodeplug()));
  connect(ui->actionOpenCodeplug, SIGNAL(triggered()), app, SLOT(loadCodeplug()));
  connect(ui->actionSaveCodeplug, SIGNAL(triggered()), app, SLOT(saveCodeplug()));
  connect(ui->actionExportToCHIRP, SIGNAL(triggered()), app, SLOT(exportCodeplugToChirp()));
  connect(ui->actionImport, SIGNAL(triggered()), app, SLOT(importCodeplug()));
  connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionAbout, SIGNAL(triggered()), app, SLOT(showAbout()));
  connect(ui->actionSettings, SIGNAL(triggered()), app, SLOT(showSettings()));
  connect(ui->actionHelp, SIGNAL(triggered()), app, SLOT(showHelp()));

  connect(ui->actionRefreshCallsignDB, &QAction::triggered, [app, progress]() {
    progress->addTask(app->user()->download());
  });

  connect(ui->actionRefreshTalkgroupDB, &QAction::triggered, [app, progress]() {
    progress->addTask(app->talkgroup()->download());
  });

  connect(ui->actionRefreshOrbitalElements, &QAction::triggered, app->satellite(), &SatelliteDatabase::update);
  QObject::connect(app->satellite(), &SatelliteDatabase::error, this, [this](const QString &msg) {
    this->ui->statusbar->showMessage(tr("Cannot update orbital elements: %1").arg(msg), 10000);
  }, Qt::QueuedConnection);
  QObject::connect(app->satellite(), &SatelliteDatabase::loaded, this, [this]() {
    this->ui->statusbar->showMessage(tr("Orbital elements updated & loaded."), 10000);
  }, Qt::QueuedConnection);

  connect(ui->actionUploadCallsignDB, SIGNAL(triggered()), app, SLOT(uploadCallsignDB()));
  connect(app->user(), &UserDatabase::readyChanged, this, [this](bool ready){
    this->ui->actionUploadCallsignDB->setEnabled(ready);
  }, Qt::QueuedConnection);
  ui->actionUploadCallsignDB->setEnabled(app->user()->ready());

  connect(app->repeater(), &RepeaterDatabase::error, this, [this](const QString &msg) {
    this->ui->statusbar->showMessage(msg);
  }, Qt::QueuedConnection);

  connect(ui->actionEditSatellites, SIGNAL(triggered()), app, SLOT(editSatellites()));
  connect(ui->actionDetectDevice, SIGNAL(triggered()), app, SLOT(detectRadio()));
  connect(ui->actionVerifyCodeplug, SIGNAL(triggered()), app, SLOT(verifyCodeplug()));
  connect(ui->actionDownload, SIGNAL(triggered()), app, SLOT(downloadCodeplug()));
  connect(ui->actionUpload, SIGNAL(triggered()), app, SLOT(uploadCodeplug()));
  connect(ui->actionWriteSatellites, SIGNAL(triggered()), app, SLOT(uploadSatellites()));

  // Wire-up "General Settings" view
  _generalSettings = new GeneralSettingsView(config);
  ui->tabs->addTab(_generalSettings, tr("Settings"));

  // Wire-up "Radio IDs" view
  _radioIdTab = new RadioIDListView(config);
  ui->tabs->addTab(_radioIdTab, tr("Radio IDs"));
  // Wire-up "Contact List" view
  ui->tabs->addTab(new ContactListView(config), tr("Contacts"));
  // Wire-up "RX Group List" view
  ui->tabs->addTab(new GroupListsView(config), tr("Group Lists"));
  // Wire-up "Channel List" view
  ui->tabs->addTab(new ChannelListView(config), tr("Channels"));
  // Wire-up "Zone List" view
  ui->tabs->addTab(new ZoneListView(config), tr("Zones"));
  // Wire-up "Scan List" view
  ui->tabs->addTab(new ScanListsView(config), tr("Scan Lists"));
  // Wire-up "GPS System List" view
  ui->tabs->addTab(new PositioningSystemListView(config), tr("GPS/APRS"));
  // Wire-up "Roaming Zone List" view
  ui->tabs->addTab(new RoamingChannelListView(config), tr("Roaming Channels"));
  // Wire-up "Roaming Zone List" view
  _roamingZoneList = new RoamingZoneListView(config);
  ui->tabs->addTab(_roamingZoneList, tr("Roaming Zones"));
  // Wire-up "extension view" for direct editing
  _extensionView = new ExtensionView();
  _extensionView->setObject(config, config);
  ui->tabs->addTab(_extensionView, tr("Extensions"));

  restoreGeometry(settings.windowState(objectName()));

  connect(config, &ConfigItem::modified, [this, config]() {
    this->setWindowModified(config->isModified());
  });
}


void
MainWindow::closeEvent(QCloseEvent *event) {
  if (qobject_cast<Application*>(Application::instance())->isModified()) {
    if (QMessageBox::Ok != QMessageBox::question(nullptr, tr("Unsaved changes to codeplug."),
                                                 tr("There are unsaved changes to the current codeplug. "
                                                    "These changes are lost if you proceed."),
                                                 QMessageBox::Cancel|QMessageBox::Ok))
      event->ignore();
    return;
  }

  Settings().setWindowState(objectName(), saveGeometry());
  event->accept();
}
