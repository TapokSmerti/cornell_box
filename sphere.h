#ifndef SPHERE_H
#define SPHERE_H

#include "sceneobject.h"
#include <QVector3D>

class Sphere : public SceneObject
{
public:
    Sphere(const QVector3D& center, float radius);

    HitInfo intersect(const Ray& ray) const override;

private:
    QVector3D center;
    float radius;
};

#endif // SPHERE_H
