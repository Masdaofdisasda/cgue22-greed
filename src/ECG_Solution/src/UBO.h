#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "LightSource.h"
#include <vector>

class UBO
{
    private:

        // reference ID
        GLuint ubo_id_ = 0;

        void release()
        {
            glDeleteBuffers(1, &ubo_id_);
			ubo_id_ = 0;
        }

    public:

		void fill_buffer(std::vector<directional_light>& buffer_data) const;
		void fill_buffer(std::vector<positional_light>& buffer_data) const;
		void fill_buffer(PerFrameData pfbuffer);
		UBO();
        
		~UBO() { release(); }

		void update(PerFrameData pfbuffer) const;

		// ensure RAII compliance
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		UBO(UBO&& other) noexcept : ubo_id_(other.ubo_id_)
		{
			other.ubo_id_ = 0; //Use the "null" ID for the old object.
		}

		UBO& operator=(UBO&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				release();
				//obj_ is now 0.
				std::swap(ubo_id_, other.ubo_id_);
			}
		}
		
		GLuint get_id() const {return ubo_id_;}
};