#include "camera.h"


using namespace glm;

Camera::Camera(GLFWwindow* window) : Movable(window), statusNames{ "None", "Thowable", "Light" } {
    FoV = 95.0f;
    maxFoV = 100.0f;
    minFoV = 50.0f;
    fovSpeed = 2.0f;

    nearClippingPlane = 0.1f;
    farClippingPlane = 250.0f;

    status = 0;
}

void Camera::update(const Throwable* _throwable, const Light* _light){
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        status = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        status = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        status = 2;
    }


    if (status == 0) {
        followNone();
       // previousPosition = position;
    }
    if (status == 1) followThrowable(_throwable);
    if (status == 2) followLight(_light);


    // Handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        FoV -= fovSpeed;
        if (FoV < minFoV)
            FoV = minFoV;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        FoV += fovSpeed;
        if (FoV > maxFoV)
            FoV = maxFoV;
    }

    // construct projection and view matrices
    projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, nearClippingPlane, farClippingPlane);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    return;
}

void Camera::followNone() {
    this->move();

    // Look from the bottom
    if ((glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)) {
        position = vec3(0, -dist, 0);
        up = vec3(0, 1, 0);
        direction = vec3(0, 1, 0);
        horizontalAngle = 3.14159f;
        verticalAngle = 0.5 * 3.14159f;
    }
    // Look from the left
    if ((glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)) {
        position = vec3(-dist, 0, 0);
        up = vec3(0, 1, 0);
        direction = vec3(1, 0, 0);
        horizontalAngle = 0.5 * 3.15149f;
        verticalAngle = 0;
    };
    // Look from the front
    if ((glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)) {
        position = vec3(0, 0, dist);
        up = vec3(0, 1, 0);
        direction = vec3(0, 0, -1);
        horizontalAngle = 3.15149f;
        verticalAngle = 0;
    }
    // Look from the right
    if ((glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)) {
        position = vec3(dist, 0, 0);
        up = vec3(0, 1, 0);
        direction = vec3(-1, 0, 0);
        horizontalAngle = -0.5 * 3.15149f;
        verticalAngle = 0;
    }
    // Look from the bottom
    if ((glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)) {
        position = vec3(0, dist, 0);
        up = vec3(0, 1, 0);
        direction = vec3(0, -1, 0);
        horizontalAngle = 3.14159f;
        verticalAngle = -0.5 * 3.14159f;
    }
}

void Camera::followThrowable(const Throwable* _throwable) {
    //direction = velocity of throwable, so we compute verticalAngle and horizontalAngle

    if (_throwable->canMove == true) {
        float r = length(_throwable->_state.velocity);
        verticalAngle = asin(_throwable->_state.velocity.y / r);
        horizontalAngle = atan2(_throwable->_state.velocity.x, _throwable->_state.velocity.z);
        //direction = normalize(_throwable->_state.velocity);

        direction = normalize(vec3(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        ));


        // Right vector
        right = normalize(vec3(
            -cos(horizontalAngle),
            0,
            sin(horizontalAngle)
        ));

        // Up vector
        up = normalize(cross(right, direction));
        //up = vec3(0, 1, 0);

        vec3 offset = 0.0f * up - 0.0f * normalize(_throwable->_state.velocity);
        position = _throwable->_state.position + offset;
    }
    else {
        this->active = true;
        followNone();
        this->position = _throwable->_state.position;
    }   
}

void Camera::followLight(const Light* _light) {
    verticalAngle = _light->verticalAngle;
    horizontalAngle = _light->horizontalAngle;
    position = _light->position;
    direction = _light->direction;
    right = _light->right;
    up = _light->up;

    vec3 offset = 1.0f * (up)-5.0f * normalize(_light->direction);
    position = _light->position + offset;
}
