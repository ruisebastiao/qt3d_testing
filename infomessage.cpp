#include "infomessage.h"

#include "infowindow.h"


InfoMessage::InfoMessage(QObject *parent) : QObject(parent)
{

}

InfoMessage::InfoMessage(QString entityName, QString message)
{
    this->setEntityName(entityName);
    this->setMessage(message);

}

QString InfoMessage::entityName() const
{
    return m_entityName;
}

InfoWindow *InfoMessage::infoWindow() const
{
    return m_infoWindow;
}

void InfoMessage::setMessage(QString message)
{
    if (m_message == message)
        return;

    m_message = message;
    emit messageChanged(m_message);
}

void InfoMessage::setInfoWindow(InfoWindow *infoWindow)
{
    if (m_infoWindow == infoWindow)
        return;

    m_infoWindow = infoWindow;
    emit infoWindowChanged(m_infoWindow);
}

void InfoMessage::setEntityName(QString entityName)
{
    if (m_entityName == entityName)
        return;

    m_entityName = entityName;
    emit entityNameChanged(m_entityName);
}
