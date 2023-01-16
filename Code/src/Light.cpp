#include "Light.h"

using namespace glm;

Light::Light(GLFWwindow* window) : Movable(window) {
    color = vec3(1.0f, 1.0f, 1.0f);
    ambient = 0.2f;
    power = 1.0f;

    // setting near and far plane affects the detail of the shadow
    nearPlane = 0.01;
    farPlane = 50.0;

    projectionMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    //orthoProj = true;

    plane = new Drawable("../resources/objs/quad.obj");
    model = new Drawable("../resources/objs/sphere.obj");
    for (int i = 0; i < model->normals.size(); i++) {
        model->normals[i] = -model->normals[i];
    }
    model = new Drawable(model->vertices, model->uvs, model->normals);


    sphericalTrajectory = false;
    posR = 1;
    posVerticalAngle = 3.14159f / 2.0f;
    posHorizontalAngle = 0;

    showPlane = false;
    planeScale = 1;
    planeOffset = 2;

    loadTexture();
}

void Light::loadTexture() {
    std::string path = this->texturePath.c_str();
    std::string extension = path.substr(path.size() - 3);

    if (extension == "bmp")
        texture = loadBMP(path.c_str());
    else if (extension == "png")
        texture = loadSOIL(path.c_str());
    else
        std::cout << "Wrong format of planets texture" << std::endl;
}


void Light::update() {
    this->move();
    if (sphericalTrajectory == true) {
        this->moveOnSphere();
    }
    projectionMatrix = ortho(-planeScale, planeScale, -planeScale, planeScale, nearPlane, farPlane);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
}

mat4 Light::lightVP() {
    return projectionMatrix * viewMatrix;
}

void Light::renderPlane() {
    if (showPlane == false ) return;

    glDisable(GL_CULL_FACE);

    this->plane->bind();

    glm::mat4 modelMatrixScale = scale(mat4(1.0f), vec3(planeScale));
    glm::mat4 modelMatrixTranslation = translate(mat4(1.0f), this->position + this->direction * planeOffset);
    glm::mat4 modelMatrixRotationHorizontal = glm::rotate(mat4(1.0f), horizontalAngle, up);
    glm::mat4 modelMatrixRotationVertical = glm::rotate(mat4(1.0f), verticalAngle, right);

    modelMatrixPlane = modelMatrixTranslation * modelMatrixRotationHorizontal * modelMatrixRotationVertical * modelMatrixScale;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    this->plane->draw();
    glEnable(GL_CULL_FACE);
}

void Light::renderSource() {
    this->model->bind();

    glm::mat4 modelMatrixTranslation = translate(mat4(1.0f), this->position);

    glm::mat4 modelMatrixRotationHorizontal = glm::rotate(mat4(1.0f), horizontalAngle, up);
    glm::mat4 modelMatrixRotationVertical = glm::rotate(mat4(1.0f), verticalAngle, right);

    modelMatrixSource = modelMatrixTranslation * modelMatrixRotationHorizontal * modelMatrixRotationVertical;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    this->model->draw();

    return;
}

void Light::moveOnSphere() {
    //increase horizontal angle
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        posVerticalAngle -= 0.0001 * speed;
    }
    //dencrease horizontal angle
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        posVerticalAngle += 0.0001 * speed;
    }
    //increase vertical angle
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        posHorizontalAngle += 0.0001 * speed;
    }
    //dencrease vertical angle
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        posHorizontalAngle -= 0.0001 * speed;
    }

    position.x = posR * sin(posVerticalAngle) * sin(posHorizontalAngle);
    position.y = posR * cos(posVerticalAngle);
    position.z = posR * sin(posVerticalAngle) * cos(posHorizontalAngle);

    return;
}