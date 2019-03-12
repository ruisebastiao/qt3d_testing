#ifndef MYSCENELOADER_H
#define MYSCENELOADER_H
#include "sceneitem.h"

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

private:
   QMap<QNodeId, SceneItem *> m_sceneItems;

   void addEntity(Qt3DCore::QEntity *entity, int index = -1, Qt3DCore::QEntity *parent = nullptr);


   Qt3DRender::QObjectPicker *createObjectPickerForEntity(Qt3DCore::QEntity *entity);
};

#endif // MYSCENELOADER_H
