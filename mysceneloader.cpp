#include "mysceneloader.h"

#include <QMetalRoughMaterial>
#include <QRenderTarget>


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


        Qt3DCore::QEntity *root = entities()[0];

//        QVariant value = this->property("customMaterial");

//        QMetalRoughMaterial* metalMaterial= new QMetalRoughMaterial(this);
//        metalMaterial->setRoughness(0.2);
//        metalMaterial->setMetalness(0.9);

//        //metalMaterial->se



//        auto entitynames=entityNames();
//        for (int i = 0; i < entitynames.length(); ++i) {

//            QString entityname=entitynames[i];
//            auto entityvar=entity(entityname);

//            for (int j = 0; j< entityvar->components().length(); ++j) {

//                QMaterial* materialcomp=dynamic_cast<QMaterial*>(entityvar->components().at(j));
//                if(materialcomp){
//                    entityvar->removeComponent(materialcomp);
//                    entityvar->addComponent(metalMaterial);
//                    break;
//                }


//            }

//        }




    }



}

//        traverseEntity(this->entities());




