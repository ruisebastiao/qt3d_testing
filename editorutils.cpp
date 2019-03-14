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
#include "editorutils.h"
#include "qdummyobjectpicker.h"

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderState>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DExtras/QDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QGoochMaterial>
#include <Qt3DExtras/QNormalDiffuseMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QAbstractLight>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QSpotLight>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryFactory>

#include <Qt3DRender/QAlphaCoverage>
#include <Qt3DRender/QAlphaTest>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDithering>
#include <Qt3DRender/QFrontFace>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QPolygonOffset>
#include <Qt3DRender/QScissorTest>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestArguments>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilOperation>
#include <Qt3DRender/QStencilOperationArguments>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QSeamlessCubemap>
//#include <Qt3DRender/private/qrenderstate_p.h>

#include <QtCore/QtMath>

static const QString internalPrefix = QStringLiteral("__internal");

bool EditorUtils::isObjectInternal(QObject *obj)
{
    if (obj)
        return obj->objectName().startsWith(internalPrefix);
    else
        return false;
}


Qt3DRender::QGeometryRenderer *EditorUtils::createWireframeBoxMesh(float extent)
{
    // Creates a box 'mesh' that is is made up of 12 GL_LINES between 8 vertices
    Qt3DRender::QGeometryRenderer *boxMesh = new Qt3DRender::QGeometryRenderer();
    Qt3DRender::QGeometry *boxGeometry = new Qt3DRender::QGeometry(boxMesh);
    Qt3DRender::QBuffer *boxDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,
                                                                 boxGeometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer,
                                                                   boxGeometry);
    QByteArray vertexBufferData;
    QByteArray indexBufferData;

    vertexBufferData.resize(8 * 3 * sizeof(float));
    indexBufferData.resize(12 * 2 * sizeof(ushort));

    float dimension = extent / 2.0f;

    float *vPtr = reinterpret_cast<float *>(vertexBufferData.data());
    vPtr[0]  = -dimension; vPtr[1]  = -dimension; vPtr[2]  = -dimension;
    vPtr[3]  = dimension;  vPtr[4]  = -dimension; vPtr[5]  = -dimension;
    vPtr[6]  = dimension;  vPtr[7]  = -dimension; vPtr[8]  = dimension;
    vPtr[9]  = -dimension; vPtr[10] = -dimension; vPtr[11] = dimension;
    vPtr[12] = -dimension; vPtr[13] = dimension;  vPtr[14] = -dimension;
    vPtr[15] = dimension;  vPtr[16] = dimension;  vPtr[17] = -dimension;
    vPtr[18] = dimension;  vPtr[19] = dimension;  vPtr[20] = dimension;
    vPtr[21] = -dimension; vPtr[22] = dimension;  vPtr[23] = dimension;

    ushort *iPtr = reinterpret_cast<ushort *>(indexBufferData.data());
    iPtr[0]  = 0; iPtr[1]  = 1;
    iPtr[2]  = 1; iPtr[3]  = 2;
    iPtr[4]  = 2; iPtr[5]  = 3;
    iPtr[6]  = 3; iPtr[7]  = 0;
    iPtr[8]  = 0; iPtr[9]  = 4;
    iPtr[10] = 1; iPtr[11] = 5;
    iPtr[12] = 2; iPtr[13] = 6;
    iPtr[14] = 3; iPtr[15] = 7;
    iPtr[16] = 4; iPtr[17] = 5;
    iPtr[18] = 5; iPtr[19] = 6;
    iPtr[20] = 6; iPtr[21] = 7;
    iPtr[22] = 7; iPtr[23] = 4;

    boxDataBuffer->setData(vertexBufferData);
    indexDataBuffer->setData(indexBufferData);

    addPositionAttributeToGeometry(boxGeometry, boxDataBuffer, 8);
    addIndexAttributeToGeometry(boxGeometry, indexDataBuffer, 24);

    boxMesh->setInstanceCount(1);
    boxMesh->setIndexOffset(0);
    boxMesh->setFirstInstance(0);
    boxMesh->setVertexCount(24);
    boxMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    boxMesh->setGeometry(boxGeometry);

    return boxMesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createWireframePlaneMesh(int lineCount)
{
    Qt3DRender::QGeometryRenderer *planeMesh = new Qt3DRender::QGeometryRenderer();
    Qt3DRender::QGeometry *planeGeometry = new Qt3DRender::QGeometry(planeMesh);
    Qt3DRender::QBuffer *planeDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,
                                                                   planeGeometry);
    QByteArray vertexBufferData;
    QVector<QVector3D> vertices;

    // lineCount lines on x and z directions, each with two vector3Ds
    vertices.resize(lineCount * 2 * 2);
    vertexBufferData.resize(vertices.size() * 3 * sizeof(float));

    for (int i = 0; i < lineCount; i++) {
        int index = i * 2;
        vertices[index] = QVector3D(-1.0f + (float(i) * (2.0 / (lineCount - 1))), -1.0f, 0.0f);
        vertices[index + 1] = QVector3D(-1.0f + (float(i) * (2.0 / (lineCount - 1))), 1.0f, 0.0f);
        vertices[index + lineCount * 2] = QVector3D(-1.0f, -1.0f + (float(i) * (2.0 / (lineCount - 1))), 0.0f);
        vertices[index + lineCount * 2 + 1] = QVector3D(1.0f, -1.0f + (float(i) * (2.0 / (lineCount - 1))), 0.0f);
    }

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    planeDataBuffer->setData(vertexBufferData);

    addPositionAttributeToGeometry(planeGeometry, planeDataBuffer, lineCount * 4);

    planeMesh->setInstanceCount(1);
    planeMesh->setIndexOffset(0);
    planeMesh->setFirstInstance(0);
    planeMesh->setVertexCount(lineCount * 4);
    planeMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    planeMesh->setGeometry(planeGeometry);

    return planeMesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createSingleLineMesh()
{
    Qt3DRender::QGeometryRenderer *mesh = new Qt3DRender::QGeometryRenderer();
    Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry(mesh);
    Qt3DRender::QBuffer *dataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,
                                                              geometry);
    QByteArray vertexBufferData;
    QVector<QVector3D> vertices;

    vertices.resize(2);
    vertexBufferData.resize(vertices.size() * 3 * sizeof(float));

    vertices[0] = QVector3D(0.0f, 0.0f, 0.0f);
    vertices[1] = QVector3D(0.0f, 0.0f, 1.0f);

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    dataBuffer->setData(vertexBufferData);

    addPositionAttributeToGeometry(geometry, dataBuffer, 2);

    mesh->setInstanceCount(1);
    mesh->setIndexOffset(0);
    mesh->setFirstInstance(0);
    mesh->setVertexCount(2);
    mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    mesh->setGeometry(geometry);

    return mesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createDefaultCustomMesh()
{
    Qt3DRender::QMesh *customMesh = new Qt3DRender::QMesh();
    customMesh->setSource(QUrl(QStringLiteral("qrc:/qt3deditorlib/meshes/defaultmesh.obj")));
    return customMesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createVisibleCameraMesh()
{
    Qt3DRender::QMesh *customMesh = new Qt3DRender::QMesh();
    customMesh->setSource(QUrl(QStringLiteral("qrc:/qt3deditorlib/meshes/camera.obj")));
    return customMesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createCameraViewVectorMesh()
{
    // Creates a camera target indicator 'mesh' that is is made up of GL_LINES
    Qt3DRender::QGeometryRenderer *mesh = new Qt3DRender::QGeometryRenderer();
    Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry(mesh);
    Qt3DRender::QBuffer *dataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,
                                                              geometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer,
                                                                   geometry);
    QByteArray vertexBufferData;
    QByteArray indexBufferData;

    vertexBufferData.resize(2 * 3 * sizeof(float));
    indexBufferData.resize(1 * 2 * sizeof(ushort));

    float *vPtr = reinterpret_cast<float *>(vertexBufferData.data());
    vPtr[0]  = 0.0f;  vPtr[1]  = 0.0f;  vPtr[2]  = 0.0f;
    vPtr[3]  = 0.0f;  vPtr[4]  = 0.0f;  vPtr[5]  = -1.0f;

    ushort *iPtr = reinterpret_cast<ushort *>(indexBufferData.data());
    iPtr[0]  = 0; iPtr[1]  = 1;

    dataBuffer->setData(vertexBufferData);
    indexDataBuffer->setData(indexBufferData);

    addPositionAttributeToGeometry(geometry, dataBuffer, 2);
    addIndexAttributeToGeometry(geometry, indexDataBuffer, 2);

    mesh->setInstanceCount(1);
    mesh->setIndexOffset(0);
    mesh->setFirstInstance(0);
    mesh->setVertexCount(2);
    mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    mesh->setGeometry(geometry);

    return mesh;
}

Qt3DRender::QGeometry *EditorUtils::getMeshGeometry(Qt3DCore::QEntity *entity)
{
//    entity->
}

Qt3DRender::QGeometryRenderer *EditorUtils::createCameraViewCenterMesh(float size)
{
    // TODO: proper mesh
    Qt3DExtras::QSphereMesh *mesh = new Qt3DExtras::QSphereMesh;
    mesh->setRadius(size / 2.0f);
    return mesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createLightMesh(EditorUtils::ComponentTypes type)
{
    Qt3DRender::QGeometryRenderer *mesh = nullptr;

    switch (type) {
    case LightDirectional: {
        Qt3DRender::QMesh *directionalMesh = new Qt3DRender::QMesh();
        directionalMesh->setSource(QUrl(QStringLiteral("qrc:/qt3deditorlib/meshes/directionallight.obj")));
        mesh = directionalMesh;
        break;
    }
    case LightPoint: {
        Qt3DExtras::QSphereMesh *pointMesh = new Qt3DExtras::QSphereMesh();
        pointMesh->setRadius(0.2f);
        pointMesh->setRings(10);
        pointMesh->setSlices(10);
        mesh = pointMesh;
        break;
    }
    case LightSpot: {
        Qt3DRender::QMesh *spotMesh = new Qt3DRender::QMesh();
        spotMesh->setSource(QUrl(QStringLiteral("qrc:/qt3deditorlib/meshes/spotlight.obj")));
        mesh = spotMesh;
        break;
    }
    default: {
        qCritical("Should not get here.");
        break;
    }
    }

    return mesh;
}

Qt3DRender::QGeometryRenderer *EditorUtils::createMeshForInsertableType(InsertableEntities type)
{
    Qt3DRender::QGeometryRenderer *mesh = nullptr;
    switch (type) {
    case CuboidEntity: {
        mesh = new Qt3DExtras::QCuboidMesh();
        break;
    }
    case CylinderEntity: {
        mesh = new Qt3DExtras::QCylinderMesh();
        break;
    }
    case PlaneEntity: {
        mesh = new Qt3DExtras::QPlaneMesh();
        break;
    }
    case SphereEntity: {
        mesh = new Qt3DExtras::QSphereMesh();
        break;
    }
    case TorusEntity: {
        mesh = new Qt3DExtras::QTorusMesh();
        break;
    }
    case CustomEntity: {
        mesh = createDefaultCustomMesh();
        break;
    }
    case GroupEntity: {
        // Group entity mesh is only used for drag-insert placeholder
        mesh = new Qt3DExtras::QCuboidMesh();
        break;
    }
    default:
        break;
    }

    return mesh;
}

// Creates a single arrow
Qt3DRender::QGeometryRenderer *EditorUtils::createArrowMesh()
{
    Qt3DRender::QMesh *customMesh = new Qt3DRender::QMesh();
    customMesh->setSource(QUrl(QStringLiteral("qrc:/meshes/arrow.obj")));
    return customMesh;
}

Qt3DCore::QEntity *EditorUtils::createArrowEntity(const QColor &color,
                                                  Qt3DCore::QEntity *parent,
                                                  const QMatrix4x4 &matrix,
                                                  const QString &name)
{
    Qt3DCore::QEntity *arrow = new Qt3DCore::QEntity(parent);
    arrow->setObjectName(name);

    Qt3DRender::QGeometryRenderer *mesh = EditorUtils::createArrowMesh();

    Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
//    material->setEffect(new OnTopEffect());
    material->addParameter(new Qt3DRender::QParameter(QStringLiteral("handleColor"), color));

    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    transform->setMatrix(matrix);

    arrow->addComponent(mesh);
    arrow->addComponent(material);
    arrow->addComponent(transform);

    return arrow;
}

void EditorUtils::addPositionAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                                 Qt3DRender::QBuffer *buffer, int count)
{
    Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
    posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    posAttribute->setBuffer(buffer);
    posAttribute->setDataType(Qt3DRender::QAttribute::Float);
    posAttribute->setDataSize(3);
    posAttribute->setByteOffset(0);
    posAttribute->setByteStride(0);
    posAttribute->setCount(count);
    posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    geometry->addAttribute(posAttribute);
}

void EditorUtils::addIndexAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                              Qt3DRender::QBuffer *buffer, int count)
{
    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(buffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(count);

    geometry->addAttribute(indexAttribute);
}

void EditorUtils::updateCameraFrustumMesh(Qt3DRender::QGeometryRenderer *mesh,
                                          Qt3DRender::QCamera *camera)
{
    QMatrix4x4 projectionMatrix = camera->projectionMatrix().inverted();

    Qt3DRender::QGeometry *geometry = mesh->geometry();

    Qt3DRender::QBuffer *dataBuffer = nullptr;
    Q_FOREACH (Qt3DRender::QAttribute *attribute, geometry->attributes()) {
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            dataBuffer = attribute->buffer();
            break;
        }
    }

    if (dataBuffer) {
        QByteArray newData;
        newData.resize(dataBuffer->data().size());
        float *vPtr = reinterpret_cast<float *>(newData.data());

        vPtr[0]  = -1.0f; vPtr[1]  = -1.0f; vPtr[2]  = -1.0f;
        vPtr[3]  = 1.0f;  vPtr[4]  = -1.0f; vPtr[5]  = -1.0f;
        vPtr[6]  = 1.0f;  vPtr[7]  = 1.0f;  vPtr[8]  = -1.0f;
        vPtr[9]  = -1.0f; vPtr[10] = 1.0f;  vPtr[11] = -1.0f;
        vPtr[12] = -1.0f; vPtr[13] = -1.0f; vPtr[14] = 1.0f;
        vPtr[15] = 1.0f;  vPtr[16] = -1.0f; vPtr[17] = 1.0f;
        vPtr[18] = 1.0f;  vPtr[19] = 1.0f;  vPtr[20] = 1.0f;
        vPtr[21] = -1.0f; vPtr[22] = 1.0f;  vPtr[23] = 1.0f;

        for (int i = 0; i < 24; i += 3) {
            QVector3D vertex(vPtr[i], vPtr[i + 1], vPtr[i + 2]);
            vertex = projectionMatrix * vertex;
            vPtr[i] = vertex.x();
            vPtr[i + 1] = vertex.y();
            vPtr[i + 2] = vertex.z();
        }
        dataBuffer->setData(newData);
    }
}

Qt3DCore::QTransform *EditorUtils::entityTransform(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DCore::QTransform *transform = qobject_cast<Qt3DCore::QTransform *>(components.value(i));
        if (transform)
            return transform;
    }

    return nullptr;
}

Qt3DRender::QAbstractLight *EditorUtils::entityLight(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DRender::QAbstractLight *light = qobject_cast<Qt3DRender::QAbstractLight *>(components.value(i));
        if (light)
            return light;
    }

    return nullptr;
}

Qt3DRender::QObjectPicker *EditorUtils::entityPicker(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DRender::QObjectPicker *picker
                = qobject_cast<Qt3DRender::QObjectPicker *>(components.value(i));
        if (picker)
            return picker;
    }

    return nullptr;
}

Qt3DRender::QSceneLoader *EditorUtils::entitySceneLoader(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DRender::QSceneLoader *loader
                = qobject_cast<Qt3DRender::QSceneLoader *>(components.value(i));
        if (loader)
            return loader;
    }

    return nullptr;
}

Qt3DRender::QGeometryRenderer *EditorUtils::entityMesh(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DRender::QGeometryRenderer *mesh
                = qobject_cast<Qt3DRender::QGeometryRenderer *>(components.value(i));
        if (mesh)
            return mesh;
    }

    return nullptr;
}

Qt3DRender::QCameraLens *EditorUtils::entityCameraLens(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    for (int i = 0; i < components.size(); i++) {
        Qt3DRender::QCameraLens *lens
                = qobject_cast<Qt3DRender::QCameraLens *>(components.value(i));
        if (lens)
            return lens;
    }

    return nullptr;
}

bool EditorUtils::isGroupEntity(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QComponentVector components = entity->components();
    return (components.size() == 0 || (components.size() == 1 && entityTransform(entity)));
}

// Returns the intersection point of a plane and a ray.
// Parameter t returns the distance in ray lengths. If t is negative, intersection
// is behind rayOrigin.
// If there is no intersection, i.e. plane and the ray are paraller, t is set to -1 and
// rayOrigin is returned.
QVector3D EditorUtils::findIntersection(const QVector3D &rayOrigin, const QVector3D &ray,
                                        float planeOffset, const QVector3D &planeNormal,
                                        float &t)
{
    float divisor = QVector3D::dotProduct(ray, planeNormal);
    if (qFuzzyCompare(1.0f, 1.0f + divisor)) {
        t = -1.0f;
        return rayOrigin;
    }

    t = -(QVector3D::dotProduct(rayOrigin, planeNormal) - planeOffset) / divisor;

    return rayOrigin + ray * t;

}

// Returns a direction vector from camera origin to viewport pixel in world coordinates
QVector3D EditorUtils::unprojectRay(const QMatrix4x4 &viewMatrix,
                                    const QMatrix4x4 &projectionMatrix,
                                    int viewPortWidth, int viewPortHeight,
                                    const QPoint &pos)
{
    float x = ((2.0f * pos.x()) / viewPortWidth) - 1.0f;
    float y = 1.0f - ((2.0f * pos.y()) / viewPortHeight);

    // Figure out the ray to the screen position
    QVector4D ray = projectionMatrix.inverted() * QVector4D(x, y, -1.0f, 1.0f);
    ray.setZ(-1.0f);
    ray.setW(0.0f);
    ray = viewMatrix.inverted() * ray;
    return ray.toVector3D().normalized();
}

// Returns a viewport position for a ray from camera origin to world position
// If returned z-value is negative, the position is behind camera
QVector3D EditorUtils::projectRay(const QMatrix4x4 &viewMatrix,
                                  const QMatrix4x4 &projectionMatrix,
                                  int viewPortWidth, int viewPortHeight,
                                  const QVector3D &worldPos)
{
    QVector3D localPos = projectionMatrix * viewMatrix * worldPos;
    localPos *= QVector3D(0.5f, -0.5f, -0.5f);
    localPos += QVector3D(0.5f, 0.5f, 0.5f);
    return QVector3D(viewPortWidth * localPos.x(), viewPortHeight * localPos.y(), localPos.z());
}

QVector3D EditorUtils::absVector3D(const QVector3D &vector)
{
    return QVector3D(qAbs(vector.x()),
                     qAbs(vector.y()),
                     qAbs(vector.z()));
}

QVector3D EditorUtils::maxVector3D(const QVector3D &vector, float minValue)
{
    return QVector3D(qMax(minValue, vector.x()),
                     qMax(minValue, vector.y()),
                     qMax(minValue, vector.z()));
}

EditorUtils::ComponentTypes EditorUtils::componentType(Qt3DCore::QComponent *component)
{
    ComponentTypes componentType = Unknown;

    if (qobject_cast<Qt3DRender::QAbstractLight *>(component)) {
        if (qobject_cast<Qt3DRender::QDirectionalLight *>(component))
            componentType = LightDirectional;
        else if (qobject_cast<Qt3DRender::QPointLight *>(component))
            componentType = LightPoint;
        else if (qobject_cast<Qt3DRender::QSpotLight *>(component))
            componentType = LightSpot;
    } else if (qobject_cast<Qt3DRender::QMaterial *>(component)) {
        if (qobject_cast<Qt3DExtras::QDiffuseMapMaterial *>(component))
            componentType = MaterialDiffuseMap;
        else if (qobject_cast<Qt3DExtras::QDiffuseSpecularMapMaterial *>(component))
            componentType = MaterialDiffuseSpecularMap;
        else if (qobject_cast<Qt3DExtras::QGoochMaterial *>(component))
            componentType = MaterialGooch;
        // Inherits QNormalDiffuseMapMaterial, so must be tested first
        else if (qobject_cast<Qt3DExtras::QNormalDiffuseMapAlphaMaterial *>(component))
            componentType = MaterialNormalDiffuseMapAlpha;
        else if (qobject_cast<Qt3DExtras::QNormalDiffuseMapMaterial *>(component))
            componentType = MaterialNormalDiffuseMap;
        else if (qobject_cast<Qt3DExtras::QNormalDiffuseSpecularMapMaterial *>(component))
            componentType = MaterialNormalDiffuseSpecularMap;
        else if (qobject_cast<Qt3DExtras::QPerVertexColorMaterial *>(component))
            componentType = MaterialPerVertexColor;
        else if (qobject_cast<Qt3DExtras::QPhongAlphaMaterial *>(component))
            componentType = MaterialPhongAlpha;
        else if (qobject_cast<Qt3DExtras::QPhongMaterial *>(component))
            componentType = MaterialPhong;
        else
            componentType = MaterialGeneric;
    } else if (qobject_cast<Qt3DRender::QGeometryRenderer *>(component)) {
        if (qobject_cast<Qt3DRender::QMesh *>(component))
            componentType = MeshCustom;
        else if (qobject_cast<Qt3DExtras::QCuboidMesh *>(component))
            componentType = MeshCuboid;
        else if (qobject_cast<Qt3DExtras::QCylinderMesh *>(component))
            componentType = MeshCylinder;
        else if (qobject_cast<Qt3DExtras::QPlaneMesh *>(component))
            componentType = MeshPlane;
        else if (qobject_cast<Qt3DExtras::QSphereMesh *>(component))
            componentType = MeshSphere;
        else if (qobject_cast<Qt3DExtras::QTorusMesh *>(component))
            componentType = MeshTorus;
        else
            componentType = MeshGeneric;
    } else if (qobject_cast<Qt3DCore::QTransform *>(component)) {
        componentType = Transform;
    } else if (qobject_cast<QDummyObjectPicker *>(component)) {
        componentType = ObjectPicker;
    } else if (qobject_cast<Qt3DRender::QSceneLoader *>(component)) {
        componentType = SceneLoader;
    }

    return componentType;
}

Qt3DRender::QAttribute *EditorUtils::copyAttribute(
        Qt3DRender::QAttribute *oldAtt,
        QMap<Qt3DRender::QBuffer *, Qt3DRender::QBuffer *> &bufferMap)
{
    Qt3DRender::QAttribute *newAtt = nullptr;
    if (oldAtt) {
        newAtt = new Qt3DRender::QAttribute;

        newAtt->setName(oldAtt->name());
        newAtt->setDataType(oldAtt->vertexBaseType());
        newAtt->setDataSize(oldAtt->vertexSize());
        newAtt->setCount(oldAtt->count());
        newAtt->setByteStride(oldAtt->byteStride());
        newAtt->setByteOffset(oldAtt->byteOffset());
        newAtt->setDivisor(oldAtt->divisor());
        newAtt->setAttributeType(oldAtt->attributeType());

        Qt3DRender::QBuffer *oldBuf = oldAtt->buffer();
        if (oldBuf) {
            Qt3DRender::QBuffer *newBuf = bufferMap.value(oldBuf);
            if (!newBuf) {
                newBuf = new Qt3DRender::QBuffer;
                bufferMap.insert(oldBuf, newBuf);

                if (oldBuf->data().isEmpty())
                    newBuf->setData(oldBuf->dataGenerator()->operator()());
                else
                    newBuf->setData(oldBuf->data());
                newBuf->setType(oldBuf->type());
                newBuf->setUsage(oldBuf->usage());
                newBuf->setSyncData(oldBuf->isSyncData());
            }

            newAtt->setBuffer(newBuf);
        }
    }

    return newAtt;
}

// Rotates vector around rotationAxis. The rotationAxis must be normalized.
QVector3D EditorUtils::rotateVector(const QVector3D &vector,
                                    const QVector3D &rotationAxis,
                                    qreal radians)
{
    const qreal cosAngle = qCos(radians);

    // Use Rodrigues' rotation formula to find rotated vector
    return (vector * cosAngle
            + (QVector3D::crossProduct(rotationAxis, vector) * qSin(radians))
            + rotationAxis * QVector3D::dotProduct(rotationAxis, vector) * (1.0 - cosAngle));
}

QVector3D EditorUtils::projectVectorOnPlane(const QVector3D &vector, const QVector3D &planeNormal)
{
    float distance = vector.distanceToPlane(QVector3D(), planeNormal);
    return vector - distance * planeNormal;
}

QMatrix4x4 EditorUtils::totalAncestralTransform(Qt3DCore::QEntity *entity)
{
    QMatrix4x4 totalTransform;
    QList<Qt3DCore::QTransform *> transforms = ancestralTransforms(entity);

    for (int i = transforms.size() - 1; i >= 0; i--)
        totalTransform *= transforms.at(i)->matrix();

    return totalTransform;
}

QVector3D EditorUtils::totalAncestralScale(Qt3DCore::QEntity *entity)
{
    QVector3D totalScale(1.0f, 1.0f, 1.0f);
    QList<Qt3DCore::QTransform *> transforms = ancestralTransforms(entity);

    for (int i = transforms.size() - 1; i >= 0; i--)
        totalScale *= transforms.at(i)->scale3D();

    return totalScale;
}

QQuaternion EditorUtils::totalAncestralRotation(Qt3DCore::QEntity *entity)
{
    QQuaternion totalRotation;
    QList<Qt3DCore::QTransform *> transforms = ancestralTransforms(entity);

    for (int i = transforms.size() - 1; i >= 0; i--)
        totalRotation *= transforms.at(i)->rotation();

    return totalRotation;
}

QList<Qt3DCore::QTransform *> EditorUtils::ancestralTransforms(Qt3DCore::QEntity *entity,
                                                               Qt3DCore::QEntity *topAncestor)
{
    Qt3DCore::QEntity *parent = entity->parentEntity();
    QList<Qt3DCore::QTransform *> transforms;
    while (parent && parent != topAncestor) {
        Qt3DCore::QTransform *transform = entityTransform(parent);
        if (transform)
            transforms.append(transform);
        parent = parent->parentEntity();
    }
    return transforms;
}

QVector3D EditorUtils::lightDirection(const Qt3DRender::QAbstractLight *light)
{
    QVector3D direction;
    const Qt3DRender::QDirectionalLight *dirLight =
            qobject_cast<const Qt3DRender::QDirectionalLight *>(light);
    const Qt3DRender::QSpotLight *spotLight =
            qobject_cast<const Qt3DRender::QSpotLight *>(light);
    if (dirLight)
        direction = dirLight->worldDirection();
    else if (spotLight)
        direction = spotLight->localDirection();
    return direction;
}

void EditorUtils::copyLockProperties(const QObject *source, QObject *target)
{
    QList<QByteArray> customProps = source->dynamicPropertyNames();
    Q_FOREACH (const QByteArray &propName, customProps) {
        if (propName.endsWith(lockPropertySuffix8())) {
            target->setProperty(propName.constData(),
                                source->property(propName.constData()));
        }
    }
}

void EditorUtils::lockProperty(const QByteArray &lockPropertyName, QObject *obj, bool lock)
{
    QVariant propVal = obj->property(lockPropertyName);
    if (propVal.isValid() && propVal.toBool() != lock)
        obj->setProperty(lockPropertyName, QVariant::fromValue(lock));
}

QVector3D EditorUtils::cameraNormal(Qt3DRender::QCamera *camera)
{
    QVector3D planeNormal;
    if (camera) {
        planeNormal = camera->position() - camera->viewCenter();
        planeNormal.normalize();
    }
    return planeNormal;
}


EditorUtils::InsertableEntities EditorUtils::insertableEntityType(Qt3DCore::QEntity *entity)
{
    InsertableEntities insertableType = InvalidEntity;

    Qt3DRender::QAbstractLight *light = entityLight(entity);
    Qt3DRender::QGeometryRenderer *mesh = entityMesh(entity);

    if (light) {
        insertableType = LightEntity;
    } else if (mesh) {
        if (qobject_cast<Qt3DRender::QMesh *>(mesh))
            insertableType = CustomEntity;
        else if (qobject_cast<Qt3DExtras::QCuboidMesh *>(mesh))
            insertableType = CuboidEntity;
        else if (qobject_cast<Qt3DExtras::QCylinderMesh *>(mesh))
            insertableType = CylinderEntity;
        else if (qobject_cast<Qt3DExtras::QPlaneMesh *>(mesh))
            insertableType = PlaneEntity;
        else if (qobject_cast<Qt3DExtras::QSphereMesh *>(mesh))
            insertableType = SphereEntity;
        else if (qobject_cast<Qt3DExtras::QTorusMesh *>(mesh))
            insertableType = TorusEntity;
    } else if (qobject_cast<Qt3DRender::QCamera *>(entity)) {
        insertableType = CameraEntity;
    } else if (entity->children().count() == 1
               && qobject_cast<Qt3DCore::QTransform *>(entity->children().at(0))) {
        insertableType = GroupEntity;
    }

    return insertableType;
}

void EditorUtils::setEnabledToSubtree(Qt3DCore::QEntity *entity, bool enable)
{
    entity->setEnabled(enable);
    Q_FOREACH (QObject *child, entity->children()) {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity)
            setEnabledToSubtree(childEntity, enable);
    }
}

Qt3DCore::QEntity *EditorUtils::findEntityByName(Qt3DCore::QEntity *entity, const QString &name)
{
    if (entity->objectName() == name) {
        return entity;
    } else {
        for (QObject *child : entity->children()) {
            Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
            if (childEntity) {
                Qt3DCore::QEntity *foundEntity = findEntityByName(childEntity, name);
                if (foundEntity)
                    return foundEntity;
            }
        }
    }
    return nullptr;
}

template <typename T>
void EditorUtils::copyRenderParameters(T *source, T *target)
{
    Q_FOREACH (Qt3DRender::QParameter *param, source->parameters()) {
        Qt3DRender::QParameter *newParam = new Qt3DRender::QParameter;
        newParam->setName(param->name());

        // Textures need special handling
        Qt3DRender::QAbstractTexture *texture =
                param->value().value<Qt3DRender::QAbstractTexture *>();
        if (texture) {
            Qt3DRender::QTexture2D *texture2D = qobject_cast<Qt3DRender::QTexture2D *>(texture);
            if (texture2D) {
                // TODO: Only support texture2D for now (as that's what qgltf supports), todo rest
                Qt3DRender::QTexture2D *newTexture = new Qt3DRender::QTexture2D;

                Q_FOREACH (Qt3DRender::QAbstractTextureImage *ti, texture->textureImages()) {
                    Qt3DRender::QTextureImage *sourceImage =
                            qobject_cast<Qt3DRender::QTextureImage *>(ti);
                    if (sourceImage) {
                        Qt3DRender::QTextureImage *newImage = new Qt3DRender::QTextureImage;
                        newImage->setMipLevel(sourceImage->mipLevel());
                        newImage->setLayer(sourceImage->layer());
                        newImage->setFace(sourceImage->face());
                        newImage->setSource(sourceImage->source());
                        newTexture->addTextureImage(newImage);
                    }
                }

                newTexture->setFormat(texture->format());
                newTexture->setGenerateMipMaps(texture->generateMipMaps());
                newTexture->setWidth(texture->width());
                newTexture->setHeight(texture->height());
                newTexture->setDepth(texture->depth());
                newTexture->setMagnificationFilter(texture->magnificationFilter());
                newTexture->setMinificationFilter(texture->minificationFilter());
                newTexture->setMaximumAnisotropy(texture->maximumAnisotropy());
                newTexture->setComparisonFunction(texture->comparisonFunction());
                newTexture->setComparisonMode(texture->comparisonMode());
                newTexture->setLayers(texture->layers());
                newTexture->wrapMode()->setX(texture->wrapMode()->x());
                newTexture->wrapMode()->setY(texture->wrapMode()->y());
                newTexture->wrapMode()->setZ(texture->wrapMode()->z());

                newParam->setValue(QVariant::fromValue(newTexture));
            }
        } else {
            newParam->setValue(param->value());
        }

        target->addParameter(newParam);
    }
}

template <typename T>
void EditorUtils::copyFilterKeys(T *source, T *target) {
    Q_FOREACH (Qt3DRender::QFilterKey *key, source->filterKeys()) {
        Qt3DRender::QFilterKey *newKey = new Qt3DRender::QFilterKey;
        newKey->setName(key->name());
        newKey->setValue(key->value());
        target->addFilterKey(newKey);
    }
}
