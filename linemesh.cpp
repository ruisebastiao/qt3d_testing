#include "linemesh.h"
#include "linemeshgeometry.h"
#include "Qt3DRender/QCamera"
#include "infowindow.h"

#include "editorscene.h"
#include "editorviewportitem.h"

#include <QVector3D>

#include <QVector4D>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QTransform>
#include <sceneitem.h>

LineMesh::LineMesh(Qt3DCore::QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
    , _lineMeshGeo(nullptr)
{
    setInstanceCount(1);
    setIndexOffset(0);
    setFirstInstance(0);
    setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);

    qRegisterMetaType<QList<QVector4D> >("QList<QVector4D>");
    //    connect(&_gcode, &GcodeTo4D::posFinished, this, &LineMesh::posUpdate);
}

void LineMesh::readAndRun(const QString &path)
{
    //    _gcode.read(path);
}

void LineMesh::read(const QString &path)
{
    emit run(path);
}


void LineMesh::posUpdate(QList<QVector4D> pos)
{

    _vertices = pos;
    _lineMeshGeo = new LineMeshGeometry(_vertices, this);
    setVertexCount(_lineMeshGeo->vertexCount());
    setGeometry(_lineMeshGeo);
    emit finished();
}

void LineMesh::posUpdate(QVariantList poslistvariant)
{


    QList<QVector4D> pos;


    for (int var = 0; var < poslistvariant.length(); ++var) {
        QVector4D vec=poslistvariant.at(var).value<QVector4D>();
        pos.append(vec);
    }

    posUpdate(pos);

    emit finished();
}

//InfoWindow *LineMesh::targetItem() const
//{
//    return m_targetItem;
//}

//SceneItem *LineMesh::sourceSceneItem() const
//{
//    return m_sourceSceneItem;
//}

void LineMesh::updateLinePosition(){
    QList<QVector4D> pos;


//    if(m_sourceSceneItem && m_targetItem){

//        //        qDebug()<<"Transform:"<< EditorUtils::totalAncestralTransform(m_sourceSceneItem->entity());
//        pos.append(m_sourceSceneItem->selectionBoxCenter());

//        EditorViewportItem* viewport=m_sourceSceneItem->scene()->viewport();

//        if(viewport){
//            QRectF mapped=m_targetItem->mapRectToItem(viewport,m_targetItem->childrenRect());

//            qDebug()<<"mapped:"<<mapped;
//            QVector3D worldpos=m_sourceSceneItem->scene()->getWorldPosition(mapped.x(),mapped.y()+m_targetItem->height()/2);

//            pos.append(QVector4D(worldpos.x(),worldpos.y(),worldpos.z(),0));

//            posUpdate(pos);
//        }

//    }

}

//void LineMesh::setTargetItem(InfoWindow *targetItem)
//{
//    if (m_targetItem == targetItem)
//        return;

//    m_targetItem = targetItem;

//    if(m_targetItem){
//        connect(targetItem,&InfoWindow::showConnectorChanged,[&](bool showconnector){

//            this->parentNode()->setEnabled(showconnector);

//        });
//        connect(targetItem,&InfoWindow::childrenRectChanged,this,&LineMesh::updateLinePosition);

//        this->parentNode()->setEnabled(targetItem->showConnector());
//    }


//    updateLinePosition();

//    emit targetItemChanged(m_targetItem);
//}

//void LineMesh::setSourceSceneItem(SceneItem *sourceSceneItem)
//{
//    if (m_sourceSceneItem== sourceSceneItem)
//        return;

//    m_sourceSceneItem= sourceSceneItem;
//    if(m_sourceSceneItem){

//        Qt3DRender::QCamera* camera= m_sourceSceneItem->scene()->activeCamera();


//        connect(m_sourceSceneItem->scene(),&EditorScene::activeCameraChanged,[&](Qt3DRender::QCamera* activecamera){
//            if(activecamera){
//                connect(activecamera,&Qt3DRender::QCamera::viewVectorChanged,this,&LineMesh::updateLinePosition);
//                //                connect(activecamera,&Qt3DRender::QCamera::upVectorChanged,this,&LineMesh::updateLinePosition);
//                this->updateLinePosition();
//            }

//        });

//        if(camera){
//            connect(camera,&Qt3DRender::QCamera::viewVectorChanged,this,&LineMesh::updateLinePosition);
//        }


//        EditorViewportItem* viewport=m_sourceSceneItem->scene()->viewport();
//        if(viewport){
//            //            connect(viewport,&EditorViewportItem::widthChanged,this,&LineMesh::updateLinePosition);
//            connect(viewport,&EditorViewportItem::widthChanged,[&](){
//                this->updateLinePosition();
//            });

//            connect(viewport,&EditorViewportItem::heightChanged,this,&LineMesh::updateLinePosition);
//        }
//        else{
//            connect(m_sourceSceneItem->scene(),&EditorScene::viewportChanged,[&](EditorViewportItem *viewport){
//                if(viewport){
//                    connect(viewport,&EditorViewportItem::widthChanged,[&](){
//                        this->updateLinePosition();
//                    });
//                    connect(viewport,&EditorViewportItem::heightChanged,this,&LineMesh::updateLinePosition);
//                }
//            });
//        }







//    }


//    updateLinePosition();

//    emit sourceSceneItemChanged(m_sourceSceneItem);
//}

LineMesh::~LineMesh()
{
}
