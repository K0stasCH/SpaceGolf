#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "src/Throwable.h"
#include "src/Light.h"
#include "common/Movable.h"


class Camera :public Movable {
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    // Field of View
    float FoV;
    float maxFoV;
    float minFoV;
    float fovSpeed;

    float nearClippingPlane;
    float farClippingPlane;
   
    //what object the camera follows
    //0->none, 1->throwable, 2->light
    int status;  
    char* statusNames[3];

public:
    Camera(GLFWwindow* window);
    void update(const Throwable* _throwable, const Light* Light);

private:
    void followNone();
    void followThrowable(const Throwable* _throwable);
    void followLight(const Light* _light);
};