#include "sphere.h"
#include <QtMath>

Sphere::Sphere(const QVector3D& c, float r)
    : center(c), radius(r)
{
}

HitInfo Sphere::intersect(const Ray& ray) const
{
    HitInfo hit;

    QVector3D oc = ray.origin - center;

    float a = QVector3D::dotProduct(ray.direction, ray.direction);
    float b = 2.0f * QVector3D::dotProduct(oc, ray.direction);
    float c = QVector3D::dotProduct(oc, oc) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return hit;

    float sqrtD = qSqrt(discriminant);
    float t1 = (-b - sqrtD) / (2.0f * a);
    float t2 = (-b + sqrtD) / (2.0f * a);

    float t = (t1 > 0) ? t1 : t2;
    if (t <= 0)
        return hit;

    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + t * ray.direction;
    hit.normal = (hit.position - center).normalized();

    return hit;
}
