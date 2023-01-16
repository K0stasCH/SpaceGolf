#pragma once

#include <iostream>
#include <algorithm>

#include "common/texture.h"
#include "src/IntParticleEmitter.h"

using namespace glm;

class ParticleTail : public IntParticleEmitter {
public:
    ParticleTail(Drawable* _model, int number);
    //bool checkForCollision(particleAttributes& particle);
    void createNewParticle(int index) override;
    void updateParticles(float dt) override;
    void renderAll();

public:
    bool active = false;
    int active_particles = 0; //number of particles that have been instantiated
    float dist_threshold = 10.0f; //data member for collision checking

private:
    void loadTexture();

private:
    std::string texturePath = "../resources/textures/rock.png";
    GLuint texture;

};