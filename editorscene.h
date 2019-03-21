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
#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include "editorutils.h"
#include "infomessagelistmodel.h"
#include "qquickitem.h"

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QUrl>
#include <QtCore/QStringListModel>
#include <QtCore/QTranslator>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <Qt3DCore/QNodeId>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <QtQml/QJSValue>
#include <Qt3DRender/QSceneLoader>

class InfoWindow;

namespace Qt3DCore {
    class QTransform;
    class QComponent;
}

namespace Qt3DRender {
    class QPickEvent;
    class QObjectPicker;
    class QRenderSettings;
    class QMaterial;
    class QGeometryRenderer;
    class QCamera;
    class QCameraLens;
    class QSceneExporter;
    class QSceneLoader;
}

namespace Qt3DExtras {
    class QPhongAlphaMaterial;
    class QForwardRenderer;
}

class MySceneLoader;
class SceneItem;
class EditorViewportItem;

class QMouseEvent;


class EditorScene : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Qt3DCore::QEntity *selection READ selection WRITE setSelection NOTIFY selectionChanged)
    Q_PROPERTY(SceneItem *selectionItem READ selectionItem NOTIFY selectionItemChanged)

    Q_PROPERTY(Qt3DCore::QEntity *sceneEntity READ sceneEntity WRITE setSceneEntity NOTIFY sceneEntityChanged)

    Q_PROPERTY(QStringList multiSelectionList READ multiSelectionList NOTIFY multiSelectionListChanged)
    Q_PROPERTY(bool multiSelection READ multiSelection NOTIFY multiSelectionChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

    Q_PROPERTY(EditorViewportItem *viewport READ viewport WRITE setViewport NOTIFY viewportChanged)

    Q_PROPERTY(bool helperArrowsLocal READ helperArrowsLocal WRITE setHelperArrowsLocal NOTIFY helperArrowsLocalChanged)


    Q_PROPERTY(Qt3DCore::QEntity *helperPlane READ helperPlane CONSTANT)
    Q_PROPERTY(Qt3DCore::QTransform *helperPlaneTransform READ helperPlaneTransform CONSTANT)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString emptyString READ emptyString NOTIFY translationChanged)
    Q_PROPERTY(QString lockPropertySuffix READ lockPropertySuffix CONSTANT)
    Q_PROPERTY(QString lockTransformPropertyName READ lockTransformPropertyName CONSTANT)
    Q_PROPERTY(int gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged)

    Q_PROPERTY(Qt3DRender::QCamera* activeCamera READ activeCamera WRITE setActiveCamera NOTIFY activeCameraChanged)


    Q_PROPERTY(InfoMessageListModel* infoMessages READ infoMessages WRITE setInfoMessages NOTIFY infoMessagesChanged)


public:
    enum DragMode {
        DragNone = 0,
        DragTranslate,
        DragScale,
        DragRotate,
        DragDebug  // Can be used to debugging positions
    };
    Q_ENUM(DragMode)

    enum TranslateHandleIndex {
        TranslateHandleBoxCenter,
        TranslateHandleMeshCenter,
        TranslateHandleArrowX,
        TranslateHandleArrowY,
        TranslateHandleArrowZ
    };
    Q_ENUM(TranslateHandleIndex)



    enum CameraPosition {
        CameraPositionNone = 0,
        CameraPositionTop,
        CameraPositionBottom,
        CameraPositionLeft,
        CameraPositionRight,
        CameraPositionFront,
        CameraPositionBack
    };
    Q_ENUM(CameraPosition)

private:


public:
    explicit EditorScene(QObject *parent = 0);
    ~EditorScene();

    Qt3DCore::QEntity *rootEntity() const { return m_rootEntity; }
    SceneItem *rootItem() const { return m_rootItem; }
    SceneItem *sceneEntityItem() const { return m_sceneEntityItem; }
    SceneItem *entityItem(Qt3DCore::QEntity *entity) const;
    Qt3DExtras::QForwardRenderer *renderer() const { return m_renderer; }

    const QMap<Qt3DCore::QNodeId, SceneItem *> &items() const { return m_sceneItems; }

    void addEntity(Qt3DCore::QEntity *entity, int index = -1, Qt3DCore::QEntity *parent = nullptr);
    void removeEntity(Qt3DCore::QEntity *entity);

    Q_INVOKABLE void resetScene();

    Q_INVOKABLE void resetFreeViewCamera();

    Q_INVOKABLE QVector3D getWorldPosition(int xPos, int yPos);

    Q_INVOKABLE void allTransparent(SceneItem *skipItem = nullptr);

    Q_INVOKABLE void loadFile(QString filepath);
    

    Q_INVOKABLE QString sceneRootName() const { return m_sceneEntity->objectName(); }

    Q_INVOKABLE void toggleEntityMultiSelection(const QString &name);
    Q_INVOKABLE void clearMultiSelection();
    Q_INVOKABLE QVector3D getMultiSelectionCenter();
    Q_INVOKABLE void updateWorldPositionLabel(int xPos, int yPos);

    Q_INVOKABLE void changeCameraPosition(CameraPosition preset);

//    Q_INVOKABLE void setWireframe(SceneItem* item);

    void removeEntityFromMultiSelection(const QString &name);
    void addEntityToMultiSelection(const QString &name);
    void renameEntityInMultiSelectionList(const QString &oldName, const QString &newName);

    Qt3DRender::QCamera *freeViewCamera() const { return m_freeViewCameraEntity; }

    void setSelection(Qt3DCore::QEntity *entity);
    Qt3DCore::QEntity *selection() const { return m_selectedEntity; }

    QStringList multiSelectionList() { return m_selectedEntityNameList; }

    bool multiSelection() const { return m_selectedEntityNameList.size() > 0; }

    const QString &error() const { return m_errorString; }






    void setHelperArrowsLocal(bool enable);
    bool helperArrowsLocal() const { return m_helperArrowsLocal; }

    int gridSize() const;
    void setGridSize(int size);

    void setLanguage(const QString &language);
    const QString language() const;

    const QString emptyString() const;
    const QString lockPropertySuffix() const { return EditorUtils::lockPropertySuffix(); }
    const QString lockTransformPropertyName() const {
        return EditorUtils::lockTransformPropertyName();
    }

    void setViewport(EditorViewportItem *viewport);
    EditorViewportItem *viewport() const { return m_viewport; }


    Qt3DCore::QEntity *helperPlane() const { return m_helperPlane; }
    Qt3DCore::QTransform *helperPlaneTransform() const { return m_helperPlaneTransform; }

    Qt3DRender::QMaterial *selectionBoxMaterial() const { return m_selectionBoxMaterial; }
    Qt3DRender::QGeometryRenderer *selectionBoxMesh() const { return m_selectionBoxMesh; }






    void handleEnabledChanged(Qt3DCore::QEntity *entity, bool enabled);
    void setError(const QString &errorString);
    Qt3DRender::QObjectPicker *createObjectPickerForEntity(Qt3DCore::QEntity *entity);


    void queueEnsureSelection();
    void queueUpdateGroupSelectionBoxes();



    Qt3DRender::QCamera* activeCamera() const
    {
        return m_activeCamera;
    }

    Qt3DCore::QEntity * sceneEntity() const
    {
        return m_sceneEntity;
    }

    SceneItem * selectionItem() const
    {
        return m_selectionItem;
    }

    Q_INVOKABLE void addNewMessage(QString entityName,QString message);

    InfoMessageListModel* infoMessages() const
    {
        return m_infoMessages;
    }

public slots:
    void clearSelectionBoxes(Qt3DCore::QEntity *skipEntity = nullptr);

    void setSceneEntity(Qt3DCore::QEntity *newSceneEntity = nullptr);

    void setSelectionItem(SceneItem * selectionItem);

    void setInfoMessages(InfoMessageListModel* infoMessages)
    {
        if (m_infoMessages == infoMessages)
            return;

        m_infoMessages = infoMessages;
        emit infoMessagesChanged(m_infoMessages);
    }

signals:
    void selectionChanged(Qt3DCore::QEntity *selection);
    void multiSelectionListChanged();
    void multiSelectionChanged(bool multiSelection);
    void errorChanged(const QString &error);
    void freeViewChanged(bool enabled);

    void viewportChanged(EditorViewportItem *viewport);

    void languageChanged(const QString &language);
    void translationChanged(const QString &translation);
    void gridSizeChanged(int gridSize);
    void mouseRightButtonReleasedWithoutDragging();





    void worldPositionLabelUpdate(const QString &wpX, const QString &wpY, const QString &wpZ);
    void helperArrowsLocalChanged(bool enable);

    void activeCameraChanged(Qt3DRender::QCamera* activeCamera);

    void sceneEntityChanged(Qt3DCore::QEntity * sceneEntity);

    void selectionItemChanged(SceneItem * selectionItem);

    void loaderstatusChanged(Qt3DRender::QSceneLoader::Status status);

    void infoMessagesChanged(InfoMessageListModel* infoMessages);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void handlePickerPress(Qt3DRender::QPickEvent *event);
    void loaderstatusChange(Qt3DRender::QSceneLoader::Status status);

    void handleViewportSizeChange();
    void handleEntityNameChange();
    void endSelectionHandling();


private:

    void setupDefaultScene();
    void createRootEntity();
    void createHelperPlane();
    void createHelperArrows();
    void setFrameGraphCamera(Qt3DCore::QEntity *cameraEntity);
    ///


//    QList<InfoWindow*> m_infoWindows;

    void retranslateUi();





    bool handleMousePress(QMouseEvent *event);
    bool handleMouseRelease(QMouseEvent *event);
    bool handleMouseMove(QMouseEvent *event);
    QVector3D helperPlaneNormal(Qt3DCore::QTransform *planeTransform) const;
    QVector3D projectVectorOnCameraPlane(const QVector3D &vector) const;
    void resizeConstantScreenSizeEntities();
    bool isPropertyLocked(const QString &propertyName, QObject *obj);


    void createSceneLoaderChildPickers(Qt3DCore::QEntity *entity,
                                       QList<Qt3DRender::QObjectPicker *> *pickers);
    void checkMultiSelectionHighlights();

    Q_INVOKABLE void doEnsureSelection();




    SceneItem *itemByName(const QString &name);
    void clearSingleSelection();
    Q_INVOKABLE void doUpdateGroupSelectionBoxes();
    void enableHelperArrows(bool enable);
    void updateWorldPositionLabel(const QVector3D &worldPos);
    Qt3DRender::QCamera *frameGraphCamera() const;
    void setActiveCamera(Qt3DRender::QCamera* activeCamera);

private:
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DCore::QEntity *m_componentCache;
    SceneItem *m_rootItem;


    QMap<Qt3DCore::QNodeId, SceneItem *> m_sceneItems;

    Qt3DRender::QRenderSettings *m_renderSettings;
    Qt3DExtras::QForwardRenderer *m_renderer;
    Qt3DCore::QEntity *m_sceneEntity;
    SceneItem *m_sceneEntityItem=nullptr;
    Qt3DCore::QEntity *m_selectedEntity;
    Qt3DCore::QTransform *m_selectedEntityTransform;
    QString m_ensureSelectionEntityName;
    bool m_cameraViewCenterSelected;

    QString m_errorString;


    bool m_freeView=true;
    Qt3DRender::QCamera *m_freeViewCameraEntity;

    EditorViewportItem *m_viewport; // Not owned

    Qt3DCore::QEntity *m_helperPlane;


    Qt3DCore::QTransform *m_helperPlaneTransform;

    Qt3DCore::QEntity *m_helperArrows;
    Qt3DCore::QTransform *m_helperArrowsTransform;
    bool m_helperArrowsLocal;
    QMap<Qt3DCore::QEntity *, int> m_helperArrowHandleIndexMap;
    QVector3D m_helperArrowGrabOffset;

    Qt3DRender::QMaterial *m_selectionBoxMaterial;
    Qt3DRender::QGeometryRenderer *m_selectionBoxMesh;
    Qt3DCore::QEntity *m_meshCenterIndicatorLine;
    Qt3DCore::QTransform *m_meshCenterIndicatorLineTransform;

    QTranslator *m_qtTranslator;
    QTranslator *m_appTranslator;

    QString m_language;

    QString m_sceneRootString;



    Qt3DRender::QSceneLoader* m_sceneloader= new Qt3DRender::QSceneLoader();

    SceneItem *m_baseEntityItem=nullptr;

    Qt3DCore::QEntity *m_baseEntity=nullptr;






    DragMode m_dragMode;
    QPoint m_previousMousePosition;







    bool m_viewCenterLocked;
    Qt3DCore::QEntity *m_pickedEntity;
    float m_pickedDistance;
    int m_gridSize;
    int m_duplicateCount;
    Qt3DCore::QEntity *m_previousDuplicate;
    bool m_ctrlDownOnLastLeftPress;
    QStringList m_selectedEntityNameList;
    Qt::MouseButton m_mouseButton;

    bool m_groupBoxUpdatePending;

    Qt3DRender::QCamera* m_activeCamera=nullptr;
    SceneItem * m_selectionItem;
    InfoMessageListModel* m_infoMessages=new InfoMessageListModel();
};
//Q_DECLARE_METATYPE(Qt3DCore::QTransform*);
#endif // EDITORSCENE_H
