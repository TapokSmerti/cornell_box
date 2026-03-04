#ifndef RAY_H
#define RAY_H

#include <QVector3D>

struct Ray
{
    QVector3D origin;
    QVector3D direction;

    Ray() {}
    Ray(const QVector3D& o, const QVector3D& d)
        : origin(o), direction(d.normalized()) {}
};

#endif // RAY_H
