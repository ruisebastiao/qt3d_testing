#include "linemesh.h"
#include "linemeshgeometry.h"

#include <QVector3D>
#include <QVector4D>
#include <Qt3DRender/QGeometryRenderer>

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

void LineMesh::posUpdate(QVariantList poslistvariant)
{


    QList<QVector4D> pos;


    for (int var = 0; var < poslistvariant.length(); ++var) {
        QVector4D vec=poslistvariant.at(var).value<QVector4D>();
        pos.append(vec);
    }


    _vertices = pos;
    _lineMeshGeo = new LineMeshGeometry(_vertices, this);
    setVertexCount(_lineMeshGeo->vertexCount());
    setGeometry(_lineMeshGeo);
    emit finished();
}

LineMesh::~LineMesh()
{
}
