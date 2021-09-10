#include "aprssystemdialog.hh"
#include "ui_aprssystemdialog.h"

static QVector<QPair<APRSSystem::Icon, QString>> aprsIconTable{
  {APRSSystem::APRS_ICON_NO_SYMBOL, QObject::tr("[None]")},
  {APRSSystem::APRS_ICON_POLICE_STN, QObject::tr("Police station")},
  {APRSSystem::APRS_ICON_DIGI, QObject::tr("Digipeater")},
  {APRSSystem::APRS_ICON_PHONE, QObject::tr("Phone")},
  {APRSSystem::APRS_ICON_DX_CLUSTER, QObject::tr("DX cluster")},
  {APRSSystem::APRS_ICON_HF_GATEWAY, QObject::tr("HF gateway")},
  {APRSSystem::APRS_ICON_PLANE_SMALL, QObject::tr("Plane small")},
  {APRSSystem::APRS_ICON_MOB_SAT_STN, QObject::tr("Mobile Satellite station")},
  {APRSSystem::APRS_ICON_WHEEL_CHAIR, QObject::tr("Wheel Chair")},
  {APRSSystem::APRS_ICON_SNOWMOBILE, QObject::tr("Snowmobile")},
  {APRSSystem::APRS_ICON_RED_CROSS, QObject::tr("Red cross")},
  {APRSSystem::APRS_ICON_BOY_SCOUT, QObject::tr("Boy scout")},
  {APRSSystem::APRS_ICON_HOME,  QObject::tr("Home")},
  {APRSSystem::APRS_ICON_X, QObject::tr("X")},
  {APRSSystem::APRS_ICON_RED_DOT, QObject::tr("Red dot")},
  {APRSSystem::APRS_ICON_CIRCLE_0, QObject::tr("Circle 0")},
  {APRSSystem::APRS_ICON_CIRCLE_1, QObject::tr("Circle 1")},
  {APRSSystem::APRS_ICON_CIRCLE_2, QObject::tr("Circle 2")},
  {APRSSystem::APRS_ICON_CIRCLE_3, QObject::tr("Circle 3")},
  {APRSSystem::APRS_ICON_CIRCLE_4, QObject::tr("Circle 4")},
  {APRSSystem::APRS_ICON_CIRCLE_5, QObject::tr("Circle 5")},
  {APRSSystem::APRS_ICON_CIRCLE_6, QObject::tr("Circle 6")},
  {APRSSystem::APRS_ICON_CIRCLE_7, QObject::tr("Circle 7")},
  {APRSSystem::APRS_ICON_CIRCLE_8, QObject::tr("Circle 8")},
  {APRSSystem::APRS_ICON_CIRCLE_9, QObject::tr("Circle 9")},
  {APRSSystem::APRS_ICON_FIRE, QObject::tr("Fire")},
  {APRSSystem::APRS_ICON_CAMPGROUND, QObject::tr("Campground")},
  {APRSSystem::APRS_ICON_MOTORCYCLE, QObject::tr("Motorcycle")},
  {APRSSystem::APRS_ICON_RAIL_ENGINE, QObject::tr("Rail engine")},
  {APRSSystem::APRS_ICON_CAR, QObject::tr("Car")},
  {APRSSystem::APRS_ICON_FILE_SERVER, QObject::tr("File server")},
  {APRSSystem::APRS_ICON_HC_FUTURE, QObject::tr("HC Future")},
  {APRSSystem::APRS_ICON_AID_STN, QObject::tr("Aid station")},
  {APRSSystem::APRS_ICON_BBS, QObject::tr("BBS")},
  {APRSSystem::APRS_ICON_CANOE, QObject::tr("Canoe")},
  {APRSSystem::APRS_ICON_EYEBALL, QObject::tr("Eyeball")},
  {APRSSystem::APRS_ICON_TRACTOR, QObject::tr("Tractor")},
  {APRSSystem::APRS_ICON_GRID_SQ, QObject::tr("Grid Square")},
  {APRSSystem::APRS_ICON_HOTEL, QObject::tr("Hotel")},
  {APRSSystem::APRS_ICON_TCP_IP, QObject::tr("TCP/IP")},
  {APRSSystem::APRS_ICON_SCHOOL, QObject::tr("School")},
  {APRSSystem::APRS_ICON_USER_LOGON, QObject::tr("Logon")},
  {APRSSystem::APRS_ICON_MAC, QObject::tr("MacOS")},
  {APRSSystem::APRS_ICON_NTS_STN, QObject::tr("NTS station")},
  {APRSSystem::APRS_ICON_BALLOON, QObject::tr("Balloon")},
  {APRSSystem::APRS_ICON_POLICE, QObject::tr("Police car")},
  {APRSSystem::APRS_ICON_TBD, QObject::tr("TBD")},
  {APRSSystem::APRX_ICON_RV, QObject::tr("RV")},
  {APRSSystem::APRS_ICON_SHUTTLE, QObject::tr("Shuttle")},
  {APRSSystem::APRS_ICON_SSTV, QObject::tr("SSTV")},
  {APRSSystem::APRS_ICON_BUS, QObject::tr("Bus")},
  {APRSSystem::APRS_ICON_ATV, QObject::tr("ATV")},
  {APRSSystem::APRS_ICON_WX_SERVICE, QObject::tr("Weather service")},
  {APRSSystem::APRS_ICON_HELO, QObject::tr("Helo")},
  {APRSSystem::APRS_ICON_YACHT, QObject::tr("Yacht")},
  {APRSSystem::APRS_ICON_WIN, QObject::tr("MS Windows")},
  {APRSSystem::APRS_ICON_JOGGER, QObject::tr("Jogger")},
  {APRSSystem::APRS_ICON_TRIANGLE, QObject::tr("Triangle")},
  {APRSSystem::APRS_ICON_PBBS, QObject::tr("PBBS")},
  {APRSSystem::APRS_ICON_PLANE_LARGE, QObject::tr("Pane large")},
  {APRSSystem::APRS_ICON_WX_STN, QObject::tr("Weather station")},
  {APRSSystem::APRS_ICON_DISH_ANT, QObject::tr("Dish antenna")},
  {APRSSystem::APRS_ICON_AMBULANCE, QObject::tr("Ambulance")},
  {APRSSystem::APRS_ICON_BIKE, QObject::tr("Bike")},
  {APRSSystem::APRS_ICON_ICP, QObject::tr("ICP")},
  {APRSSystem::APRS_ICON_FIRE_STATION, QObject::tr("Fire station")},
  {APRSSystem::APRS_ICON_HORSE, QObject::tr("Horse")},
  {APRSSystem::APRS_ICON_FIRE_TRUCK, QObject::tr("Fire truck")},
  {APRSSystem::APRS_ICON_GLIDER, QObject::tr("Glider")},
  {APRSSystem::APRS_ICON_HOSPITAL, QObject::tr("Hospital")},
  {APRSSystem::APRS_ICON_IOTA, QObject::tr("IOTA")},
  {APRSSystem::APRS_ICON_JEEP, QObject::tr("Jeep")},
  {APRSSystem::APRS_ICON_TRUCK_SMALL, QObject::tr("Truck small")},
  {APRSSystem::APRS_ICON_LAPTOP, QObject::tr("Laptop")},
  {APRSSystem::APRS_ICON_MIC_E, QObject::tr("Mic-E")},
  {APRSSystem::APRS_ICON_NODE, QObject::tr("Node")},
  {APRSSystem::APRS_ICON_EOC, QObject::tr("EOC")},
  {APRSSystem::APRS_ICON_ROVER, QObject::tr("Rover")},
  {APRSSystem::APRS_ICON_GRID, QObject::tr("Grid")},
  {APRSSystem::APRS_ICON_ANTENNA, QObject::tr("Antenna")},
  {APRSSystem::APRS_ICON_POWER_BOAT, QObject::tr("Power boat")},
  {APRSSystem::APRS_ICON_TRUCK_STOP, QObject::tr("Truck stop")},
  {APRSSystem::APRS_ICON_TUCK_LARGE, QObject::tr("Truck large")},
  {APRSSystem::APRS_ICON_VAN, QObject::tr("Van")},
  {APRSSystem::APRS_ICON_WATER, QObject::tr("Water")},
  {APRSSystem::APRS_ICON_XAPRS, QObject::tr("XAPRS")},
  {APRSSystem::APRS_ICON_YAGI, QObject::tr("Yagi")},
  {APRSSystem::APRS_ICON_SHELTER, QObject::tr("Shelter")}};



APRSSystemDialog::APRSSystemDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _aprs(nullptr), ui(new Ui::aprssystemdialog),
    _icons0(":/icons/aprs/table0.png")
{
  construct();
}

APRSSystemDialog::APRSSystemDialog(Config *config, APRSSystem *aprs, QWidget *parent)
  : QDialog(parent), _config(config), _aprs(aprs), ui(new Ui::aprssystemdialog),
    _icons0(":/icons/aprs/table0.png")
{
  construct();
}

APRSSystemDialog::~APRSSystemDialog() {
  delete ui;
}


void
APRSSystemDialog::construct() {
  // Construct UI
  ui->setupUi(this);

  // Setup name
  if (_aprs)
    ui->name->setText(_aprs->name());

  // Setup analog channels
  for (int i=0, j=0; i<_config->channelList()->count(); i++) {
    if (! _config->channelList()->channel(i)->is<AnalogChannel>())
      continue;
    AnalogChannel *ch = _config->channelList()->channel(i)->as<AnalogChannel>();
    ui->channel->addItem(ch->name(), QVariant::fromValue(ch));
    if (_aprs && (_aprs->revertChannel() == ch))
      ui->channel->setCurrentIndex(j);
    j++;
  }

  // Setup source
  if (_aprs) {
    ui->source->setText(_aprs->source());
    ui->srcSSID->setValue(_aprs->srcSSID());
  } else if (_config->radioIDs()->defaultId()) {
    ui->source->setText(_config->radioIDs()->defaultId()->name());
  }

  // Setup dest
  if (_aprs) {
    ui->destination->setText(_aprs->destination());
    ui->destSSID->setValue(_aprs->destSSID());
  }

  // Setup path
  if (_aprs) {
    ui->path->setText(_aprs->path());
  }

  // Setup icons
  for (int i=0; i<aprsIconTable.size(); i++) {
    const QPair<APRSSystem::Icon, QString> &item = aprsIconTable[i];
    ui->icon->addItem(aprsIcon(item.first), item.second, uint(item.first));
    if (_aprs && (_aprs->icon() == item.first))
      ui->icon->setCurrentIndex(i);
  }

  // Setup update period
  if (_aprs)
    ui->updatePeriod->setValue(_aprs->period());

  // Setup message
  if (_aprs)
    ui->message->setText(_aprs->message());
}

APRSSystem *
APRSSystemDialog::aprsSystem() {
  if (_aprs) {
    _aprs->setName(ui->name->text().simplified());
    _aprs->setRevertChannel(ui->channel->currentData().value<AnalogChannel*>());
    _aprs->setSource(ui->source->text().simplified(), ui->srcSSID->value());
    _aprs->setDestination(ui->destination->text().simplified(), ui->destSSID->value());
    _aprs->setIcon(APRSSystem::Icon(ui->icon->currentData().toUInt()));
    _aprs->setPeriod(ui->updatePeriod->value());
    _aprs->setMessage(ui->message->text().simplified());

    return _aprs;
  }

  return new APRSSystem(ui->name->text().simplified(), ui->channel->currentData().value<AnalogChannel *>(),
                        ui->destination->text().simplified(), ui->destSSID->value(),
                        ui->source->text().simplified(), ui->srcSSID->value(),
                        ui->path->text().simplified(),
                        APRSSystem::Icon(ui->icon->currentData().toUInt()),
                        ui->message->text().simplified());
}

QIcon
APRSSystemDialog::aprsIcon(APRSSystem::Icon icon) {
  uint table = (APRSSystem::TABLE_MASK & icon);
  uint idx   = (APRSSystem::ICON_MASK & icon);
  uint row   = idx/16, col = idx % 16;

  if (APRSSystem::PRIMARY_TABLE == table) {
    return QPixmap::fromImage(_icons0.copy(24*col, 24*row, 24,24));
  }

  return QPixmap::fromImage(_icons0.copy(24,0,24,24));
}
