#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <QObject>
#include <qcamera.h>

using namespace Qt3DRender;

class MyCamera:public QCamera
{
public:
    MyCamera();
};

#endif // MYCAMERA_H
