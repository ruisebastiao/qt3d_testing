

import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

import QtQuick 2.12
import QtQuick.Controls 2.5

import base 1.0

import "scene_teste/qml"

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    EditorUtils{
        id:utils
    }

    Scene3D {
        id: scene3d

        anchors.fill: parent
        anchors.margins: 10
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
        hoverEnabled: true

        Entity {
            id: scene_root


            property real metalness
            property real roughness
            property real exposure:1.5
            property real gamma:1.8

            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        camera: mainCamera
                        clearColor: "white"
                        //                        gamma: scene.gamma
                    }
                },
                // Event Source will be set by the Qt3DQuickWindow
                InputSettings { }
            ]

            OrbitCameraController {
                camera: mainCamera
                linearSpeed: 20
                lookSpeed: 400
            }

            Camera {
                id: mainCamera
                //                position:  sceneLoader.structureTransform?sceneLoader.structureTransform.translation:Qt.vector3d(0, 0, 0)
                //                position: Qt.vector3d(-11.4611, 9.17624, -14.4893)
                //                viewCenter: sceneLoader.structureTransform?sceneLoader.structureTransform.translation:Qt.vector3d(0, 0, 0)
                //                viewCenter: Qt.vector3d(1.9125, 1.19805, 1.73572)
                viewCenter: "0,0,0"
                //upVector: "0,0,1"
                position: "0,0,10"
                onViewCenterChanged: {
                    console.log("viewCenter:"+viewCenter)
                }
                projectionType: CameraLens.PerspectiveProjection

                onPositionChanged: {
                    console.log("Position:"+position)
                }



                Behavior on viewCenter {
                    Vector3dAnimation{
                        easing: Easing.InOutCubic
                        duration: 1000
                    }
                }

                Behavior on position {
                    Vector3dAnimation{
                        easing: Easing.InOutCubic
                        duration: 1000
                    }
                }

                exposure: scene_root.exposure
            }
            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(0, -3.0, 0.0).normalized();
                        color: "#fbf9ce"
                        intensity: 0.1
                    }
                ]
            }
            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(-3, 0, 0.0).normalized();
                        color: "#fbf9ce"
                        intensity: 0.1
                    }
                ]
            }

            Entity {
                components: [
                    DirectionalLight {
                        worldDirection: Qt.vector3d(3, 0, 0.0).normalized();
                        color: "#fbf9ce"
                        intensity: 0.1
                    }
                ]
            }

            Entity {
                id:lm36_scene



                Transform{
                    id: lm36_scene_Transform
                    Behavior on translation {
                        Vector3dAnimation{
                            duration: 1000
                        }
                    }

                    translation: Qt.vector3d(0, 0, 0) //random values, choose your own
                }

                property Entity monkey
                onMonkeyChanged: {
                    console.log("monkey entity set")
                    for (var i = 0; i < monkey.components.length; ++i) {

                        var cmp=monkey.components[i]
                        if(cmp){
                            var cmp_class=cmp.toString();
                            if(cmp_class.indexOf("Qt3DCore::QTransform")>=0){
                                monkeyTransform=cmp
                            }

                        }



                    }

                }

                property Transform monkeyTransform
                onMonkeyTransformChanged: {
                    console.log("monkeyTransform:"+monkeyTransform.matrix)
                }


                property Entity torus
                onTorusChanged: {
                    console.log("torus entity set")
                    for (var i = 0; i < torus.components.length; ++i) {

                        var cmp=torus.components[i]
                        if(cmp){
                            var cmp_class=cmp.toString();
                            if(cmp_class.indexOf("Qt3DCore::QTransform")>=0){
                                torusTransform=cmp
                            }

                        }



                    }

                }

                property Transform torusTransform
                onTorusTransformChanged: {
                    console.log("qentityTransform:"+utils.qentityTransform(torus).translation)

                }




                SceneLoader{
                    id:sceneLoader
                    source: "file:///home/rui/projects/cad/teste.obj"

                    onStatusChanged: {
                        console.log("SceneLoader status: " + status);
                        if (status == SceneLoader.Ready) {
                            console.log("Scene is ready");

                            console.log("entityNames:"+sceneLoader.entityNames());


                            lm36_scene.monkey=sceneLoader.entity("monkey");
                            lm36_scene.torus=sceneLoader.entity("torus");
                        }
                    }
                }

                components:[sceneLoader,lm36_scene_Transform]

            }

        }
    }

    Button{
        width: 100
        height: 50
        text: "test"


        onClicked: {

//            mainCamera.viewEntity(lm36_scene.torus)
            //mainCamera.viewAll()
            console.log("lm36_scene.monkey:"+lm36_scene.monkey)
            mainCamera.translate(Qt.vector3d(1, 1, 1),Camera.TranslateViewCenter)
        }
    }
}
