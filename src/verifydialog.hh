#ifndef VERIFYDIALOG_HH
#define VERIFYDIALOG_HH

#include <QDialog>
#include "radio.hh"
#include "ui_verifydialog.h"


class VerifyDialog : public QDialog, private Ui::VerifyDialog
{
  Q_OBJECT
public:

public:
  explicit VerifyDialog(const QList<VerifyIssue> &issues, bool upload, QWidget *parent = nullptr);
};

#endif // VERIFYDIALOG_HH
