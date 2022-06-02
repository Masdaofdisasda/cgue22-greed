#include "FontRenderer.h"

void font_renderer::init(const char* tex_path, const int w, const int h)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Error: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, tex_path, 0, &face))
    {
        std::cout << "Error: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Error: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // now store character for later use
        letter l = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        letters_.insert(std::pair<char, letter>(c, l));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    Shader text_vert("../assets/shaders/HUD/textPrinter.vert");
    Shader text_frag("../assets/shaders/HUD/textPrinter.frag");
    printer_.build_from(text_vert, text_frag);

    proj_ = glm::ortho(0.0f, static_cast<float>(w), 0.0f, static_cast<float>(h));

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void font_renderer::print(std::string msg, float x, const float y, const float size, const glm::vec3 color)
{
    // activate corresponding render state	
    printer_.use();
    printer_.set_vec3("color", color);
    printer_.set_mat4("projection", proj_);

    glActiveTexture(GL_TEXTURE13);
    glBindVertexArray(vao_);

    for (std::string::const_iterator c = msg.begin(); c != msg.end(); ++c)
    {
        const letter l = letters_[*c];

        const float xpos = x + l.bearing.x * size;
        const float ypos = y - (l.size.y - l.bearing.y) * size;

        const float w = l.size.x * size;
        const float h = l.size.y * size;
        // update VBO for each character
        const float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTextureUnit(19, l.texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (l.advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

void font_renderer::release()
{
    for (auto p : letters_)
    {
        auto l = p.second;
        glDeleteTextures(1, &l.texture_id);
    }

    glDeleteBuffers(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}