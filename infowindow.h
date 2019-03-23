#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include "infomessage.h"

#include <QQuickItem>

class LineMesh;
class EditorScene;
class SceneItem;
class EditorViewportItem;

namespace Qt3DCore{
class QEntity;
}

namespace Qt3DRender{
class QCamera;
}

class InfoWindow : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(InfoMessage* infoMessage READ infoMessage WRITE setInfoMessage NOTIFY infoMessageChanged)

    Q_PROPERTY(bool showConnector READ showConnector WRITE setShowConnector NOTIFY showConnectorChanged)

    Q_PROPERTY(EditorScene* editorScene READ editorScene WRITE setEditorScene NOTIFY editorSceneChanged)

    Q_PROPERTY(SceneItem* sceneItem READ sceneItem WRITE setSceneItem NOTIFY sceneItemChanged)

    Q_PROPERTY(bool isSelected READ isSelected WRITE setIsSelected NOTIFY isSelectedChanged)



public:
    InfoWindow(QQuickItem *parent=nullptr);


    bool showConnector() const;

    InfoMessage* infoMessage() const;

    EditorScene* editorScene() const;

    SceneItem* sceneItem() const;

    bool isSelected() const
    {
        return m_isSelected;
    }

private:


    LineMesh* m_linemesh=nullptr;
    EditorViewportItem* m_viewport=nullptr;
    Qt3DCore::QEntity *m_InfoConnectorEntity=m_InfoConnectorEntity=nullptr;

    Qt3DRender::QCamera *m_camera=nullptr;

    bool m_showConnector=false;

    InfoMessage* m_infoMessage=nullptr;

    EditorScene* m_editorScene=nullptr;


    SceneItem* m_sceneItem=nullptr;

    bool m_isSelected=false;

signals:


    void showConnectorChanged(bool showConnector);

    void infoMessageChanged(InfoMessage* infoMessage);

    void editorSceneChanged(EditorScene* editorScene);

    void sceneItemChanged(SceneItem* sceneItem);

    void isSelectedChanged(bool isSelected);

public slots:

    void setShowConnector(bool showConnector);
    void setInfoMessage(InfoMessage* infoMessage);
    void setEditorScene(EditorScene* editorScene);
    void setSceneItem(SceneItem* sceneItem);

    Q_INVOKABLE void updateLinePosition();

    void setIsSelected(bool isSelected);

private slots:

};

#endif // INFOWINDOW_H
