#include "camera.h"
#include <QtMath>

Camera::Camera()
{
    position = QVector3D(0.0f, 0.0f, 3.0f);
    target   = QVector3D(0.0f, 0.0f, 0.0f);
    up       = QVector3D(0.0f, 1.0f, 0.0f);

    fov = 60.0f;
    orthoScale = 2.0f;
    aspectRatio = 1.0f;

    projection = ProjectionType::Perspective;

    updateBasis();
}

void Camera::updateBasis()
{
    forward = (target - position).normalized();
    right   = QVector3D::crossProduct(forward, up).normalized();
    camUp   = QVector3D::crossProduct(right, forward).normalized();
}

Ray Camera::generateRay(float u, float v) const
{
    if (projection == ProjectionType::Perspective)
    {
        float scale = qTan(qDegreesToRadians(fov * 0.5f));
        QVector3D dir =
            forward +
            right * (u * aspectRatio * scale) +
            camUp * (v * scale);

        return Ray(position, dir);
    }
    else // Orthographic
    {
        QVector3D origin =
            position +
            right * (u * orthoScale) +
            camUp * (v * orthoScale);

        return Ray(origin, forward);
    }
}
