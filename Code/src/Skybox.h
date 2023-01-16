#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>
#include "common/model.h"
#include "common/shader.h"
#include "common/texture.h"


class Skybox :public Drawable {
public:
	Skybox();
	void render();

private:
	void loadTexture();

private:
	std::string texturePath = "../resources/textures/Skybox/cubemap_3.png";
	GLuint cubemapTexture;
};

