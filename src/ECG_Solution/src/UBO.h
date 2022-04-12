#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "LightSource.h"
#include <vector>

class UBO
{
    private:

        // reference ID
        GLuint ubo_ID = 0;

        void Release()
        {
            glDeleteBuffers(1, &ubo_ID);
			ubo_ID = 0;
        }

    public:

		void fillBuffer(std::vector<DirectionalLight>& bufferData);
		void fillBuffer(std::vector<PositionalLight>& bufferData);
		void fillBuffer(PerFrameData pfbuffer);
		UBO();
        
		~UBO() { Release(); }

		void Update(PerFrameData pfbuffer);

		// ensure RAII compliance
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		UBO(UBO&& other) noexcept : ubo_ID(other.ubo_ID)
		{
			other.ubo_ID = 0; //Use the "null" ID for the old object.
		}

		UBO& operator=(UBO&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				Release();
				//obj_ is now 0.
				std::swap(ubo_ID, other.ubo_ID);
			}
		}
		
		GLuint getID() const {return ubo_ID;}
};