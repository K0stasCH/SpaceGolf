#include "Movable.h"

using namespace glm;

Movable::Movable(GLFWwindow* window) : window(window) {
    dist = 10;
    position = glm::vec3(0, 0, dist);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;

    
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    right = vec3(
        -cos(horizontalAngle),
        0,
        sin(horizontalAngle)
    );

    up = cross(right, direction);

    speed = 50.0f;

    active = false;
}

void Movable::move() {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    if (active == false) {
        lastTime = currentTime;
        return;
    }
    deltaTime = 0.001;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    vec2 angles = this->onMouseMove(xpos, ypos);
    this->horizontalAngle = angles.x;
    this->verticalAngle = angles.y;

    // Right and up vectors of the camera coordinate system use spherical coordinates
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    right = vec3(
        -cos(horizontalAngle),
        0,
        sin(horizontalAngle)
    );

    // Up vector
    up = cross(right, direction);

    // Move forward
    if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)) {
        position += direction * speed * deltaTime;
    }
    // Move backward
    if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)) {
        position -= direction * speed * deltaTime;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * speed * deltaTime;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * speed * deltaTime;
    }

    // Move down
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)) {
        position -= up * speed * deltaTime;
    }
    // Move up
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)) {
        position += up * speed * deltaTime;
    }

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

vec2 Movable::onMouseMove(double xPos, double yPos) {
    static double lastxPos = xPos;
    static double lastyPos = yPos;

    static float horizontalAngleStatic = 0;
    static float verticalAngleStatic = 0;

    float mouseSpeed = 0.001f;

    horizontalAngleStatic += mouseSpeed * (lastxPos - xPos);
    verticalAngleStatic += mouseSpeed * (lastyPos - yPos);

    float horizontalAngle = horizontalAngleStatic;

    lastxPos = xPos;
    lastyPos = yPos;

    return vec2(horizontalAngleStatic, verticalAngleStatic);
}