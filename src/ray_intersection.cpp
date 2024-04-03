#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>
#include <glad/glad.h>
#include <glad/glut.h>
#include <GLFW/glfw3.h>

using namespace std; 

const int screenWidth = 512;
const int screenHeight = 512;

unsigned char* Image = new unsigned char[screenWidth * screenHeight * 3];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool hit_sphere(const point3& center, double radius, const ray& r){
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    return(discriminant >= 0);
}

color ray_color(const ray& r){
    if (hit_sphere(point3(0, 0, -1), 0.5, r)){
        return color(1, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    color white = color(1.0, 1.0, 1.0);
    color black = color(0.0, 0.0, 0.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
};

class Sphere{
    public:
        point3 o;
        double r;
};

class Plane{
    public:
        vec3 p;
        vec3 n;
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
        ray get_ray(int ix, int iy, EyePoint e){
            ray get_ray = {vec3(0, 0, 0), vec3((e.l + (e.r - e.l) * (ix + 0.5) / screenWidth), (e.b + (e.t - e.b) * (iy + 0.5) / screenHeight), -e.d)};

            return get_ray;
        }
};

bool intersect(ray r, Sphere s){
    vec3 oc = r.origin() - s.o;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - s.r * s.r;
    auto discriminant = b * b - 4 * a * c;
    
    bool T_F;

    if(discriminant >= 0){
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

int main(int argc, char* argv[])
{
    Sphere sphere1 = {vec3(-4.0, 0.0, -7.0), 1.0};
    Sphere sphere2 = {vec3(0.0, 0.0, -7.0), 2.0};
    Sphere sphere3 = {vec3(4.0, 0.0, -7.0), 1.0};

    Plane plane = {vec3(0.0, -2.0, 0.0),
                vec3(0.0, 1.0, 0.0)};

    Camera camera = {vec3(0.0, 0.0, 0.0),
                    vec3(1.0, 0.0, 0.0),
                    vec3(0.0, 1.0, 0.0),
                    vec3(0.0, 0.0, 1.0)};

    EyePoint eye_point = {-0.1, 0.1, -0.1, 0.1, 0.1};

    glfwInit();


    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Ray Intersection", NULL, NULL);
    
    if(!window){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    for (int j = 0; j < screenHeight; j++){
        for (int i = 0; i < screenWidth; i++){
            ray r = camera.get_ray(i, j, eye_point);

            bool hit_sphere1 = intersect(r, sphere1);
            bool hit_sphere2 = intersect(r, sphere2);
            bool hit_sphere3 = intersect(r, sphere3);
            bool hit_plane = intersect_p(r, plane);

            int pixel_index = (j * screenWidth + i) * 3;

            if (hit_sphere1 == true){
                Image[pixel_index] = 255;
                Image[pixel_index + 1] = 255;
                Image[pixel_index + 2] = 255;
            }
            else if (hit_sphere2 == true){
                Image[pixel_index] = 255;
                Image[pixel_index + 1] = 255;
                Image[pixel_index + 2] = 255;
            }
            else if (hit_sphere3 == true){
                Image[pixel_index] = 255;
                Image[pixel_index + 1] = 255;
                Image[pixel_index + 2] = 255;
            }
            else if (hit_plane == true){
                Image[pixel_index] = 255;
                Image[pixel_index + 1] = 255;
                Image[pixel_index + 2] = 255;
            }
            else{
                Image[pixel_index] = 0;
                Image[pixel_index + 1] = 0;
                Image[pixel_index + 2] = 0;
            }
        
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