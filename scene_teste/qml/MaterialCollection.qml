import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.9

Entity {
    id: root

     readonly property Material material1: PhongMaterial {
        ambient: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        diffuse: Qt.rgba(0.800000011920929, 0.800000011920929, 0.800000011920929, 1.0)
        specular: Qt.rgba(1.0, 1.0, 1.0, 1.0)
    }
 
}
