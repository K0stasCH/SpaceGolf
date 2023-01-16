#include "ParticleTail.h"

#include <iostream>

ParticleTail::ParticleTail(Drawable* _model, int number) : IntParticleEmitter(_model, number) {
    loadTexture();
}

void ParticleTail::updateParticles(float dt) {
    if (active == false) return;

    //This is for the fountain to slowly increase the number of its particles to the max amount
    //instead of shooting all the particles at once
    if (active_particles < number_of_particles) {
        int batch = 10 * length(emitter_vel);
        int limit = std::min(number_of_particles - active_particles, batch);
        for (int i = 0; i < limit; i++) {
            createNewParticle(active_particles);
            active_particles++;
        }
    }
    else {
        active_particles = number_of_particles; //In case we resized our ermitter to a smaller particle number
    }

    for(int i = 0; i < active_particles; i++){
        particleAttributes & particle = p_attributes[i];

        if(particle.life < 0.0f){
            createNewParticle(i);
        }

        particle.rot_angle += 90 * dt;
        particle.position = particle.position + particle.velocity * dt;

        particle.life = 1 - (length(particle.position - this->emitter_pos) / (dist_threshold + RAND));
    }
}

void ParticleTail::createNewParticle(int index){
    particleAttributes & particle = p_attributes[index];

    particle.position = emitter_pos + glm::vec3(1 - 2 * RAND, 1 - 2 * RAND, 1 - 2 * RAND);
    particle.velocity = 0.5f * vec3(RAND * emitter_vel.x, RAND * emitter_vel.y, RAND * emitter_vel.z);

    particle.scale = 0.01 * RAND + 0.05f;
    particle.rot_axis = glm::normalize(glm::vec3(1 - 2*RAND, 1 - 2*RAND, 1 - 2*RAND));
    particle.rot_angle = RAND*360;
    particle.life = 1.0f; //mark it alive
}

void ParticleTail::loadTexture() {
    std::string path = this->texturePath.c_str();
    std::string extension = path.substr(path.size() - 3);

    if (extension == "bmp")
        texture = loadBMP(path.c_str());
    else if (extension == "png")
        texture = loadSOIL(path.c_str());
    else
        std::cout << "Wrong format of tail particles texture" << std::endl;
}

void ParticleTail::renderAll() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    IntParticleEmitter::renderParticles();
}
