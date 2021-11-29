#include "aprssystemdialog.hh"
#include "ui_aprssystemdialog.h"
#include "settings.hh"


static QVector<QPair<APRSSystem::Icon, QString>> aprsIconTable{
  {APRSSystem::Icon::None, QObject::tr("[None]")},
  {APRSSystem::Icon::PoliceStation, QObject::tr("Police station")},
  {APRSSystem::Icon::Digipeater, QObject::tr("Digipeater")},
  {APRSSystem::Icon::Phone, QObject::tr("Phone")},
  {APRSSystem::Icon::DXCluster, QObject::tr("DX cluster")},
  {APRSSystem::Icon::HFGateway, QObject::tr("HF gateway")},
  {APRSSystem::Icon::SmallPlane, QObject::tr("Plane small")},
  {APRSSystem::Icon::MobileSatelliteStation, QObject::tr("Mobile Satellite station")},
  {APRSSystem::Icon::WheelChair, QObject::tr("Wheel Chair")},
  {APRSSystem::Icon::Snowmobile, QObject::tr("Snowmobile")},
  {APRSSystem::Icon::RedCross, QObject::tr("Red cross")},
  {APRSSystem::Icon::BoyScout, QObject::tr("Boy scout")},
  {APRSSystem::Icon::Home,  QObject::tr("Home")},
  {APRSSystem::Icon::X, QObject::tr("X")},
  {APRSSystem::Icon::RedDot, QObject::tr("Red dot")},
  {APRSSystem::Icon::Circle0, QObject::tr("Circle 0")},
  {APRSSystem::Icon::Circle1, QObject::tr("Circle 1")},
  {APRSSystem::Icon::Circle2, QObject::tr("Circle 2")},
  {APRSSystem::Icon::Circle3, QObject::tr("Circle 3")},
  {APRSSystem::Icon::Circle4, QObject::tr("Circle 4")},
  {APRSSystem::Icon::Circle5, QObject::tr("Circle 5")},
  {APRSSystem::Icon::Circle6, QObject::tr("Circle 6")},
  {APRSSystem::Icon::Circle7, QObject::tr("Circle 7")},
  {APRSSystem::Icon::Circle8, QObject::tr("Circle 8")},
  {APRSSystem::Icon::Circle9, QObject::tr("Circle 9")},
  {APRSSystem::Icon::Fire, QObject::tr("Fire")},
  {APRSSystem::Icon::Campground, QObject::tr("Campground")},
  {APRSSystem::Icon::Motorcycle, QObject::tr("Motorcycle")},
  {APRSSystem::Icon::RailEngine, QObject::tr("Rail engine")},
  {APRSSystem::Icon::Car, QObject::tr("Car")},
  {APRSSystem::Icon::FileServer, QObject::tr("File server")},
  {APRSSystem::Icon::HCFuture, QObject::tr("HC Future")},
  {APRSSystem::Icon::AidStation, QObject::tr("Aid station")},
  {APRSSystem::Icon::BBS, QObject::tr("BBS")},
  {APRSSystem::Icon::Canoe, QObject::tr("Canoe")},
  {APRSSystem::Icon::Eyeball, QObject::tr("Eyeball")},
  {APRSSystem::Icon::Tractor, QObject::tr("Tractor")},
  {APRSSystem::Icon::GridSquare, QObject::tr("Grid Square")},
  {APRSSystem::Icon::Hotel, QObject::tr("Hotel")},
  {APRSSystem::Icon::TCPIP, QObject::tr("TCP/IP")},
  {APRSSystem::Icon::School, QObject::tr("School")},
  {APRSSystem::Icon::Logon, QObject::tr("Logon")},
  {APRSSystem::Icon::MacOS, QObject::tr("MacOS")},
  {APRSSystem::Icon::NTSStation, QObject::tr("NTS station")},
  {APRSSystem::Icon::Balloon, QObject::tr("Balloon")},
  {APRSSystem::Icon::PoliceCar, QObject::tr("Police car")},
  {APRSSystem::Icon::TBD, QObject::tr("TBD")},
  {APRSSystem::Icon::RV, QObject::tr("RV")},
  {APRSSystem::Icon::Shuttle, QObject::tr("Shuttle")},
  {APRSSystem::Icon::SSTV, QObject::tr("SSTV")},
  {APRSSystem::Icon::Bus, QObject::tr("Bus")},
  {APRSSystem::Icon::ATV, QObject::tr("ATV")},
  {APRSSystem::Icon::WXService, QObject::tr("Weather service")},
  {APRSSystem::Icon::Helo, QObject::tr("Helo")},
  {APRSSystem::Icon::Yacht, QObject::tr("Yacht")},
  {APRSSystem::Icon::Windows, QObject::tr("MS Windows")},
  {APRSSystem::Icon::Jogger, QObject::tr("Jogger")},
  {APRSSystem::Icon::Triangle, QObject::tr("Triangle")},
  {APRSSystem::Icon::PBBS, QObject::tr("PBBS")},
  {APRSSystem::Icon::LargePlane, QObject::tr("Pane large")},
  {APRSSystem::Icon::WXStation, QObject::tr("Weather station")},
  {APRSSystem::Icon::DishAntenna, QObject::tr("Dish antenna")},
  {APRSSystem::Icon::Ambulance, QObject::tr("Ambulance")},
  {APRSSystem::Icon::Bike, QObject::tr("Bike")},
  {APRSSystem::Icon::ICP, QObject::tr("ICP")},
  {APRSSystem::Icon::FireStation, QObject::tr("Fire station")},
  {APRSSystem::Icon::Horse, QObject::tr("Horse")},
  {APRSSystem::Icon::FireTruck, QObject::tr("Fire truck")},
  {APRSSystem::Icon::Glider, QObject::tr("Glider")},
  {APRSSystem::Icon::Hospital, QObject::tr("Hospital")},
  {APRSSystem::Icon::IOTA, QObject::tr("IOTA")},
  {APRSSystem::Icon::Jeep, QObject::tr("Jeep")},
  {APRSSystem::Icon::SmallTruck, QObject::tr("Truck small")},
  {APRSSystem::Icon::Laptop, QObject::tr("Laptop")},
  {APRSSystem::Icon::MicE, QObject::tr("Mic-E")},
  {APRSSystem::Icon::Node, QObject::tr("Node")},
  {APRSSystem::Icon::EOC, QObject::tr("EOC")},
  {APRSSystem::Icon::Rover, QObject::tr("Rover")},
  {APRSSystem::Icon::Grid, QObject::tr("Grid")},
  {APRSSystem::Icon::Antenna, QObject::tr("Antenna")},
  {APRSSystem::Icon::PowerBoat, QObject::tr("Power boat")},
  {APRSSystem::Icon::TruckStop, QObject::tr("Truck stop")},
  {APRSSystem::Icon::TruckLarge, QObject::tr("Truck large")},
  {APRSSystem::Icon::Van, QObject::tr("Van")},
  {APRSSystem::Icon::Water, QObject::tr("Water")},
  {APRSSystem::Icon::XAPRS, QObject::tr("XAPRS")},
  {APRSSystem::Icon::Yagi, QObject::tr("Yagi")},
  {APRSSystem::Icon::Shelter, QObject::tr("Shelter")}};



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
  for (int i=0, j=0; i<_config->channelList()->count(); i++) {
    if (! _config->channelList()->channel(i)->is<AnalogChannel>())
      continue;
    AnalogChannel *ch = _config->channelList()->channel(i)->as<AnalogChannel>();
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

  ui->extensionView->setObject(_myAPRS);
  if (! settings.showCommercialFeatures())
    ui->tabWidget->tabBar()->hide();
}

APRSSystem *
APRSSystemDialog::aprsSystem() {
  _myAPRS->setName(ui->name->text().simplified());
  _myAPRS->setRevertChannel(ui->channel->currentData().value<AnalogChannel*>());
  _myAPRS->setSource(ui->source->text().simplified(), ui->srcSSID->value());
  _myAPRS->setDestination(ui->destination->text().simplified(), ui->destSSID->value());
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
