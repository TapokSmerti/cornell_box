#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "sceneobject.h"

class Scene
{
public:
    void addObject(SceneObject* obj);
    HitInfo intersect(const Ray& ray) const;

private:
    std::vector<SceneObject*> objects;
};

#endif // SCENE_H
