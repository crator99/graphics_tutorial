#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

using namespace std;

const int screenWidth = 512;
const int screenHeight = 512;

unsigned char* Image = new unsigned char[screenWidth * screenHeight * 3];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Sphere{
public:
    vec3 o;
    double r;
};

class Plane{
public:
    vec3 p;
    vec3 n;
};

class Light{
public:
    vec3 o;
};

class EyePoint{
public:
    double l;
    double r;
    double b;
    double t;
    double d;
};

class Camera{
public:
    vec3 e;
    vec3 u;
    vec3 v;
    vec3 w;
    
    ray get_ray(int ix, int iy, EyePoint e, double random_number){
        ray get_ray = {vec3(0, 0, 0), vec3((e.l + (e.r - e.l) * (ix + random_number) / screenWidth), (e.b + (e.t - e.b) * (iy + random_number) / screenHeight), -e.d)};

        return get_ray;
    }
};

class k_ads{
public:
    vec3 a;
    vec3 d;
    vec3 s;
    double s_p;
};

bool intersect(ray r, Sphere s){
    vec3 oc = r.origin() - s.o;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - s.r * s.r;
    auto discriminant = b * b - 4 * a * c;
    double t2 = (-b - sqrt(b * b  - 4 * a * c)) / (2 * a);

    bool T_F;

    if(discriminant >= 0){
        if(t2 > 0)
            T_F = true;
    }
    else{
        T_F = false;
    }

    return T_F;
}

bool intersect_p(ray r, Plane p){
    bool T_P = false;
    if (dot(r.direction(), p.n) < 0){
        double T = dot((p.p - r.origin()), p.n) / dot(r.direction(), p.n);
        if (T > 0){
            T_P = true;
        }
    }
    return T_P;
}

double cal_t(ray r, Sphere s){
    vec3 oc = r.origin() - s.o;
    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - s.r * s.r;
    double t1 = (-b + sqrt(b * b  - 4 * a * c)) / (2 * a);
    double t2 = (-b - sqrt(b * b  - 4 * a * c)) / (2 * a);
    return t2;
}

double cal_tp(ray r, Plane p){
    double T = dot((p.p - r.origin()), p.n) / dot(r.direction(), p.n);
    return T;
}

vec3 point(ray r, double t){
    vec3 p = r.origin() + t * r.direction();
    return p;
}

vec3 cal_L(k_ads k, int I, vec3 n, vec3 l, ray r){
    vec3 La = k.a * I;
    vec3 Ld = k.d * I * max(0.0, dot(n, l));
    vec3 v = (-r.direction()) / (-r.direction()).length();
    vec3 Ls = k.s * I * pow(max(0.0, dot(n, (v + l) / (v + l).length())), k.s_p);
    vec3 L = La + Ld + Ls;
    return L;
}

vec3 cal_La(k_ads k, int I, vec3 n, vec3 l, ray r){
    vec3 La = k.a * I;
    return La;
}

int main(int argc, char* argv[])
{
    Sphere sphere1 = {vec3(-4.0, 0.0, -7.0), 1.0};
    Sphere sphere2 = {vec3(0.0, 0.0, -7.0), 2.0};
    Sphere sphere3 = {vec3(4.0, 0.0, -7.0), 1.0};

    Sphere sphere_best;
    float t_best;
    int closest;

    Camera camera = {vec3(0.0, 0.0, 0.0),
                     vec3(1.0, 0.0, 0.0),
                     vec3(0.0, 1.0, 0.0),
                     vec3(0.0, 0.0, 1.0)};

    k_ads kp = {vec3(0.2, 0.2, 0.2),
                vec3(1, 1, 1),
                vec3(0, 0, 0),
                0};

    k_ads k1 = {vec3(0.2, 0, 0),
                vec3(1, 0, 0),
                vec3(0, 0, 0),
                0};

    k_ads k2 = {vec3(0, 0.2, 0),
                vec3(0, 0.5, 0),
                vec3(0.5, 0.5, 0.5),
                32};

    k_ads k3 = {vec3(0, 0, 0.2),
                vec3(0, 0, 1),
                vec3(0, 0, 0),
                0};

    Plane plane = {vec3(0.0, -2.0, 0.0),
                   vec3(0.0, 1.0, 0.0)};

    vec3 light = {vec3(-4.0, 4.0, -3.0)};

    EyePoint eye_point = {-0.1, 0.1, -0.1, 0.1, 0.1};

    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Antialiasing", NULL, NULL);

    if(!window){
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }
    vec3 pixel_color = vec3(0,0,0);
    float gamma = 2.2f;

    for (int j = 0; j < screenHeight; j++){
        for (int i = 0; i < screenWidth; i++){
            int pixel_index = (j * screenWidth + i) * 3;

            for (int N = 0; N < 64; N++){
                random_device rd;
                mt19937 gen(rd());
                uniform_real_distribution<double> dis(0.0, 1.0);
                double random_number = dis(gen);
                ray r = camera.get_ray(i, j, eye_point, random_number);

                bool hit_sphere1 = intersect(r, sphere1);
                bool hit_sphere2 = intersect(r, sphere2);
                bool hit_sphere3 = intersect(r, sphere3);
                bool hit_plane = intersect_p(r, plane);
                bool check = false;
                double t1 = INFINITY, t2 = INFINITY, t3 = INFINITY, t_p = INFINITY;

                if (hit_sphere1 == true){
                    t1 = cal_t(r, sphere1);
                    check = true;
                }
                if (hit_sphere2 == true){
                    t2 = cal_t(r, sphere2);
                    check = true;
                }
                if (hit_sphere3 == true){
                    t3 = cal_t(r, sphere3);
                    check = true;
                }
                if (hit_plane == true){
                    t_p = cal_tp(r, plane);
                    check = true;
                }
                if (check == false){
                    closest = 0;
                }
                else{
                    t_best = INFINITY;

                    if (t1 < t_best){
                        t_best = t1;
                        closest = 1;
                    }
                    if (t2 < t_best){
                        t_best = t2;
                        closest = 2;
                    }
                    if (t3 < t_best){
                        t_best = t3;
                        closest = 3;
                    }
                    if (t_p < t_best){
                        t_best = t_p;
                        closest = 4;
                    }
                }

                vec3 point1 = point(r, t1);
                vec3 point2 = point(r, t2);
                vec3 point3 = point(r, t3);
                vec3 point_tp = point(r, t_p);

                vec3 point_best = point(r, t_best);

                
                if (closest != 0){
                    if (closest == 1){
                        vec3 v = (-r.direction()) / (-r.direction()).length();
                        vec3 l = (light - point1) / (light - point1).length();
                        vec3 n = (point1 - sphere1.o) / (point1 - sphere1.o).length();

                        vec3 l1 = cal_L(k1, 1, n, l, r);
                        vec3 l1a = cal_La(k1, 1, n, l, r);

                        ray shadRay = {point1, light - point1};
                        bool hit_shad = false;
                        if (intersect(shadRay, sphere1)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere2)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere3)){
                            hit_shad = true;
                        }
                        if (intersect_p(shadRay, plane)){
                            hit_shad = true;
                        }

                        if (hit_shad == true){
                            pixel_color += vec3(l1a.x(), l1a.y(), l1a.z());
                        }
                        else{
                            pixel_color += vec3(l1.x(), l1.y(), l1.z());
                        }
                    }
                    else if (closest == 2){
                        vec3 v = (-r.direction()) / (-r.direction()).length();
                        vec3 l = (light - point2) / (light - point2).length();
                        vec3 n = (point2 - sphere2.o) / (point2 - sphere2.o).length();

                        vec3 l2 = cal_L(k2, 1, n, l, r);
                        vec3 l2a = cal_La(k2, 1, n, l, r);

                        ray shadRay = {point2, light - point2};
                        bool hit_shad = false;
                        if (intersect(shadRay, sphere1)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere2)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere3)){
                            hit_shad = true;
                        }
                        if (intersect_p(shadRay, plane)){
                            hit_shad = true;
                        }

                        if (hit_shad == true){
                            pixel_color += vec3(l2a.x(), l2a.y(), l2a.z());
                        }
                        else{
                            pixel_color += vec3(l2.x(), l2.y(), l2.z());
                        }
                    }
                    else if (closest == 3){
                        vec3 v = (-r.direction()) / (-r.direction()).length();
                        vec3 l = (light - point3) / (light - point3).length();
                        vec3 n = (point3 - sphere3.o) / (point3 - sphere3.o).length();

                        vec3 l3 = cal_L(k3, 1, n, l, r);
                        vec3 l3a = cal_La(k3, 1, n, l, r);

                        ray shadRay = {point3, light - point3};
                        bool hit_shad = false;
                        if (intersect(shadRay, sphere1)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere2)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere3)){
                            hit_shad = true;
                        }
                        if (intersect_p(shadRay, plane)){
                            hit_shad = true;
                        }

                        if (hit_shad == true){
                            pixel_color += vec3(l3a.x(), l3a.y(), l3a.z());
                        }
                        else{
                            pixel_color += vec3(l3.x(), l3.y(), l3.z());
                        }
                    }
                    else if (closest == 4){
                        vec3 v = (-r.direction()) / (-r.direction()).length();
                        vec3 l = (light - point_tp) / (light - point_tp).length();
                        vec3 n = plane.n;

                        vec3 l_p = cal_L(kp, 1, n, l, r);
                        vec3 l_pa = cal_La(kp, 1, n, l, r);

                        ray shadRay = {point_tp, light - point_tp};
                        bool hit_shad = false;
                        if (intersect(shadRay, sphere1)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere2)){
                            hit_shad = true;
                        }
                        if (intersect(shadRay, sphere3)){
                            hit_shad = true;
                        }
                        if (intersect_p(shadRay, plane)){
                            hit_shad = true;
                        }

                        if (hit_shad == true){
                            pixel_color += vec3(l_pa.x(), l_pa.y(), l_pa.z());
                        }
                        else{
                            pixel_color += vec3(l_p.x(), l_p.y(), l_p.z());
                        }
                    }
                }
                else{
                    pixel_color += vec3(0, 0, 0);
                }
            }
            pixel_color /= 64;

            Image[pixel_index] = pow(pixel_color.x() > 1 ? 1 : pixel_color.x(), 1/gamma) * 255;
            Image[pixel_index + 1] = pow(pixel_color.y() > 1 ? 1 : pixel_color.y(), 1/gamma) * 255;
            Image[pixel_index + 2] = pow(pixel_color.z() > 1 ? 1 : pixel_color.z(), 1/gamma) * 255;
        }
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glad_glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, Image);

        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    glfwTerminate();
    return 0;
}