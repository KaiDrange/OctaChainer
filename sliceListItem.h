#ifndef SLICELISTITEM_H
#define SLICELISTITEM_H

#include <QListWidgetItem>

class SliceListItem : public QListWidgetItem
{
public:
    SliceListItem(const QString file, const QString info);
    QString file;
    QString info;
};


#endif // SLICELISTITEM_H
