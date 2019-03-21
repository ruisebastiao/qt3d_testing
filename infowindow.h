#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include "infomessage.h"

#include <QQuickItem>

class InfoWindow : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(InfoMessage* infoMessage READ infoMessage WRITE setInfoMessage NOTIFY infoMessageChanged)

    Q_PROPERTY(bool showConnector READ showConnector WRITE setShowConnector NOTIFY showConnectorChanged)


public:
    InfoWindow(QQuickItem *parent=nullptr);


    bool showConnector() const
    {
        return m_showConnector;
    }

    InfoMessage* infoMessage() const
    {
        return m_infoMessage;
    }

private:



    bool m_showConnector=false;

    InfoMessage* m_infoMessage=nullptr;

signals:


void showConnectorChanged(bool showConnector);

void infoMessageChanged(InfoMessage* infoMessage);

public slots:

void setShowConnector(bool showConnector)
{
    if (m_showConnector == showConnector)
        return;

    m_showConnector = showConnector;
    emit showConnectorChanged(m_showConnector);
}
void setInfoMessage(InfoMessage* infoMessage)
{
    if (m_infoMessage == infoMessage)
        return;

    m_infoMessage = infoMessage;
    emit infoMessageChanged(m_infoMessage);
}
};

#endif // INFOWINDOW_H
