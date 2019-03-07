#include "editorutils.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qopenglcontext.h>
#include <qsurfaceformat.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) { // Learn OpenGL
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
    } else if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGLES) { // Learn OpenGLES??
        format.setRenderableType(QSurfaceFormat::OpenGLES);
        format.setVersion(3, 0);
        format.setSamples(0);
    }
    format.setAlphaBufferSize(0);
    format.setDepthBufferSize(0);
    format.setStencilBufferSize(0);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(0); // Full speed rendering
    QSurfaceFormat::setDefaultFormat(format);

    //EditorUtils* utils= new EditorUtils();

    //qmlRegisterSingletonType<EditorUtils>("base", 1, 0,"",utils);

    qmlRegisterType<EditorUtils>("base", 1, 0, "EditorUtils");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
