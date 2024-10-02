#ifndef CTCSSBOX_HH
#define CTCSSBOX_HH

#include <QComboBox>
#include "signaling.hh"

class CTCSSBox : public QComboBox
{
	Q_OBJECT

public:
	explicit CTCSSBox(QWidget *parent=nullptr);
	double selectedFreq() const;
};

void populateCTCSSBox(QComboBox *box, const SelectiveCall &call=SelectiveCall());

#endif // CTCSSBOX_HH
