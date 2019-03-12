#include "mysceneloader.h"

#include <QMetalRoughMaterial>
#include <QObjectPicker>
#include <QRenderTarget>


using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

MySceneLoader::MySceneLoader(QNode *parent):QSceneLoader(parent)
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



        }




    }



}



