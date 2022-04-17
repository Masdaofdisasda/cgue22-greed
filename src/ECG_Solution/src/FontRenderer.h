#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H 

struct Letter {
    unsigned int TextureID;  
    glm::ivec2   Size;       
    glm::ivec2   Bearing;    
    unsigned int Advance;    
};

/// @brief 
/// https://learnopengl.com/In-Practice/Text-Rendering
class FontRenderer
{
public:
    FontRenderer() = default;
	~FontRenderer();

    void init(const char* texPath, int w, int h);
    void print(std::string, float x, float y, float size, glm::vec3 color);

private:
    std::map<char, Letter> letters;
    Program printer;
    glm::mat4 proj;
    GLuint vao = 0, vbo = 0;
};

void FontRenderer::init(const char* texPath, int w, int h)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Error: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, texPath, 0, &face))
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Letter l = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        letters.insert(std::pair<char, Letter>(c, l));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    Shader textVert("../../assets/shaders/HUD/textPrinter.vert");
    Shader textFrag("../../assets/shaders/HUD/textPrinter.frag");
    printer.buildFrom(textVert, textFrag);

    proj = glm::ortho(0.0f, (float)w, 0.0f, (float)h);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FontRenderer::print(std::string msg, float x, float y, float size, glm::vec3 color)
{
    // activate corresponding render state	
    printer.Use();
    printer.setVec3("color", color);
    printer.setMat4("projection", proj);

    glActiveTexture(GL_TEXTURE13);
    glBindVertexArray(vao);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = msg.begin(); c != msg.end(); c++)
    {
        Letter l = letters[*c];

        float xpos = x + l.Bearing.x * size;
        float ypos = y - (l.Size.y - l.Bearing.y) * size;

        float w = l.Size.x * size;
        float h = l.Size.y * size;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTextureUnit(13, l.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (l.Advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

FontRenderer::~FontRenderer()
{
}