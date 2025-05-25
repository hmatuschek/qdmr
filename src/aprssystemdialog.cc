#include "aprssystemdialog.hh"
#include "ui_aprssystemdialog.h"
#include "settings.hh"


static QVector<QPair<APRSSystem::Icon, QString>> aprsIconTable{
  {APRSSystem::Icon::None, APRSSystem::tr("[None]")},
  {APRSSystem::Icon::PoliceStation, APRSSystem::tr("Police station")},
  {APRSSystem::Icon::Digipeater, APRSSystem::tr("Digipeater")},
  {APRSSystem::Icon::Phone, APRSSystem::tr("Phone")},
  {APRSSystem::Icon::DXCluster, APRSSystem::tr("DX cluster")},
  {APRSSystem::Icon::HFGateway, APRSSystem::tr("HF gateway")},
  {APRSSystem::Icon::SmallPlane, APRSSystem::tr("Plane small")},
  {APRSSystem::Icon::MobileSatelliteStation, APRSSystem::tr("Mobile Satellite station")},
  {APRSSystem::Icon::WheelChair, APRSSystem::tr("Wheel Chair")},
  {APRSSystem::Icon::Snowmobile, APRSSystem::tr("Snowmobile")},
  {APRSSystem::Icon::RedCross, APRSSystem::tr("Red cross")},
  {APRSSystem::Icon::BoyScout, APRSSystem::tr("Boy scout")},
  {APRSSystem::Icon::Home,  APRSSystem::tr("Home")},
  {APRSSystem::Icon::X, APRSSystem::tr("X")},
  {APRSSystem::Icon::RedDot, APRSSystem::tr("Red dot")},
  {APRSSystem::Icon::Circle0, APRSSystem::tr("Circle 0")},
  {APRSSystem::Icon::Circle1, APRSSystem::tr("Circle 1")},
  {APRSSystem::Icon::Circle2, APRSSystem::tr("Circle 2")},
  {APRSSystem::Icon::Circle3, APRSSystem::tr("Circle 3")},
  {APRSSystem::Icon::Circle4, APRSSystem::tr("Circle 4")},
  {APRSSystem::Icon::Circle5, APRSSystem::tr("Circle 5")},
  {APRSSystem::Icon::Circle6, APRSSystem::tr("Circle 6")},
  {APRSSystem::Icon::Circle7, APRSSystem::tr("Circle 7")},
  {APRSSystem::Icon::Circle8, APRSSystem::tr("Circle 8")},
  {APRSSystem::Icon::Circle9, APRSSystem::tr("Circle 9")},
  {APRSSystem::Icon::Fire, APRSSystem::tr("Fire")},
  {APRSSystem::Icon::Campground, APRSSystem::tr("Campground")},
  {APRSSystem::Icon::Motorcycle, APRSSystem::tr("Motorcycle")},
  {APRSSystem::Icon::RailEngine, APRSSystem::tr("Rail engine")},
  {APRSSystem::Icon::Car, APRSSystem::tr("Car")},
  {APRSSystem::Icon::FileServer, APRSSystem::tr("File server")},
  {APRSSystem::Icon::HCFuture, APRSSystem::tr("HC Future")},
  {APRSSystem::Icon::AidStation, APRSSystem::tr("Aid station")},
  {APRSSystem::Icon::BBS, APRSSystem::tr("BBS")},
  {APRSSystem::Icon::Canoe, APRSSystem::tr("Canoe")},
  {APRSSystem::Icon::Eyeball, APRSSystem::tr("Eyeball")},
  {APRSSystem::Icon::Tractor, APRSSystem::tr("Tractor")},
  {APRSSystem::Icon::GridSquare, APRSSystem::tr("Grid Square")},
  {APRSSystem::Icon::Hotel, APRSSystem::tr("Hotel")},
  {APRSSystem::Icon::TCPIP, APRSSystem::tr("TCP/IP")},
  {APRSSystem::Icon::School, APRSSystem::tr("School")},
  {APRSSystem::Icon::Logon, APRSSystem::tr("Logon")},
  {APRSSystem::Icon::MacOS, APRSSystem::tr("MacOS")},
  {APRSSystem::Icon::NTSStation, APRSSystem::tr("NTS station")},
  {APRSSystem::Icon::Balloon, APRSSystem::tr("Balloon")},
  {APRSSystem::Icon::PoliceCar, APRSSystem::tr("Police car")},
  {APRSSystem::Icon::TBD, APRSSystem::tr("TBD")},
  {APRSSystem::Icon::RV, APRSSystem::tr("RV")},
  {APRSSystem::Icon::Shuttle, APRSSystem::tr("Shuttle")},
  {APRSSystem::Icon::SSTV, APRSSystem::tr("SSTV")},
  {APRSSystem::Icon::Bus, APRSSystem::tr("Bus")},
  {APRSSystem::Icon::ATV, APRSSystem::tr("ATV")},
  {APRSSystem::Icon::WXService, APRSSystem::tr("Weather service")},
  {APRSSystem::Icon::Helo, APRSSystem::tr("Helo")},
  {APRSSystem::Icon::Yacht, APRSSystem::tr("Yacht")},
  {APRSSystem::Icon::Windows, APRSSystem::tr("MS Windows")},
  {APRSSystem::Icon::Jogger, APRSSystem::tr("Jogger")},
  {APRSSystem::Icon::Triangle, APRSSystem::tr("Triangle")},
  {APRSSystem::Icon::PBBS, APRSSystem::tr("PBBS")},
  {APRSSystem::Icon::LargePlane, APRSSystem::tr("Plane large")},
  {APRSSystem::Icon::WXStation, APRSSystem::tr("Weather station")},
  {APRSSystem::Icon::DishAntenna, APRSSystem::tr("Dish antenna")},
  {APRSSystem::Icon::Ambulance, APRSSystem::tr("Ambulance")},
  {APRSSystem::Icon::Bike, APRSSystem::tr("Bike")},
  {APRSSystem::Icon::ICP, APRSSystem::tr("ICP")},
  {APRSSystem::Icon::FireStation, APRSSystem::tr("Fire station")},
  {APRSSystem::Icon::Horse, APRSSystem::tr("Horse")},
  {APRSSystem::Icon::FireTruck, APRSSystem::tr("Fire truck")},
  {APRSSystem::Icon::Glider, APRSSystem::tr("Glider")},
  {APRSSystem::Icon::Hospital, APRSSystem::tr("Hospital")},
  {APRSSystem::Icon::IOTA, APRSSystem::tr("IOTA")},
  {APRSSystem::Icon::Jeep, APRSSystem::tr("Jeep")},
  {APRSSystem::Icon::SmallTruck, APRSSystem::tr("Truck small")},
  {APRSSystem::Icon::Laptop, APRSSystem::tr("Laptop")},
  {APRSSystem::Icon::MicE, APRSSystem::tr("Mic-E")},
  {APRSSystem::Icon::Node, APRSSystem::tr("Node")},
  {APRSSystem::Icon::EOC, APRSSystem::tr("EOC")},
  {APRSSystem::Icon::Rover, APRSSystem::tr("Rover")},
  {APRSSystem::Icon::Grid, APRSSystem::tr("Grid")},
  {APRSSystem::Icon::Antenna, APRSSystem::tr("Antenna")},
  {APRSSystem::Icon::PowerBoat, APRSSystem::tr("Power boat")},
  {APRSSystem::Icon::TruckStop, APRSSystem::tr("Truck stop")},
  {APRSSystem::Icon::TruckLarge, APRSSystem::tr("Truck large")},
  {APRSSystem::Icon::Van, APRSSystem::tr("Van")},
  {APRSSystem::Icon::Water, APRSSystem::tr("Water")},
  {APRSSystem::Icon::XAPRS, APRSSystem::tr("XAPRS")},
  {APRSSystem::Icon::Yagi, APRSSystem::tr("Yagi")},
  {APRSSystem::Icon::Shelter, APRSSystem::tr("Shelter")}};



APRSSystemDialog::APRSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myAPRS(new APRSSystem()), _aprs(nullptr),
    ui(new Ui::aprssystemdialog), _icons0(":/icons/aprs/table0.png")
{
  setWindowTitle(tr("Create APRS system"));
  construct();
}

APRSSystemDialog::APRSSystemDialog(Config *config, APRSSystem *aprs, QWidget *parent)
  : QDialog(parent), _config(config), _myAPRS(new APRSSystem()), _aprs(aprs),
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

  // Setup analog channels
  ui->channel->addItem(tr("[Selected]"), QVariant::fromValue((FMChannel *)nullptr));
  for (int i=0, j=0; i<_config->channelList()->count(); i++) {
    if (! _config->channelList()->channel(i)->is<FMChannel>())
      continue;
    FMChannel *ch = _config->channelList()->channel(i)->as<FMChannel>();
    ui->channel->addItem(ch->name(), QVariant::fromValue(ch));
    if (_myAPRS->revertChannel() == ch)
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
    const QPair<APRSSystem::Icon, QString> &item = aprsIconTable[i];
    ui->icon->addItem(aprsIcon(item.first), item.second, unsigned(item.first));
    if (_myAPRS->icon() == item.first)
      ui->icon->setCurrentIndex(i);
  }

  ui->updatePeriod->setValue(_myAPRS->period());
  ui->message->setText(_myAPRS->message());

  ui->extensionView->setObjectName("aprsSystemExtension");
  ui->extensionView->setObject(_myAPRS, _config);
  if (! settings.showExtensions())
    ui->tabWidget->tabBar()->hide();
}

APRSSystem *
APRSSystemDialog::aprsSystem() {
  _myAPRS->setName(ui->name->text().simplified());
  _myAPRS->setRevertChannel(ui->channel->currentData().value<FMChannel*>());
  _myAPRS->setSource(ui->source->text().simplified(), ui->srcSSID->value());
  _myAPRS->setDestination(ui->destination->text().simplified(), ui->destSSID->value());
  _myAPRS->setPath(ui->path->text().simplified());
  _myAPRS->setIcon(APRSSystem::Icon(ui->icon->currentData().toUInt()));
  _myAPRS->setPeriod(ui->updatePeriod->value());
  _myAPRS->setMessage(ui->message->text().simplified());

  APRSSystem *system = _myAPRS;
  if (_aprs) {
    _aprs->copy(*_myAPRS);
    system = _aprs;
  } else {
    _myAPRS->setParent(nullptr);
  }

  return system;
}

QIcon
APRSSystemDialog::aprsIcon(APRSSystem::Icon icon) {
  unsigned table = (APRSSystem::TABLE_MASK & (unsigned)icon);
  unsigned idx   = (APRSSystem::ICON_MASK & (unsigned)icon);
  unsigned row   = idx/16, col = idx % 16;

  if (APRSSystem::PRIMARY_TABLE == table) {
    return QPixmap::fromImage(_icons0.copy(24*col, 24*row, 24,24));
  }

  return QPixmap::fromImage(_icons0.copy(24,0,24,24));
}
