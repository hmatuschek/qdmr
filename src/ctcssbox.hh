#ifndef CTCSSBOX_HH
#define CTCSSBOX_HH

#include <QComboBox>

class CTCSSBox : public QComboBox
{
	Q_OBJECT

public:
	explicit CTCSSBox(QWidget *parent=nullptr);
	double selectedFreq() const;
};

#endif // CTCSSBOX_HH
