#include "scene.h"
#include <limits>

void Scene::addObject(SceneObject* obj)
{
    objects.push_back(obj);
}

HitInfo Scene::intersect(const Ray& ray) const
{
    HitInfo closestHit;
    float closestT = std::numeric_limits<float>::max();

    for (const auto& obj : objects)
    {
        HitInfo hit = obj->intersect(ray);
        if (hit.hit && hit.t < closestT)
        {
            closestT = hit.t;
            closestHit = hit;
            closestHit.material = obj->material;

        }
    }

    return closestHit;
}
