#pragma once

#include <glm/glm.hpp>
#include <glfw3.h>

class Movable {
public:
    GLFWwindow* window;

    double dist;
    // Initial position : on +dist
    glm::vec3 position;
    // Initial horizontal angle : toward -Z
    float horizontalAngle;
    // Initial vertical angle : none
    float verticalAngle;

    glm::vec3 direction, up, right;

    float speed; // units / second

    // when is true the user can move the instance with the keyboarad 
    //    w  -> ⓧ    s  -> ☉
    //    a  -> ←    d   -> →
    // alt+w -> ↑  alt+s -> ↓
    bool active;

public:
    Movable(GLFWwindow* window);
    //return a vec2 = (horizontalAngle, horizontalAngle)
    static glm::vec2 onMouseMove(double xPos, double yPos);
    void move();
};