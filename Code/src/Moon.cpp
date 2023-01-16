#include "Moon.h"


Moon::Moon(Planet* _planets, float density) : IntCosmicBody( floor(_planets->number_of_bodies * density) ) {
	this->_planets_ptr = _planets;
	this->densityMoons = density;
	mtl = {
		vec3(0.1, 0.1, 0.1),
		vec3(1.0, 1.0, 1.0),
		vec3(0.0079, 0.0079, 0.0079),
		10
	};

	generateMoons();
	loadTexture();
	return;
}

void Moon::update(float dt) {
	for (int i = 0; i < number_of_bodies; i++) {
		_attributes[i].rot_angle += 10*dt;
	}
	return;
}

//the plantes that have moons are the 0->actualMoons. 
//If actualMoons=5, the five first planets have moons.
void Moon::createNewBody(int index) {
	const bodyAttributes planetAttributes = _planets_ptr->_attributes[index];

	bodyAttributes& moon = _attributes[index];

	vec3 offset = normalize(cross(vec3(planetAttributes.position.y, -planetAttributes.position.x, 0), planetAttributes.position));
	moon.position = planetAttributes.position + 2.5f * offset * planetAttributes.mass;

	moon.rot_axis = normalize(planetAttributes.position);
	moon.rot_angle = RAND * 360;          // 0 - 360
	moon.mass = (0.1 + RAND * (0.2 - 0.1)) * planetAttributes.mass; //10%-20% mass of planet
}

void Moon::renderAll() {
	//glUseProgram(cosmicBodyShader);
	//glm::mat4 PV = (*_projectionMatrix) * (*_viewMatrix);

	//glUniformMatrix4fv(glGetUniformLocation(cosmicBodyShader, "P"), 1, GL_FALSE, &(*_projectionMatrix)[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(cosmicBodyShader, "V"), 1, GL_FALSE, &(*_viewMatrix)[0][0]);
	//glUniform1i(glGetUniformLocation(cosmicBodyShader, "textureSampler"), 0);
	//glUniform1f(glGetUniformLocation(cosmicBodyShader, "time"), (float)glfwGetTime() / 20.0);
	//glUniform1f(glGetUniformLocation(cosmicBodyShader, "num_of_bodies"), number_of_bodies);
	//glUniform1i(glGetUniformLocation(cosmicBodyShader, "moonsEnabled"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	IntCosmicBody::render();
}

void Moon::loadTexture() {
	std::string path = this->texturePath.c_str();
	std::string extension = path.substr(path.size() - 3);

	if (extension == "bmp")
		texture = loadBMP(path.c_str());
	else if (extension == "png")
		texture = loadSOIL(path.c_str());
	else
		std::cout << "Wrong format of moons texture" << std::endl;
}

void Moon::generateMoons() {
	this->actualMoons = floor(_planets_ptr->number_of_bodies * this->densityMoons);
	this->changeBodiesNumber(actualMoons);
	srand(1);
	for (int i = 0; i < this->actualMoons; i++) {
		createNewBody(i);
	}
}
