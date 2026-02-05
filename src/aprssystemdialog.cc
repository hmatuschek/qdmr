#include "aprssystemdialog.hh"
#include "ui_aprssystemdialog.h"
#include "settings.hh"


static QVector<QPair<FMAPRSSystem::Icon, QString>> aprsIconTable{
  {FMAPRSSystem::Icon::None, FMAPRSSystem::tr("[None]")},
  {FMAPRSSystem::Icon::PoliceStation, FMAPRSSystem::tr("Police station")},
  {FMAPRSSystem::Icon::Digipeater, FMAPRSSystem::tr("Digipeater")},
  {FMAPRSSystem::Icon::Phone, FMAPRSSystem::tr("Phone")},
  {FMAPRSSystem::Icon::DXCluster, FMAPRSSystem::tr("DX cluster")},
  {FMAPRSSystem::Icon::HFGateway, FMAPRSSystem::tr("HF gateway")},
  {FMAPRSSystem::Icon::SmallPlane, FMAPRSSystem::tr("Plane small")},
  {FMAPRSSystem::Icon::MobileSatelliteStation, FMAPRSSystem::tr("Mobile Satellite station")},
  {FMAPRSSystem::Icon::WheelChair, FMAPRSSystem::tr("Wheel Chair")},
  {FMAPRSSystem::Icon::Snowmobile, FMAPRSSystem::tr("Snowmobile")},
  {FMAPRSSystem::Icon::RedCross, FMAPRSSystem::tr("Red cross")},
  {FMAPRSSystem::Icon::BoyScout, FMAPRSSystem::tr("Boy scout")},
  {FMAPRSSystem::Icon::Home,  FMAPRSSystem::tr("Home")},
  {FMAPRSSystem::Icon::X, FMAPRSSystem::tr("X")},
  {FMAPRSSystem::Icon::RedDot, FMAPRSSystem::tr("Red dot")},
  {FMAPRSSystem::Icon::Circle0, FMAPRSSystem::tr("Circle 0")},
  {FMAPRSSystem::Icon::Circle1, FMAPRSSystem::tr("Circle 1")},
  {FMAPRSSystem::Icon::Circle2, FMAPRSSystem::tr("Circle 2")},
  {FMAPRSSystem::Icon::Circle3, FMAPRSSystem::tr("Circle 3")},
  {FMAPRSSystem::Icon::Circle4, FMAPRSSystem::tr("Circle 4")},
  {FMAPRSSystem::Icon::Circle5, FMAPRSSystem::tr("Circle 5")},
  {FMAPRSSystem::Icon::Circle6, FMAPRSSystem::tr("Circle 6")},
  {FMAPRSSystem::Icon::Circle7, FMAPRSSystem::tr("Circle 7")},
  {FMAPRSSystem::Icon::Circle8, FMAPRSSystem::tr("Circle 8")},
  {FMAPRSSystem::Icon::Circle9, FMAPRSSystem::tr("Circle 9")},
  {FMAPRSSystem::Icon::Fire, FMAPRSSystem::tr("Fire")},
  {FMAPRSSystem::Icon::Campground, FMAPRSSystem::tr("Campground")},
  {FMAPRSSystem::Icon::Motorcycle, FMAPRSSystem::tr("Motorcycle")},
  {FMAPRSSystem::Icon::RailEngine, FMAPRSSystem::tr("Rail engine")},
  {FMAPRSSystem::Icon::Car, FMAPRSSystem::tr("Car")},
  {FMAPRSSystem::Icon::FileServer, FMAPRSSystem::tr("File server")},
  {FMAPRSSystem::Icon::HCFuture, FMAPRSSystem::tr("HC Future")},
  {FMAPRSSystem::Icon::AidStation, FMAPRSSystem::tr("Aid station")},
  {FMAPRSSystem::Icon::BBS, FMAPRSSystem::tr("BBS")},
  {FMAPRSSystem::Icon::Canoe, FMAPRSSystem::tr("Canoe")},
  {FMAPRSSystem::Icon::Eyeball, FMAPRSSystem::tr("Eyeball")},
  {FMAPRSSystem::Icon::Tractor, FMAPRSSystem::tr("Tractor")},
  {FMAPRSSystem::Icon::GridSquare, FMAPRSSystem::tr("Grid Square")},
  {FMAPRSSystem::Icon::Hotel, FMAPRSSystem::tr("Hotel")},
  {FMAPRSSystem::Icon::TCPIP, FMAPRSSystem::tr("TCP/IP")},
  {FMAPRSSystem::Icon::School, FMAPRSSystem::tr("School")},
  {FMAPRSSystem::Icon::Logon, FMAPRSSystem::tr("Logon")},
  {FMAPRSSystem::Icon::MacOS, FMAPRSSystem::tr("MacOS")},
  {FMAPRSSystem::Icon::NTSStation, FMAPRSSystem::tr("NTS station")},
  {FMAPRSSystem::Icon::Balloon, FMAPRSSystem::tr("Balloon")},
  {FMAPRSSystem::Icon::PoliceCar, FMAPRSSystem::tr("Police car")},
  {FMAPRSSystem::Icon::TBD, FMAPRSSystem::tr("TBD")},
  {FMAPRSSystem::Icon::RV, FMAPRSSystem::tr("RV")},
  {FMAPRSSystem::Icon::Shuttle, FMAPRSSystem::tr("Shuttle")},
  {FMAPRSSystem::Icon::SSTV, FMAPRSSystem::tr("SSTV")},
  {FMAPRSSystem::Icon::Bus, FMAPRSSystem::tr("Bus")},
  {FMAPRSSystem::Icon::ATV, FMAPRSSystem::tr("ATV")},
  {FMAPRSSystem::Icon::WXService, FMAPRSSystem::tr("Weather service")},
  {FMAPRSSystem::Icon::Helo, FMAPRSSystem::tr("Helo")},
  {FMAPRSSystem::Icon::Yacht, FMAPRSSystem::tr("Yacht")},
  {FMAPRSSystem::Icon::Windows, FMAPRSSystem::tr("MS Windows")},
  {FMAPRSSystem::Icon::Jogger, FMAPRSSystem::tr("Jogger")},
  {FMAPRSSystem::Icon::Triangle, FMAPRSSystem::tr("Triangle")},
  {FMAPRSSystem::Icon::PBBS, FMAPRSSystem::tr("PBBS")},
  {FMAPRSSystem::Icon::LargePlane, FMAPRSSystem::tr("Plane large")},
  {FMAPRSSystem::Icon::WXStation, FMAPRSSystem::tr("Weather station")},
  {FMAPRSSystem::Icon::DishAntenna, FMAPRSSystem::tr("Dish antenna")},
  {FMAPRSSystem::Icon::Ambulance, FMAPRSSystem::tr("Ambulance")},
  {FMAPRSSystem::Icon::Bike, FMAPRSSystem::tr("Bike")},
  {FMAPRSSystem::Icon::ICP, FMAPRSSystem::tr("ICP")},
  {FMAPRSSystem::Icon::FireStation, FMAPRSSystem::tr("Fire station")},
  {FMAPRSSystem::Icon::Horse, FMAPRSSystem::tr("Horse")},
  {FMAPRSSystem::Icon::FireTruck, FMAPRSSystem::tr("Fire truck")},
  {FMAPRSSystem::Icon::Glider, FMAPRSSystem::tr("Glider")},
  {FMAPRSSystem::Icon::Hospital, FMAPRSSystem::tr("Hospital")},
  {FMAPRSSystem::Icon::IOTA, FMAPRSSystem::tr("IOTA")},
  {FMAPRSSystem::Icon::Jeep, FMAPRSSystem::tr("Jeep")},
  {FMAPRSSystem::Icon::SmallTruck, FMAPRSSystem::tr("Truck small")},
  {FMAPRSSystem::Icon::Laptop, FMAPRSSystem::tr("Laptop")},
  {FMAPRSSystem::Icon::MicE, FMAPRSSystem::tr("Mic-E")},
  {FMAPRSSystem::Icon::Node, FMAPRSSystem::tr("Node")},
  {FMAPRSSystem::Icon::EOC, FMAPRSSystem::tr("EOC")},
  {FMAPRSSystem::Icon::Rover, FMAPRSSystem::tr("Rover")},
  {FMAPRSSystem::Icon::Grid, FMAPRSSystem::tr("Grid")},
  {FMAPRSSystem::Icon::Antenna, FMAPRSSystem::tr("Antenna")},
  {FMAPRSSystem::Icon::PowerBoat, FMAPRSSystem::tr("Power boat")},
  {FMAPRSSystem::Icon::TruckStop, FMAPRSSystem::tr("Truck stop")},
  {FMAPRSSystem::Icon::TruckLarge, FMAPRSSystem::tr("Truck large")},
  {FMAPRSSystem::Icon::Van, FMAPRSSystem::tr("Van")},
  {FMAPRSSystem::Icon::Water, FMAPRSSystem::tr("Water")},
  {FMAPRSSystem::Icon::XAPRS, FMAPRSSystem::tr("XAPRS")},
  {FMAPRSSystem::Icon::Yagi, FMAPRSSystem::tr("Yagi")},
  {FMAPRSSystem::Icon::Shelter, FMAPRSSystem::tr("Shelter")}};



APRSSystemDialog::APRSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myAPRS(new FMAPRSSystem()), _aprs(nullptr),
    ui(new Ui::aprssystemdialog), _icons0(":/icons/aprs/table0.png")
{
  setWindowTitle(tr("Create APRS system"));
  construct();
}

APRSSystemDialog::APRSSystemDialog(Config *config, FMAPRSSystem *aprs, QWidget *parent)
  : QDialog(parent), _config(config), _myAPRS(new FMAPRSSystem()), _aprs(aprs),
    ui(new Ui::aprssystemdialog), _icons0(":/icons/aprs/table0.png")
{
  setWindowTitle(tr("Edit APRS system"));

  if (_aprs)
    _myAPRS->copy(*_aprs);

  construct();
}

APRSSystemDialog::~APRSSystemDialog() {
  delete ui;
}


void
APRSSystemDialog::construct() {
  // Construct UI
  ui->setupUi(this);
  Settings settings;

  // Setup name
  ui->name->setText(_myAPRS->name());

  // Setup analog revert channels
  ui->channel->addItem(tr("[Selected]"), QVariant::fromValue<FMChannel*>(nullptr));
  if (! _myAPRS->hasRevertChannel())
    ui->channel->setCurrentIndex(0);
  for (int i=0, j=1; i<_config->channelList()->count(); i++) {
    if (! _config->channelList()->channel(i)->is<FMChannel>())
      continue;
    FMChannel *ch = _config->channelList()->channel(i)->as<FMChannel>();
    ui->channel->addItem(ch->name(), QVariant::fromValue(ch));
    if (_myAPRS->hasRevertChannel() && (_myAPRS->revertChannel() == ch))
      ui->channel->setCurrentIndex(j);
    j++;
  }

  ui->source->setText(_myAPRS->source());
  ui->srcSSID->setValue(_myAPRS->srcSSID());
  ui->destination->setText(_myAPRS->destination());
  ui->destSSID->setValue(_myAPRS->destSSID());
  ui->path->setText(_myAPRS->path());

  // Setup icons
  for (int i=0; i<aprsIconTable.size(); i++) {
    const QPair<FMAPRSSystem::Icon, QString> &item = aprsIconTable[i];
    ui->icon->addItem(aprsIcon(item.first), item.second, unsigned(item.first));
    if (_myAPRS->icon() == item.first)
      ui->icon->setCurrentIndex(i);
  }

  if (_myAPRS->periodDisabled())
    ui->updatePeriod->setValue(0);
  else
    ui->updatePeriod->setValue(_myAPRS->period().seconds());
  ui->message->setText(_myAPRS->message());

  ui->extensionView->setObjectName("aprsSystemExtension");
  ui->extensionView->setObject(_myAPRS, _config);
  if (! settings.showExtensions())
    ui->tabWidget->tabBar()->hide();
}

FMAPRSSystem *
APRSSystemDialog::aprsSystem() {
  _myAPRS->setName(ui->name->text().simplified());
  if (ui->channel->currentData().isNull())
    _myAPRS->resetRevertChannel();
  else
    _myAPRS->setRevertChannel(ui->channel->currentData().value<FMChannel*>());
  _myAPRS->setSource(ui->source->text().simplified(), ui->srcSSID->value());
  _myAPRS->setDestination(ui->destination->text().simplified(), ui->destSSID->value());
  _myAPRS->setPath(ui->path->text().simplified());
  _myAPRS->setIcon(FMAPRSSystem::Icon(ui->icon->currentData().toUInt()));
  if (0 == ui->updatePeriod->value())
    _myAPRS->disablePeriod();
  else
    _myAPRS->setPeriod(Interval::fromSeconds(ui->updatePeriod->value()));
  _myAPRS->setMessage(ui->message->text().simplified());

  FMAPRSSystem *system = _myAPRS;
  if (_aprs) {
    _aprs->copy(*_myAPRS);
    system = _aprs;
  } else {
    _myAPRS->setParent(nullptr);
  }

  return system;
}

QIcon
APRSSystemDialog::aprsIcon(FMAPRSSystem::Icon icon) {
  unsigned table = (FMAPRSSystem::TABLE_MASK & (unsigned)icon);
  unsigned idx   = (FMAPRSSystem::ICON_MASK & (unsigned)icon);
  unsigned row   = idx/16, col = idx % 16;

  if (FMAPRSSystem::PRIMARY_TABLE == table) {
    return QPixmap::fromImage(_icons0.copy(24*col, 24*row, 24,24));
  }

  return QPixmap::fromImage(_icons0.copy(24,0,24,24));
}
