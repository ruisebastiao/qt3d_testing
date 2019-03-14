#include "editorutils.h"

#include "editorscene.h"

#include "editorviewportitem.h"


#include <QtWidgets/QApplication>

#include <QQmlApplicationEngine>
#include <qopenglcontext.h>
#include <qsurfaceformat.h>
#include "mysceneloader.h"

#include <editorcameracontroller.h>



int main(int argc, char *argv[])
{

    QApplication app(argc, argv);



    qmlRegisterType<EditorUtils>("base", 1, 0, "EditorUtils");

//    qmlRegisterType<MyCamera>("base", 1, 0, "MyCamera");
    qmlRegisterType<MySceneLoader>("base", 1, 0, "MySceneLoader");

    qmlRegisterType<EditorScene>("base", 1, 0, "EditorScene");
    qmlRegisterType<EditorViewportItem>("base", 1, 0, "EditorViewport");
    qmlRegisterUncreatableType<EditorCameraController>("base", 1, 0, "EditorCameraController", "Created by viewport");
    qmlRegisterUncreatableType<SceneItem>("base", 1, 0, "EditorSceneItem", "Created by EditorScene");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
