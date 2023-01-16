#include "Planet.h"


Planet::Planet(int num_of_Planets) : IntCosmicBody(num_of_Planets) {
	generatePlanets();
	loadTexture();

	mtl = {
		vec3(0.1, 0.1, 0.1),
		vec3(1.0, 1.0, 1.0),
		vec3(0.0079, 0.0079, 0.0079),
		10
	};

	return;
}

//Change the number of planets
void Planet::update(float dt) {
	int actualPlanets = _attributes.size();
	if (actualPlanets < number_of_bodies) {
		this->changeBodiesNumber(number_of_bodies);
		for (int i = actualPlanets; i < number_of_bodies; i++) {
			createNewBody(i);
		}
	}
	else {
		this->changeBodiesNumber(number_of_bodies);
	}
	return;
}

void Planet::createNewBody(int index) {
	bodyAttributes& planet = _attributes[index];

	float x = -terrainLimit + RAND * (2 * terrainLimit),    //cords -10 -> 10
		  y = -terrainLimit + RAND * (2 * terrainLimit),
		  z = -terrainLimit + RAND * (2 * terrainLimit);
	planet.position = glm::vec3(x, y, z);
	planet.rot_axis = glm::normalize(vec3(RAND, RAND, RAND));
	planet.rot_angle = RAND * 360;          // 0 - 360
	planet.mass = 1 + RAND * (10 - 1); //1 - 10
}

void Planet::renderAll() {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureDiffuse);

	IntCosmicBody::render();
}

void Planet::loadTexture(){
	std::string path = this->texturePath_diffuse.c_str();
	std::string extension = path.substr(path.size() - 3);

	if (extension == "bmp")
		textureDiffuse = loadBMP(path.c_str());
	else if (extension == "png")
		textureDiffuse = loadSOIL(path.c_str());
	else
		std::cout << "Wrong format of planets texture diffuse" << std::endl;
}

void Planet::generatePlanets() {
	srand(1);
	for (int i = 0; i < this->number_of_bodies; i++) {
		createNewBody(i);
	}
}
