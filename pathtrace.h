#ifndef PATHTRACE_H
#define PATHTRACE_H

#include "scene.h"
#include "ray.h"
#include <QVector3D>

QVector3D randomUnitVector()
{
    float z = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
    float t = ((float) rand() / RAND_MAX) * 2.0f * M_PI;
    float r = sqrtf(1 - z*z);
    return QVector3D(r * cosf(t), r * sinf(t), z).normalized();
}

QVector3D randomHemisphereDirection(const QVector3D& normal)
{
    QVector3D dir = randomUnitVector();
    if (QVector3D::dotProduct(dir, normal) < 0)
        dir = -dir;
    return dir;
}


QVector3D trace(const Ray& ray, const Scene& scene, int depth)
{
    if (depth <= 0)
        return QVector3D(0,0,0);

    HitInfo hit = scene.intersect(ray);
    if (!hit.hit)
        return QVector3D(0.1f, 0.1f, 0.15f); // фон

    // случайное направление для диффузного отражения
    QVector3D targetDir = randomHemisphereDirection(hit.normal);
    Ray newRay(hit.position + hit.normal * 1e-4f, targetDir);

    QVector3D incoming = trace(newRay, scene, depth - 1);

    return hit.material.albedo * incoming;
}

#endif // PATHTRACE_H
