#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>

struct Material
{
    QVector3D albedo = QVector3D(0.8f, 0.8f, 0.8f);      // Диффузный цвет (diffuse)
    QVector3D emission = QVector3D(0.0f, 0.0f, 0.0f);    // Излучение
    
    // Параметры Blinn-Phong
    QVector3D specular = QVector3D(1.0f, 1.0f, 1.0f);    // Цвет зеркального отражения
    float shininess = 32.0f;                             // Гладкость (чем больше - тем более зеркальное)
    
    // Прозрачность
    float transparency = 0.0f;                           // 0.0 = непрозрачный, 1.0 = полностью прозрачный
    float ior = 1.5f;                                    // Index of Refraction (показатель преломления)
    
    // Тип материала
    enum Type {
        DIFFUSE,      // Обычный диффузный
        SPECULAR,     // Зеркальный (металл)
        REFRACTIVE    // Прозрачный (стекло)
    };
    Type type = DIFFUSE;
};

#endif // MATERIAL_H
