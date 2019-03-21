#include "infomessagelistmodel.h"

InfoMessageListModel::InfoMessageListModel(QObject *parent)
{

}

int InfoMessageListModel::rowCount(const QModelIndex &parent) const
{
    return m_internalList.length();
}

QVariant InfoMessageListModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_internalList.length())
    {
        return QVariant();
    }

    InfoMessage *infomessage= m_internalList.at(index.row());
    if (role == InfoMessageRole){
        return QVariant::fromValue(infomessage);
    }


    return QVariant();
}

void InfoMessageListModel::addItem(InfoMessage *item)
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));   // kindly provided by superclass

    m_internalList.append(item);

    endInsertRows();
}

QHash<int, QByteArray> InfoMessageListModel::roleNames() const
{
    return {
        { InfoMessageRole, "infoMessageItem" }

    };
}

int InfoMessageListModel::length(){
    return m_internalList.length();
}

InfoMessage *InfoMessageListModel::at(int index){
    if(index>=0 && index<m_internalList.length()){
        return m_internalList.at(index);
    }

    return nullptr;
}
