#include "mysceneloader.h"

#include <QMetalRoughMaterial>
#include <QObjectPicker>
#include <QRenderTarget>
#include <QPickEvent>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

MySceneLoader::MySceneLoader()
{

    //    connect(this,&Qt3DRender::QSceneLoader::statusChanged,this,[&](Qt3DRender::QSceneLoader::Status status){
    //        if(status==QSceneLoader::Status::Ready){
    //            qDebug()<<"teste";
    //        }
    //    });


    connect(this,&QSceneLoader::statusChanged,this,&MySceneLoader::mystatusChanged);

}


void MySceneLoader::mystatusChanged(QSceneLoader::Status status)
{
    if(status==Status::Ready){


        //        Qt3DCore::QEntity *root = entities()[0];

        //        qDebug()<<"Length:"<<entities().length();

        //        for (int var = 0; var < entities().length(); ++var) {
        //            qDebug()<<entities().length();
        //        }
        //        addEntity(root);
        //        QVariant value = this->property("customMaterial");

        //        QMetalRoughMaterial* metalMaterial= new QMetalRoughMaterial(this);
        //        metalMaterial->setRoughness(0.2);
        //        metalMaterial->setMetalness(0.9);

        //        //metalMaterial->se



        auto entitynames=entityNames();
        for (int i = 0; i < entitynames.length(); ++i) {

            QString entityname=entitynames[i];
            auto entityvar=entity(entityname);

            addEntity(entityvar);

        }




    }



}

void MySceneLoader::addEntity(QEntity *entity, int index, QEntity *parent)
{
    if (entity == nullptr)
        return;

    if (parent == nullptr) {
        //make sure that entity has a parent, otherwise make its parent the root entity
        //        if (entity->parentEntity() == nullptr)
        //            entity->setParent(m_rootEntity);
    } else if (entity->parentEntity() != parent) {
        entity->setParent(parent);
    }

    SceneItem *item = m_sceneItems.value(entity->id(), nullptr);
    if (!item) {
        item = new SceneItem(entity, m_sceneItems.value(entity->parentEntity()->id(),
                                                        nullptr), index, this);

        //        if (entity == m_sceneEntity)
        //            m_sceneEntityItem = item;

        m_sceneItems.insert(entity->id(), item);
        //        connect(entity, &QObject::objectNameChanged,
        //                this, &EditorScene::handleEntityNameChange);


        createObjectPickerForEntity(entity);



    }

    //    if (item->itemType() != EditorSceneItem::SceneLoader) {
    foreach (QObject *child, entity->children()) {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity)
            addEntity(childEntity);
    }
    //    }
}


Qt3DRender::QObjectPicker *MySceneLoader::createObjectPickerForEntity(Qt3DCore::QEntity *entity)
{
    Qt3DRender::QObjectPicker *picker = nullptr;
    SceneItem *item = m_sceneItems.value(entity->id());
    //    if (item && item->itemType() == SceneItem::SceneLoader) {
    //        // Scene loaders need multiple pickers. Null picker is returned.
    //        createSceneLoaderChildPickers(entity, item->internalPickers());
    //    } else if (!item || item->itemType() != EditorSceneItem::Group) {
    // Group is not visible by itself (has no mesh), so no picker is needed
    picker = new Qt3DRender::QObjectPicker(entity);
    picker->setHoverEnabled(false);
    picker->setObjectName(QStringLiteral("__internal object picker ") + entity->objectName());
    entity->addComponent(picker);
    connect(picker, &Qt3DRender::QObjectPicker::pressed, [](Qt3DRender::QPickEvent *pick){
        if(pick){

        }
    });
    //    }

    return picker;
}

//        traverseEntity(this->entities());




