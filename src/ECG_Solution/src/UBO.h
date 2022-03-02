#pragma once
#include "Utils.h"
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

        UBO(std::vector<DirectionalLight>& bufferData);
        UBO(std::vector<PositionalLight>& bufferData);
        UBO(std::vector<SpotLight>& bufferData);
        
		~UBO() { Release(); }

		// ensure RAII compliance
		UBO(const UBO&) = delete;
		UBO& operator=(const UBO&) = delete;

		UBO(UBO&& other) : ubo_ID(other.ubo_ID)
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

		void bindBufferBaseToBindingPoint(const GLuint bindingPoint);
		
		GLuint* getID() {return &ubo_ID;}
};