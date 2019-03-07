import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.9
import Qt3D.Input 2.1

Entity {
    id: mainRoot

    components: [
       RenderSettings {
           activeFrameGraph: ForwardRenderer {
               clearColor: Qt.rgba(0, 0.5, 1, 1)
               camera: camera
               frustumCulling: false
           }
       },
       // Event Source will be set by the Qt3DQuickWindow
       InputSettings { }
    ]

    OrbitCameraController
    {
        camera: camera
    }

    MeshCollection {
        id: _meshCollection
    }

    MaterialCollection {
        id: _materialCollection
    }

    Entity {
        // Apply global blender transformation
        components: Transform { matrix: Qt.matrix4x4(-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0)}

        Torus {
            id: torus
        }

        Monkey {
            id: monkey
        }

        Lamp {
            id: lamp
        }

        Blendercamera {
            id: camera
        }

    }
}
