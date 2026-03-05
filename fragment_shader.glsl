#version 330 core

in vec2 fragCoord;
out vec4 FragColor;

// Uniforms
uniform vec2 u_resolution;
uniform int u_samples;
uniform int u_maxDepth;
uniform float u_time;

// Camera
uniform vec3 u_cameraPos;
uniform vec3 u_cameraTarget;
uniform vec3 u_cameraUp;
uniform float u_fov;
uniform int u_projectionType; // 0 = perspective, 1 = orthographic

// Light
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;

// Material controls для UI
uniform vec3 u_material1_albedo;
uniform vec3 u_material1_specular;
uniform float u_material1_shininess;
uniform float u_material1_transparency;

uniform vec3 u_material2_albedo;
uniform vec3 u_material2_specular;
uniform float u_material2_shininess;
uniform float u_material2_transparency;

uniform vec3 u_material3_albedo;
uniform vec3 u_material3_specular;
uniform float u_material3_shininess;
uniform float u_material3_transparency;

// Материалы
struct Material {
    vec3 albedo;
    vec3 emission;
    vec3 specular;
    float shininess;
    float transparency;
    float ior;
    int type; // 0=diffuse, 1=specular, 2=refractive
};

// Предустановленные материалы для стен
const Material RED_WALL = Material(
    vec3(0.75, 0.25, 0.25), vec3(0.0), vec3(0.3, 0.3, 0.3), 8.0, 0.0, 1.0, 0
);
const Material GREEN_WALL = Material(
    vec3(0.25, 0.75, 0.25), vec3(0.0), vec3(0.3, 0.3, 0.3), 8.0, 0.0, 1.0, 0
);
const Material WHITE_WALL = Material(
    vec3(0.73, 0.73, 0.73), vec3(0.0), vec3(0.3, 0.3, 0.3), 8.0, 0.0, 1.0, 0
);

// Динамические материалы (управляются через UI)
Material getMaterial1() {
    return Material(
        u_material1_albedo,
        vec3(0.0),
        u_material1_specular,
        u_material1_shininess,
        u_material1_transparency,
        1.5,
        u_material1_transparency > 0.5 ? 2 : 0
    );
}

Material getMaterial2() {
    return Material(
        u_material2_albedo,
        vec3(0.0),
        u_material2_specular,
        u_material2_shininess,
        u_material2_transparency,
        1.5,
        u_material2_transparency > 0.5 ? 2 : 0
    );
}

Material getMaterial3() {
    return Material(
        u_material3_albedo,
        vec3(0.0),
        u_material3_specular,
        u_material3_shininess,
        u_material3_transparency,
        1.5,
        u_material3_transparency > 0.5 ? 2 : 0
    );
}

// Структуры
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

// ===== RNG =====
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

// ===== Вспомогательные функции =====
vec3 randomUnitVector() {
    float z = random() * 2.0 - 1.0;
    float t = random() * 2.0 * 3.14159265;
    float r = sqrt(1.0 - z * z);
    return vec3(r * cos(t), r * sin(t), z);
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

// Отражение
vec3 reflect(vec3 I, vec3 N) {
    return I - 2.0 * dot(I, N) * N;
}

// Преломление (Snell's law)
vec3 refract(vec3 I, vec3 N, float eta) {
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
        return vec3(0.0);
    else
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

// Френель (Schlick approximation)
float fresnel(vec3 I, vec3 N, float ior) {
    float cosTheta = abs(dot(I, N));
    float r0 = pow((1.0 - ior) / (1.0 + ior), 2.0);
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

// ===== Пересечения =====
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

// ===== Сцена =====
HitInfo intersectScene(Ray ray) {
    HitInfo closestHit;
    closestHit.hit = false;
    float closestT = 1e20;
    
    HitInfo hit;
    
    // Пол
    hit = intersectPlane(ray, vec3(0, -1, 0), vec3(0, 1, 0), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Потолок
    hit = intersectPlane(ray, vec3(0, 4, 0), vec3(0, -1, 0), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Задняя стена
    hit = intersectPlane(ray, vec3(0, 0, -5), vec3(0, 0, 1), WHITE_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Левая стена (красная)
    hit = intersectPlane(ray, vec3(-3, 0, 0), vec3(1, 0, 0), RED_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Правая стена (зелёная)
    hit = intersectPlane(ray, vec3(3, 0, 0), vec3(-1, 0, 0), GREEN_WALL);
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Сфера 1 (бирюзовая непрозрачная)
    hit = intersectSphere(ray, vec3(0, -0.5, 0), 0.5, getMaterial1());
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Сфера 2 (желтая полупрозрачная)
    hit = intersectSphere(ray, vec3(-0.9, -0.7, 0), 0.3, getMaterial2());
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    // Сфера 3 (темная полупрозрачная)
    hit = intersectSphere(ray, vec3(0.9, -0.7, 0), 0.3, getMaterial3());
    if (hit.hit && hit.t < closestT) {
        closestT = hit.t;
        closestHit = hit;
    }
    
    return closestHit;
}

// ===== Blinn-Phong освещение =====
vec3 blinnPhong(vec3 lightDir, vec3 viewDir, vec3 normal, Material mat, vec3 lightColor) {
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * mat.albedo * lightColor;
    
    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), mat.shininess);
    vec3 specular = spec * mat.specular * lightColor;
    
    return diffuse + specular;
}

// ===== Path Tracing =====
vec3 trace(Ray ray, int maxDepth) {
    vec3 color = vec3(0.0);
    vec3 throughput = vec3(1.0);
    
    for (int depth = 0; depth < maxDepth; depth++) {
        HitInfo hit = intersectScene(ray);
        
        if (!hit.hit) break;
        
        vec3 viewDir = normalize(-ray.direction);
        vec3 toLight = u_lightPos - hit.position;
        float dist = length(toLight);
        vec3 lightDir = normalize(toLight);
        
        // Тень
        Ray shadowRay;
        shadowRay.origin = hit.position + hit.normal * 0.001;
        shadowRay.direction = lightDir;
        HitInfo shadow = intersectScene(shadowRay);
        
        if (!shadow.hit || shadow.t > dist) {
            float attenuation = u_lightIntensity / (dist * dist);
            vec3 lighting = blinnPhong(lightDir, viewDir, hit.normal, hit.material, u_lightColor * attenuation);
            color += throughput * lighting * (1.0 - hit.material.transparency);
        }
        
        // Прозрачность
        if (hit.material.transparency > 0.5) {
            bool entering = dot(ray.direction, hit.normal) < 0.0;
            vec3 faceNormal = entering ? hit.normal : -hit.normal;
            float eta = entering ? (1.0 / hit.material.ior) : hit.material.ior;
            
            vec3 refracted = refract(normalize(ray.direction), faceNormal, eta);
            
            if (dot(refracted, refracted) > 0.001) {
                ray.origin = hit.position - faceNormal * 0.005;
                ray.direction = normalize(refracted);
                throughput *= hit.material.albedo;
                continue;
            } else {
                // Полное внутреннее отражение
                ray.origin = hit.position + faceNormal * 0.005;
                ray.direction = normalize(reflect(ray.direction, faceNormal));
                continue;
            }
        }
        
        // Диффузный отскок
        vec3 randomDir = randomCosineHemisphere(hit.normal);
        ray.origin = hit.position + hit.normal * 0.001;
        ray.direction = randomDir;
        throughput *= hit.material.albedo * 0.5;
        
        if (max(throughput.x, max(throughput.y, throughput.z)) < 0.01) break;
    }
    
    return clamp(color, 0.0, 1.0);
}
// ===== Генерация луча камеры =====
Ray generateCameraRay(vec2 uv) {
    vec3 forward = normalize(u_cameraTarget - u_cameraPos);
    vec3 right = normalize(cross(forward, u_cameraUp));
    vec3 up = cross(right, forward);
    
    Ray ray;
    
    if (u_projectionType == 0) {
        // Perspective
        float aspectRatio = u_resolution.x / u_resolution.y;
        float scale = tan(radians(u_fov * 0.5));
        
        vec3 direction = normalize(
            forward +
            right * ((uv.x * 2.0 - 1.0) * aspectRatio * scale) +
            up * ((uv.y * 2.0 - 1.0) * scale)
        );
        
        ray.origin = u_cameraPos;
        ray.direction = direction;
    } else {
        // Orthographic
        float orthoScale = 2.0;
        vec3 origin = u_cameraPos +
            right * ((uv.x * 2.0 - 1.0) * orthoScale) +
            up * ((uv.y * 2.0 - 1.0) * orthoScale);
        
        ray.origin = origin;
        ray.direction = forward;
    }
    
    return ray;
}

// ===== Main =====
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
