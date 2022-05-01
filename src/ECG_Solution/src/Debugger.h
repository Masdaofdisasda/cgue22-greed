#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <string>
#include <sstream>
#include <iostream>

/* contains debugging callbacks to keep the main class cleaner
*/
class debugger
{
public:
	static void APIENTRY debug_callback_default(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
	static std::string format_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
};