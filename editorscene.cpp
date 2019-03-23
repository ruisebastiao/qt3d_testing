/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D Editor of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "editorscene.h"
#include "editorutils.h"
#include "sceneitem.h"
#include "ontopeffect.h"
#include "editorsceneitemcomponentsmodel.h"
#include "linemesh.h"

#include "editorviewportitem.h"


#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QSpotLight>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QPickingSettings>

#include <Qt3DInput/QInputSettings>

#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QKeySequence>
#include <QtGui/QMatrix4x4>

#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QtMath>

#include <QtQml/QQmlEngine>

#include <QCameraLens>
#include <cfloat>


#include "infowindow.h"
#include "mysceneloader.h"

//#define TEST_SCENE // If a test scene is wanted instead of the default scene

#ifdef TEST_SCENE
#include <Qt3DRender/QCylinderMesh>
#include <Qt3DRender/QNormalDiffuseSpecularMapMaterial>
#endif

static const QString cameraVisibleEntityName = QStringLiteral("__internal camera visible entity");
static const QString lightVisibleEntityName = QStringLiteral("__internal light visible entity");
static const QString sceneLoaderSubEntityName = QStringLiteral("__internal sceneloader sub entity");
static const QString helperArrowName = QStringLiteral("__internal helper arrow");
static const float freeViewCameraNearPlane = 0.1f;
static const float freeViewCameraFarPlane = 10000.0f;
static const float freeViewCameraFov = 45.0f;
static const QColor selectionBoxColor("#43adee");
static const QColor cameraFrustumColor("#c22555");
static const QColor helperPlaneColor("#585a5c");
static const QColor helperArrowColorX("red");
static const QColor helperArrowColorY("green");
static const QColor helperArrowColorZ("blue");
#ifndef GLTF_EXPORTER_AVAILABLE
static const QString autoSavePostfix = QStringLiteral(".autosave");
#endif

EditorScene::EditorScene(QObject *parent)
    : QObject(parent)
    , m_rootEntity(nullptr)
    , m_componentCache(nullptr)
    , m_rootItem(nullptr)
    , m_renderSettings(nullptr)
    , m_renderer(nullptr)
    , m_sceneEntity(nullptr)
    , m_sceneEntityItem(nullptr)
    , m_selectedEntity(nullptr)
    , m_selectedEntityTransform(nullptr)
    , m_freeView(false)
    , m_freeViewCameraEntity(nullptr)
    , m_viewport(nullptr)
    , m_helperPlane(nullptr)
    , m_helperPlaneTransform(nullptr)
    , m_helperArrows(nullptr)
    , m_helperArrowsTransform(nullptr)
    , m_helperArrowsLocal(false)
    , m_meshCenterIndicatorLine(nullptr)
    , m_meshCenterIndicatorLineTransform(nullptr)
    , m_qtTranslator(new QTranslator(this))
    , m_appTranslator(new QTranslator(this))
    , m_dragMode(DragNone)
    , m_viewCenterLocked(false)
    , m_pickedEntity(nullptr)
    , m_pickedDistance(-1.0f)
    , m_gridSize(3)
    , m_duplicateCount(0)
    , m_previousDuplicate(nullptr)
    , m_ctrlDownOnLastLeftPress(false)
    , m_groupBoxUpdatePending(false)
{

    setLanguage(language());
    retranslateUi();
    createRootEntity();
    setupDefaultScene();

    // Install event filter to handle undo/redo globally, instead of each TextField having
    // their own stack.
    // TODO: This might need to be done differently if we make this Creator plugin
    qGuiApp->installEventFilter(this);

    //    qRegisterMetaType<Qt3DCore::QTransform*>("Qt3DCore::QTransform*");
}

EditorScene::~EditorScene()
{
    setActiveCamera(nullptr);
    // Remove all entities recursively to ensure the root item is last one to be deleted
    removeEntity(m_sceneEntity);

    // TODO: Check if it is necessary to delete rootentity and associated components, or do they get
    // TODO: properly deleted by aspect engine shutdown?

    delete m_componentCache;


}

SceneItem *EditorScene::entityItem(Qt3DCore::QEntity *entity) const
{
    return m_sceneItems.value(entity->id());
}

void EditorScene::addEntity(Qt3DCore::QEntity *entity, int index, Qt3DCore::QEntity *parent)
{
    if (entity == nullptr)
        return;

    if (parent == nullptr) {
        //make sure that entity has a parent, otherwise make its parent the root entity
        if (entity->parentEntity() == nullptr)
            entity->setParent(m_rootEntity);
    } else if (entity->parentEntity() != parent) {
        entity->setParent(parent);
    }

    SceneItem *item = m_sceneItems.value(entity->id(), nullptr);
    if (!item) {
        item = new SceneItem(this, entity, m_sceneItems.value(entity->parentEntity()->id(),
                                                              nullptr), index, this);

        if (entity == m_sceneEntity)
            m_sceneEntityItem = item;

        if (entity == m_baseEntity){
            m_baseEntityItem= item;

        }



        m_sceneItems.insert(entity->id(), item);
        connect(entity, &QObject::objectNameChanged,
                this, &EditorScene::handleEntityNameChange);

        if (entity->isEnabled() && item->itemType() != SceneItem::SceneLoader){
            Qt3DRender::QObjectPicker* picker= createObjectPickerForEntity(entity);
//            picker->
        }

        // Note: Scene loader pickers are created asynchronously after scene is loaded fully

        item->componentsModel()->initializeModel();
    }

//    for (int var = 0; var < m_infoMessages->length(); ++var) {
//        InfoWindow* infowindow=m_infoMessages->at(var)->infoWindow();
//        if(infowindow->infoMessage() && infowindow->infoMessage()->entityName()==entity->objectName()){
//            item->getLinemesh()->setTargetItem(infowindow);
//            break;
//        }
//    }

    emit sceneItemAdded(item);
    if (item->itemType() != SceneItem::SceneLoader) {
        foreach (QObject *child, entity->children()) {
            Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
            if (childEntity && childEntity->objectName().contains(QStringLiteral("__internal"))==false)
                addEntity(childEntity);
        }
    }
}

// Removed entity is deleted
void EditorScene::removeEntity(Qt3DCore::QEntity *entity)
{
    if (entity == nullptr || entity == m_rootEntity)
        return;



    if (entity == m_sceneEntity) {
        m_sceneEntity = nullptr;
        m_sceneEntityItem = nullptr;
    }

    disconnect(entity, 0, this, 0);

    SceneItem *item = m_sceneItems.value(entity->id());

    if(item){
        if (item->itemType() != SceneItem::SceneLoader) {
            foreach (QObject *child, entity->children()) {
                Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
                removeEntity(childEntity);
            }
        }





        m_sceneItems.remove(entity->id());

        if (m_selectedEntity == entity || multiSelection())
            queueEnsureSelection();

        queueUpdateGroupSelectionBoxes();

        delete item;
        delete entity;
    }

}

void EditorScene::resetScene()
{
    clearSingleSelection();

    // Clear the existing scene
    setFrameGraphCamera(nullptr);


    removeEntity(m_sceneEntity);

    // Create new scene root
    //    setSceneEntity();

    // Set up default scene
    setupDefaultScene();


    m_freeView = true;
    resetFreeViewCamera();
    setFrameGraphCamera(m_freeViewCameraEntity);



    emit freeViewChanged(m_freeView);

    // Reset entity tree

    setSelection(m_sceneEntity);
}


void EditorScene::resetFreeViewCamera()
{
    if (m_viewport)
        m_freeViewCameraEntity->setAspectRatio(m_viewport->width() / qMax(m_viewport->height(), 1.0));
    else
        m_freeViewCameraEntity->setAspectRatio(16.0f / 9.0f);
    m_freeViewCameraEntity->setBottom(-0.5f);
    m_freeViewCameraEntity->setFarPlane(freeViewCameraFarPlane);
    m_freeViewCameraEntity->setFieldOfView(freeViewCameraFov);
    m_freeViewCameraEntity->setLeft(-0.5f);
    m_freeViewCameraEntity->setNearPlane(freeViewCameraNearPlane);
    m_freeViewCameraEntity->setPosition(QVector3D(20.0f, 20.0f, 20.0f));
    m_freeViewCameraEntity->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    m_freeViewCameraEntity->setRight(0.5f);
    m_freeViewCameraEntity->setTop(0.5f);
    m_freeViewCameraEntity->setUpVector(QVector3D(0, 1, 0));
    m_freeViewCameraEntity->setViewCenter(QVector3D(0, 0, 0));
    setFrameGraphCamera(m_freeViewCameraEntity);
}

// Resolves a world position for given viewport position.
// The world position is the intersection of the eye ray at specified position and the active
// helper plane. If there is no intersection, (0, 0, 0) position is returned.
QVector3D EditorScene::getWorldPosition(int xPos, int yPos)
{
    QVector3D retVec;
    if (xPos >= 0 && yPos >= 0 && xPos < m_viewport->width() && yPos < m_viewport->height()) {
        QPoint pos(xPos, yPos);
        Qt3DRender::QCamera *camera = activeCamera();
        if (camera) {
            QVector3D planeOrigin_x;
            QVector3D planeOrigin_y;
            QVector3D planeOrigin_z;




            Qt3DCore::QTransform* temphelperTransform_y = new Qt3DCore::QTransform();
            temphelperTransform_y->setScale3D(QVector3D(m_gridSize * 25.0f, m_gridSize * 25.0f, 1.0f));
            temphelperTransform_y->setRotation(
                        temphelperTransform_y->fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f));


            Qt3DCore::QTransform* temphelperTransform_z= new Qt3DCore::QTransform();
            temphelperTransform_z->setScale3D(QVector3D(m_gridSize * 25.0f, m_gridSize * 25.0f, 1.0f));
            temphelperTransform_z->setRotation(temphelperTransform_z->fromAxisAndAngle(0.0f, 0.0f, 1.0f, 90.0f));

            QVector3D planeNormal_x = helperPlaneNormal(m_helperPlaneTransform);
            QVector3D planeNormal_y = helperPlaneNormal(temphelperTransform_y);
            QVector3D planeNormal_z = helperPlaneNormal(temphelperTransform_z);

            float cosAngle_x = QVector3D::dotProduct(planeOrigin_x.normalized(), planeNormal_x);
            float planeOffset_x = planeOrigin_x.length() * cosAngle_x;

            QVector3D ray = EditorUtils::unprojectRay(camera->viewMatrix(), camera->projectionMatrix(),
                                                      m_viewport->width(), m_viewport->height(),
                                                      pos);
            float t = 0.0f;
            QVector3D intersection = EditorUtils::findIntersection(camera->position(), ray,
                                                                   planeOffset_x, planeNormal_x, t);

            if (t > camera->nearPlane()){
                retVec = intersection;
            }
            else{
                float cosAngle_y = QVector3D::dotProduct(planeOrigin_y.normalized(), planeNormal_y);
                float planeOffset_y = planeOrigin_y.length() * cosAngle_y;

                t = 0.0f;
                intersection = EditorUtils::findIntersection(camera->position(), ray,
                                                                       planeOffset_y, planeNormal_y, t);

                if (t > camera->nearPlane()){
                    retVec = intersection;
                }
                else{
                    float cosAngle_z = QVector3D::dotProduct(planeOrigin_z.normalized(), planeNormal_z);
                    float planeOffset_z = planeOrigin_z.length() * cosAngle_z;

                    t = 0.0f;
                    intersection = EditorUtils::findIntersection(camera->position(), ray,
                                                                           planeOffset_z, planeNormal_z, t);

                    if (t > camera->nearPlane()){
                        retVec = intersection;
                    }
                    else{

                    }
                }

            }



        }
    }

    return retVec;
}
void EditorScene::allVisible(SceneItem *skipItem)
{

    Q_FOREACH (SceneItem *item, m_sceneItems.values()) {

        if(item && item->parentItem()==m_baseEntityItem){

            if (item!= skipItem)
                item->setTransparent(false);
            else {

            }
        }
    }

    if(skipItem){
        skipItem->setTransparent(true);
    }


}

void EditorScene::allTransparent(SceneItem *skipItem)
{

    Q_FOREACH (SceneItem *item, m_sceneItems.values()) {

        if(item && item->parentItem()==m_baseEntityItem){

            if (item!= skipItem)
                item->setTransparent(true);
            else {
                //                item->setTransparent(false);
            }
        }
    }

    if(skipItem){
        skipItem->setTransparent(false);
    }


}

void EditorScene::loadFile(QString filepath)
{
    m_sceneloader->setSource(filepath);
}


int EditorScene::gridSize() const
{
    return m_gridSize;
}

void EditorScene::setGridSize(int size)
{
    if (m_gridSize != size) {
        delete m_helperPlane;
        m_gridSize = size;
        createHelperPlane();
        emit gridSizeChanged(size);
    }
}

const QString EditorScene::language() const
{
    if (m_language.isEmpty())
        return QLocale::system().name().left(2);
    else
        return m_language;
}

void EditorScene::setLanguage(const QString &language)
{
    if (!m_qtTranslator->isEmpty())
        QCoreApplication::removeTranslator(m_qtTranslator);
    if (!m_appTranslator->isEmpty())
        QCoreApplication::removeTranslator(m_appTranslator);

    if (m_qtTranslator->load("qt_" + language,
                             QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(m_qtTranslator);
    }

    if (m_appTranslator->load(":/qt3deditorlib/editorlib_" + language)) {
        QCoreApplication::installTranslator(m_appTranslator);
        m_language = language;
    } else {
        m_language = "C";
    }

    emit languageChanged(m_language);
    emit translationChanged("");
    retranslateUi();
}

void EditorScene::retranslateUi()
{
    //: This string is entity name, no non-ascii characters allowed
    m_sceneRootString = tr("Scene root");
}

const QString EditorScene::emptyString() const
{
    return QStringLiteral("");
}

Qt3DRender::QObjectPicker *EditorScene::createObjectPickerForEntity(Qt3DCore::QEntity *entity)
{
    Qt3DRender::QObjectPicker *picker = nullptr;
    SceneItem *item = m_sceneItems.value(entity->id());
    if (item && item->itemType() == SceneItem::SceneLoader) {
        // Scene loaders need multiple pickers. Null picker is returned.
        createSceneLoaderChildPickers(entity, item->internalPickers());
    } else if (!item || item->itemType() != SceneItem::Group) {
        // Group is not visible by itself (has no mesh), so no picker is needed
        picker = new Qt3DRender::QObjectPicker(entity);
        picker->setHoverEnabled(false);
        picker->setObjectName(QStringLiteral("__internal object picker ") + entity->objectName());
        entity->addComponent(picker);
        connect(picker, &Qt3DRender::QObjectPicker::pressed, this, &EditorScene::handlePickerPress);
    }

    return picker;
}



void EditorScene::queueEnsureSelection()
{
    // Ensure that something is selected after the current pending events have executed
    if (m_sceneEntity && m_ensureSelectionEntityName.isEmpty()) {
        if (m_selectedEntity) {
            m_ensureSelectionEntityName = m_selectedEntity->objectName();
            clearSingleSelection();
        } else {
            m_ensureSelectionEntityName = m_sceneEntity->objectName();
        }
        QMetaObject::invokeMethod(this, "doEnsureSelection", Qt::QueuedConnection);
    }
}

void EditorScene::doEnsureSelection()
{
    if (!m_selectedEntity) {
        // If we have multiselection active, update the multiselection list
        const int count = m_selectedEntityNameList.size();
        if (count > 0) {
            QStringList newList;
            newList.reserve(count);
            for (int i = 0; i < count; ++i) {
                QString entityName = m_selectedEntityNameList.at(i);
                if (itemByName(entityName))
                    newList.append(entityName);
            }
            if (newList.size() == 1) {
                m_ensureSelectionEntityName = newList.at(0);
                newList.clear();
            }

            if (count != newList.size()) {
                m_selectedEntityNameList = newList;
                if (newList.size() == 0)
                    emit multiSelectionChanged(false);
            }
            checkMultiSelectionHighlights();
            emit multiSelectionListChanged();
        }
        if (!m_selectedEntityNameList.size()) {
            SceneItem *item = itemByName(m_ensureSelectionEntityName);
            if (item)
                setSelection(item->entity());
            else
                setSelection(m_sceneEntity);
        }
    }
    m_ensureSelectionEntityName.clear();
}

void EditorScene::addNewMessage(QString entityName,QString message)
{
    m_infoMessages->addItem(new InfoMessage(entityName,message));
}

void EditorScene::queueUpdateGroupSelectionBoxes()
{
    // Queue asynchronous update to group selection boxes
    if (!m_groupBoxUpdatePending) {
        m_groupBoxUpdatePending = true;
        QMetaObject::invokeMethod(this, "doUpdateGroupSelectionBoxes", Qt::QueuedConnection);
    }
}


void EditorScene::doUpdateGroupSelectionBoxes()
{
    const QList<SceneItem *> items = m_sceneItems.values();
    for (int i = 0; i < items.size(); ++i) {
        SceneItem *item = items.at(i);
        if (item && item->itemType() == SceneItem::Group && item->isSelectionBoxShowing())
            item->updateGroupExtents();
    }
    m_groupBoxUpdatePending = false;
}

void EditorScene::enableHelperArrows(bool enable)
{
    // TODO: Remove this function once disabling parent properly hides the children
    Q_FOREACH (QObject *child, m_helperArrows->children()) {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity)
            childEntity->setEnabled(enable);
    }
    m_helperArrows->setEnabled(enable);
}

SceneItem *EditorScene::itemByName(const QString &name)
{
    const QList<SceneItem *> items = m_sceneItems.values();
    for (int i = 0; i < items.size(); ++i) {
        if (items.at(i)->entity()->objectName() == name)
            return items.at(i);
    }
    return nullptr;
}

void EditorScene::clearSingleSelection()
{
    if (m_selectedEntity) {
        SceneItem *oldItem = m_sceneItems.value(m_selectedEntity->id(), nullptr);
        if (oldItem) {

            oldItem->setShowSelectionBox(false);
        }
        m_selectedEntity = nullptr;
    }
}


void EditorScene::handleEnabledChanged(Qt3DCore::QEntity *entity, bool enabled)
{
    bool freeViewEnabled = enabled && m_freeView;
    //    Qt3DRender::QCamera *camera = qobject_cast<Qt3DRender::QCamera *>(entity);
    //    if (camera != nullptr) {
    //        int cameraIndex = cameraIndexForEntity(camera);
    //        if (cameraIndex >= 0) {
    //            enableVisibleCamera(m_sceneCameras[cameraIndex], freeViewEnabled,
    //                                cameraIndex == m_activeSceneCameraIndex);
    //        }

    //    } else if (EditorUtils::entityLight(entity) != nullptr) {
    //        LightData *lightData = m_sceneLights.value(entity->id());
    //        if (lightData)
    //            enableVisibleLight(*lightData, freeViewEnabled);

    //    } else {
    //        SceneItem *item = m_sceneItems.value(entity->id());
    //        if (item && item->itemType() == SceneItem::SceneLoader) {
    //            if (enabled) {
    //                if (item->internalPickers()->size() == 0)
    //                    createObjectPickerForEntity(entity);
    //            } else {
    //                Q_FOREACH (Qt3DRender::QObjectPicker *picker, *item->internalPickers())
    //                    delete picker;
    //                item->internalPickers()->clear();
    //            }
    //        } else {
    //            // Picker doesn't get disabled with the entity - we have to delete it to disable
    //            Qt3DRender::QObjectPicker *picker = EditorUtils::entityPicker(entity);
    //            // Other objects aren't affected by m_freeView, so just check enabled flag
    //            if (enabled) {
    //                if (!picker)
    //                    createObjectPickerForEntity(entity);
    //            } else {
    //                delete picker;
    //            }
    //        }
    //    }
}

void EditorScene::setError(const QString &errorString)
{
    m_errorString = errorString;
    emit errorChanged(m_errorString);
    qWarning() << m_errorString;
}

void EditorScene::setupDefaultScene()
{



}

void EditorScene::createRootEntity()
{
    m_rootEntity = new Qt3DCore::QEntity();
    // Grab explicit ownership of the root entity, otherwise QML garbage collector may
    // clean it up.
    QQmlEngine::setObjectOwnership(m_rootEntity, QQmlEngine::CppOwnership);

    m_rootEntity->setObjectName(QStringLiteral("__internal root entity"));

    // Create a component cache for components that are needed after Load/New/possible other
    // reason for deleting scene root (m_sceneEntity)
    m_componentCache = new Qt3DCore::QEntity(m_rootEntity);
    m_componentCache->setObjectName("__internal component cache");
    m_componentCache->setEnabled(false);

    // Selection box material and mesh need to be created before any
    // SceneItem are created
    Qt3DExtras::QPhongMaterial *selectionBoxMaterial = new Qt3DExtras::QPhongMaterial();
    selectionBoxMaterial->setAmbient(selectionBoxColor);
    selectionBoxMaterial->setDiffuse(QColor(Qt::black));
    selectionBoxMaterial->setSpecular(QColor(Qt::black));
    selectionBoxMaterial->setShininess(0);
    m_selectionBoxMaterial = selectionBoxMaterial;
    m_selectionBoxMesh = EditorUtils::createWireframeBoxMesh();

    m_meshCenterIndicatorLine = new Qt3DCore::QEntity();
    m_meshCenterIndicatorLine->setObjectName(QStringLiteral("__internal mesh center indicator line"));
    Qt3DRender::QGeometryRenderer *indicatorMesh = EditorUtils::createSingleLineMesh();

    Qt3DRender::QMaterial *meshCenterLineMaterial = new Qt3DRender::QMaterial();
    meshCenterLineMaterial->setEffect(new OnTopEffect());
    meshCenterLineMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("handleColor"),
                                                                    selectionBoxColor));

    m_meshCenterIndicatorLineTransform = new Qt3DCore::QTransform();
    m_meshCenterIndicatorLine->addComponent(indicatorMesh);
    m_meshCenterIndicatorLine->addComponent(meshCenterLineMaterial);
    m_meshCenterIndicatorLine->addComponent(m_meshCenterIndicatorLineTransform);
    m_meshCenterIndicatorLine->setParent(m_rootEntity);
    m_meshCenterIndicatorLine->setEnabled(false);

    // Save to cache, as these are needed after Load/New
    m_componentCache->addComponent(m_selectionBoxMesh);
    m_componentCache->addComponent(m_selectionBoxMaterial);

    m_rootItem = new SceneItem(this, m_rootEntity, nullptr, -1, this);

    m_sceneItems.insert(m_rootEntity->id(), m_rootItem);

    m_renderSettings = new Qt3DRender::QRenderSettings();
    m_renderSettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    m_renderSettings->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::AllPicks);
    m_renderSettings->setObjectName(QStringLiteral("__internal Scene frame graph"));
    m_renderer = new Qt3DExtras::QForwardRenderer();
    m_renderer->setClearColor(Qt::lightGray);
    m_renderSettings->setActiveFrameGraph(m_renderer);

    // Setting the FrameGraph to actual root entity to protect it from accidental removal
    m_rootEntity->addComponent(m_renderSettings);

    m_rootEntity->addComponent(new Qt3DInput::QInputSettings());

    // Scene entity (i.e. the visible root)
    //  setSceneEntity();


    // Free view camera
    m_freeViewCameraEntity = new Qt3DRender::QCamera(m_rootEntity);
    m_freeViewCameraEntity->setObjectName(QStringLiteral("__internal free view camera"));
    setActiveCamera(m_freeViewCameraEntity);
    resetFreeViewCamera();

    // Helper plane
    createHelperPlane();

    // Helper arrows
    createHelperArrows();


    Qt3DRender::QGeometryRenderer *viewVectorMesh = EditorUtils::createCameraViewVectorMesh();
    Qt3DRender::QGeometryRenderer *viewCenterMesh = EditorUtils::createCameraViewCenterMesh(1.0f);

    //    m_sceneloader= new QSceneLoader(m_sceneEntity);

    connect(m_sceneloader,&QSceneLoader::statusChanged,this,&EditorScene::loaderstatusChange);



//    m_InfoConnectorEntity->setEnabled(false);





}

void EditorScene::createHelperPlane()
{
    m_helperPlane = new Qt3DCore::QEntity();

    m_helperPlane->setObjectName(QStringLiteral("__internal helper plane"));



    // Helper plane origin must be at the meeting point of lines, hence the odd lineCount
    Qt3DRender::QGeometryRenderer *planeMesh = EditorUtils::createWireframePlaneMesh(51);

    Qt3DExtras::QPhongMaterial *helperPlaneMaterial = new Qt3DExtras::QPhongMaterial();
    helperPlaneMaterial->setAmbient(helperPlaneColor);
    helperPlaneMaterial->setDiffuse(QColor(Qt::black));
    helperPlaneMaterial->setSpecular(QColor(Qt::black));
    helperPlaneMaterial->setShininess(0);

    m_helperPlaneTransform = new Qt3DCore::QTransform();



    m_helperPlaneTransform->setScale3D(QVector3D(m_gridSize * 25.0f, m_gridSize * 25.0f, 1.0f));
    m_helperPlaneTransform->setRotation(
                m_helperPlaneTransform->fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f));


    m_helperPlane->addComponent(planeMesh);
    m_helperPlane->addComponent(helperPlaneMaterial);
    m_helperPlane->addComponent(m_helperPlaneTransform);
    m_helperPlane->setParent(m_rootEntity);






}

void EditorScene::createHelperArrows()
{
    m_helperArrowHandleIndexMap.clear();
    m_helperArrows = new Qt3DCore::QEntity();
    m_helperArrows->setObjectName(QStringLiteral("__internal helper arrows"));

    QMatrix4x4 matrix;
    Qt3DCore::QEntity *arrow = EditorUtils::createArrowEntity(helperArrowColorY, m_helperArrows,
                                                              matrix, helperArrowName);
    createObjectPickerForEntity(arrow);
    m_helperArrowHandleIndexMap.insert(arrow, TranslateHandleArrowY);

    matrix.rotate(90.0f, QVector3D(1.0f, 0.0f, 0.0f));
    arrow = EditorUtils::createArrowEntity(helperArrowColorZ, m_helperArrows, matrix,
                                           helperArrowName);
    createObjectPickerForEntity(arrow);
    m_helperArrowHandleIndexMap.insert(arrow, TranslateHandleArrowZ);

    matrix = QMatrix();
    matrix.rotate(-90.0f, QVector3D(0.0f, 0.0f, 1.0f));
    arrow = EditorUtils::createArrowEntity(helperArrowColorX, m_helperArrows, matrix,
                                           helperArrowName);
    createObjectPickerForEntity(arrow);
    m_helperArrowHandleIndexMap.insert(arrow, TranslateHandleArrowX);

    m_helperArrowsTransform = new Qt3DCore::QTransform();
    m_helperArrows->addComponent(m_helperArrowsTransform);
    m_helperArrows->setParent(m_rootEntity);
    enableHelperArrows(false);
}

void EditorScene::setFrameGraphCamera(Qt3DCore::QEntity *cameraEntity)
{
    if (m_renderer) {
        Qt3DCore::QTransform *cameraTransform = nullptr;
        Qt3DRender::QCamera *currentCamera =
                qobject_cast<Qt3DRender::QCamera *>(m_renderer->camera());
        if (currentCamera) {
            //            cameraTransform = currentCamera->transform();
            //            if (cameraTransform) {
            //                disconnect(cameraTransform, &Qt3DCore::QTransform::matrixChanged,
            //                           this, &EditorScene::handleSelectionTransformChange);
            //            }
        }
        m_renderer->setCamera(cameraEntity);
        currentCamera = qobject_cast<Qt3DRender::QCamera *>(cameraEntity);
        if (currentCamera) {
            cameraTransform = currentCamera->transform();
            //            if (cameraTransform) {
            //                connect(cameraTransform, &Qt3DCore::QTransform::matrixChanged,
            //                        this, &EditorScene::handleSelectionTransformChange);
            //            }
            setActiveCamera(currentCamera);
        }


        // This will update drag handle positions if needed
        //        handleSelectionTransformChange();
    }
}

Qt3DRender::QCamera *EditorScene::frameGraphCamera() const
{
    if (m_renderer)
        return qobject_cast<Qt3DRender::QCamera *>(m_renderer->camera());
    else
        return nullptr;
}

void EditorScene::setActiveCamera(Qt3DRender::QCamera *activeCamera)
{
    if (m_activeCamera == activeCamera)
        return;

    m_activeCamera = activeCamera;
    //m_activeCamera->setProjectionType(Qt3DRender::QCameraLens::ProjectionType::OrthographicProjection);

    //    //m_activeCamera->setAspectRatio(1);
    //    m_activeCamera->setTop(0);
    //    m_activeCamera->setBottom(0);

    emit activeCameraChanged(m_activeCamera);
}




void EditorScene::setSelection(Qt3DCore::QEntity *entity)
{
    // Setting selection implies end to multiSelection

    if(!entity){
        clearMultiSelection();
        clearSingleSelection();
        setSelectionItem(nullptr);
        allVisible(nullptr);
        return;
    }


    if (m_selectedEntityNameList.size())
        clearMultiSelection();
    SceneItem *item = m_sceneItems.value(entity->id(), nullptr);
    if (item) {
        if (entity != m_selectedEntity) {
            clearSingleSelection();

            m_selectedEntity = entity;

            if (m_selectedEntity) {



                if (m_selectedEntity != m_sceneEntity)
                    item->setShowSelectionBox(true);
                m_selectedEntityTransform = EditorUtils::entityTransform(m_selectedEntity);
            }

            // Emit signal to highlight the entity from the list
            emit selectionChanged(m_selectedEntity);
        }

        enableHelperArrows(item->isSelectionBoxShowing());


        Qt3DCore::QTransform *transform = EditorUtils::entityTransform(m_selectedEntity);
        bool transformPropertiesLocked = item->customProperty(m_selectedEntity,
                                                              lockTransformPropertyName()).toBool();






        // Update drag handles transforms to initial state
        //        handleSelectionTransformChange();
    } else {
        //        m_dragHandlesTransform->setEnabled(false);
        enableHelperArrows(false);
        if (m_selectedEntity != m_sceneEntity)
            setSelection(m_sceneEntity);
    }

    setSelectionItem(item);

}

void EditorScene::toggleEntityMultiSelection(const QString &name)
{
    // If the new one is already in, remove it. Otherwise add it.
    if (m_selectedEntityNameList.contains(name))
        removeEntityFromMultiSelection(name);
    else
        addEntityToMultiSelection(name);
}

void EditorScene::clearMultiSelection()
{
    if (m_selectedEntityNameList.size() > 0) {
        m_selectedEntityNameList.clear();
        checkMultiSelectionHighlights();
        emit multiSelectionChanged(false);
        emit multiSelectionListChanged();
        setSelection(m_sceneEntity);
    }
}

QVector3D EditorScene::getMultiSelectionCenter()
{
    QVector3D pos;
    for (int i = 0; i < m_selectedEntityNameList.size(); i++) {
        SceneItem *item = itemByName(m_selectedEntityNameList.at(i));
        if (item) {
            item->doUpdateSelectionBoxTransform();
            pos += item->selectionBoxCenter();
        }
    }
    return m_selectedEntityNameList.size() ? (pos / m_selectedEntityNameList.size()) : QVector3D();
}

void EditorScene::updateWorldPositionLabel(int xPos, int yPos)
{
    updateWorldPositionLabel(getWorldPosition(xPos,yPos));
}

void EditorScene::changeCameraPosition(EditorScene::CameraPosition preset)
{
    Qt3DRender::QCamera *camera = frameGraphCamera();

    QVector3D cameraDirection;
    QVector3D up(0.0f, 0.0f, 0.0f);
    switch (preset) {
    case CameraPositionTop:
        cameraDirection.setY(1.0f);
        up.setZ(1.0f);
        break;
    case CameraPositionBottom:
        cameraDirection.setY(-1.0f);
        up.setZ(-1.0f);
        break;
    case CameraPositionLeft:
        cameraDirection.setX(1.0f);
        up.setY(1.0f);
        break;
    case CameraPositionRight:
        cameraDirection.setX(-1.0f);
        up.setY(1.0f);
        break;
    case CameraPositionFront:
        cameraDirection.setZ(-1.0f);
        up.setY(1.0f);
        break;
    case CameraPositionBack:
        cameraDirection.setZ(1.0f);
        up.setY(1.0f);
        break;
    default:
        return;
    }

    // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
    float len = camera->viewVector().length();
    camera->setPosition(camera->viewCenter() + cameraDirection * len);
    camera->setUpVector(up);
}

//void EditorScene::setWireframe(SceneItem *item)
//{
//    item->setWireFrame();
//}


void EditorScene::updateWorldPositionLabel(const QVector3D &worldPos)
{
    emit worldPositionLabelUpdate(QString::number(qreal(worldPos.x()), 'f', 2),
                                  QString::number(qreal(worldPos.y()), 'f', 2),
                                  QString::number(qreal(worldPos.z()), 'f', 2));
}

void EditorScene::addEntityToMultiSelection(const QString &name)
{
    const int oldSize = m_selectedEntityNameList.size();
    if (oldSize == 0) {
        // Do not add if multiselecting the currently selected entity as the first entity
        if (!m_selectedEntity || m_selectedEntity->objectName() == name)
            return;

        if (m_selectedEntity != m_sceneEntity) {
            m_selectedEntityNameList.append(m_selectedEntity->objectName());
            //            m_dragHandlesTransform->setEnabled(false);
            enableHelperArrows(false);
            //            handleSelectionTransformChange();
            clearSingleSelection();
        } else {
            // Just single-select the new entity and return if the other entity was scene entity
            SceneItem *item = nullptr;// m_sceneModel->getItemByName(name);
            if (item) {
                setSelection(item->entity());
                return;
            }
        }
    }
    m_selectedEntityNameList.append(name);

    checkMultiSelectionHighlights();

    if (oldSize == 0)
        emit multiSelectionChanged(true);
    emit multiSelectionListChanged();
}

void EditorScene::removeEntityFromMultiSelection(const QString &name)
{
    bool removed = m_selectedEntityNameList.removeOne(name);

    if (removed) {
        bool lastRemoved = m_selectedEntityNameList.size() == 1;
        SceneItem *lastItem = nullptr;
        if (lastRemoved) {
            //            lastItem = m_sceneModel->getItemByName(m_selectedEntityNameList.at(0));
            m_selectedEntityNameList.clear();
            emit multiSelectionChanged(false);
        }
        checkMultiSelectionHighlights();
        emit multiSelectionListChanged();
        if (lastRemoved) {
            if (lastItem)
                setSelection(lastItem->entity());
            else
                setSelection(m_sceneEntity);
        }
    }
}

void EditorScene::renameEntityInMultiSelectionList(const QString &oldName, const QString &newName)
{
    int index = m_selectedEntityNameList.indexOf(oldName);
    if (index > 0)
        m_selectedEntityNameList.replace(index, newName);
}



void EditorScene::setHelperArrowsLocal(bool enable)
{
    if (enable != m_helperArrowsLocal) {
        m_helperArrowsLocal = enable;
        //        handleSelectionTransformChange();
        emit helperArrowsLocalChanged(m_helperArrowsLocal);
    }
}

void EditorScene::setViewport(EditorViewportItem *viewport)
{
    if (m_viewport != viewport) {
        if (m_viewport)
            disconnect(m_viewport, 0, this, 0);

        m_viewport = viewport;
        connect(viewport, &EditorViewportItem::heightChanged,
                this, &EditorScene::handleViewportSizeChange);
        connect(viewport, &EditorViewportItem::widthChanged,
                this, &EditorScene::handleViewportSizeChange);
        handleViewportSizeChange();

        // Set the viewport up as a source of input events for the input aspect
        Qt3DInput::QInputSettings *inputSettings =
                m_rootEntity->findChild<Qt3DInput::QInputSettings *>();
        if (inputSettings) {
            inputSettings->setEventSource(viewport);
        } else {
            qWarning() << "No Input Settings found, keyboard and mouse events won't be handled";
        }

        emit viewportChanged(viewport);
    }
}

void EditorScene::clearSelectionBoxes(Qt3DCore::QEntity *skipEntity)
{
    Q_FOREACH (SceneItem *item, m_sceneItems.values()) {
        if (item->entity() != skipEntity)
            item->setShowSelectionBox(false);
    }
}

void EditorScene::endSelectionHandling()
{
    if (m_dragMode == DragNone && m_pickedEntity) {
        if (m_ctrlDownOnLastLeftPress) {
            // Multiselection handling
            toggleEntityMultiSelection(m_pickedEntity->objectName());
        } else {
            // Single selection handling
            setSelection(m_pickedEntity);

            // Selecting an object also starts drag, if translate handle is enabled
            Qt3DRender::QCamera *cameraEntity = qobject_cast<Qt3DRender::QCamera *>(m_pickedEntity);
            bool viewCenterDrag = cameraEntity && m_cameraViewCenterSelected && !m_viewCenterLocked;

        }
        m_pickedEntity = nullptr;
        m_pickedDistance = -1.0f;
    }
}

void EditorScene::handlePickerPress(Qt3DRender::QPickEvent *event)
{
    if (m_dragMode == DragNone && m_mouseButton == Qt::LeftButton) {
        Qt3DCore::QEntity *pressedEntity = qobject_cast<Qt3DCore::QEntity *>(sender()->parent());
        // If pressedEntity is not enabled, it typically means the pressedEntity is a drag handle
        // and the selection has changed to a different type of entity since the mouse press was
        // registered. Since the new entity is not the one we wanted to modify anyway, just
        // skip handling the pick event.
        if (pressedEntity && pressedEntity->isEnabled()) {
            if (pressedEntity->objectName() == helperArrowName) {
                if (m_selectedEntity) {
                    m_helperArrowGrabOffset =
                            event->worldIntersection() - m_helperArrowsTransform->translation();

                    m_pickedEntity = m_selectedEntity;
                }
            } else if (!m_pickedEntity || m_pickedDistance > event->distance()) {
                // Ignore presses that are farther away than the closest one
                m_pickedDistance = event->distance();
                bool select = false;
                SceneItem *item = m_sceneItems.value(pressedEntity->id(), nullptr);
                //                item->
                if (item) {
                    select = true;
                } else if (m_freeView) {
                    if (pressedEntity->objectName() == sceneLoaderSubEntityName) {
                        // Select the scene loader entity instead when picking one of loader's
                        // internal mesh entites.
                        Qt3DCore::QEntity *parentEntity = pressedEntity->parentEntity();
                        while (parentEntity) {
                            SceneItem *parentItem = m_sceneItems.value(parentEntity->id());
                            if (parentItem) {
                                pressedEntity = parentEntity;
                                select = true;
                                break;
                            } else {
                                parentEntity = parentEntity->parentEntity();
                            }
                        }
                    }
                }
                if (select && !m_pickedEntity)
                    QMetaObject::invokeMethod(this, "endSelectionHandling", Qt::QueuedConnection);
                m_pickedEntity = pressedEntity;
            }
        }
    }
    event->setAccepted(true);
}

void EditorScene::loaderstatusChange(QSceneLoader::Status status)
{
    if(status==QSceneLoader::Status::Ready){


        if(m_sceneloader->entityNames().length()>0){
            qDebug()<<"entityNames:"<<m_sceneloader->entityNames();

            m_baseEntity=m_sceneloader->entity(m_sceneloader->entityNames()[0])->parentEntity();
            this->addEntity(m_baseEntity);
        }



    }
    emit loaderstatusChanged(status);
}

bool EditorScene::handleMousePress(QMouseEvent *event)
{
    m_previousMousePosition = event->pos();
    m_mouseButton = event->button();
    if (m_mouseButton == Qt::LeftButton)
        m_ctrlDownOnLastLeftPress = event->modifiers() & Qt::ControlModifier;

    updateWorldPositionLabel(event->pos().x(), event->pos().y());
    return false; // Never consume press event
}

bool EditorScene::handleMouseRelease(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QPoint delta = event->pos() - m_previousMousePosition;
        if (delta.manhattanLength() < 5 && (m_dragMode == DragNone))
            emit mouseRightButtonReleasedWithoutDragging();
    }
    m_cameraViewCenterSelected = false;

    updateWorldPositionLabel(event->pos().x(), event->pos().y());
    return false; // Never consume release event
}

bool EditorScene::handleMouseMove(QMouseEvent *event)
{


    if (m_dragMode != DragNone) {
        // Selection dragging updates world position label to mesh center while dragging
        //        updateWorldPositionLabelToDragHandle(EditorScene::DragTranslate, 1);
    } else {
        updateWorldPositionLabel(event->pos().x(), event->pos().y());
    }
    return (m_dragMode != DragNone);
}

// Find out the normal of the helper plane.
QVector3D EditorScene::helperPlaneNormal(Qt3DCore::QTransform *planeTransform) const
{
    QVector3D helperPlaneNormal = planeTransform->matrix() * QVector3D(0.0f, 0.0f, 1.0f);

    helperPlaneNormal.setX(qAbs(helperPlaneNormal.x()));
    helperPlaneNormal.setY(qAbs(helperPlaneNormal.y()));
    helperPlaneNormal.setZ(qAbs(helperPlaneNormal.z()));
    return helperPlaneNormal.normalized();
}





// Projects vector to a plane defined by active frame graph camera
QVector3D EditorScene::projectVectorOnCameraPlane(const QVector3D &vector) const
{
    QVector3D projectionVector;
    Qt3DRender::QCamera *camera = frameGraphCamera();
    if (camera) {
        QVector3D planeNormal = camera->position() - camera->viewCenter();
        planeNormal.normalize();
        projectionVector = EditorUtils::projectVectorOnPlane(vector, planeNormal);
        // Have some valid vector at least if vector is too close to zero
        if (projectionVector.length() < 0.00001f) {
            projectionVector = QVector3D::crossProduct(planeNormal,
                                                       camera->upVector().normalized());
        }
    }
    return projectionVector;
}

// Rescales various entities that need to be constant size on the screen
void EditorScene::resizeConstantScreenSizeEntities()
{
    if (frameGraphCamera()) {
        // Camera viewcenter
        const float vcEntityAngle = 0.0045f;


        // Helper arrows
        const float arrowsEntityAngle = 0.035f;
        QVector3D arrowsPos = m_helperArrowsTransform->translation();
        float distanceToArrows = (arrowsPos - frameGraphCamera()->position()).length();
        float arrowsScale = arrowsEntityAngle * distanceToArrows;
        m_helperArrowsTransform->setScale(arrowsScale * 2.0f);
    }
}

bool EditorScene::isPropertyLocked(const QString &propertyName, QObject *obj)
{
    if (!obj)
        return false;
    QString lockProperty = propertyName + lockPropertySuffix();
    QByteArray nameArray = lockProperty.toLatin1();
    const char *namePtr = nameArray.constData();
    QVariant propertyVariant = obj->property(namePtr);
    if (propertyVariant.isValid())
        return propertyVariant.toBool();
    else
        return false;
}


void EditorScene::setSceneEntity(Qt3DCore::QEntity *newSceneEntity)
{
    if(m_sceneEntity){
        removeEntity(m_sceneEntity);
    }
    m_sceneEntity = newSceneEntity;
    if (m_sceneEntity ){

        addEntity(m_sceneEntity);
        //        m_sceneloader->setParent(m_sceneEntity);
        m_sceneEntity->addComponent(m_sceneloader);

    }
    emit sceneEntityChanged(m_sceneEntity);
}

void EditorScene::setSelectionItem(SceneItem *selectionItem)
{
    if (m_selectionItem == selectionItem)
        return;

    m_selectionItem = selectionItem;



    emit selectionItemChanged(m_selectionItem);
}

void EditorScene::createSceneLoaderChildPickers(Qt3DCore::QEntity *entity,
                                                QList<Qt3DRender::QObjectPicker *> *pickers)
{
    if (EditorUtils::entityMesh(entity)) {
        pickers->append(createObjectPickerForEntity(entity));
        // Rename entity so we can identify it later
        entity->setObjectName(sceneLoaderSubEntityName);
    }

    Q_FOREACH (QObject *child, entity->children()) {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity)
            createSceneLoaderChildPickers(childEntity, pickers);
    }
}


void EditorScene::handleViewportSizeChange()
{
    qreal aspectRatio = m_viewport->width() / qMax(m_viewport->height(), 1.0);
    m_freeViewCameraEntity->lens()->setPerspectiveProjection(
                freeViewCameraFov, aspectRatio, freeViewCameraNearPlane, freeViewCameraFarPlane);
    // Need to update drag handle positions
    //   handleSelectionTransformChange();
}

void EditorScene::handleEntityNameChange()
{
    Qt3DCore::QEntity *entity = qobject_cast<Qt3DCore::QEntity *>(sender());
    //    int cameraIndex = cameraIndexForEntity(entity);

}

bool EditorScene::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    // Filter undo and redo keysequences so TextFields don't get them
    switch (event->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke == QKeySequence::Redo) {

            return true;
        } else  if (ke == QKeySequence::Undo) {

            return true;
        }
        break;
    }
    case QEvent::MouseButtonPress:
        if (obj == m_viewport)
            return handleMousePress(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonRelease:
        if (obj == m_viewport)
            return handleMouseRelease(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseMove:
        if (obj == m_viewport)
            return handleMouseMove(static_cast<QMouseEvent *>(event));
        break;
    default:
        break;
    }

    return false;
}

void EditorScene::checkMultiSelectionHighlights()
{
    const QList<SceneItem *> items = m_sceneItems.values();
    for (int i = 0; i < items.size(); ++i) {
        SceneItem *item = items.at(i);
        if (m_selectedEntityNameList.contains(item->entity()->objectName()))
            item->setShowSelectionBox(true);
        else
            item->setShowSelectionBox(false);
    }
}

