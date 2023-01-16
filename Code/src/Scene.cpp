#include <Scene.h>


Scene::~Scene() {
    mySkybox->~Skybox();
    myPlanets->~Planet();
    myMoons->~Moon();
    myThrowable->~Throwable();
    myTarget->~Target();
}

void Scene::initialize() {
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        Movable::onMouseMove(xpos, ypos);
        }
    );
}

void Scene::createContext() {

    skyboxShaderProgram = loadShaders(shaderPath_Skybox[0].c_str(), shaderPath_Skybox[1].c_str());
    mainShaderProgram = loadShaders(shaderPath_Main[0].c_str(), shaderPath_Main[1].c_str());
    depthShaderProgam = loadShaders(shaderPath_DepthMap[0].c_str(), shaderPath_DepthMap[1].c_str());

    myCamera = new Camera(window);
    myCamera->active = true;

    mySkybox = new Skybox();
    myPlanets = new Planet(PLANETS_NUM);
    myMoons = new Moon(myPlanets, 1);
    myThrowable = new Throwable(myPlanets, myMoons);
    myLight = new Light(window);
    myTarget = new Target();

    shadowMap_CreateContext();

    vector<vec3> quadVertices = {
        vec3(0.5, 0.5, 0.0),
        vec3(1.0, 0.5, 0.0),
        vec3(1.0, 1.0, 0.0),
        vec3(1.0, 1.0, 0.0),
        vec3(0.5, 1.0, 0.0),
        vec3(0.5, 0.5, 0.0)
    };

    vector<vec2> quadUVs = {
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0),
        vec2(1.0, 1.0),
        vec2(0.0, 1.0),
        vec2(0.0, 0.0)
    };

    quad = new Drawable(quadVertices, quadUVs);
}

void Scene::update(float dt) {
    myCamera->update(myThrowable, myLight);
    myLight->update();

    if (stopSimulation == true) return;

    myThrowable->update(dt);
    myThrowable->tail->updateParticles(dt);
    myMoons->update(dt);
}

void Scene::uploadInfoSkybox(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->skyboxShaderProgram, "view"), 1, GL_FALSE, &(*_viewMatrix)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->skyboxShaderProgram, "projection"), 1, GL_FALSE, &(*_projectionMatrix)[0][0]);
    glUniform1i(glGetUniformLocation(this->skyboxShaderProgram, "textureSampler"), 0);
}


void Scene::uploadInfoMain(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "P"), 1, GL_FALSE, &(*_projectionMatrix)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "V"), 1, GL_FALSE, &(*_viewMatrix)[0][0]);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "textureSampler"), 0);
    glUniform1f(glGetUniformLocation(this->mainShaderProgram, "time"), (float)glfwGetTime() / 20.0);
    uploadLight();
    uploadShadowMap();
}

void Scene::uploadLight() {
    Light* light = this->myLight;
    glUniform3f(glGetUniformLocation(this->mainShaderProgram, "light.color"), light->color.r, light->color.g, light->color.b);
    glUniform1f(glGetUniformLocation(this->mainShaderProgram, "light.ambient"), light->ambient);
    glUniform3fv(glGetUniformLocation(this->mainShaderProgram, "light.lightPosition_worldspace"), 1, &(light->position[0]));
    glUniform3fv(glGetUniformLocation(this->mainShaderProgram, "light.direction"), 1, &(light->direction[0]));
    glUniform1f(glGetUniformLocation(this->mainShaderProgram, "light.power"), light->power);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "light_VP"), 1, GL_FALSE, &(light->lightVP())[0][0]);
    
}

void Scene::uploadShadowMap() {
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "shadowMapSampler"), 1);
}

void Scene::uploadInfoPlanets() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 1);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), myPlanets->number_of_bodies);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(mat4(1.0))[0][0]);
    uploadMaterial(0);
}

void Scene::uploadInfoMoons() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 1);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 1);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), myMoons->number_of_bodies);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(mat4(1.0))[0][0]);
    uploadMaterial(1);
}

void Scene::uploadInfoThrowable() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), 1);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(myThrowable->modelMatrix)[0][0]);
    uploadMaterial(2);
}

void Scene::uploadInfoLight_Plane() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), 1);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(myLight->modelMatrixPlane)[0][0]);
}

void Scene::uploadInfoLight_Source() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), 1);
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(myLight->modelMatrixSource)[0][0]);
}

void Scene::uploadInfoTail() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 1);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), 1); //because we dont want to move the texure
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(mat4(1.0))[0][0]);
    uploadMaterial(2);
}

void Scene::uploadInfoTarget() {
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "enableInstacing"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "moonsEnabled"), 0);
    glUniform1i(glGetUniformLocation(this->mainShaderProgram, "num_of_bodies"), 1); //because we dont want to move the texure
    glUniformMatrix4fv(glGetUniformLocation(this->mainShaderProgram, "modelMat"), 1, GL_FALSE, &(myTarget->modelMatrix)[0][0]);
    uploadMaterial(3);
}

void Scene::uploadMaterial(int type) {
    material* _mtl;
    if (type == 0) _mtl = &(this->myPlanets->mtl);
    else if (type == 1) _mtl = &(this->myMoons->mtl);
    else if (type == 2) _mtl = &(this->myThrowable->mtl);
    else if (type == 3) _mtl = &(this->myTarget->mtl);
    glUniform3f(glGetUniformLocation(this->mainShaderProgram, "mtl.Ka"), _mtl->Ka.r, _mtl->Ka.g, _mtl->Ka.b);
    glUniform3f(glGetUniformLocation(this->mainShaderProgram, "mtl.Ka"), _mtl->Kd.r, _mtl->Kd.g, _mtl->Kd.b);
    glUniform3f(glGetUniformLocation(this->mainShaderProgram, "mtl.Ka"), _mtl->Ks.r, _mtl->Ks.g, _mtl->Ks.b);
    glUniform1f(glGetUniformLocation(this->mainShaderProgram, "mtl.Ns"), _mtl->Ns);
}

void Scene::mainRender() {
    shadowMapCalculation(this->onlyTail);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm:: mat4 projectionMatrix = myCamera->projectionMatrix;
    glm::mat4 viewMatrix = myCamera->viewMatrix;

    glUseProgram(skyboxShaderProgram);
    uploadInfoSkybox(&projectionMatrix, &viewMatrix);
    mySkybox->render();

    renderScene(&(myCamera->projectionMatrix), &(myCamera->viewMatrix), false, false);
    renderDepthMap();
}

void Scene::renderScene(glm::mat4* _projectionMatrix, glm::mat4* _viewMatrix, bool _onlyTail, bool depthPass) {
    glm::mat4 projectionMatrix = *_projectionMatrix;
    glm::mat4 viewMatrix = *_viewMatrix;

    glUseProgram(mainShaderProgram);
    uploadInfoMain(&projectionMatrix, &viewMatrix);

    if (_onlyTail == false) {
        uploadInfoLight_Plane();
        myLight->renderPlane();

        if (depthPass == false) {
            uploadInfoLight_Source();
            myLight->renderSource();
        }

        uploadInfoMoons();
        myMoons->renderAll();

        uploadInfoPlanets();
        myPlanets->renderAll();

        uploadInfoThrowable();
        myThrowable->renderThrowable();
    
        uploadInfoTarget();
        myTarget->render();
    }

    uploadInfoTail();
    myThrowable->tail->renderAll();
}

void Scene::renderDepthMap() {
    glUseProgram(depthShaderProgam);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(glGetUniformLocation(this->depthShaderProgam, "textureSampler"), 1);
    glUniform1f(glGetUniformLocation(this->depthShaderProgam, "near_plane"), this->myLight->nearPlane);
    glUniform1f(glGetUniformLocation(this->depthShaderProgam, "far_plane"), this->myLight->farPlane);

    quad->bind();
    quad->draw();
}

void Scene::shadowMap_CreateContext() {
    // Framebuffer for Shadow Map
    glGenFramebuffers(1, &shadowMapFBO);

    // Texture for Shadow Map FBO
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Prevents darkness outside the frustrum
    float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
    // Needed since we don't touch the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Finally, we have to always check that our frame buffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glfwTerminate();
        throw runtime_error("Frame buffer 1 (Shadow Map) not initialized correctly");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::shadowMapCalculation(bool _onlyTail) {
    glEnable(GL_DEPTH_TEST);

    // Preparations for the Shadow Map
    glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    this->renderScene(&(myLight->projectionMatrix), &(myLight->viewMatrix), _onlyTail, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::getMousePosition(double *xpos, double*ypos) {
    glfwGetCursorPos(window, xpos, ypos);
}