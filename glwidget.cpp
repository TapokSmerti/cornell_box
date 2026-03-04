#include "glwidget.h"
#include <QDebug>
#include <QFile>

GLWidgetGPU::GLWidgetGPU(QWidget *parent)
    : QOpenGLWidget(parent)
    , shaderProgram(nullptr)
    , vbo(nullptr)
    , vao(nullptr)
    , samples(300)  // Уменьшил для быстрого тестирования
    , maxDepth(7)   // Уменьшил для быстрого тестирования
    , time(0.0f)
{
    // Настройка камеры
    camera.position = QVector3D(0.0f, 1.5f, 8.0f);
    camera.target = QVector3D(0.0f, 1.0f, 0.0f);
    camera.up = QVector3D(0.0f, 1.0f, 0.0f);
    camera.fov = 60.0f;

    // Настройка света - ИСПРАВЛЕНО!
    QVector3D lightPos(0, 3.5f, 0);
    QVector3D lightIntensity(1.0f, 1.0f, 1.0f); // Уменьшенная интенсивность
    lights.push_back(Light(lightPos, lightIntensity));

    // Таймер для обновления (опционально, для анимации)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        time += 0.016f; // ~60 FPS
        update();
    });
    // timer->start(16); // Раскомментируйте для постоянного обновления
}

GLWidgetGPU::~GLWidgetGPU()
{
    makeCurrent();
    delete vao;
    delete vbo;
    delete shaderProgram;
    doneCurrent();
}

void GLWidgetGPU::setSamples(int s)
{
    samples = s;
    update();
}

void GLWidgetGPU::setMaxDepth(int d)
{
    maxDepth = d;
    update();
}

void GLWidgetGPU::initializeGL()
{
    initializeOpenGLFunctions();
    
    qDebug() << "OpenGL Version:" << (char*)glGetString(GL_VERSION);
    qDebug() << "GLSL Version:" << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    setupShaders();
    setupQuad();
}

void GLWidgetGPU::setupShaders()
{
    shaderProgram = new QOpenGLShaderProgram(this);

    // Попробуем несколько путей к шейдерам
    QStringList possibleVertexPaths = {
        "vertex_shader.glsl",
        "./vertex_shader.glsl",
        "../vertex_shader.glsl",
        "G:/study/University/11_semester/graphics/final_project/versions/GPU_bound_shaders_material/vertex_shader.glsl"
    };
    
    QStringList possibleFragmentPaths = {
        "fragment_shader.glsl",
        "./fragment_shader.glsl",
        "../fragment_shader.glsl",
        "G:/study/University/11_semester/graphics/final_project/versions/GPU_bound_shaders_material/fragment_shader.glsl"
    };

    QString vertexShaderSource;
    QString fragmentShaderSource;
    bool vertexLoaded = false;
    bool fragmentLoaded = false;
    
    // Попытка загрузить vertex shader
    for (const QString& path : possibleVertexPaths) {
        QFile vertFile(path);
        if (vertFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "✓ Loading vertex shader from:" << path;
            vertexShaderSource = QString::fromUtf8(vertFile.readAll());
            vertFile.close();
            vertexLoaded = true;
            break;
        }
    }
    
    if (!vertexLoaded) {
        qDebug() << "✗ vertex_shader.glsl not found, using embedded shader";
        vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 position;
out vec2 fragCoord;

void main()
{
    fragCoord = position * 0.5 + 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}
        )";
    }
    
    // Попытка загрузить fragment shader
    for (const QString& path : possibleFragmentPaths) {
        QFile fragFile(path);
        if (fragFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "✓ Loading fragment shader from:" << path;
            fragmentShaderSource = QString::fromUtf8(fragFile.readAll());
            fragFile.close();
            fragmentLoaded = true;
            break;
        }
    }
    
    if (!fragmentLoaded) {
        qDebug() << "✗ fragment_shader.glsl not found, using embedded shader";
        fragmentShaderSource = R"(
#version 330 core

in vec2 fragCoord;
out vec4 FragColor;

uniform vec2 u_resolution;
uniform int u_samples;
uniform int u_maxDepth;
uniform float u_time;

uniform vec3 u_cameraPos;
uniform vec3 u_cameraTarget;
uniform vec3 u_cameraUp;
uniform float u_fov;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;

struct Material {
    vec3 albedo;
    vec3 emission;
};

const Material RED_WALL = Material(vec3(0.75, 0.25, 0.25), vec3(0.0));
const Material GREEN_WALL = Material(vec3(0.25, 0.75, 0.25), vec3(0.0));
const Material WHITE_WALL = Material(vec3(0.73, 0.73, 0.73), vec3(0.0));

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitInfo {
    bool hit;
    float t;
    vec3 position;
    vec3 normal;
    Material material;
};

uint seed = 0u;

uint hash(uint x) {
    x += (x << 10u);
    x ^= (x >> 6u);
    x += (x << 3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

float random() {
    seed = hash(seed);
    return float(seed) / 4294967295.0;
}

void initRandom(vec2 coord, float time) {
    seed = hash(uint(coord.x * 1973.0 + coord.y * 9277.0 + time * 26699.0));
}

vec3 randomCosineHemisphere(vec3 normal) {
    float r1 = random();
    float r2 = random();
    
    float phi = 2.0 * 3.14159265 * r1;
    float cosTheta = sqrt(r2);
    float sinTheta = sqrt(1.0 - r2);
    
    vec3 localDir = vec3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
    
    vec3 tangent = abs(normal.x) > 0.1 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 bitangent = normalize(cross(normal, tangent));
    tangent = normalize(cross(bitangent, normal));
    
    return normalize(tangent * localDir.x + bitangent * localDir.y + normal * localDir.z);
}

HitInfo intersectSphere(Ray ray, vec3 center, float radius, Material mat) {
    HitInfo hit;
    hit.hit = false;
    
    vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    
    float discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) return hit;
    
    float sqrtD = sqrt(discriminant);
    float t1 = (-b - sqrtD) / (2.0 * a);
    float t2 = (-b + sqrtD) / (2.0 * a);
    
    float t = t1 > 0.0 ? t1 : t2;
    if (t <= 0.0) return hit;
    
    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + t * ray.direction;
    hit.normal = normalize(hit.position - center);
    hit.material = mat;
    
    return hit;
}

HitInfo intersectPlane(Ray ray, vec3 point, vec3 normal, Material mat) {
    HitInfo hit;
    hit.hit = false;
    
    float denom = dot(normal, ray.direction);
    if (abs(denom) < 0.0001) return hit;
    
    float t = dot(point - ray.origin, normal) / denom;
    if (t <= 0.0) return hit;
    
    hit.hit = true;
    hit.t = t;
    hit.position = ray.origin + t * ray.direction;
    hit.normal = normal;
    hit.material = mat;
    
    return hit;
}

HitInfo intersectScene(Ray ray) {
    HitInfo closestHit;
    closestHit.hit = false;
    float closestT = 1e20;
    
    HitInfo hit;
    
    hit = intersectPlane(ray, vec3(0, -1, 0), vec3(0, 1, 0), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectPlane(ray, vec3(0, 4, 0), vec3(0, -1, 0), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectPlane(ray, vec3(0, 0, -5), vec3(0, 0, 1), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectPlane(ray, vec3(-3, 0, 0), vec3(1, 0, 0), RED_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectPlane(ray, vec3(3, 0, 0), vec3(-1, 0, 0), GREEN_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectSphere(ray, vec3(0, -0.5, 0), 0.5, WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectSphere(ray, vec3(-0.9, -0.7, 0), 0.3, WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    hit = intersectSphere(ray, vec3(0.9, -0.9, 0), 0.1, WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    return closestHit;
}

vec3 trace(Ray ray, int maxDepth) {
    vec3 color = vec3(0.0);
    vec3 throughput = vec3(1.0);
    
    for (int depth = 0; depth < maxDepth; depth++) {
        HitInfo hit = intersectScene(ray);
        
        if (!hit.hit) break;
        
        vec3 toLight = u_lightPos - hit.position;
        float dist = length(toLight);
        vec3 lightDir = normalize(toLight);
        
        Ray shadowRay;
        shadowRay.origin = hit.position + hit.normal * 0.001;
        shadowRay.direction = lightDir;
        
        HitInfo shadow = intersectScene(shadowRay);
        
        if (!shadow.hit || shadow.t > dist) {
            float NdotL = max(dot(hit.normal, lightDir), 0.0);
            float attenuation = 1.0 / (dist * dist);
            color += throughput * hit.material.albedo * u_lightColor * NdotL * attenuation;
        }
        
        float p = max(throughput.x, max(throughput.y, throughput.z));
        if (random() > p) break;
        throughput /= p;
        
        vec3 randomDir = randomCosineHemisphere(hit.normal);
        ray.origin = hit.position + hit.normal * 0.001;
        ray.direction = randomDir;
        throughput *= hit.material.albedo;
    }
    
    return color;
}

Ray generateCameraRay(vec2 uv) {
    vec3 forward = normalize(u_cameraTarget - u_cameraPos);
    vec3 right = normalize(cross(forward, u_cameraUp));
    vec3 up = cross(right, forward);
    
    float aspectRatio = u_resolution.x / u_resolution.y;
    float scale = tan(radians(u_fov * 0.5));
    
    vec3 direction = normalize(
        forward +
        right * ((uv.x * 2.0 - 1.0) * aspectRatio * scale) +
        up * ((uv.y * 2.0 - 1.0) * scale)
    );
    
    Ray ray;
    ray.origin = u_cameraPos;
    ray.direction = direction;
    return ray;
}

void main() {
    vec2 uv = fragCoord;
    initRandom(gl_FragCoord.xy, u_time);
    
    vec3 color = vec3(0.0);
    
    for (int i = 0; i < u_samples; i++) {
        vec2 jitter = vec2(random(), random()) / u_resolution;
        vec2 sampledUV = uv + jitter;
        
        Ray ray = generateCameraRay(sampledUV);
        color += trace(ray, u_maxDepth);
    }
    
    color /= float(u_samples);
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
        )";
    }

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qCritical() << "Vertex shader compilation failed:" << shaderProgram->log();
        return;
    }

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qCritical() << "Fragment shader compilation failed:" << shaderProgram->log();
        return;
    }

    if (!shaderProgram->link()) {
        qCritical() << "Shader program linking failed:" << shaderProgram->log();
        return;
    }

    qDebug() << "✓ Shaders compiled and linked successfully!";
    
    // ДИАГНОСТИКА: Проверим какие uniforms требуются
    shaderProgram->bind();
    
    qDebug() << "\n=== Checking uniforms ===";
    QStringList requiredUniforms = {
        "u_resolution", "u_samples", "u_maxDepth", "u_time",
        "u_cameraPos", "u_cameraTarget", "u_cameraUp", "u_fov", "u_projectionType",
        "u_lightPos", "u_lightColor", "u_lightIntensity",
        "u_material1_albedo", "u_material1_specular", "u_material1_shininess", "u_material1_transparency",
        "u_material2_albedo", "u_material2_specular", "u_material2_shininess", "u_material2_transparency",
        "u_material3_albedo", "u_material3_specular", "u_material3_shininess", "u_material3_transparency"
    };
    
    for (const QString& uniformName : requiredUniforms) {
        int location = shaderProgram->uniformLocation(uniformName);
        if (location >= 0) {
            qDebug() << "  ✓" << uniformName << "- location:" << location;
        } else {
            qDebug() << "  ✗" << uniformName << "- NOT FOUND (will use default value 0)";
        }
    }
    qDebug() << "=========================\n";
    
    shaderProgram->release();
}

void GLWidgetGPU::setupQuad()
{
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->bind();
    vbo->allocate(vertices, sizeof(vertices));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    vao->release();
    vbo->release();
}

void GLWidgetGPU::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    camera.aspectRatio = float(w) / float(h);
}

void GLWidgetGPU::updateUniforms()
{
    if (!shaderProgram) return;

    shaderProgram->bind();

    // Базовые параметры
    shaderProgram->setUniformValue("u_resolution", QVector2D(width(), height()));
    shaderProgram->setUniformValue("u_samples", samples);
    shaderProgram->setUniformValue("u_maxDepth", maxDepth);
    shaderProgram->setUniformValue("u_time", time);

    // Камера
    camera.updateBasis();
    shaderProgram->setUniformValue("u_cameraPos", camera.position);
    shaderProgram->setUniformValue("u_cameraTarget", camera.target);
    shaderProgram->setUniformValue("u_cameraUp", camera.up);
    shaderProgram->setUniformValue("u_fov", camera.fov);
    
    // ВАЖНО: Проверяем есть ли этот uniform перед установкой
    if (shaderProgram->uniformLocation("u_projectionType") >= 0) {
        shaderProgram->setUniformValue("u_projectionType", 0); // 0 = perspective
    }

    // Свет
    if (!lights.empty()) {
        shaderProgram->setUniformValue("u_lightPos", lights[0].position);
        shaderProgram->setUniformValue("u_lightColor", lights[0].color);
        
        // ВАЖНО: Проверяем есть ли этот uniform перед установкой
        if (shaderProgram->uniformLocation("u_lightIntensity") >= 0) {
            shaderProgram->setUniformValue("u_lightIntensity", 5.0f);
        }
    }

    // Материалы - ВАЖНО: Проверяем наличие uniforms
    if (shaderProgram->uniformLocation("u_material1_albedo") >= 0) {
        // Material 1 (бирюзовая непрозрачная)
        shaderProgram->setUniformValue("u_material1_albedo", QVector3D(0.2f, 0.8f, 0.8f));
        shaderProgram->setUniformValue("u_material1_specular", QVector3D(0.9f, 0.9f, 0.9f));
        shaderProgram->setUniformValue("u_material1_shininess", 64.0f);
        shaderProgram->setUniformValue("u_material1_transparency", 0.0f);

        // Material 2 (желтая полупрозрачная)
        shaderProgram->setUniformValue("u_material2_albedo", QVector3D(0.9f, 0.8f, 0.2f));
        shaderProgram->setUniformValue("u_material2_specular", QVector3D(0.9f, 0.9f, 0.9f));
        shaderProgram->setUniformValue("u_material2_shininess", 32.0f);
        shaderProgram->setUniformValue("u_material2_transparency", 0.7f);

        // Material 3 (темная полупрозрачная)
        shaderProgram->setUniformValue("u_material3_albedo", QVector3D(0.9f, 0.8f, 0.2f));
        shaderProgram->setUniformValue("u_material3_specular", QVector3D(0.9f, 0.9f, 0.9f));
        shaderProgram->setUniformValue("u_material3_shininess", 32.0f);
        shaderProgram->setUniformValue("u_material3_transparency", 0.7f);
        shaderProgram->setUniformValue("u_material3_albedo", QVector3D(0.3f, 0.2f, 0.4f));
        shaderProgram->setUniformValue("u_material3_specular", QVector3D(0.8f, 0.8f, 0.8f));
        shaderProgram->setUniformValue("u_material3_shininess", 16.0f);
        shaderProgram->setUniformValue("u_material3_transparency", 0.8f);
    }

    shaderProgram->release();
}

void GLWidgetGPU::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    updateUniforms();

    shaderProgram->bind();
    vao->bind();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    vao->release();
    shaderProgram->release();
}
