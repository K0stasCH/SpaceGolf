#pragma once


#include <stdlib.h>

#include "common/model.h"
#include "common/texture.h"
#include "IntCosmicBody.h"
#include "Planet.h"
#include "Material.h"

#define RAND ((float) rand()) / (float) RAND_MAX


using namespace std;
using namespace glm;


class Moon :public IntCosmicBody {
public:
	Moon(Planet* _planets, float density);
	void update(float dt = 0) override;
	void createNewBody(int index) override;
	void renderAll();
	void generateMoons();

public:
	float densityMoons; //float 0-1, show how many planets have moons
	int actualMoons;
	material mtl;

	int numOfTextures = 3;

private:
	void loadTexture();

private:
	Planet* _planets_ptr;
	std::string texturePath = "../resources/textures/moonsAtlas.png";
	GLuint texture;

};