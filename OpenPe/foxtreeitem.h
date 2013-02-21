#ifndef FOXTREEITEM_H
#define FOXTREEITEM_H

#include <QTreeWidgetItem>
#include "qstructitem.h"
class FoxTreeItem :QObject, public QTreeWidgetItem
{
	Q_OBJECT

public:
	FoxTreeItem(QTreeWidget *parent = 0);
	FoxTreeItem(QStructItem*);
	~FoxTreeItem();
	QStructItem* st_item;
private:
	
};

#endif // FOXTREEITEM_H
