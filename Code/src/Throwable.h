#pragma once

#include <glm/glm.hpp>

#include "common/model.h"
#include "common/shader.h"
#include "common/Movable.h"
#include "src/Planet.h"
#include "src/Moon.h"
#include "src/ParticleTail.h"
#include "src/Material.h"

#define OBJPath "../resources/objs/rock.obj"

struct state {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rot_axis = glm::normalize(glm::vec3(1 - 2 * RAND, 1 - 2 * RAND, 1 - 2 * RAND));
	float rot_angle = 0;
};

class Throwable :public Drawable{
public:
	Throwable(Planet* _planets, Moon* _moons);
	void update(float dt);
	void renderThrowable();
	void renderTail();
	void reset();
	void throwObject();
	void setInitialVelocity(double MouseX, double MouseY);
	glm::vec3 Throwable::getInitialVelocity();

public:
	state _state;
	float mass;
	bool active;
	glm::mat4 modelMatrix;
	material mtl;
	float velocityMagnitude; //magnitude of initial velocity
	bool canMove;

	bool enabledParticleTail;
	ParticleTail* tail;

	int numOfTextures = 1;

private:
	glm::vec3 computeForce(Planet* _planets, Moon* _moons);
	void loadTexture();

private:
	Planet* planets_ptr; 
	Moon* moons_ptr;

	glm::vec3 initialVelocity;

	std::string texturePath = "../resources/textures/rock.png";
	GLuint texture;
};

