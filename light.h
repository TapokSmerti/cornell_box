#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

class Light
{
public:
    QVector3D position;
    QVector3D color;   // интенсивность RGB

    Light(const QVector3D& pos, const QVector3D& col)
        : position(pos), color(col) {}
};

#endif // LIGHT_H
