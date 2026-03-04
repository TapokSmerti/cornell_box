#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "ray.h"
#include "hitinfo.h"

class SceneObject
{
public:
    Material material;

    virtual ~SceneObject() = default;
    virtual HitInfo intersect(const Ray& ray) const = 0;
};

#endif // SCENEOBJECT_H
