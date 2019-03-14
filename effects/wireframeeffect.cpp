#include "wireframeeffect.h"

#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <QtCore/QUrl>
#include <QAlphaCoverage>
#include <QMultiSampleAntiAliasing>
#include <QVector3D>
#include <QVector4D>

WireframeEffect::WireframeEffect(Qt3DCore::QNode *parent)
    : Qt3DRender::QEffect(parent)
{

    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(2);
    technique->graphicsApiFilter()->setMinorVersion(1);

    Qt3DRender::QTechnique *techniqueES2 = new Qt3DRender::QTechnique();
    techniqueES2->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    techniqueES2->graphicsApiFilter()->setMajorVersion(2);
    techniqueES2->graphicsApiFilter()->setMinorVersion(0);
    techniqueES2->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);

    Qt3DRender::QFilterKey *filterkey = new Qt3DRender::QFilterKey(this);
    filterkey->setName(QStringLiteral("renderingStyle"));
    filterkey->setValue(QStringLiteral("forward"));

    technique->addFilterKey(filterkey);
    techniqueES2->addFilterKey(filterkey);

    Qt3DRender::QShaderProgram *shader = new Qt3DRender::QShaderProgram();
    shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/robustwireframe.vert"))));
    shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/robustwireframe.frag"))));

    Qt3DRender::QRenderPass *renderPass = new Qt3DRender::QRenderPass();
    renderPass->setShaderProgram(shader);

    Qt3DRender::QAlphaCoverage* aplha_coverage= new Qt3DRender::QAlphaCoverage();
    renderPass->addRenderState(aplha_coverage);
    Qt3DRender::QMultiSampleAntiAliasing *multiSampleAntialiasing = new Qt3DRender::QMultiSampleAntiAliasing();
    renderPass->addRenderState(multiSampleAntialiasing);

    technique->addRenderPass(renderPass);
    techniqueES2->addRenderPass(renderPass);

    addTechnique(technique);
    addTechnique(techniqueES2);

}
