#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/model.h"
#include "common/shader.h"

#include <vector>

#include <iostream>
#include <algorithm>

#define USE_PARALLEL_TRANSFORM


#define G 6.674E-11

using namespace glm;

struct bodyAttributes {
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rot_axis = glm::vec3(0, 1, 0);
    float rot_angle = 0.0f; //degrees
    float mass = 0.0f;
    float life = 1.0f;
};


class IntCosmicBody {
public:
    IntCosmicBody(int number);
    virtual void update(float dt) = 0;

public:
    GLuint VAO;
    int number_of_bodies;
    std::vector<bodyAttributes> _attributes;

protected:
    void changeBodiesNumber(int new_number);
    void render(int time = 0);

private:
    void configureVAO();
    void bindAndUpdateBuffers();
    virtual void createNewBody(int index) = 0;

private:
    std::vector<glm::mat4> translations;
    std::vector<glm::mat4> rotations;
    std::vector<float> scales;
    std::vector<float> lifes;

    GLuint transformations_buffer;
    GLuint rotations_buffer;
    GLuint scales_buffer;
    GLuint lifes_buffer;

    Drawable* model;
    std::string modelPath = "../resources/objs/sphere.obj";
};