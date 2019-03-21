import QtQuick.Controls 2.5 as QQC2

import QtQuick.Scene3D 2.12
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12
import QtQuick.Scene2D 2.12
import QtQuick.Layouts 1.12

import QtQuick 2.12


import base 1.0


//import "scene_teste/qml"

QQC2.ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("")

    EditorUtils{
        id:utils
    }


    Timer{
        id:loader_ready_tmr
        interval: 500
        repeat: false
        onTriggered: {
            mainCamera.viewAll()
        }
    }

    EditorScene{
        id:scene
        viewport:viewport

        onSelectionChanged: {

        }



        onLoaderstatusChanged: {

            if(status==SceneLoader.Ready){
                //                mainCamera.viewAll()
                mainCamera.viewEntity(scene.sceneEntity)


                //                mainCamera.upVector=Qt.vector3d(0,0,-1)

                //                mainCamera.viewCenter=Qt.vector3d(0,0,0)


            }
        }

        onSelectionItemChanged: {
            if(selectionItem.entityTransform()){
                //                console.log("Selection Item:"+selectionItem.selectionTransform().translation)
                //                var pos=Qt.vector3d(selectionItem.selectionTransform().translation.x,selectionItem.selectionTransform().translation.y,selectionItem.selectionTransform().translation.z)
                //                pos.y=pos.y+selectionItem.entityMeshExtents().z
                //                console.log("Selection Item entityMeshExtents:"+selectionItem.entityMeshExtents())
                //                arrow_transform.translation=pos

                //                lineEntity.updateline(selectionItem,info_item)


                //                scene.helperPlaneTransform.setRotation(Qt.vector3d(0,0,90))

            }


        }

        Behavior on activeCamera.upVector {
            Vector3dAnimation{
                easing.type: Easing.InOutCubic
                duration: 1000
            }
        }


        //        activeCamera.onViewVectorChanged: console.log("viewVectorChanged:"+activeCamera.viewVector)
        activeCamera.onViewCenterChanged: {
            console.log("viewCenter:"+activeCamera.viewCenter)
            //            lineEntity.updateline(selectionItem,info_item)
        }


        activeCamera.onPositionChanged: {
            console.log("Position:"+activeCamera.position)
            //         lineEntity.updateline(selectionItem,info_item)
        }
        activeCamera.onUpVectorChanged: {
            console.log("Up vector:"+activeCamera.upVector)
            //            lineEntity.updateline(selectionItem,info_item)
        }



        Behavior on activeCamera.viewCenter {
            Vector3dAnimation{
                easing.type: Easing.InOutCubic
                duration: 1000
            }
        }

        Behavior on activeCamera.position {
            Vector3dAnimation{
                easing.type: Easing.InOutCubic
                duration: 1000
            }
        }


        sceneEntity:  Entity {
            id:scene_entity
            objectName: "scene entity"
            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, -30.0, 0.0).normalized();
                        color: "white"
                        intensity: 1
                    }
                ]
            }
            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, 30, 0.0).normalized();
                        color: "white"
                        intensity:1
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(-30, 0, 0.0).normalized();
                        color: "white"
                        intensity: 1
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(30, 0, 0.0).normalized();
                        color: "white"
                        intensity: 1
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, 0, -30).normalized();
                        color: "white"
                        intensity: 1
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, 0, 30).normalized();
                        color: "white"
                        intensity: 1
                    }
                ]
            }




        }
    }


    ColumnLayout{
        anchors.fill: parent
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 80

            Flow{



                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Load"


                    onClicked: {

                        //                scene.loadFile("file:///home/rui/projects/cad/teste.obj")
                        scene.loadFile("file:///home/rui/projects/cad/teste.dae")
                        //                scene.loadFile("file:///home/rui/projects/cad/bodyplacement_lm36.obj")
                        //                        scene.loadFile("file:///home/rui/projects/cad/bodyplacement_lm36.dae")
                    }
                }


                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Wireframe selected"



                    onClicked: {


                        scene.allTransparent(scene.selectionItem)





                    }
                }

                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Fit selected"


                    onClicked: {


                        mainCamera.viewCenter=scene.selectionItem.selectionBoxCenter()
                        var oL = Qt.vector3d(0,0,0)
                        var cL = Qt.vector3d(0,0,0)

                        //var FOV = 45 * (Math.PI / 180); // convert to radians

                        var objectLocation = oL = scene.selectionItem.selectionBoxCenter();
                        var cameraLocation = cL = mainCamera.position


                        //   var distToObject = Math.sqrt(Math.pow(oL.x - cL.x, 2) + Math.pow(oL.y - cL.y, 2) + Math.pow(oL.z - cL.z, 2));

                        var entity_extents=scene.selectionItem.entityMeshExtents()

                        var max=Math.max(entity_extents.x,entity_extents.y)
                        var objectRadius = Math.max(max,entity_extents.z)


                        // Approx size in pixels you want the object to occupy
                        var requieredObjectPixelSize = 900;

                        // camera distance to object
                        var distToObject = Math.sqrt(Math.pow(oL.x - cL.x, 2) + Math.pow(oL.y - cL.y, 2) + Math.pow(oL.z - cL.z, 2));

                        // get the object's angular size.
                        var objectAngularSize = Math.atan( (objectRadius) / distToObject ) * 2;

                        // get the fraction of the FOV the object must occupy to be 900 pixels
                        // var scaling = requieredObjectPixelSize / displayWidth;

                        // get the angular size the object has to be
                        // var objectAngularSize2 = FOV * scaling;

                        // use half the angular size to get the distance the camera must be from the object
                        distToObject = (objectRadius/2) / Math.tan(objectAngularSize / 2)


                        // Get the vector from the object to the camera
                        //                var toCam = {
                        //                    x : cL.x - oL.x,
                        //                    y : cL.y - oL.y,
                        //                    z : cL.z - oL.z,
                        //                }

                        // Get the vector from the object to the camera
                        var toCam = Qt.vector3d(0,0,0);
                        toCam=cL.minus(oL)

                        // First length
                        var len =toCam.length();
                        // Then divide to normalise (you may want to test for divide by zero)
                        toCam=toCam.normalized()


                        //Now you can scale the vector to make it equal to the distance the camera must be from the object.
                        toCam=toCam.times(distToObject-2)

                        cL=cL.minus(toCam)

                        mainCamera.position=cL



                        //                         var centerpoint = scene.selectionItem.selectionBoxCenter()
                        //                         var entity_extents=scene.selectionItem.entityMeshExtents()
                        //                         var backup = (entity_extents.y / 2) / Math.sin( (mainCamera.fieldOfView/2)*(Math.PI/180) );
                        //                         var camZpos = entity_extents.z + backup + mainCamera.nearPlane;

                        //                         //move cam
                        //                         mainCamera.position=Qt.vector3d(centerpoint.x, centerpoint.y, camZpos);

                        //                         mainCamera.farPlane= mainCamera.nearPlane+ 10*entity_extents.z;



                    }
                }

                QQC2.Button{
                    width: 100
                    height: 50
                    text: "View all"


                    onClicked: {


                        mainCamera.viewEntity(scene.sceneEntity)


                    }
                }
                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Top"



                    onClicked: {

                        var cameraDirection=Qt.vector3d(0,0,0);
                        var up=Qt.vector3d(0,0,-1);

                        cameraDirection.y=1

                        // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
                        var len = mainCamera.viewVector.length();
                        mainCamera.position=mainCamera.viewCenter.plus(cameraDirection.times(len));
                        mainCamera.upVector=up



                    }
                }

                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Left"



                    onClicked: {

                        var cameraDirection=Qt.vector3d(0,0,0);
                        var up=Qt.vector3d(0,1,0);

                        cameraDirection.x=1

                        // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
                        var len = mainCamera.viewVector.length();
                        mainCamera.position=mainCamera.viewCenter.plus(cameraDirection.times(len));
                        mainCamera.upVector=up



                    }
                }



                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Back"



                    onClicked: {

                        var cameraDirection=Qt.vector3d(0,0,0);
                        var up=Qt.vector3d(0,1,0);

                        cameraDirection.z=-1

                        // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
                        var len = mainCamera.viewVector.length();
                        mainCamera.position=mainCamera.viewCenter.plus(cameraDirection.times(len));
                        mainCamera.upVector=up



                    }
                }




                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Right"



                    onClicked: {

                        var cameraDirection=Qt.vector3d(0,0,0);
                        var up=Qt.vector3d(0,1,0);

                        cameraDirection.x=-1

                        // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
                        var len = mainCamera.viewVector.length();
                        mainCamera.position=mainCamera.viewCenter.plus(cameraDirection.times(len));
                        mainCamera.upVector=up



                    }
                }
                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Front"



                    onClicked: {

                        var cameraDirection=Qt.vector3d(0,0,0);
                        var up=Qt.vector3d(0,1,0);

                        cameraDirection.z=1

                        // Keep the current distance and viewcenter, but change upvector to properly orient the camera.
                        var len = mainCamera.viewVector.length();
                        mainCamera.position=mainCamera.viewCenter.plus(cameraDirection.times(len));
                        mainCamera.upVector=up



                    }
                }

                QQC2.Button{
                    width: 100
                    height: 50
                    text: "Rotate selected"


                    property double angle: 0

                    Behavior on angle {
                        NumberAnimation{
                            easing.type: Easing.InOutCubic
                            duration: 1000
                        }
                    }

                    onAngleChanged: {

                        ////                selectedTransform.rotation=selectedTransform.fromAxisAndAngle(Qt.vector3d(0, 1, 0), angle)
                        //                //take the existing matrix to not lose any previous transformations
                        //                   var m_target=scene.selectionItem.entityTransform()
                        //                   var m_matrix =Qt.matrix4x4()
                        //                    m_matrix = m_target.matrix;
                        //                    var _rotationOrigin=m_target.translation
                        //                   // Move to the origin point of the rotation, _rotationOrigin would be a member variable
                        //                   m_matrix.translate(_rotationOrigin);
                        //                   // rotate (around the y axis)
                        //                   m_matrix.rotate(angle, Qt.vector3d(0, 1, 0));
                        //                   // translate back
                        //                   m_matrix.translate(-_rotationOrigin);
                        //                 scene.selectionItem.entityTransform().matrix=m_matrix;


                        selectedTransform.rotation=selectedTransform.fromAxisAndAngle(Qt.vector3d(0, 0, 1), angle)
                    }

                    property Transform selectedTransform

                    onSelectedTransformChanged: {



                    }


                    onClicked: {


                        selectedTransform=scene.selectionItem.entityTransform()


                        if(angle!=0){
                            angle=0
                        }
                        else{
                            angle=360
                        }



                    }
                }





            }

        }


        Component{

            InfoWindow {
                id: info_item


                width: parent.width
                height: 50

                property int infoIndex: index


                //                entityName: modelData



                showConnector: info_container.selectedItemIndex==index
                MouseArea{
                    anchors.fill: parent
                    onPressed: {
                        info_container.selectedItemIndex=index
                    }
                }

                Rectangle{
                    id:info_rect
                    radius: 10

                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    width: info_container.selectedItemIndex==index?parent.width+20:parent.width
                    Behavior on width {
                        PropertyAnimation{
                            easing.type: Easing.InOutBack
                            duration: 500
                        }
                    }

                    height: parent.height

                    color: "red"
                    border.color: "black";

                    opacity: 0.5

                }
                Text {
                    anchors.centerIn: info_rect
                    color: "white"
                    font.pixelSize: 12
                    text: qsTr("text")
                }
                z:9999
            }


        }





        //        Timer{
        //            id:add_timer

        //            property int count: 0
        //            interval: 5000
        //            repeat: false
        //            onTriggered: {

        ////                scene.addInfoWindow(repeater.itemAt(count))

        //                add_timer.count++

        //                if(add_timer.count<3) add_timer.start()

        //            }
        //        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            EditorViewport{
                id:viewport
                anchors.fill: parent
                scene: scene

                Flickable{
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 100

                    onDragStarted: {
                        console.log("drag start")
                    }
                    onDragEnded: {
                        console.log("drag ended")
                    }

                    interactive: true
                    contentHeight: flow.height
                    QQC2.ScrollBar.vertical: QQC2.ScrollBar { }

                    Flow{
                        id:flow
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.left: parent.left
                        spacing: 5

                        Component.onCompleted: {
                            scene.addNewMessage("torus","torus message")


                        }

                        Repeater{
                            id:info_container
                            anchors.fill: parent
                            property int selectedItemIndex

                            model:scene.infoMessages

                            InfoWindow {
                                id: info_item


                                width: parent.width
                                height: 50

                                property int infoIndex: index


                                infoMessage: infoMessageItem
                                onInfoMessageChanged: {
                                    infoMessage.infoWindow=this
                                }



                                showConnector: info_container.selectedItemIndex==index
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        info_container.selectedItemIndex=index
                                    }
                                }

                                Rectangle{
                                    id:info_rect
                                    radius: 10

                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter

                                    width: info_container.selectedItemIndex==index?parent.width+20:parent.width
                                    Behavior on width {
                                        PropertyAnimation{
                                            easing.type: Easing.InOutBack
                                            duration: 500
                                        }
                                    }

                                    height: parent.height

                                    color: "red"
                                    border.color: "black";

                                    opacity: 0.5

                                }
                                Text {
                                    anchors.centerIn: info_rect
                                    color: "white"
                                    font.pixelSize: 12
                                    text: infoMessageItem.entityName
                                }
                                z:9999
                            }


                        }
                    }

                }


                //                ListView{
                //                    id:info_container
                //                    anchors.right: parent.right
                //                    anchors.top: parent.top
                //                    anchors.bottom: parent.bottom
                //                    width: 100

                //                    Component.onCompleted: {

                ////                        info_container.positionViewAtIndex()
                //                        scene.addNewMessage("torus","torus message")
                //                        //                        add_timer.start()
                //                    }

                //                    delegate: QQC2.ItemDelegate {
                //                        width: parent.width



                //                        InfoWindow {
                //                            id: info_item


                //                            width: parent.width
                //                            height: 50

                //                            property int infoIndex: index


                //                            infoMessage: infoMessageItem
                //                            onInfoMessageChanged: {
                //                                infoMessage.infoWindow=this
                //                            }



                //                            showConnector: info_container.selectedItemIndex==index
                //                            MouseArea{
                //                                anchors.fill: parent
                //                                onPressed: {
                //                                    info_container.selectedItemIndex=index
                //                                }
                //                            }

                //                            Rectangle{
                //                                id:info_rect
                //                                radius: 10

                //                                anchors.right: parent.right
                //                                anchors.verticalCenter: parent.verticalCenter

                //                                width: info_container.selectedItemIndex==index?parent.width+20:parent.width
                //                                Behavior on width {
                //                                    PropertyAnimation{
                //                                        easing.type: Easing.InOutBack
                //                                        duration: 500
                //                                    }
                //                                }

                //                                height: parent.height

                //                                color: "red"
                //                                border.color: "black";

                //                                opacity: 0.5

                //                            }
                //                            Text {
                //                                anchors.centerIn: info_rect
                //                                color: "white"
                //                                font.pixelSize: 12
                //                                text: infoMessageItem.entityName
                //                            }
                //                            z:9999
                //                        }


                //                    }
                //                    property int selectedItemIndex

                //                    model: scene.infoMessages
                //                    z:9999

                //                }

            }
        }

    }



    property alias mainCamera: scene.activeCamera

}
