#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "LightSource.h"
#include <vector>

class buffer
{
    private:

        // reference ID
        GLuint buffer_id_ = 0;
		GLenum type_;

        void release()
        {
            glDeleteBuffers(1, &buffer_id_);
			buffer_id_ = 0;
        }

    public:
        /**
         * \brief reserves space for the buffer and uploads initial data, call once before using update
         * \param binding of the buffer, can be accessed in every shader by the identical binding mapping
         * \param size in bytes of the maximum size the buffer will need
         * \param data that should get uploaded to the buffer immediately
         */
        void reserve_memory(GLuint binding, GLsizeiptr size, const void* data) const;

        /**
         * \brief reserves space for the buffer and uploads initial data, call once before using update
         * \param size in bytes of the maximum size the buffer will need
         * \param data that should get uploaded to the buffer immediately
         */
        void reserve_memory(GLsizeiptr size, const void* data) const;

        /**
         * \brief uploads new data to the buffer
         * \param size of the data in bytes
         * \param data to be uploaded
         */
        void update(const GLsizeiptr size, const void* data) const;

        /**
         * \brief creates a buffer id
         * \param type of the buffer (eg. uniform)
         */
        explicit buffer(GLenum type);
        
		~buffer() { release(); }
		

		// ensure RAII compliance
		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		buffer(buffer&& other) noexcept : buffer_id_(other.buffer_id_)
		{
			other.buffer_id_ = 0; //Use the "null" ID for the old object.
		}

		buffer& operator=(buffer&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				release();
				//obj_ is now 0.
				std::swap(buffer_id_, other.buffer_id_);
			}
		}
		
		GLuint get_id() const {return buffer_id_;}
};