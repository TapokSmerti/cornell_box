#include "plane.h"
#include <QtMath>

Plane::Plane(const QVector3D& p, const QVector3D& n)
    : point(p), normal(n.normalized())
{
}

HitInfo Plane::intersect(const Ray& ray) const
{
    HitInfo hit;

    float denom = QVector3D::dotProduct(normal, ray.direction);
    if (qFuzzyIsNull(denom))
        return hit;

    float t = QVector3D::dotProduct(point - ray.origin, normal) / denom;
    if (t <= 0)
        return hit;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + t * ray.direction;
    hit.normal = normal;

    return hit;
}
