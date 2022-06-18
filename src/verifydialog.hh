#ifndef VERIFYDIALOG_HH
#define VERIFYDIALOG_HH

#include <QDialog>
#include "radio.hh"
#include "ui_verifydialog.h"

class RadioLimitContext;

class VerifyDialog : public QDialog, private Ui::VerifyDialog
{
  Q_OBJECT
public:

public:
  explicit VerifyDialog(const RadioLimitContext &ctx, bool upload, QWidget *parent = nullptr);
};

#endif // VERIFYDIALOG_HH
