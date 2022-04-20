#pragma once
#include "Program.h"
#include "Texture.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <random>

class LavaSystem
{
public:
	LavaSystem();
	~LavaSystem();

    void update(float t);
    void Draw();

private:
    Program SimUpdate;
    Program SimRender;

    glm::ivec3 nParticles = glm::ivec3(20,20,20);
    GLuint totalParticles = nParticles.x * nParticles.y * nParticles.z;

    float time = 0.0f, deltaT = 0.0f, speed = 35.0f, angle = 0.0f;
    GLuint particlesVao;
    GLuint bhVao, bhBuf;  // black hole VAO and buffer
    glm::vec4 bh1 = glm::vec4(-85, -1, -85, 1);

	void setupBuffers();

};

LavaSystem::LavaSystem()
{
    Shader RenderVert("../../assets/shaders/Lava/lavaParticles.vert");
    Shader RenderFrag("../../assets/shaders/Lava/lavaParticles.frag");
    SimRender.buildFrom(RenderVert, RenderFrag);

    Shader Update("../../assets/shaders/Lava/lavaParticles.comp");
    SimUpdate.buildFrom(Update);

    setupBuffers();


}

void LavaSystem::setupBuffers()
{// Initial positions of the particles
    std::vector<GLfloat> initPos;
    std::vector<GLfloat> initVel(totalParticles * 4, 0.0f);

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0, 1.0);
    glm::vec3 spawn(-85, 150, -85);

    // We want to center the particles at (0,0,0)
    float steps = 130.0f / (float)totalParticles;
    for (int i = 0; i < totalParticles; i++) {
        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        p.x = spawn.x + distribution(generator);
        p.y = i* steps;
        p.z = spawn.z + distribution(generator);
        initPos.push_back(p.x);
        initPos.push_back(p.y);
        initPos.push_back(p.z);
        initPos.push_back(p.w);
    }

    // We need buffers for position , and velocity.
    GLuint bufs[2];
    glGenBuffers(2, bufs);
    GLuint posBuf = bufs[0];
    GLuint velBuf = bufs[1];

    GLuint bufSize = totalParticles * 4 * sizeof(GLfloat);

    // The buffers for positions
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initPos[0], GL_DYNAMIC_DRAW);

    // Velocities
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initVel[0], GL_DYNAMIC_COPY);

    // Set up the VAO
    glGenVertexArrays(1, &particlesVao);
    glBindVertexArray(particlesVao);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Set up a buffer and a VAO for drawing the attractors (the "black holes")
    glGenBuffers(1, &bhBuf);
    glBindBuffer(GL_ARRAY_BUFFER, bhBuf);
    GLfloat data[] = { bh1.x, bh1.y, bh1.z, bh1.w };
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &bhVao);
    glBindVertexArray(bhVao);

    glBindBuffer(GL_ARRAY_BUFFER, bhBuf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void LavaSystem::update(float t)
{
    if (time == 0.0f) {
        deltaT = 0.0f;
    }
    else {
        deltaT = t - time;
    }
    time = t;
}

void LavaSystem::Draw()
{
    // Execute the compute shader
    SimUpdate.Use();
    SimUpdate.setVec3("BlackHolePos1", bh1);
    glDispatchCompute(totalParticles / 100, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Draw the particles
    //glEnable(GL_BLEND);
    SimRender.Use();
    glPointSize(10.0f);
    SimRender.setVec4("Color", 0.05*glm::vec4(0.710, 0.200, 0.180, 1.0f));
    glBindVertexArray(particlesVao);
    glDrawArrays(GL_POINTS, 0, totalParticles);
    //glDisable(GL_BLEND);

#if 0
    // Draw the attractors
    glPointSize(5.0f);
    GLfloat data[] = { att1.x, att1.y, att1.z, 1.0f };
    glBindBuffer(GL_ARRAY_BUFFER, bhBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(GLfloat), data);
    SimRender.setVec4("Color", glm::vec4(1, 0, 1, 1.0f));
    glBindVertexArray(bhVao);
    glDrawArrays(GL_POINTS, 0, 2);
#endif

}

LavaSystem::~LavaSystem()
{
}