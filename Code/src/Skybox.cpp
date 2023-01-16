#include "Skybox.h"


Skybox::Skybox():Drawable("../resources/objs/skybox.obj") {
	loadTexture();
}

void Skybox::render() {
	this->bind();

	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubemapTexture);

	this->Drawable::draw();

	glDepthFunc(GL_LESS);
	return;
}

void Skybox::loadTexture() {
	std::string path = this->texturePath.c_str();
	std::string extension = path.substr(path.size() - 3);

	if (extension == "bmp")
		cubemapTexture = loadBMP(path.c_str());
	else if (extension == "png")
		cubemapTexture = loadSOIL(path.c_str());
	else
		std::cout << "Wrong format of skybox texture" << std::endl;
}