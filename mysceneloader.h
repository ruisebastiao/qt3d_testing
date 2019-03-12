#ifndef MYSCENELOADER_H
#define MYSCENELOADER_H
#include "sceneitem.h"

#include <Qt3DRender/QSceneLoader>
#include <QObject>

using namespace Qt3DCore;
using namespace Qt3DRender;

class MySceneLoader : public Qt3DRender::QSceneLoader
{
    Q_OBJECT

public:
    MySceneLoader(Qt3DCore::QNode *parent = nullptr);

private slots:


public slots:

   void mystatusChanged(Qt3DRender::QSceneLoader::Status status);

private:

};

#endif // MYSCENELOADER_H
