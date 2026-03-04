#ifndef PLANE_H
#define PLANE_H

#include "sceneobject.h"
#include <QVector3D>

class Plane : public SceneObject
{
public:
    Plane(const QVector3D& point, const QVector3D& normal);

    HitInfo intersect(const Ray& ray) const override;

private:
    QVector3D point;
    QVector3D normal;
};

#endif // PLANE_H
