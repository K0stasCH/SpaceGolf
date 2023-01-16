#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Light.h"
#include "Planet.h"
#include "Moon.h"
#include "Skybox.h"
#include "Throwable.h"
#include "Target.h"
#include "common/camera.h"
#include "common/Movable.h"
#include "common/shader.h"

#define W_WIDTH 1024
#define W_HEIGHT 768
#define SHADOWMAP_WIDTH 2048
#define SHADOWMAP_HEIGHT 2048
#define TITLE "SpaceGolf_1066615"

#define PLANETS_NUM 10

//This class manage the data that are uploaded in shaders
//and render the model with the right shader
class Scene{
public:
	~Scene();
	void mainRender();
	void update(float dt);
	void initialize();
	void createContext();
	void getMousePosition(double* xpos, double* ypos);

public:
	GLFWwindow* window;

	Light* myLight;
	Camera* myCamera;
	Planet* myPlanets;
	Moon* myMoons;
	Skybox* mySkybox;
	Throwable* myThrowable;
	Drawable* quad;
	Target* myTarget;

	GLuint skyboxShaderProgram;
	GLuint mainShaderProgram;
	GLuint depthShaderProgam; //used to render the quad of depth map

	bool stopSimulation = false;
	bool onlyTail = false; //render only the tail of comet. Used in shadowMap calculation

private:
	//void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	void uploadInfoSkybox(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix);

	//General data that needs all models to be rendered
	void uploadInfoMain(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix);
	
	void uploadInfoPlanets();
	void uploadInfoMoons();
	void uploadInfoThrowable();
	void uploadInfoLight_Plane();
	void uploadInfoLight_Source();

	//type = 0 -> upload material of the planets
	//type = 1 -> upload material of the moons
	//type = 2 -> upload material of the throwable
	//type = 3 -> upload material of the target
	void uploadMaterial(int type);
	
	void uploadLight();
	void uploadShadowMap();
	void uploadInfoTail();
	void uploadInfoTarget();
	void shadowMap_CreateContext();
	void shadowMapCalculation(bool o_nlyTail);
	void renderScene(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix, bool _onlyTail, bool depthPass);
	void renderDepthMap();

private:
	std::string shaderPath_Skybox[2] = {
		"../resources/shaders/skybox.vertexshader",
		"../resources/shaders/skybox.fragmentshader"
	};

	std::string shaderPath_Main[2] = {
		"../resources/shaders/main.vertexshader",
		"../resources/shaders/main.fragmentshader"
	};

	std::string shaderPath_DepthMap[2] = {
		"../resources/shaders/SimpleTexture.vertexshader",
		"../resources/shaders/SimpleTexture.fragmentshader"
	};

	GLuint shadowMapFBO, shadowMapTexture;
};

