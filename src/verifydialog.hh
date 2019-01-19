#ifndef VERIFYDIALOG_HH
#define VERIFYDIALOG_HH

#include <QDialog>
#include "ui_verifydialog.h"

class VerifyIssue {
public:
	typedef enum {
		WARNING, ERROR
	} Type;

public:
	inline VerifyIssue(Type type, const QString &message)
	    : _type(type), _message(message) { }

	inline Type type() const { return _type; }
	inline const QString & message() const { return _message; }

protected:
	Type _type;
	QString _message;
};


class VerifyDialog : public QDialog, private Ui::VerifyDialog
{
  Q_OBJECT
public:

public:
  explicit VerifyDialog(const QList<VerifyIssue> &issues, QWidget *parent = nullptr);
};

#endif // VERIFYDIALOG_HH
