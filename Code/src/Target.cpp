#include "Target.h"

Target::Target() {
    model = new Drawable("../resources/objs/quad.obj");
    position = glm::vec3(-20, 1, 3);
    scale = 1;
    rollX = 180.0f;
    rollY = 50.0f;
    rollZ = 0.0f;

    mtl = {
        vec3(0.1, 0.1, 0.1),
        vec3(0.64, 0.64, 0.64),
        vec3(0.0079, 0.0079, 0.0079),
        13.7
    };

    loadTexture();
}

void Target::update(float dt) {
}

void Target::loadTexture() {
    std::string path = this->texturePath.c_str();
    std::string extension = path.substr(path.size() - 3);

    if (extension == "bmp")
        texture = loadBMP(path.c_str());
    else if (extension == "png")
        texture = loadSOIL(path.c_str());
    else
        std::cout << "Wrong format of target texture" << std::endl;
}

void Target::render() {
    this->model->bind();

    mat4 rotationMatrix_X = glm::rotate(glm::mat4(1.0f), glm::radians(rollX), vec3(1.0f, 0, 0));
    mat4 rotationMatrix_Y = glm::rotate(glm::mat4(1.0f), glm::radians(rollY), vec3(0, 1.0f, 0));
    mat4 rotationMatrix_Z = glm::rotate(glm::mat4(1.0f), glm::radians(rollZ), vec3(0, 0, 1.0f));
    mat4 rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    mat4 translationMatrix = glm::translate(mat4(1.0f), this->position);
    mat4 scaleMatrix = glm::scale(mat4(1.0f), scale * vec3(1, 1, 1));

    this->modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    model->draw();
    glEnable(GL_CULL_FACE);
}

float Target::calculateDist(glm::vec3 pos) {
    glm::vec4 diagonalPoint1 = modelMatrix * vec4(model->vertices[0], 1.0f);
    glm::vec4 diagonalPoint2 = modelMatrix * vec4(model->vertices[2], 1.0f);

    glm::vec3 center = (vec3(diagonalPoint1) + vec3(diagonalPoint2)) * 0.5f;

    return glm::length(pos-center);
}
