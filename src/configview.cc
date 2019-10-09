#include "configview.hh"
#include <QDebug>
#include <QFormLayout>
#include <QIntValidator>
#include <QDateTime>


/* ********************************************************************************************* *
 * Implementation of ConfigView
 * ********************************************************************************************* */
ConfigView::ConfigView(Config *conf, QWidget *parent)
  : QWidget(parent), _config(conf)
{
  _id = new QLineEdit(QString::number(_config->id()));
  _id->setValidator(new QIntValidator());
  _name = new QLineEdit(_config->name());
  _intro1 = new QLineEdit(_config->introLine1());
  _intro2 = new QLineEdit(_config->introLine2());

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("ID"), _id);
  layout->addRow(tr("Name"), _name);
  layout->addRow(tr("Intro Line 1"), _intro1);
  layout->addRow(tr("Intro Line 2"), _intro2);

  this->setLayout(layout);

  connect(_config, SIGNAL(modified()), this, SLOT(onConfigModified()));
  connect(_id, SIGNAL(editingFinished()), this, SLOT(onIdModified()));
  connect(_name, SIGNAL(editingFinished()), this, SLOT(onNameModified()));
  connect(_intro1, SIGNAL(editingFinished()), this, SLOT(onIntro1Modified()));
  connect(_intro2, SIGNAL(editingFinished()), this, SLOT(onIntro2Modified()));
}

void
ConfigView::onConfigModified() {
  if (_id->text().toUInt() != _config->id())
    _id->setText(QString::number(_config->id()));
  if (_name->text() != _config->name())
    _name->setText(_config->name());
  if (_intro1->text() != _config->introLine1())
    _intro1->setText(_config->introLine1());
  if (_intro2->text() != _config->introLine2())
    _intro2->setText(_config->introLine2());
}

void
ConfigView::onIdModified() {
  _config->setId(_id->text().toUInt());
}

void
ConfigView::onNameModified() {
  _config->setName(_name->text().simplified());
}

void
ConfigView::onIntro1Modified() {
  _config->setIntroLine1(_intro1->text().simplified());
}

void
ConfigView::onIntro2Modified() {
  _config->setIntroLine2(_intro2->text().simplified());
}

