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
	~font_renderer(){release();}

    /**
     * \brief loads a font from file and generates bitmaps for every character
     * \param tex_path location of the font file
     * \param w width of the screen
     * \param h height of the screen
     */
    void init(const char* tex_path, int w, int h);

    /**
     * \brief prints left centered text to current framebuffer
     * \param msg text to print
     * \param x pixel offset from the left
     * \param y piyel offset from the bottom
     * \param size of the font (scale font, so everything >1 will be blurry)
     * \param color to render the text
     */
    void print(std::string msg, float x, float y, float size, glm::vec3 color);

private:
    std::map<char, letter> letters_;
    program printer_;
    glm::mat4 proj_;
    GLuint vao_ = 0, vbo_ = 0;

    void release();
};

