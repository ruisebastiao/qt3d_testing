#ifndef WIREFRAMEEFFECT_H
#define WIREFRAMEEFFECT_H

#include <QEffect>



class WireframeEffect:public Qt3DRender::QEffect
{
public:
    explicit WireframeEffect(Qt3DCore::QNode *parent = nullptr);
};

#endif // WIREFRAMEEFFECT_H
