#pragma once
#include <ft2build.h>
#include "Program.h"
#include FT_FREETYPE_H

struct letter {
    unsigned int texture_id{};  
    glm::ivec2   size;       
    glm::ivec2   bearing;    
    unsigned int advance{};    
};

/// @brief 
/// https://learnopengl.com/In-Practice/Text-Rendering
class font_renderer
{
public:
    font_renderer() = default;
	~font_renderer() = default;

    void init(const char* tex_path, int w, int h);
    void print(std::string, float x, float y, float size, glm::vec3 color);

private:
    std::map<char, letter> letters_;
    program printer_;
    glm::mat4 proj_;
    GLuint vao_ = 0, vbo_ = 0;
};

