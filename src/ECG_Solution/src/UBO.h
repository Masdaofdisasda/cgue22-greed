#pragma once
#include "Utils.h"
#include "LightSource.h"
#include <vector>

class UBO
{
public:

    // reference ID
    GLuint ubo;

    UBO(std::vector<DirectionalLight>& bufferData);
    UBO(std::vector<PositionalLight>& bufferData);
    UBO(std::vector<SpotLight>& bufferData);
    void bindBufferBaseToBindingPoint(const GLuint bindingPoint);

    // destructor
    ~UBO();
};