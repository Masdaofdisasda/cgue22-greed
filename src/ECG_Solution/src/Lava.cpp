#include "Lava.h"

void lava_system::init(glm::ivec3 lights)
{
    Shader render_vert("../../assets/shaders/Lava/lavaParticles.vert");
    Shader render_frag("../../assets/shaders/Lava/lavaParticles.frag");
    sim_render_.buildFrom(render_vert, render_frag);

    Shader update("../../assets/shaders/Lava/lavaParticles.comp");
    sim_update_.buildFrom(update);

    Shader lava_floor_vert("../../assets/shaders/Lava/lavaFloor.vert");
    Shader lava_floor_frag("../../assets/shaders/Lava/lavaPbr.frag", lights);
    lava_floor_.buildFrom(lava_floor_vert, lava_floor_frag);

    setup_buffers();
    load_lava();
}

void lava_system::setup_buffers()
{// Initial positions of the particles
    std::vector<GLfloat> init_pos;
    const std::vector<GLfloat> init_vel(total_particles_ * 4, 0.0f);

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0, 1.0);
    const glm::vec3 spawn(-85, 150, -85);

    // We want to center the particles at (0,0,0)
    const float steps = 130.0f / static_cast<float>(total_particles_);
    for (auto i = 0; i < total_particles_; i++) {
        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        p.x = spawn.x + distribution(generator);
        p.y = i * steps;
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

    // The buffers for positions
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &init_pos[0], GL_DYNAMIC_DRAW);

    // Velocities
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &init_vel[0], GL_DYNAMIC_COPY);

    // Set up the VAO
    glGenVertexArrays(1, &particles_vao_);
    glBindVertexArray(particles_vao_);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

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

void lava_system::load_lava()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("../../assets/models/Lava.obj", aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_PreTransformVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate |
        aiProcess_FixInfacingNormals |
        aiProcess_FindInvalidData |
        aiProcess_ValidateDataStructure | 0
    );

    std::vector<float> vertices;
    std::vector <GLuint> indices;

    const aiMesh* mesh = scene->mMeshes[0];

    // extract vertices from the aimesh
    for (size_t j = 0; j < mesh->mNumVertices; j++)
    {
        const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D(0.0f);
        const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);
        const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.5f, 0.5f, 0.0f);

        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(p.z);

        vertices.push_back(n.x);
        vertices.push_back(n.y);
        vertices.push_back(n.z);

        vertices.push_back(t.x);
        vertices.push_back(t.y);

    }

    //extract indices from the aimesh
    for (size_t j = 0; j < mesh->mNumFaces; j++)
    {
        for (unsigned k = 0; k != mesh->mFaces[j].mNumIndices; k++)
        {
            GLuint index = mesh->mFaces[j].mIndices[k];
            count_++;
            indices.push_back(index);
        }
    }

    GLuint vbo;
    GLuint ebo;

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), 0);
    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), 0);

    glCreateVertexArrays(1, &lava_vao_);
    glVertexArrayElementBuffer(lava_vao_, ebo);
    glVertexArrayVertexBuffer(lava_vao_, 0, vbo, 0, sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2));
    // position
    glEnableVertexArrayAttrib(lava_vao_, 0);
    glVertexArrayAttribFormat(lava_vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(lava_vao_, 0, 0);
    // normal
    glEnableVertexArrayAttrib(lava_vao_, 1);
    glVertexArrayAttribFormat(lava_vao_, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
    glVertexArrayAttribBinding(lava_vao_, 1, 0);
    // uv
    glEnableVertexArrayAttrib(lava_vao_, 2);
    glVertexArrayAttribFormat(lava_vao_, 2, 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec3) + sizeof(glm::vec3));
    glVertexArrayAttribBinding(lava_vao_, 2, 0);

}

void lava_system::simulation_step()
{
    // Execute the compute shader
    sim_update_.Use();
    sim_update_.setVec3("BlackHolePos1", bh1_);
    glDispatchCompute(total_particles_ / 100, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void lava_system::draw()
{

    lava_floor_.Use();
    glBindVertexArray(lava_vao_);
    const GLuint textures[] = { lava_.get_albedo(), lava_.get_normal_map(),lava_.get_metallic(), lava_.get_roughness(), lava_.get_ao_map() };
    glBindTextures(0, 5, textures);
    glDrawElements(GL_TRIANGLES, count_, GL_UNSIGNED_INT, 0);

    // Draw the particles
    sim_render_.Use();
    glPointSize(10.0f);
    glBindVertexArray(particles_vao_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizeiptr>(total_particles_));


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

