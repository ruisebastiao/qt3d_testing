import QtQuick.Controls 2.5 as QQC2

import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

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

    //    Scene3D {
    //        id: scene3d

    //        anchors.fill: parent
    //        anchors.margins: 10
    //        focus: true
    //        aspects: ["input", "logic"]
    //        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
    //        hoverEnabled: true

    //        Entity {
    //            id: scene_root


    //            property real metalness
    //            property real roughness
    //            property real exposure:1.5
    //            property real gamma:1.8

    //            components: [
    //                RenderSettings {
    //                    activeFrameGraph: ForwardRenderer {
    //                        camera: mainCamera
    //                        clearColor: "white"
    //                        //                        gamma: scene.gamma
    //                    }
    //                },
    //                // Event Source will be set by the Qt3DQuickWindow
    //                InputSettings { }
    //            ]

    //            SOrbitCameraController {
    //                camera: mainCamera
    //                linearSpeed: 20
    //                lookSpeed: 400
    //            }

    //            Camera {
    //                id: mainCamera




    //                // viewCenter: "0,0,0"
    //                onUpVectorChanged: {
    //                    console.log("Up vector:"+upVector)
    //                }

    //                property vector3d baseViewCenter
    //                property vector3d basePosition

    //                function setBasePosition(){
    //                    viewCenter=baseViewCenter
    //                    position=basePosition
    //                    upVector=Qt.vector3d(0,1,0);
    //                }

    //                onViewCenterChanged: {
    //                    console.log("viewCenter:"+viewCenter)
    //                }
    //                projectionType: CameraLens.PerspectiveProjection

    //                onPositionChanged: {
    //                    console.log("Position:"+position)
    //                }

    //                Behavior on upVector {
    //                    Vector3dAnimation{
    //                        easing: Easing.InOutCubic
    //                        duration: 1000
    //                    }
    //                }



    //                Behavior on viewCenter {
    //                    Vector3dAnimation{
    //                        easing: Easing.InOutCubic
    //                        duration: 1000
    //                    }
    //                }

    //                Behavior on position {
    //                    Vector3dAnimation{
    //                        easing: Easing.InOutCubic
    //                        duration: 1000
    //                    }
    //                }

    //                exposure: scene_root.exposure
    //            }
    //            Entity {
    //                components: [
    //                    DirectionalLight {
    //                        worldDirection: Qt.vector3d(0, -3.0, 0.0).normalized();
    //                        color: "white"
    //                        intensity: 0.5
    //                    }
    //                ]
    //            }
    //            Entity {
    //                components: [
    //                    DirectionalLight {
    //                        worldDirection: Qt.vector3d(-3, 0, 0.0).normalized();
    //                        color: "white"
    //                        intensity: 0.5
    //                    }
    //                ]
    //            }

    //            Entity {
    //                components: [
    //                    DirectionalLight {
    //                        worldDirection: Qt.vector3d(3, 0, 0.0).normalized();
    //                        color: "white"
    //                        intensity: 0.5
    //                    }
    //                ]
    //            }


    //            Entity {
    //                id:lm36_scene


    //                Transform{
    //                    id: lm36_scene_Transform
    //                    Behavior on translation {
    //                        Vector3dAnimation{
    //                            duration: 1000
    //                        }
    //                    }

    //                    translation: Qt.vector3d(0, 0, 0) //random values, choose your own
    //                }

    //                property Entity monkey
    //                onMonkeyChanged: {
    //                    console.log("monkey entity set")
    //                    for (var i = 0; i < monkey.components.length; ++i) {

    //                        var cmp=monkey.components[i]
    //                        if(cmp){
    //                            var cmp_class=cmp.toString();
    //                            if(cmp_class.indexOf("Qt3DCore::QTransform")>=0){
    //                                monkeyTransform=cmp
    //                            }

    //                        }



    //                    }

    //                }

    //                property Transform monkeyTransform
    //                onMonkeyTransformChanged: {
    //                    console.log("monkeyTransform:"+monkeyTransform.matrix)
    //                }


    //                property Entity torus
    //                onTorusChanged: {
    //                    console.log("torus entity set")
    //                    for (var i = 0; i < torus.components.length; ++i) {

    //                        var cmp=torus.components[i]
    //                        if(cmp){
    //                            var cmp_class=cmp.toString();
    //                            if(cmp_class.indexOf("Qt3DCore::QTransform")>=0){
    //                                torusTransform=cmp
    //                            }

    //                        }



    //                    }

    //                }

    //                property Transform torusTransform
    //                onTorusTransformChanged: {
    //                    console.log("qentityTransform:"+torusTransform.translation)

    //                }




    //                MySceneLoader{
    //                    id:sceneLoader
    //                    source: "file:///home/rui/projects/cad/teste.dae"

    //                    Timer{
    //                        id:tm01_getpos
    //                        interval: 250
    //                        onTriggered: {
    //                            mainCamera.basePosition=mainCamera.position
    //                            mainCamera.baseViewCenter=mainCamera.viewCenter
    //                        }

    //                        repeat: false
    //                    }

    //                    onStatusChanged: {
    //                        console.log("SceneLoader status: " + status);
    //                        if (status == SceneLoader.Ready) {
    //                            console.log("Scene is ready");

    //                            mainCamera.viewAll();
    //                            tm01_getpos.start()

    //                            console.log("entityNames:"+sceneLoader.entityNames());


    //                            lm36_scene.monkey=sceneLoader.entity("monkey");
    //                            lm36_scene.torus=sceneLoader.entity("torus");
    //                        }
    //                    }
    //                }

    //                components:[sceneLoader,lm36_scene_Transform]

    //            }



    //        }
    //    }

    EditorScene{
        id:scene
        viewport:viewport
        Behavior on activeCamera.upVector {
            Vector3dAnimation{
                easing: Easing.InOutCubic
                duration: 1000
            }
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

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, -3.0, 0.0).normalized();
                        color: "white"
                        intensity: 0.5
                    }
                ]
            }
            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(-3, 0, 0.0).normalized();
                        color: "white"
                        intensity: 0.5
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(3, 0, 0.0).normalized();
                        color: "white"
                        intensity: 0.5
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(-3, 0, 0.0).normalized();
                        color: "white"
                        intensity: 0.5
                    }
                ]
            }




        }
    }

    EditorViewport{
        id:viewport
        anchors.fill: parent
        scene: scene

    }

    //    SceneLoader{
    //        id:sceneLoader
    //        source: "file:///home/rui/projects/cad/teste.dae"

    //        onStatusChanged: {
    //            console.log("SceneLoader status: " + status);
    //            if (status == SceneLoader.Ready) {
    //                console.log("Scene is ready");


    ////                mainCamera.viewAll();
    ////                tm01_getpos.start()

    ////                console.log("entityNames:"+sceneLoader.entityNames());


    ////                lm36_scene.monkey=sceneLoader.entity("monkey");
    ////                lm36_scene.torus=sceneLoader.entity("torus");
    //            }
    //        }
    //    }


    property alias mainCamera: scene.activeCamera
    Flow{



        QQC2.Button{
            width: 100
            height: 50
            text: "Load"


            onClicked: {

                scene.loadFile("file:///home/rui/projects/cad/teste.dae")
            }
        }



        QQC2.Button{
            width: 100
            height: 50
            text: "Fit torus"


            onClicked: {

                var len = mainCamera.viewVector.length();
                mainCamera.viewCenter=lm36_scene.torusTransform.translation.times(Qt.vector3d(1,1,-1))
                mainCamera.position=lm36_scene.torusTransform.translation
                mainCamera.upVector=Qt.vector3d(0,1,0);

                //            mainCamera.viewAll()

                //            mainCamera.translate(lm36_scene.torusTransform.translation,Camera.TranslateViewCenter)
            }
        }

        QQC2.Button{
            width: 100
            height: 50
            text: "View all"


            onClicked: {

                mainCamera.setBasePosition()


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


    }

}
