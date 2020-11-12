#include "sliceListItem.h"


SliceListItem::SliceListItem(const QString file, const QString info) : QListWidgetItem(file + " " + info)
{
    this->file = file;
    this->info = info;
}
