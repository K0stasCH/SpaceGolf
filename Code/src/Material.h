#pragma once

#include <glm/glm.hpp>

struct material {
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;
};