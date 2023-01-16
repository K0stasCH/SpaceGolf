#include <GL/glew.h>

#include <glfw3.h>

#include <glm/glm.hpp>

#include "common/util.h"
#include "Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void renderHelpingWindow();
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);



//Contains all the objects
Scene* myScense;
//You can move the mouse and select any option in the GUI
//If it is enabled you can't interact with anything, but the simulation is still running
bool menuSelectorEnabled = false;


void renderHelpingWindow() {    
    ImGui::Begin("Editor");

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
    ImGui::Checkbox("Stop Simulation", &myScense->stopSimulation);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::Checkbox("Only tail particles cast shadows", &myScense->onlyTail);

    ImGui::Text("What camera follows:");
    ImGui::Combo("", &myScense->myCamera->status, myScense->myCamera->statusNames, IM_ARRAYSIZE(myScense->myCamera->statusNames));
    if (menuSelectorEnabled == false) {
        if (myScense->myCamera->status == 0) {
            myScense->myCamera->Movable::active = true;
            myScense->myLight->Movable::active = false;
        }
        else if (myScense->myCamera->status == 1) {
            myScense->myCamera->Movable::active = false;
            myScense->myLight->Movable::active = false;
        }
        else if (myScense->myCamera->status == 2) {
            myScense->myCamera->Movable::active = false;
            myScense->myLight->Movable::active = true;
        }
    }


    //terrain options
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    if (ImGui::CollapsingHeader("Terrain Details")) {
        if (ImGui::SliderFloat("Terrain Size", &(myScense->myPlanets->terrainLimit), 10.0f, 200.0f)) {
            myScense->myPlanets->generatePlanets();
            myScense->myMoons->generateMoons();
        }
        if (ImGui::SliderInt("Number of Planets", &(myScense->myPlanets->number_of_bodies), 1, 30)) {
            myScense->myPlanets->update();
            myScense->myMoons->generateMoons();
        }
        if (ImGui::SliderFloat("Moons Density", &(myScense->myMoons->densityMoons), 0, 1)) {
            myScense->myMoons->generateMoons();
        }
    }
    ImGui::PopStyleColor();


    //camera options
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
    if (ImGui::CollapsingHeader("Camera Details")) {
        ImGui::Text("Position: x = %.3f y = %.3f z = %.3f", myScense->myCamera->position.x, myScense->myCamera->position.y, myScense->myCamera->position.z);
        ImGui::Text("Rotation (degrees):");
        ImGui::Text("Horizontal Angle = %.3f Vertical Angle = %.3f", 180 / 3.14159 * myScense->myCamera->horizontalAngle, 180 / 3.14159 * myScense->myCamera->verticalAngle);
        ImGui::SliderFloat("Movement Speed", &(myScense->myCamera->speed), 10.0f, 200.0f);
        ImGui::SliderFloat("FOV", &(myScense->myCamera->FoV), myScense->myCamera->minFoV, myScense->myCamera->maxFoV);
        ImGui::Text("Clipping Planes");
        ImGui::SliderFloat("Far", &(myScense->myCamera->farClippingPlane), 50, 500);
        ImGui::SliderFloat("Near", &(myScense->myCamera->nearClippingPlane), 0.01, 10);
    }
    ImGui::PopStyleColor();


    //light options
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    if (ImGui::CollapsingHeader("Light Details")) {
        if (ImGui::TreeNode("Source")) {
            ImGui::Text("Position: x = %.3f y = %.3f z = %.3f", myScense->myLight->position.x, myScense->myLight->position.y, myScense->myLight->position.z);
            if (ImGui::Checkbox("Move only on the surface of a sphere", &(myScense->myLight->sphericalTrajectory))) {
                myScense->myLight->posR = 1.5 * (myScense->myPlanets->terrainLimit);
            }
            ImGui::ColorEdit3("Light Color", &(myScense->myLight->color)[0]);
            ImGui::SliderFloat("Power", &(myScense->myLight->power), 0.0f, 5.0f);
            ImGui::SliderFloat("Ambient Power", &(myScense->myLight->ambient), 0.0f, 1.0f);
            ImGui::SliderFloat("Far", &(myScense->myLight->farPlane), 50, 500);
            ImGui::SliderFloat("Near", &(myScense->myLight->nearPlane), 0.01, 10);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Plane")) {
            ImGui::Checkbox("Show Plane", &(myScense->myLight->showPlane));
            ImGui::SliderFloat("Plane Offset", &(myScense->myLight->planeOffset), 0.0f, (myScense->myPlanets->terrainLimit) * 2.5f);
            ImGui::SliderFloat("Plane Scale", &(myScense->myLight->planeScale), 0.0f, 100.0f);

            ImGui::TreePop();
        }
    }
    ImGui::PopStyleColor();


    //target stats
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.188f, 0.835f, 0.784f, 1.0f));
    if (ImGui::CollapsingHeader("Target Details")) {

        ImGui::SliderFloat("Position X axis", &(myScense->myTarget->position.x), -myScense->myPlanets->terrainLimit, myScense->myPlanets->terrainLimit);
        ImGui::SliderFloat("Position Y axis", &(myScense->myTarget->position.y), -myScense->myPlanets->terrainLimit, myScense->myPlanets->terrainLimit);
        ImGui::SliderFloat("Position Z axis", &(myScense->myTarget->position.z), -myScense->myPlanets->terrainLimit, myScense->myPlanets->terrainLimit);

        ImGui::SliderFloat("Roll X axis", &(myScense->myTarget->rollX), 0, 180);
        ImGui::SliderFloat("Roll Y axis", &(myScense->myTarget->rollY), 0, 180);
        ImGui::SliderFloat("Roll Z axis", &(myScense->myTarget->rollZ), 0, 180);

        ImGui::SliderFloat("Scale", &(myScense->myTarget->scale), 1.0f, 100.0f);

        ImGui::Text("Distance from Target: %.3f", (myScense->myTarget->calculateDist(myScense->myThrowable->_state.position)));
    }
    ImGui::PopStyleColor();


    //throwable stats
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.824f, 0.706f, 0.549f, 1.0f));
    if (ImGui::CollapsingHeader("Comet Details", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)) {
        if (ImGui::TreeNode("Main Body")) {
            
            ImGui::Text("Position: x = %.3f y = %.3f z = %.3f", myScense->myThrowable->_state.position.x, myScense->myThrowable->_state.position.y, myScense->myThrowable->_state.position.z);
            ImGui::Text("Velocity = %.3f m/s", glm::length(myScense->myThrowable->_state.velocity));
            ImGui::Text("Acceleration = %.3f m/(s*s)", glm::length(myScense->myThrowable->_state.acceleration));
            ImGui::SliderFloat("Mass", &(myScense->myThrowable->mass), 1.0f, 1.0E+8);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Tail")) {
            ImGui::SliderInt("Number of Particles", &(myScense->myThrowable->tail->number_of_particles), 1E+2, 1E+4);
            ImGui::SliderFloat("Size of Trail", &(myScense->myThrowable->tail->dist_threshold), 5.0f, 20.0f);
            ImGui::TreePop();
        }

        double xpos, ypos;
        myScense->getMousePosition(&xpos, &ypos);
        myScense->myThrowable->setInitialVelocity(xpos, ypos);
        if ((ImGui::Button("Throw")) || (glfwGetKey(myScense->window, GLFW_KEY_SPACE) == GLFW_PRESS)) 
            myScense->myThrowable->throwObject();

        if ((glfwGetKey(myScense->window, GLFW_KEY_KP_ADD) == GLFW_PRESS)) myScense->myThrowable->velocityMagnitude += 0.05f;
        if ((glfwGetKey(myScense->window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)) myScense->myThrowable->velocityMagnitude -= 0.05f;
        ImGui::SliderFloat("Initial Velocity", &(myScense->myThrowable->velocityMagnitude), 0.0f, 10.0f);
        
        ImGui::Text("Initial Velocity X axis: %.2f", myScense->myThrowable->getInitialVelocity().x);
        ImGui::Text("Initial Velocity Y axis: %.2f", myScense->myThrowable->getInitialVelocity().y);
        ImGui::Text("Initial Velocity Z axis: %.2f", myScense->myThrowable->getInitialVelocity().z);

        if (ImGui::Button("Reset"))
            myScense->myThrowable->reset();
    }
    ImGui::PopStyleColor();


    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.647f, 0.0f, 1.0f));
    ImGui::Text("Performance %.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::PopStyleColor();
    
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void createContext() {
    myScense->createContext();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetKeyCallback(myScense->window, pollKeyboard);
}

void free() {
    myScense-> ~Scene();
    glfwTerminate();
}

void mainLoop() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(myScense->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    double prevTime = 0.0;
    double crntTime = 0.0;
    double dt;
    // Keeps track of the amount of frames in timeDiff
    unsigned int counter = 0;


    do {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // set Tilte
        crntTime = glfwGetTime();
        dt = crntTime - prevTime;
        counter++;
        
        //change the tilte 10 times per sec
        if (dt >= 1.0 / 10.0) {
            // Resets times and counter
            prevTime = crntTime;
            counter = 0;
        }
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myScense->update(dt);
        myScense->mainRender();

        renderHelpingWindow();
        glfwSwapBuffers(myScense->window);
        glfwPollEvents();
        
    } while (glfwGetKey(myScense->window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(myScense->window) == 0);
}

void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    myScense = new Scene();
    myScense->initialize();

    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Set the mouse at the center of the screen
    glfwPollEvents();

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Enable blending for transparency
    // change alpha in fragment shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Keeps front faces
    glCullFace(GL_FRONT);

    // Uses counter clock-wise standard
    glFrontFace(GL_CW);


    // Log
    logGLParameters();
}

int main(void) {
    try {
        initialize();
        createContext();
        mainLoop();
        free();
    } catch (exception& ex) {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }
}


void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            menuSelectorEnabled = false;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            myScense->myCamera->active = false;
            myScense->myLight->active = false;
            menuSelectorEnabled = true;
        }
    }
}

