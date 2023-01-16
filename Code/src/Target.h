#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <GL/glew.h>

#include <iostream>

#include "common/model.h"
#include "common/texture.h"
#include "src/Material.h"

using namespace glm;

class Target{
public:
	Target();
	void update(float dt);
	void render();
	float calculateDist(glm::vec3 pos);

public:
	Drawable* model;
	glm::vec3 position;
	float scale;
	float rollX, rollY, rollZ;

	float mass;
	bool active;
	glm::mat4 modelMatrix;
	material mtl;

private:
	void loadTexture();

private:
	std::string texturePath = "../resources/textures/target.png";
	GLuint texture;
};

