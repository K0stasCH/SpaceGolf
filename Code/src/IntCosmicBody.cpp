#include "IntCosmicBody.h"


#ifdef USE_PARALLEL_TRANSFORM
    #include <execution>
#endif // USE_PARALLEL_TRANSFORM

IntCosmicBody::IntCosmicBody(int number) {
    model = new Drawable(this->modelPath.c_str());
    number_of_bodies = number;

    _attributes.resize(number_of_bodies, bodyAttributes());
    translations.resize(number_of_bodies, glm::mat4(0.0f));
    rotations.resize(number_of_bodies, glm::mat4(1.0f));
    scales.resize(number_of_bodies, 1.0f);
    lifes.resize(number_of_bodies, 1.0f);

    configureVAO();
}

void IntCosmicBody::render(int time) {
    if (number_of_bodies == 0) return;
    bindAndUpdateBuffers();
    glDrawElementsInstanced(GL_TRIANGLES, 1 * model->indices.size(), GL_UNSIGNED_INT, 0, number_of_bodies);
}

void IntCosmicBody::configureVAO()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    //We are using the model's buffer but since they are already in the GPU from the Drawable's constructor we just need to configure 
    //our own VAO by using glVertexAttribPointer and glEnableVertexAttribArray but without sending any data with glBufferData.
    glBindBuffer(GL_ARRAY_BUFFER, model->verticesVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (model->indexedNormals.size() != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, model->normalsVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (model->indexedUVS.size() != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, model->uvsVBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->elementVBO);

    //GLSL treats mat4 data as 4 vec4. So we need to enable attributes 3,4,5 and 6, one for each vec4
    glGenBuffers(1, &transformations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    //This tells opengl how each particle should get data its slice of data from the mat4
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glGenBuffers(1, &rotations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rotations_buffer);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    glGenBuffers(1, &scales_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, scales_buffer);
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(11, 1);

    glGenBuffers(1, &lifes_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, lifes_buffer);
    glEnableVertexAttribArray(12);
    glVertexAttribPointer(12, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(12, 1);

    glBindVertexArray(0);
}

void IntCosmicBody::bindAndUpdateBuffers() {
    //Calculate the model matrix in parallel to save performance
    std::transform(std::execution::par_unseq, _attributes.begin(), _attributes.end(), translations.begin(),
        [](bodyAttributes p)->glm::mat4 {
            return glm::translate(glm::mat4(), p.position);
        });

    std::transform(std::execution::par_unseq, _attributes.begin(), _attributes.end(), rotations.begin(),
        [](bodyAttributes p)->glm::mat4 {
            return glm::rotate(glm::mat4(), glm::radians(p.rot_angle), p.rot_axis);
        });

    std::transform(std::execution::par_unseq, _attributes.begin(), _attributes.end(), scales.begin(),
        [](bodyAttributes p)->float {
            return p.mass;
        });

    //Bind the VAO
    glBindVertexArray(VAO);

    //Send transformation data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_bodies * sizeof(glm::mat4), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_bodies * sizeof(glm::mat4), &translations[0]); //Sending data

    glBindBuffer(GL_ARRAY_BUFFER, rotations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_bodies * sizeof(glm::mat4), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_bodies * sizeof(glm::mat4), &rotations[0]); //Sending data

    glBindBuffer(GL_ARRAY_BUFFER, scales_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_bodies * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_bodies * sizeof(float), &scales[0]);    //Sending data

    glBindBuffer(GL_ARRAY_BUFFER, lifes_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_bodies * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_bodies * sizeof(float), &lifes[0]); //Sending data

    //glUniform1i(glGetUniformLocation(cosmicBodyShader, "enableInstacing"), 1);
}

void IntCosmicBody::changeBodiesNumber(int new_number) {
    int actualBodies = _attributes.size();
    if (actualBodies == new_number) return;

    number_of_bodies = new_number;
    _attributes.resize(new_number, bodyAttributes());
    translations.resize(new_number, glm::mat4(0.0f));
    rotations.resize(new_number, glm::mat4(1.0f));
    scales.resize(new_number, 1.0f);
    lifes.resize(new_number, 1.0f);
}