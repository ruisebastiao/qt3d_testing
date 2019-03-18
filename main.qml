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
    title: qsTr("Hello World")

    EditorUtils{
        id:utils
    }



    EditorScene{
        id:scene
        viewport:viewport

        onSelectionChanged: {

        }


        onSelectionItemChanged: {
            if(selectionItem.entityTransform()){
//                console.log("Selection Item:"+selectionItem.selectionTransform().translation)
//                var pos=Qt.vector3d(selectionItem.selectionTransform().translation.x,selectionItem.selectionTransform().translation.y,selectionItem.selectionTransform().translation.z)
//                pos.y=pos.y+selectionItem.entityMeshExtents().z
//                console.log("Selection Item entityMeshExtents:"+selectionItem.entityMeshExtents())
//                arrow_transform.translation=pos

                lineEntity.updateline(selectionItem,info_item)


//                scene.helperPlaneTransform.setRotation(Qt.vector3d(0,0,90))

            }


        }

        Behavior on activeCamera.upVector {
            Vector3dAnimation{
                easing: Easing.InOutCubic
                duration: 1000
            }
        }

        activeCamera.onViewCenterChanged: {
            console.log("viewCenter:"+activeCamera.viewCenter)
            lineEntity.updateline(selectionItem,info_item)
        }


        activeCamera.onPositionChanged: {
            console.log("Position:"+activeCamera.position)
         lineEntity.updateline(selectionItem,info_item)
        }
        activeCamera.onUpVectorChanged: {
            console.log("Up vector:"+activeCamera.upVector)
//            lineEntity.updateline(selectionItem,info_item)
        }



        Behavior on activeCamera.viewCenter {
            Vector3dAnimation{
                easing: Easing.InOutCubic
                duration: 1000
            }
        }

        Behavior on activeCamera.position {
            Vector3dAnimation{
                easing: Easing.InOutCubic
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




            Entity{
                id:lineEntity

//                var positions=[]
//                positions.push(Qt.vector4d(selectionItem.selectionTransform().translation.x,selectionItem.selectionTransform().translation.y,selectionItem.selectionTransform().translation.z,0))
//                var xpos=info_item.x
//                var ypos=info_item.y

//                var  worldpos=scene.getWorldPosition(xpos+5,ypos+info_item.height-5)
////                console.log(selectionItem.selectionTransform().translation)

//                positions.push(Qt.vector4d(worldpos.x,worldpos.y,worldpos.z,0))

////                selectionItem.selectionTransform().translation=worldpos

//                //positions.push(worldpos)

//                linemesh.posUpdate(positions);

                function updateline(entityItem,target){
                    var positions=[]
                    positions.push(Qt.vector4d(entityItem.selectionTransform().translation.x,entityItem.selectionTransform().translation.y,entityItem.selectionTransform().translation.z,0))
                    var xpos=target.x
                    var ypos=target.y

                    var  worldpos=scene.getWorldPosition(xpos+5,ypos+target.height-5)


                    console.log("worldpos:"+worldpos)
//                    if(worldpos.x<0.0001){
//                        worldpos.x=0
//                    }

//                    if(worldpos.y<0.0001){
//                        worldpos.y=0
//                    }


//                    if(worldpos.z<0.0001){
//                        worldpos.z=0
//                    }



                    positions.push(Qt.vector4d(worldpos.x,worldpos.y,worldpos.z,0))



                    linemesh.posUpdate(positions);

                }

                PhongMaterial{
                    id:material
                    ambient: Qt.rgba( 1, 0, 0, 1.0 )
                    diffuse: Qt.rgba( 1, 0, 0, 1.0 )
                    specular: Qt.rgba(1, 0, 0, 1.0 )
                    shininess: 1.0
                }



                LineMesh{
                    id:linemesh
                }

                components:[linemesh,material]

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
                        //                scene.loadFile("file:///home/rui/projects/cad/bodyplacement_lm36.dae")
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

                        var objectRadius = entity_extents.x;


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
                        distToObject = objectRadius / Math.tan(objectAngularSize / 2);


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
                        toCam=toCam.times(distToObject-5)

                        cL=cL.minus(toCam)

                        mainCamera.position=cL


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
                    text: "Rotate selected"


                    property double angle: 0

                    Behavior on angle {
                        NumberAnimation{
                            easing: Easing.InOutCubic
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

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            EditorViewport{
                id:viewport
                anchors.fill: parent
                scene: scene


                Item {
                    id: info_item
                    width: 100
                    height: 50
                    anchors.right: parent.right
                    anchors.top: parent.top

                    Rectangle{
                        id:info_rect
                        radius: 10
                        anchors.fill: parent
                        color: "red"
                        border.color: "black";
                        //        color: "orange"
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
        }

    }



    property alias mainCamera: scene.activeCamera

}
