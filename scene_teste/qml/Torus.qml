import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.9

Entity {
    id: root

    readonly property Material material: _materialCollection.material1
    readonly property Transform transform: Transform { matrix: Qt.matrix4x4(1.0, 0.0, 0.0, 3.0, 0.0, 1.0, 0.0, 3.0, 0.0, 0.0, 1.0, 3.0, 0.0, 0.0, 0.0, 1.0)}
    readonly property GeometryRenderer geometryRenderer: _meshCollection.torusmaterial1
    components: [transform, material, geometryRenderer]
}
