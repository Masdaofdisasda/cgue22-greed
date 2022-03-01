#version 430 core

// VAO Buffer data
layout (location = 0) in vec3 vPosition; // vertex position
layout (location = 1) in vec3 vColor; // vertex color
layout (location = 2) in vec3 vNormal; // vertex normal

// camera matrix
uniform mat4 model;
uniform mat4 viewProject;

out vec3 fColor;
out vec3 fNormal;
out vec3 fPosition;

void main()
{
    gl_Position =  viewProject * model * vec4(vPosition, 1.0);
	fPosition = vec3(model * vec4(vPosition, 1.0));
    fColor = vColor;
    //fNormal = mat3(model) * vNormal;   
    fNormal = mat3(transpose(inverse(model))) * vNormal;
}