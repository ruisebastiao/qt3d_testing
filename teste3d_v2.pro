QT += quick 3dcore 3drender 3dinput 3dextras 3dquick qml 3dquickextras 3danimation widgets 3drender-private

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    editorutils.cpp \    
    mycamera.cpp \
    mysceneloader.cpp \
    sceneitem.cpp \
    editorscene.cpp \
    editorviewportitem.cpp \
    inputcontrols/editorcameracontroller.cpp \
    materials/ontopeffect.cpp \
    components/editorsceneitemlightcomponentsmodel.cpp \
    components/editorsceneitemmaterialcomponentsmodel.cpp \
    components/editorsceneitemmeshcomponentsmodel.cpp \
    components/editorsceneitemtransformcomponentsmodel.cpp \
    components/lightcomponentproxyitem.cpp \
    components/materialcomponentproxyitem.cpp \
    components/meshcomponentproxyitem.cpp \
    components/qdummyobjectpicker.cpp \
    components/transformcomponentproxyitem.cpp \
    editorsceneitemcomponentsmodel.cpp

RESOURCES += qml.qrc \
    blender_teste/shaders.qrc \
    blender_teste/buffers.qrc \
    blender_teste/blender_teste_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    editorutils.h \    
    mycamera.h \
    mysceneloader.h \
    sceneitem.h \
    editorscene.h \
    editorviewportitem.h \
    inputcontrols/editorcameracontroller.h \
    materials/ontopeffect.h \
    components/editorsceneitemlightcomponentsmodel.h \
    components/editorsceneitemmaterialcomponentsmodel.h \
    components/editorsceneitemmeshcomponentsmodel.h \
    components/editorsceneitemtransformcomponentsmodel.h \
    components/lightcomponentproxyitem.h \
    components/materialcomponentproxyitem.h \
    components/meshcomponentproxyitem.h \
    components/qdummyobjectpicker.h \
    components/transformcomponentproxyitem.h \
    editorsceneitemcomponentsmodel.h

#include(scene_teste/teste1.pri)

INCLUDEPATH += components materials inputcontrols
