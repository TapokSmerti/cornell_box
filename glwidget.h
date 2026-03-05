#ifndef GLWIDGET_GPU_H
#define GLWIDGET_GPU_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QTimer>
#include "camera.h"
#include "light.h"

class GLWidgetGPU : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidgetGPU(QWidget *parent = nullptr);
    ~GLWidgetGPU();

    // Параметры рендеринга
    void setSamples(int samples);
    void setMaxDepth(int depth);

    // Контрольная панель
    void setLightIntensity(float intensity);


    void setCameraPosition(QVector3D pos);
    void setCameraTarget(QVector3D target);
    void setCameraFov(float fov);

    void setProjectionType(int type); // 0 = perspective, 1 = orthographic


    void setMaterialAlbedo(int index, QVector3D albedo);
    void setMaterialSpecular(int index, QVector3D specular);
    void setMaterialShininess(int index, float shininess);
    void setMaterialTransparency(int index, float transparency);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void setupShaders();
    void setupQuad();
    void updateUniforms();

    // OpenGL объекты
    QOpenGLShaderProgram* shaderProgram;
    QOpenGLBuffer* vbo;
    QOpenGLVertexArrayObject* vao;

    // Параметры сцены
    Camera camera;
    std::vector<Light> lights;

    // Параметры рендеринга
    int samples;
    int maxDepth;
    float time;

    // Таймер для анимации
    QTimer* timer;

    float lightIntensity;
    int projectionType;

    struct MatParams {
        QVector3D albedo;
        QVector3D specular;
        float shininess;
        float transparency;
    };
    MatParams materials[3];


};

#endif // GLWIDGET_GPU_H
