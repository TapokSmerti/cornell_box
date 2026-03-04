#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include "ray.h"

class Camera
{
public:
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    Camera();

    Ray generateRay(float u, float v) const;
    void updateBasis(); // Теперь публичный

    // параметры камеры
    QVector3D position;
    QVector3D target;
    QVector3D up;

    float fov;          // для перспективной
    float orthoScale;   // для ортографической
    float aspectRatio;

    ProjectionType projection;

private:
    QVector3D forward;
    QVector3D right;
    QVector3D camUp;
};

#endif // CAMERA_H
