#pragma once

#include <stdlib.h>  

#include "common/model.h"
#include "common/texture.h"
#include "Material.h"
#include "IntCosmicBody.h"

#define RAND ((float) rand()) / (float) RAND_MAX


using namespace std;
using namespace glm;


class Planet :public IntCosmicBody {
public :
	Planet(int num_of_Planets);
	void update(float dt=0) override;
	void createNewBody(int index) override;
	void renderAll();
	void generatePlanets();

public:
	float terrainLimit = 100;
	material mtl;

	int numOfTextures = 3;
	
private:
	void loadTexture();

private:
	std::string texturePath_diffuse = "../resources/textures/planetsAtlas.png";
	GLuint textureDiffuse;
};