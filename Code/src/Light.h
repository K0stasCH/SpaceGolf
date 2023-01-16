#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <math.h>

#include "common/Movable.h"
#include "common/model.h"
#include "common/shader.h"
#include "common/texture.h"


class Light:public Movable {
public:
    Light(GLFWwindow* window);
    void update();
    void renderPlane();
    void renderSource();
    glm::mat4 lightVP();

public:
    Drawable* model;
    Drawable* plane;

    // Light parameters
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrixPlane;
    glm::mat4 modelMatrixSource;

    //glm::vec3 lightPosition_worldspace;

    glm::vec3 color;
    float ambient;
    float power;

    float nearPlane;
    float farPlane;

    //bool orthoProj;

    bool showPlane;
    float planeOffset;
    float planeScale;

    // Where the light will look at
    //glm::vec3 targetPosition;

    bool sphericalTrajectory;
    float posHorizontalAngle, posVerticalAngle, posR; //coordinates when we move on a sphere

private:
    void loadTexture();
    //Move on the surface of a sphere
    void moveOnSphere();

private:
    std::string texturePath = "../resources/textures/white.png";
    GLuint texture;
};
