#include "Lava.h"

void lava_system::init(glm::ivec3 lights)
{
    Shader render_vert("../../assets/shaders/Lava/lavaParticles.vert");
    Shader render_frag("../../assets/shaders/Lava/lavaParticles.frag");
    sim_render_.build_from(render_vert, render_frag);

    Shader update("../../assets/shaders/Lava/lavaParticles.comp");
    sim_update_.build_from(update);

    setup_buffers();
}

void lava_system::setup_buffers()
{// Initial positions of the particles
    std::vector<GLfloat> init_pos;
    const std::vector<GLfloat> init_vel(total_particles_ * 4, 0.0f);

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0, 1.0);
    const glm::vec3 spawn(-6, 29, -16);

    // We want to center the particles at (0,0,0)
    const float steps = 29.0f / static_cast<float>(total_particles_);
    for (auto i = 0; i < total_particles_; i++) {
        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        p.x = spawn.x + distribution(generator);
        p.y = 29.0f + i * steps;
        p.z = spawn.z + distribution(generator);
        init_pos.push_back(p.x);
        init_pos.push_back(p.y);
        init_pos.push_back(p.z);
        init_pos.push_back(p.w);
    }

    // We need buffers for position , and velocity.
    GLuint bufs[2];
    glGenBuffers(2, bufs);
    GLuint posBuf = bufs[0];
    GLuint velBuf = bufs[1];

    GLuint bufSize = total_particles_ * 4 * sizeof(GLfloat);

    std::vector<float> quad_vertices = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
        1.0f,  1.0f,
        1.0f, -1.0f };

    // The buffers for positions
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &init_pos[0], GL_DYNAMIC_DRAW);

    // The buffer the quad
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad_vertices.size() * sizeof(float)), quad_vertices.data(), GL_STATIC_DRAW);

    // Velocities
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &init_vel[0], GL_DYNAMIC_COPY);

    // Set up the VAO
    glGenVertexArrays(1, &particles_vao_);
    glBindVertexArray(particles_vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

#if 0
    // Set up a buffer and a VAO for drawing the attractors (the "black holes")
    glGenBuffers(1, &bh_buf_);
    glBindBuffer(GL_ARRAY_BUFFER, bh_buf_);
    const GLfloat data[] = { bh1_.x, bh1_.y, bh1_.z, bh1_.w };
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &bh_vao_);
    glBindVertexArray(bh_vao_);

    glBindBuffer(GL_ARRAY_BUFFER, bh_buf_);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
#endif
    
    glBindTextureUnit(15, lava_particle);
}

void lava_system::update(const float t)
{
    if (time_ == 0.0f) {
        delta_t_ = 0.0f;
    }
    else {
        delta_t_ = t - time_;
    }
    time_ = t;
}


void lava_system::simulation_step()
{
    // Execute the compute shader
    sim_update_.use();
    sim_update_.set_vec3("BlackHolePos1", bh1_);
    glDispatchCompute(total_particles_ / 100, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void lava_system::draw()
{

    // Draw the particles
    sim_render_.use();
    glBindVertexArray(particles_vao_);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizeiptr>(total_particles_));


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

