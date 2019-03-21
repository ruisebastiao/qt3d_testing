#ifndef INFOMESSAGE_H
#define INFOMESSAGE_H

class InfoWindow;

#include <QObject>

class InfoMessage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(InfoWindow* infoWindow READ infoWindow WRITE setInfoWindow NOTIFY infoWindowChanged)

    Q_PROPERTY(QString entityName READ entityName WRITE setEntityName NOTIFY entityNameChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)


public:
    explicit InfoMessage(QObject *parent = nullptr);
    explicit InfoMessage(QString entityName,QString message);

    QString entityName() const;

    QString message() const
    {
        return m_message;
    }

    InfoWindow* infoWindow() const;

signals:

    void entityNameChanged(QString entityName);

    void messageChanged(QString message);

    void infoWindowChanged(InfoWindow* infoWindow);

public slots:

void setEntityName(QString entityName);

void setMessage(QString message);

void setInfoWindow(InfoWindow* infoWindow);

private:

QString m_entityName="";
QString m_message="";
InfoWindow* m_infoWindow=nullptr;
};



#endif // INFOMESSAGE_H
