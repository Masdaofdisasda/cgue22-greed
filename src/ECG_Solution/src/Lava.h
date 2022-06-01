#pragma once
#include "Program.h"
#include "Texture.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <random>

class lava_system
{
public:
	lava_system()= default;
	~lava_system() = default;

    void init(glm::ivec3 lights);
    void update(float t);
    void simulation_step();
    void draw();

private:
    program sim_update_;
    program sim_render_;
    int count_ = 0;
    GLuint lava_particle = Texture::load_texture("../assets/shaders/Lava/lava_particle.ktx");

    glm::ivec3 n_particles_ = glm::ivec3(20,20,20);
    GLuint total_particles_ = n_particles_.x * n_particles_.y * n_particles_.z;

    float time_ = 0.0f, delta_t_ = 0.0f, speed_ = 5.0f, angle_ = 0.0f;
    GLuint particles_vao_ = 0;
    GLuint bh_vao_ = 0, bh_buf_ = 0;  // black hole VAO and buffer
    glm::vec4 bh1_ = glm::vec4(-8, -5, -16, 1);

	void setup_buffers();

};

