#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QMatrix4x4>
#include <qobject.h>



namespace Qt3DCore {
    class QEntity;
    class QTransform;
}

namespace Qt3DRender {
    class QGeometryRenderer;
    class QObjectPicker;
}


//class EditorSceneItemComponentsModel;

class SceneItem : public QObject
{
    Q_OBJECT
    Q_ENUMS(ItemType)

//    Q_PROPERTY(EditorSceneItemComponentsModel* componentsModel READ componentsModel CONSTANT)
    Q_PROPERTY(bool showSelectionBox READ isSelectionBoxShowing WRITE setShowSelectionBox NOTIFY showSelectionBoxChanged)

public:

    enum ItemType {
        Camera = 1,
        Light,
        Mesh,
        SceneLoader,
        Group,
        Other
    };

    SceneItem(Qt3DCore::QEntity *entity,
              SceneItem *parentItem,
              int index, QObject *parent);
    ~SceneItem();

    Q_INVOKABLE Qt3DCore::QEntity *entity();

    const QList<SceneItem *> &childItems();

    SceneItem *parentItem();


     Q_INVOKABLE ItemType itemType() { return m_itemType; }

    int childNumber() const;

    void addChild(SceneItem *child, int index = -1);
    void removeChild(SceneItem *child);

    void setParentItem(SceneItem *parentItem);



    void setShowSelectionBox(bool enabled);
    bool isSelectionBoxShowing() const;





    Q_INVOKABLE bool setCustomProperty(QObject *component, const QString name,
                                       const QVariant &value);
    Q_INVOKABLE QVariant customProperty(QObject *component, const QString name) const;
    Qt3DCore::QTransform *selectionTransform() const { return m_selectionTransform; }
    const QMatrix4x4 &unadjustedSelectionBoxMatrix() const { return m_unadjustedSelectionBoxMatrix; }
    Qt3DCore::QTransform *entityTransform() const { return m_entityTransform; }
    QVector3D unadjustedSelectionBoxExtents() const { return m_unadjustedSelectionBoxExtents; }
    QVector3D entityMeshExtents() const { return m_entityMeshExtents; }
    Q_INVOKABLE QVector3D selectionBoxCenter() const { return m_selectionBoxCenter; }
    QVector3D entityMeshCenter() const { return m_entityMeshCenter; }

    bool canRotate() const { return m_canRotate; }
    void setCanRotate(bool canRotate) { m_canRotate = canRotate; }

    QList<Qt3DRender::QObjectPicker *> *internalPickers();
    void recalculateSubMeshesExtents();
    void doUpdateSelectionBoxTransform();
    void updateGroupExtents();

public slots:
    void updateSelectionBoxTransform();
    void handleMeshChange(Qt3DRender::QGeometryRenderer *newMesh);
    void recalculateMeshExtents();

signals:
    void showSelectionBoxChanged(bool enabled);


private:

    void recalculateCustomMeshExtents(Qt3DRender::QGeometryRenderer *mesh,
                                      QVector3D &meshExtents,
                                      QVector3D &meshCenter);
    void populateSubMeshData(Qt3DCore::QEntity *entity, QVector<QVector3D> &subMeshPoints);

    void findTotalExtents(QVector3D &min, QVector3D &max, const QMatrix4x4 &matrix);


    Qt3DCore::QEntity *m_entity; // Not owned

    SceneItem *m_parentItem; // Not owned
    QList<SceneItem *> m_children;




    Qt3DCore::QEntity *m_selectionBox;  // Created, but not owned
    Qt3DCore::QTransform *m_selectionTransform;  // Created, but not owned
    QMatrix4x4 m_unadjustedSelectionBoxMatrix;
    Qt3DCore::QTransform *m_entityTransform; // Not owned
    Qt3DRender::QGeometryRenderer *m_entityMesh; // Not owned




    QVector3D m_entityMeshExtents;
    QVector3D m_entityMeshCenter;
    QVector3D m_unadjustedSelectionBoxExtents;
    QVector3D m_selectionBoxCenter;

    ItemType m_itemType;

    bool m_canRotate;
    // Internal pickers are for example pickers of hidden scene loader meshes.
    // Pickers not owned, but the list itself is.
    QList<Qt3DRender::QObjectPicker *> *m_internalPickers;
};

Q_DECLARE_METATYPE(SceneItem*)


#endif // SCENEITEM_H
