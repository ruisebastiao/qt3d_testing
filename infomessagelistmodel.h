#ifndef INFOMESSAGELISTMODEL_H
#define INFOMESSAGELISTMODEL_H

#include "infomessage.h"

#include <qabstractitemmodel.h>



class InfoMessageListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_ENUMS(InfoMessageRoles)

public:
    enum InfoMessageRoles {
        InfoMessageRole = Qt::UserRole + 1
    };

    InfoMessageListModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addItem(InfoMessage* item);
private:
    QList<InfoMessage*> m_internalList;

    // QAbstractItemModel interface
public:
    QHash<int, QByteArray> roleNames() const;
    int length();
    InfoMessage* at(int index);



};

#endif // INFOMESSAGELISTMODEL_H
