#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <string>
#include <sstream>
#include <iostream>

class Debugger
{
public:
	static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
	static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
};