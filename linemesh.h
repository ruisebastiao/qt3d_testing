/* Atelier KDE Printer Host for 3D Printing
    Copyright (C) <2017>
    Author: Patrick José Pereira - patrickelectric@gmail.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of
    the License or any later version accepted by the membership of
    KDE e.V. (or its successor approved by the membership of KDE
    e.V.), which shall act as a proxy defined in Section 14 of
    version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


#include <QList>
#include <QObject>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>
#include <QVector4D>
#include <qquickitem.h>

class LineMeshGeometry;
class QString;
class QVector4D;
class SceneItem;
class InfoWindow;

class LineMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT

//    Q_PROPERTY(InfoWindow* targetItem READ targetItem WRITE setTargetItem NOTIFY targetItemChanged)

//    Q_PROPERTY(SceneItem* sourceSceneItem READ sourceSceneItem WRITE setSourceSceneItem NOTIFY sourceSceneItemChanged)

public:
    explicit LineMesh(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~LineMesh();
    Q_INVOKABLE void readAndRun(const QString &path);
    void read(const QString &path);
   Q_INVOKABLE void posUpdate(QVariantList poslistvariant);
   void posUpdate(QList<QVector4D> poslistvariant);

//    Q_INVOKABLE void setFrom2Points();


//    InfoWindow* targetItem() const;

//    SceneItem* sourceSceneItem() const;

public slots:
//    void setTargetItem(InfoWindow *targetItem);

//    void setSourceSceneItem(SceneItem* sourceSceneItem);

    void updateLinePosition();
signals:
    void finished();
    void run(const QString &path);

//    void targetItemChanged(InfoWindow* targetItem);

//    void sourceSceneItemChanged(SceneItem* sourceSceneItem);

private:
    QList<QVector4D> _vertices;
    LineMeshGeometry *_lineMeshGeo;

//    InfoWindow* m_targetItem=nullptr;
//    SceneItem* m_sourceSceneItem =nullptr;
};
