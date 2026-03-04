#ifndef HITINFO_H
#define HITINFO_H

#include <QVector3D>
#include "material.h"

struct HitInfo
{
    bool hit = false;
    float t = 0.0f;

    QVector3D position;
    QVector3D normal;

    Material material; // <- добавляем материал
};

#endif // HITINFO_H
