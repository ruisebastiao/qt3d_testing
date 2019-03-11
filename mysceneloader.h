#ifndef MYSCENELOADER_H
#define MYSCENELOADER_H
#include <QEntity>
#include <QMaterial>
#include <QMesh>

#include <Qt3DRender/QSceneLoader>
#include <QObject>

using namespace Qt3DCore;
using namespace Qt3DRender;

class MySceneLoader : public Qt3DRender::QSceneLoader
{
    Q_OBJECT

public:
    MySceneLoader();

public slots:

   void mystatusChanged(Qt3DRender::QSceneLoader::Status status);

};

#endif // MYSCENELOADER_H
