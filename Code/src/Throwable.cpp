#include "Throwable.h"

Throwable::Throwable(Planet* _planets, Moon* _moons):Drawable(OBJPath) {
    initialVelocity = vec3(0, 0, 0);
    _state = state{
        vec3(0.0f, 0.0f, 0.0f), //position
        vec3(0.0f, 0.0f, 0.0f), //velosity
        vec3(0.0f, 0.0f, 0.0f)  //acceleration
    };

    mass = 1E+0;

    planets_ptr = _planets;
    moons_ptr = _moons;

    mtl = {
        vec3(0.1, 0.1, 0.1),
        vec3(0.64, 0.64, 0.64),
        vec3(0.0079, 0.0079, 0.0079),
        13.7
    };

    canMove = false;
	active = false;

    velocityMagnitude = 1.0f;

    loadTexture();

    Drawable* model = new Drawable(OBJPath);
    tail = new ParticleTail(model,10000);
}

glm::vec3 Throwable::computeForce(Planet* _planets, Moon* _moons) {
    vec3 totalForce = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < _planets->number_of_bodies; i++) {
        float distance = glm::length(_planets->_attributes[i].position - this->_state.position);

        //Because the mass of planets are used as scale factor, the mass-values are very small
        //So we add a factor 1E+10
        float tempForce_Magnitude = G * this->mass * (_planets->_attributes[i].mass * 1E+10) / (distance * distance);

        vec3 tempForce = tempForce_Magnitude * glm::normalize(_planets->_attributes[i].position - this->_state.position);
        totalForce += tempForce;
    }

    for (int i = 0; i < _moons->number_of_bodies; i++) {
        //due to movement of moons we need to compute the currnet position of it
        vec4 currentPositionMoon = glm::rotate(mat4(1.0f), _moons->_attributes[i].rot_angle, _moons->_attributes[i].rot_axis) * vec4(_moons->_attributes[i].position,1.0f);
        //if (i == 0) {
        //    std::cout << currentPositionMoon.x <<" "<< currentPositionMoon.y <<" "<< currentPositionMoon.z << std::endl;
        //}

        float distance = glm::length(this->_state.position - vec3(currentPositionMoon));

        //Because the mass of planets are used as scale factor, the mass-values are very small
        //So we add a factor 1E+10
        float tempForce_Magnitude = G * this->mass * (_moons->_attributes[i].mass * 1E+10) / (distance * distance);

        vec3 tempForce = tempForce_Magnitude * glm::normalize(_moons->_attributes[i].position - this->_state.position);
        totalForce += tempForce;

    }

    return totalForce;
}

void Throwable::update(float dt) {
    if (canMove == false) return;

    tail->emitter_pos = this->_state.position;
    tail->emitter_vel = this->_state.velocity;

    this->_state.rot_angle += 10 * length(this->_state.velocity) * dt;

    vec3 force = this->computeForce(this->planets_ptr, this->moons_ptr);
    this->_state.acceleration = force / this->mass;
    this->_state.velocity = this->_state.velocity + this->_state.acceleration * dt;
    this->_state.position = this->_state.position + this->_state.velocity * dt + 0.5f * this->_state.acceleration * (dt * dt);
}

void Throwable::loadTexture() {
    std::string path = this->texturePath.c_str();
    std::string extension = path.substr(path.size() - 3);

    if (extension == "bmp")
        texture = loadBMP(path.c_str());
    else if (extension == "png")
        texture = loadSOIL(path.c_str());
    else
        std::cout << "Wrong format of throwable texture" << std::endl;
}

void Throwable::renderThrowable() {
    this->bind();
    mat4 rotationMatrix = glm::rotate(glm::mat4(), glm::radians(this->_state.rot_angle), this->_state.rot_axis);
    mat4 translationMatrix = glm::translate(mat4(1.0f), this->_state.position);
    modelMatrix = translationMatrix * rotationMatrix;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    Drawable::draw();
}

void Throwable::reset() {
    canMove = false;
    tail->active = false;
    this->_state = state{
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f),   
        vec3(0.0f, 0.0f, 0.0f)
    };
}

void Throwable::throwObject() {
    canMove = true;
    tail->active = true;
    this->_state.velocity = initialVelocity;
}

void Throwable::renderTail() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    tail->renderParticles();
}

void Throwable::setInitialVelocity(double MouseX, double MouseY) {
    vec2 angles = Movable::onMouseMove(MouseX, MouseY); //horizontal , vertical
    float horizontalAngle = angles.x;
    float verticalAngle = angles.y;
    this->initialVelocity = velocityMagnitude * vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
}

glm::vec3 Throwable::getInitialVelocity() {
    return this->initialVelocity;
}