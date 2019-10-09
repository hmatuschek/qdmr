#ifndef SCANLISTVIEW_HH
#define SCANLISTVIEW_HH

#include <QWidget>
#include <QListView>

class Config;


class ScanListsView: public QWidget
{
	Q_OBJECT

public:
	ScanListsView(Config *config, QWidget *parent=nullptr);

protected slots:
	void onAddScanList();
	void onRemScanList();
	void onEditScanList(const QModelIndex &idx);

protected:
	Config *_config;
	QListView *_view;
};



#endif // SCANLISTVIEW_HH
