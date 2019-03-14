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

#include "sceneitem.h"
#include "editorscene.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QJoint>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometryFactory>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QAbstractLight>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QSpotLight>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <cfloat>
#include <effects/wireframeeffect.h>

#include "editorsceneitemcomponentsmodel.h"


const QVector3D selectionBoxAdjuster(0.002f, 0.002f, 0.002f);

SceneItem::SceneItem(EditorScene *scene, Qt3DCore::QEntity *entity, SceneItem *parentItem, int index, QObject *parent)
    : QObject(parent)
    , m_entity(entity)
    , m_parentItem(parentItem)
    , m_componentsModel(new EditorSceneItemComponentsModel(this, this))
    , m_scene(scene)
    , m_selectionBox(nullptr)
    , m_selectionTransform(nullptr)
    , m_entityTransform(nullptr)
    , m_entityMesh(nullptr)
    , m_entityMeshExtents(1.0f, 1.0f, 1.0f)
    , m_canRotate(true)
    , m_internalPickers(nullptr)
{
    if (m_parentItem != nullptr)
        m_parentItem->addChild(this, index);


//    m_entityWireFrameMaterial= new Qt3DExtras::QPhongAlphaMaterial();



//    m_entityWireFrameMaterial->set
    //joint->

    // Selection box
    Qt3DCore::QComponentVector components = entity->components();
    Qt3DRender::QGeometryRenderer *entityMesh = nullptr;
    bool isLight = false;
    bool isSceneLoader = false;
    Q_FOREACH (Qt3DCore::QComponent *component, components) {
        if (!m_entityTransform)
            m_entityTransform = qobject_cast<Qt3DCore::QTransform *>(component);
        if (!entityMesh)
            entityMesh = qobject_cast<Qt3DRender::QGeometryRenderer *>(component);
        if (qobject_cast<Qt3DRender::QAbstractLight *>(component)) {
            isLight = true;
            if (!qobject_cast<Qt3DRender::QDirectionalLight *>(component)
                    && !qobject_cast<Qt3DRender::QSpotLight *>(component)) {
                m_canRotate = false;
            }
        }
        if (!m_entityMaterial)
            m_entityMaterial= qobject_cast<Qt3DRender::QMaterial*>(component);

        if (qobject_cast<Qt3DRender::QSceneLoader *>(component))
            isSceneLoader = true;
    }
    bool isCamera = qobject_cast<Qt3DRender::QCamera *>(entity);
    if (isCamera)
        m_entityMeshExtents = QVector3D(1.0f, 1.0f, 1.75f);

    // Selection transform is needed for child items, even if we don't have a box
    m_selectionTransform = new Qt3DCore::QTransform;

    // Save entity item type
    if (isLight)
        m_itemType = SceneItem::Light;
    else if (isCamera)
        m_itemType = SceneItem::Camera;
    else if (entityMesh)
        m_itemType = SceneItem::Mesh;
    else if (isSceneLoader)
        m_itemType = SceneItem::SceneLoader;
    else if (m_entityTransform)
        m_itemType = SceneItem::Group;
    else
        m_itemType = SceneItem::Other;

    // Show box if entity has transform
    if (m_entityTransform) {


        m_selectionBox = new Qt3DCore::QEntity(scene->rootEntity());


        m_selectionBox->setObjectName(QStringLiteral("__internal selection box"));

        m_selectionBox->addComponent(m_selectionTransform);

        m_selectionBox->addComponent(m_scene->selectionBoxMaterial());
        m_selectionBox->addComponent(m_scene->selectionBoxMesh());

        m_selectionBox->setEnabled(false);

        connect(m_selectionBox, &Qt3DCore::QEntity::enabledChanged,
                this, &SceneItem::showSelectionBoxChanged);

        // SceneLoader updates mesh extents asynchronously
        if (isLight || isCamera || m_itemType == SceneItem::Group || isSceneLoader)
            updateSelectionBoxTransform();
        else
            handleMeshChange(entityMesh);
    } else {
        updateSelectionBoxTransform();
    }
}

SceneItem::~SceneItem()
{
    setParentItem(nullptr);
    if (m_selectionBox) {
        m_selectionBox->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
        m_selectionBox->deleteLater();
    }
    delete m_internalPickers;
}

Qt3DCore::QEntity *SceneItem::entity()
{
    return m_entity;
}

const QList<SceneItem *> &SceneItem::childItems()
{
    return m_children;
}

SceneItem *SceneItem::parentItem()
{
    return m_parentItem;
}

int SceneItem::childNumber() const
{
    if (m_parentItem)
        return m_parentItem->childItems().indexOf(const_cast<SceneItem*>(this));

    return 0;
}

void SceneItem::addChild(SceneItem *child, int index)
{
    if (child == nullptr)
        return;

    if (!m_children.contains(child)) {
        if (index < 0)
            m_children.append(child);
        else
            m_children.insert(index, child);
    }
}

void SceneItem::removeChild(SceneItem *child)
{
    if (child == nullptr && m_children.length()<=0)
        return;

    if (m_children.contains(child))
        m_children.removeOne(child);
}

void SceneItem::setParentItem(SceneItem *parentItem)
{
    if (m_parentItem)
        m_parentItem->removeChild(this);

    if (parentItem)
        parentItem->addChild(this);

    m_parentItem = parentItem;
}

void SceneItem::setWireFrame(Qt3DRender::QEffect* effect)
{
//    m_entity->removeComponent(m_entityMaterial);
//    effect->setParent(m_entityMaterial);
//    m_entityMaterial->setEffect(effect);
    m_entityMaterial->setEffect(new WireframeEffect());
//    effect->setParent(m_entityMaterial);
//    m_entityMaterial->setEffect(effect);

//    Qt3DRender::QMaterial *meshCenterLineMaterial = new Qt3DRender::QMaterial();
////    meshCenterLineMaterial->setEffect(new WireframeEffect());
//    meshCenterLineMaterial->setEffect(effect);

//    m_entity->addComponent(meshCenterLineMaterial);
}

//SceneItemComponentsModel *SceneItem::componentsModel() const
//{
//    return m_componentsModel;
//}

//EditorScene *SceneItem::scene() const
//{
//    return m_scene;
//}

void SceneItem::handleMeshChange(Qt3DRender::QGeometryRenderer *newMesh)
{
    if (newMesh != m_entityMesh) {
        if (m_entityMesh && isSelectionBoxShowing())
            connectEntityMesh(false);

        EditorSceneItemMeshComponentsModel::MeshComponentTypes newType =
                EditorSceneItemMeshComponentsModel::meshType(newMesh);
        m_entityMeshType = newType;
        m_entityMesh = newMesh;

        if (m_entityMesh && isSelectionBoxShowing())
            connectEntityMesh(true);

        recalculateMeshExtents();
    }
}

void SceneItem::recalculateMeshExtents()
{
    m_entityMeshCenter = QVector3D();
    switch (m_entityMeshType) {
    case EditorSceneItemMeshComponentsModel::Custom: {
        recalculateCustomMeshExtents(m_entityMesh, m_entityMeshExtents, m_entityMeshCenter);
        break;
    }
    case EditorSceneItemMeshComponentsModel::Cuboid: {
        Qt3DExtras::QCuboidMesh *mesh =
                qobject_cast<Qt3DExtras::QCuboidMesh *>(m_entityMesh);
        m_entityMeshExtents = QVector3D(mesh->xExtent(), mesh->yExtent(), mesh->zExtent());
        break;
    }
    case EditorSceneItemMeshComponentsModel::Cylinder: {
        Qt3DExtras::QCylinderMesh *mesh =
                qobject_cast<Qt3DExtras::QCylinderMesh *>(m_entityMesh);
        float diameter = mesh->radius() * 2.0f;
        m_entityMeshExtents = QVector3D(diameter, mesh->length(), diameter);
        break;
    }
    case EditorSceneItemMeshComponentsModel::Plane: {
        Qt3DExtras::QPlaneMesh *mesh =
                qobject_cast<Qt3DExtras::QPlaneMesh *>(m_entityMesh);
        m_entityMeshExtents = QVector3D(mesh->width(), 0, mesh->height());
        break;
    }
    case EditorSceneItemMeshComponentsModel::Sphere: {
        Qt3DExtras::QSphereMesh *mesh =
                qobject_cast<Qt3DExtras::QSphereMesh *>(m_entityMesh);
        float diameter = mesh->radius() * 2.0f;
        m_entityMeshExtents = QVector3D(diameter, diameter, diameter);
        break;
    }
    case EditorSceneItemMeshComponentsModel::Torus: {
        Qt3DExtras::QTorusMesh *mesh =
                qobject_cast<Qt3DExtras::QTorusMesh *>(m_entityMesh);
        float minorDiameter = mesh->minorRadius() * 2.0f;
        float diameter = mesh->radius() * 2.0f + minorDiameter;
        m_entityMeshExtents = QVector3D(diameter, diameter, minorDiameter);
        break;
    }
    case EditorSceneItemMeshComponentsModel::Unknown: {
        // Unknown means generic mesh
        recalculateCustomMeshExtents(m_entityMesh, m_entityMeshExtents, m_entityMeshCenter);
        break;
    }
    default:
        //Unsupported mesh type
        m_entityMeshExtents = QVector3D(1.0f, 1.0f, 1.0f);
        break;
    }
    updateSelectionBoxTransform();
}

void SceneItem::recalculateCustomMeshExtents(Qt3DRender::QGeometryRenderer *mesh,
                                             QVector3D &meshExtents,
                                             QVector3D &meshCenter)
{
    // For custom meshes we need to calculate the extents from geometry
    Qt3DRender::QGeometry *meshGeometry = mesh->geometry();
    if (!meshGeometry || m_useGeometryFunctor) {
        Qt3DRender::QGeometryFactoryPtr geometryFunctorPtr = mesh->geometryFactory();
        if (geometryFunctorPtr.data()) {
            // Execute the geometry functor to get the geometry, since its not normally available
            // on the application side.
            meshGeometry = geometryFunctorPtr.data()->operator()();
            // Use geometry functor also in future for this item go get the geometry, if the mesh
            // needs to be recalculated. Otherwise we are likely to get obsolete geometry.
            m_useGeometryFunctor = true;
        }
    }

    if (meshGeometry) {
        // Set default in case we can't determine the geometry: normalized mesh in range [-1,1]
        meshExtents = QVector3D(2.0f, 2.0f, 2.0f);
        meshCenter =  QVector3D();

        Qt3DRender::QAttribute *vPosAttribute = nullptr;
        Q_FOREACH (Qt3DRender::QAttribute *attribute, meshGeometry->attributes()) {
            if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
                vPosAttribute = attribute;
                break;
            }
        }
        if (vPosAttribute) {
            const float *bufferPtr =
                    reinterpret_cast<const float *>(vPosAttribute->buffer()->data().constData());
            uint stride = vPosAttribute->byteStride() / sizeof(float);
            uint offset = vPosAttribute->byteOffset() / sizeof(float);
            bufferPtr += offset;
            uint vertexCount = vPosAttribute->count();
            uint dataCount = vPosAttribute->buffer()->data().size() / sizeof(float);

            // Make sure we have valid data
            if (((vertexCount * stride) + offset) > dataCount)
                return;

            float minX = FLT_MAX;
            float minY = FLT_MAX;
            float minZ = FLT_MAX;
            float maxX = -FLT_MAX;
            float maxY = -FLT_MAX;
            float maxZ = -FLT_MAX;

            if (stride)
                stride = stride - 3; // Three floats per vertex
            for (uint i = 0; i < vertexCount; i++) {
                float xVal = *bufferPtr++;
                minX = qMin(xVal, minX);
                maxX = qMax(xVal, maxX);
                float yVal = *bufferPtr++;
                minY = qMin(yVal, minY);
                maxY = qMax(yVal, maxY);
                float zVal = *bufferPtr++;
                minZ = qMin(zVal, minZ);
                maxZ = qMax(zVal, maxZ);
                bufferPtr += stride;
            }
            meshExtents = QVector3D(maxX - minX, maxY - minY, maxZ - minZ);
            meshCenter = QVector3D(minX + meshExtents.x() / 2.0f,
                                   minY + meshExtents.y() / 2.0f,
                                   minZ + meshExtents.z() / 2.0f);
        }
    } else {
        meshExtents = QVector3D();
        meshCenter =  QVector3D();
    }
}

void SceneItem::recalculateSubMeshesExtents()
{
    QVector<QVector3D> subMeshPoints;
    m_entityMeshType = EditorSceneItemMeshComponentsModel::SubMeshes;

    populateSubMeshData(m_entity, subMeshPoints);

    if (subMeshPoints.size()) {
        // Calculate complete extents from submesh data
        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float minZ = FLT_MAX;
        float maxX = -FLT_MAX;
        float maxY = -FLT_MAX;
        float maxZ = -FLT_MAX;
        Q_FOREACH (QVector3D points, subMeshPoints) {
            minX = qMin(points.x(), minX);
            maxX = qMax(points.x(), maxX);
            minY = qMin(points.y(), minY);
            maxY = qMax(points.y(), maxY);
            minZ = qMin(points.z(), minZ);
            maxZ = qMax(points.z(), maxZ);
        }

        m_entityMeshExtents = QVector3D(maxX - minX, maxY - minY, maxZ - minZ);
        m_entityMeshCenter = QVector3D(minX + m_entityMeshExtents.x() / 2.0f,
                                       minY + m_entityMeshExtents.y() / 2.0f,
                                       minZ + m_entityMeshExtents.z() / 2.0f);
    }
    updateSelectionBoxTransform();
}

void SceneItem::populateSubMeshData(Qt3DCore::QEntity *entity,
                                    QVector<QVector3D> &subMeshPoints)
{
    Qt3DRender::QGeometryRenderer *mesh = EditorUtils::entityMesh(entity);
    if (mesh) {
        QVector3D meshExtents;
        QVector3D meshCenter;
        recalculateCustomMeshExtents(mesh, meshExtents, meshCenter);
        Qt3DCore::QTransform *transform = EditorUtils::entityTransform(entity);
        if (transform) {
            // We are only interested on internal transforms when determining the extents
            QMatrix4x4 totalTransform;
            QList<Qt3DCore::QTransform *> transforms =
                    EditorUtils::ancestralTransforms(entity, m_entity);
            for (int i = transforms.size() - 1; i >= 0; i--)
                totalTransform *= transforms.at(i)->matrix();
            totalTransform *= transform->matrix();
            // Apply transform to two opposite extent box corners to find actual geometry points
            QVector3D halfExtents = meshExtents / 2.0f;
            subMeshPoints.append(totalTransform.map(meshCenter + halfExtents));
            subMeshPoints.append(totalTransform.map(meshCenter - halfExtents));
        }
    }

    Q_FOREACH (QObject *child, entity->children()) {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity)
            populateSubMeshData(childEntity, subMeshPoints);
    }
}

//void SceneItem::updateChildLightTransforms()
//{
//    Q_FOREACH (SceneItem *child, m_children) {
//        child->updateChildLightTransforms();
//        if (child->itemType() == Light)
//            m_scene->updateLightVisibleTransform(child->entity());
//    }
//}

void SceneItem::updateGroupExtents()
{
    if (m_itemType == SceneItem::Group) {
        QVector3D min(FLT_MAX, FLT_MAX, FLT_MAX);
        QVector3D max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        QMatrix4x4 matrix;

        // Go through all children and map their selection box extents to group coordinates
        if (childItems().size()) {
            Q_FOREACH (SceneItem *child, childItems())
                child->findTotalExtents(min, max, matrix);

            m_entityMeshExtents = QVector3D(max.x() - min.x(),
                                            max.y() - min.y(),
                                            max.z() - min.z());
            m_entityMeshCenter = QVector3D(min.x() + m_entityMeshExtents.x() / 2.0f,
                                           min.y() + m_entityMeshExtents.y() / 2.0f,
                                           min.z() + m_entityMeshExtents.z() / 2.0f);
        }

        // Finally update the group's selection box
        doUpdateSelectionBoxTransform();
    }
}

QMatrix4x4 SceneItem::composeSelectionBoxTransform()
{
    QMatrix4x4 totalTransform;



    totalTransform = EditorUtils::totalAncestralTransform(m_entity);
    if (m_entityTransform)
        totalTransform *= m_entityTransform->matrix();


    return totalTransform;
}

void SceneItem::doUpdateSelectionBoxTransform()
{
    // Transform mesh extents, first scale, then translate
    QMatrix4x4 transformMatrix = composeSelectionBoxTransform();
    transformMatrix.translate(m_entityMeshCenter);
    m_selectionBoxCenter = transformMatrix * QVector3D();
    m_unadjustedSelectionBoxExtents = m_entityMeshExtents;
    m_unadjustedSelectionBoxMatrix = transformMatrix;
    transformMatrix.scale(m_unadjustedSelectionBoxExtents + selectionBoxAdjuster);

    QVector3D ancestralScale = EditorUtils::totalAncestralScale(m_entity);
    m_unadjustedSelectionBoxExtents *= ancestralScale;
    if (m_entityTransform)
        m_unadjustedSelectionBoxExtents *= m_entityTransform->scale3D();

    m_selectionTransform->setMatrix(transformMatrix);

    // Check if we have lights as children and update their visible translations, as they are
    // not part of the normal scene.
    //        updateChildLightTransforms();

    emit selectionBoxTransformChanged(this);
}

void SceneItem::findTotalExtents(QVector3D &min, QVector3D &max, const QMatrix4x4 &matrix)
{
    QMatrix4x4 newMatrix = matrix;
    doUpdateSelectionBoxTransform();
    if (m_entityTransform)
        newMatrix *= m_entityTransform->matrix();

    // Skip groups when finding total extents
    if (m_itemType != SceneItem::Group) {
        QVector<QVector3D> corners = getSelectionBoxCorners(newMatrix);
        Q_FOREACH (QVector3D corner, corners) {
            min.setX(qMin(corner.x(), min.x()));
            min.setY(qMin(corner.y(), min.y()));
            min.setZ(qMin(corner.z(), min.z()));
            max.setX(qMax(corner.x(), max.x()));
            max.setY(qMax(corner.y(), max.y()));
            max.setZ(qMax(corner.z(), max.z()));
        }
    }

    Q_FOREACH (SceneItem *child, childItems())
        child->findTotalExtents(min, max, newMatrix);
}


void SceneItem::connectEntityMesh(bool enabled)
{
    if (enabled) {
        switch (m_entityMeshType) {
        case EditorSceneItemMeshComponentsModel::Custom: {
            Qt3DRender::QMesh *mesh = qobject_cast<Qt3DRender::QMesh *>(m_entityMesh);
            connect(mesh, &Qt3DRender::QMesh::sourceChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Cuboid: {
            Qt3DExtras::QCuboidMesh *mesh =
                    qobject_cast<Qt3DExtras::QCuboidMesh *>(m_entityMesh);
            connect(mesh, &Qt3DExtras::QCuboidMesh::xExtentChanged,
                    this, &SceneItem::recalculateMeshExtents);
            connect(mesh, &Qt3DExtras::QCuboidMesh::yExtentChanged,
                    this, &SceneItem::recalculateMeshExtents);
            connect(mesh, &Qt3DExtras::QCuboidMesh::zExtentChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Cylinder: {
            Qt3DExtras::QCylinderMesh *mesh =
                    qobject_cast<Qt3DExtras::QCylinderMesh *>(m_entityMesh);
            connect(mesh, &Qt3DExtras::QCylinderMesh::radiusChanged,
                    this, &SceneItem::recalculateMeshExtents);
            connect(mesh, &Qt3DExtras::QCylinderMesh::lengthChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Plane: {
            Qt3DExtras::QPlaneMesh *mesh =
                    qobject_cast<Qt3DExtras::QPlaneMesh *>(m_entityMesh);
            connect(mesh, &Qt3DExtras::QPlaneMesh::widthChanged,
                    this, &SceneItem::recalculateMeshExtents);
            connect(mesh, &Qt3DExtras::QPlaneMesh::heightChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Sphere: {
            Qt3DExtras::QSphereMesh *mesh =
                    qobject_cast<Qt3DExtras::QSphereMesh *>(m_entityMesh);
            connect(mesh, &Qt3DExtras::QSphereMesh::radiusChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Torus: {
            Qt3DExtras::QTorusMesh *mesh =
                    qobject_cast<Qt3DExtras::QTorusMesh *>(m_entityMesh);
            connect(mesh, &Qt3DExtras::QTorusMesh::radiusChanged,
                    this, &SceneItem::recalculateMeshExtents);
            connect(mesh, &Qt3DExtras::QTorusMesh::minorRadiusChanged,
                    this, &SceneItem::recalculateMeshExtents);
            break;
        }
        default:
            //Unsupported mesh type
            break;
        }
    } else {
        switch (m_entityMeshType) {
        case EditorSceneItemMeshComponentsModel::Custom: {
            Qt3DRender::QMesh *mesh = qobject_cast<Qt3DRender::QMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DRender::QMesh::sourceChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Cuboid: {
            Qt3DExtras::QCuboidMesh *mesh =
                    qobject_cast<Qt3DExtras::QCuboidMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DExtras::QCuboidMesh::xExtentChanged,
                       this, &SceneItem::recalculateMeshExtents);
            disconnect(mesh, &Qt3DExtras::QCuboidMesh::yExtentChanged,
                       this, &SceneItem::recalculateMeshExtents);
            disconnect(mesh, &Qt3DExtras::QCuboidMesh::zExtentChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Cylinder: {
            Qt3DExtras::QCylinderMesh *mesh =
                    qobject_cast<Qt3DExtras::QCylinderMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DExtras::QCylinderMesh::radiusChanged,
                       this, &SceneItem::recalculateMeshExtents);
            disconnect(mesh, &Qt3DExtras::QCylinderMesh::lengthChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Plane: {
            Qt3DExtras::QPlaneMesh *mesh =
                    qobject_cast<Qt3DExtras::QPlaneMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DExtras::QPlaneMesh::widthChanged,
                       this, &SceneItem::recalculateMeshExtents);
            disconnect(mesh, &Qt3DExtras::QPlaneMesh::heightChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Sphere: {
            Qt3DExtras::QSphereMesh *mesh =
                    qobject_cast<Qt3DExtras::QSphereMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DExtras::QSphereMesh::radiusChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        case EditorSceneItemMeshComponentsModel::Torus: {
            Qt3DExtras::QTorusMesh *mesh =
                    qobject_cast<Qt3DExtras::QTorusMesh *>(m_entityMesh);
            disconnect(mesh, &Qt3DExtras::QTorusMesh::radiusChanged,
                       this, &SceneItem::recalculateMeshExtents);
            disconnect(mesh, &Qt3DExtras::QTorusMesh::minorRadiusChanged,
                       this, &SceneItem::recalculateMeshExtents);
            break;
        }
        default:
            //Unsupported mesh type
            break;
        }
    }
}

QVector<QVector3D> SceneItem::getSelectionBoxCorners(const QMatrix4x4 &matrix)
{
    QVector<QVector3D> corners(8);
    const QVector3D &e = (selectionBoxAdjuster + m_entityMeshExtents) / 2.0f;
    const QVector3D &c = m_entityMeshCenter;

    corners[0] = matrix * (c + QVector3D( e.x(),  e.y(),  e.z()));
    corners[1] = matrix * (c + QVector3D( e.x(),  e.y(), -e.z()));
    corners[2] = matrix * (c + QVector3D( e.x(), -e.y(),  e.z()));
    corners[3] = matrix * (c + QVector3D( e.x(), -e.y(), -e.z()));
    corners[4] = matrix * (c + QVector3D(-e.x(),  e.y(),  e.z()));
    corners[5] = matrix * (c + QVector3D(-e.x(),  e.y(), -e.z()));
    corners[6] = matrix * (c + QVector3D(-e.x(), -e.y(),  e.z()));
    corners[7] = matrix * (c + QVector3D(-e.x(), -e.y(), -e.z()));

    return corners;
}

void SceneItem::updateSelectionBoxTransform()
{
    if (isSelectionBoxShowing())
        doUpdateSelectionBoxTransform();

    // Update selection boxes of all child entites, too
    Q_FOREACH (SceneItem *child, childItems())
        child->updateSelectionBoxTransform();
}

void SceneItem::setShowSelectionBox(bool enabled)
{
    if (m_selectionBox && m_selectionBox->isEnabled() != enabled) {
        //        connectSelectionBoxTransformsRecursive(enabled);
        //        connectEntityMesh(enabled);
        m_selectionBox->setEnabled(enabled);

        if (enabled) {
            // Update mesh extents if they are dirty, otherwise just update transform
            if (m_entityMeshExtents.isNull())
                recalculateMeshExtents();
            else if (m_itemType == SceneItem::Group)
                updateGroupExtents();
            else
                updateSelectionBoxTransform();
        }
    }
}

bool SceneItem::isSelectionBoxShowing() const
{
    if (m_selectionBox)
        return m_selectionBox->isEnabled();
    else
        return false;
}

EditorSceneItemComponentsModel *SceneItem::componentsModel() const
{
    return m_componentsModel;
}

EditorScene *SceneItem::scene() const
{
    return m_scene;
}

bool SceneItem::setCustomProperty(QObject *component, const QString name,
                                  const QVariant &value)
{
    // Sets the 'name' property to value to a component or the entity itself if component is null.
    // If the property is defined using Q_PROPERTY, returns true
    // on success, else returns false. This complies with QObject::property().

    QObject *targetObj = component;
    if (!targetObj)
        targetObj = m_entity;

    QByteArray nameArray = name.toLatin1();
    const char *propertyName = nameArray.constData();

    //    m_scene->handlePropertyLocking(this, name, value.toBool());

    return targetObj->setProperty(propertyName, value);
}

QVariant SceneItem::customProperty(QObject *component, const QString name) const
{
    // Returns the value of the 'name' property of the component.
    // If component is null, entity itself is checked for the property.
    // If the property doesn't exist, the returned variant
    // is invalid. This complies with QObject::property().

    QObject *targetObj = component;
    if (!targetObj)
        targetObj = m_entity;

    QByteArray nameArray = name.toLatin1();
    const char *propertyName = nameArray.constData();
    QVariant propertyVariant = targetObj->property(propertyName);
    if (propertyVariant.isValid())
        return propertyVariant;
    else
        return QVariant();
}

QList<Qt3DRender::QObjectPicker *> *SceneItem::internalPickers()
{
    if (!m_internalPickers)
        m_internalPickers = new QList<Qt3DRender::QObjectPicker *>();
    return m_internalPickers;
}
