import Qt3D.Core 2.9
import Qt3D.Render 2.9

Entity {
    id: root

    Buffer {
        id: vertexBuffermonkey
        type: Buffer.VertexBuffer
        data: readBinaryFile("qrc:/assets/binaries/monkey.vertices")
    }

    Buffer {
        id: indexBuffermonkey
        type: Buffer.VertexBuffer
        data: readBinaryFile("qrc:/assets/binaries/monkey.indices")
    }
    Attribute {
        id: monkeyPositionVertexAttribute
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        byteOffset: 0
        byteStride: 24
        name: defaultPositionAttributeName
        buffer: vertexBuffermonkey
    }

    Attribute {
        id: monkeyNormalVertexAttribute
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        byteOffset: 12
        byteStride: 24
        name: defaultNormalAttributeName
        buffer: vertexBuffermonkey
    }
    readonly property GeometryRenderer monkeymaterial1: GeometryRenderer {
        instanceCount: 1
        indexOffset: 0
        firstInstance: 0
        primitiveType: GeometryRenderer.Triangles
        geometry: Geometry {
             readonly property Attribute indexAttribute: Attribute {
                 attributeType: Attribute.IndexAttribute
                 vertexBaseType: Attribute.UnsignedShort
                 vertexSize: 1
                 byteOffset: 0 * 2
                 byteStride: 2
                 count: 2904
                 buffer: indexBuffermonkey
               }

            attributes: [indexAttribute, monkeyPositionVertexAttribute, monkeyNormalVertexAttribute]
        }
    }


    Buffer {
        id: vertexBuffertorus
        type: Buffer.VertexBuffer
        data: readBinaryFile("qrc:/assets/binaries/torus.vertices")
    }

    Buffer {
        id: indexBuffertorus
        type: Buffer.VertexBuffer
        data: readBinaryFile("qrc:/assets/binaries/torus.indices")
    }
    Attribute {
        id: torusPositionVertexAttribute
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        byteOffset: 0
        byteStride: 24
        name: defaultPositionAttributeName
        buffer: vertexBuffertorus
    }

    Attribute {
        id: torusNormalVertexAttribute
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        byteOffset: 12
        byteStride: 24
        name: defaultNormalAttributeName
        buffer: vertexBuffertorus
    }
    readonly property GeometryRenderer torusmaterial1: GeometryRenderer {
        instanceCount: 1
        indexOffset: 0
        firstInstance: 0
        primitiveType: GeometryRenderer.Triangles
        geometry: Geometry {
             readonly property Attribute indexAttribute: Attribute {
                 attributeType: Attribute.IndexAttribute
                 vertexBaseType: Attribute.UnsignedShort
                 vertexSize: 1
                 byteOffset: 0 * 2
                 byteStride: 2
                 count: 3456
                 buffer: indexBuffertorus
               }

            attributes: [indexAttribute, torusPositionVertexAttribute, torusNormalVertexAttribute]
        }
    }



}
