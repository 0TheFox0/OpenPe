#include "foxtreeitem.h"

FoxTreeItem::FoxTreeItem(QTreeWidget *parent)
	: QTreeWidgetItem(parent)
{

}
FoxTreeItem::FoxTreeItem(QStructItem* item)
{
	this->st_item = item;
}
FoxTreeItem::~FoxTreeItem()
{

}
