#include "infowindow.h"

#include "editorscene.h"
#include "editorviewportitem.h"
#include "linemesh.h"
#include "sceneitem.h"

#include <QCamera>
#include <QPhongMaterial>

InfoWindow::InfoWindow(QQuickItem *parent)
{
    setParentItem( parent );
    setAntialiasing( true );
    setFlag( QQuickItem::ItemHasContents, true );

    m_linemesh=new LineMesh();
    m_InfoConnectorEntity= new  Qt3DCore::QEntity();
    m_InfoConnectorEntity->setObjectName(QStringLiteral("__internal mesh info connector"));

    Qt3DExtras::QPhongMaterial *infoConnectorMaterial = new Qt3DExtras::QPhongMaterial();
    infoConnectorMaterial ->setAmbient(QColor("red"));
    infoConnectorMaterial ->setDiffuse(QColor(Qt::red));
    infoConnectorMaterial ->setSpecular(QColor(Qt::black));
    infoConnectorMaterial ->setShininess(0);

    Qt3DCore::QTransform* infoConnectorTransform = new Qt3DCore::QTransform();



    m_InfoConnectorEntity->addComponent(m_linemesh);
    m_InfoConnectorEntity->addComponent(infoConnectorMaterial);
    m_InfoConnectorEntity->addComponent(infoConnectorTransform);


    //    m_InfoConnectorEntity->setEnabled(false);

    //    m_linemesh->setSourceSceneItem(this);



}

bool InfoWindow::showConnector() const
{
    return m_showConnector;
}

InfoMessage *InfoWindow::infoMessage() const
{
    return m_infoMessage;
}

EditorScene *InfoWindow::editorScene() const
{
    return m_editorScene;
}

SceneItem *InfoWindow::sceneItem() const
{
    return m_sceneItem;
}

void InfoWindow::setShowConnector(bool showConnector)
{
    if (m_showConnector == showConnector)
        return;

    m_showConnector = showConnector;

    m_linemesh->setEnabled(m_showConnector);

    emit showConnectorChanged(m_showConnector);
}

void InfoWindow::setInfoMessage(InfoMessage *infoMessage)
{
    if (m_infoMessage == infoMessage)
        return;

    m_infoMessage = infoMessage;
    emit infoMessageChanged(m_infoMessage);
}


void InfoWindow::updateLinePosition(){
    QList<QVector4D> pos;



    if(!m_sceneItem){
        return;
    }

    //        qDebug()<<"Transform:"<< EditorUtils::totalAncestralTransform(m_sourceSceneItem->entity());
    pos.append(m_sceneItem->selectionBoxCenter());



    if(m_viewport){
        QRectF mapped=this->mapRectToItem(m_viewport,this->childrenRect());

        qDebug()<<"mapped:"<<mapped;

        double ypos=mapped.y()+mapped.height()/2;
        if(ypos<0){
            m_linemesh->setEnabled(false);
            return;
        }
        else {

            m_linemesh->setEnabled(showConnector());

        }
        QVector3D worldpos=m_editorScene->getWorldPosition(mapped.x(),ypos);

        pos.append(QVector4D(worldpos.x(),worldpos.y(),worldpos.z(),0));

        m_linemesh->posUpdate(pos);
    }



}

void InfoWindow::setIsSelected(bool isSelected)
{
    if (m_isSelected == isSelected)
        return;

    m_isSelected = isSelected;
    emit isSelectedChanged(m_isSelected);
}

void InfoWindow::setEditorScene(EditorScene *editorscene)
{
    if (m_editorScene == editorscene)
        return;

    m_editorScene = editorscene;
    if(m_editorScene){
        m_viewport=m_editorScene->viewport();
        connect(m_viewport,&EditorViewportItem::widthChanged,this,&InfoWindow::updateLinePosition);
        connect(m_viewport,&EditorViewportItem::heightChanged,this,&InfoWindow::updateLinePosition);

        m_camera=m_editorScene->activeCamera();
        connect(m_camera,&Qt3DRender::QCamera::viewVectorChanged,this,&InfoWindow::updateLinePosition);
        connect(m_camera,&Qt3DRender::QCamera::upVectorChanged,this,&InfoWindow::updateLinePosition);



        m_InfoConnectorEntity->setParent(m_editorScene->rootEntity());
        connect(m_editorScene,&EditorScene::sceneItemAdded,[this](SceneItem* itemadded){

            if(infoMessage()){
                if(itemadded->objectName()==infoMessage()->entityName()){
                    this->setSceneItem(itemadded);
                    this->updateLinePosition();
                }

            }

        });

    }
    else {
        //        m_InfoConnectorEntity->setParent();

    }
    emit editorSceneChanged(m_editorScene);
}

void InfoWindow::setSceneItem(SceneItem *sceneItem)
{
    if (m_sceneItem == sceneItem)
        return;

    m_sceneItem = sceneItem;
    emit sceneItemChanged(m_sceneItem);
}
