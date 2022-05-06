#pragma once
#include <Windows.h>
#include <GL\glew.h>
#include <string>
#include <sstream>
#include <iostream>

/* contains debugging callbacks to keep the main class cleaner
*/
namespace debug
{
	/**
	 * \brief from https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#detailed-messages-with-debug-output
	 * \param source 
	 * \param type 
	 * \param id 
	 * \param severity 
	 * \param length 
	 * \param message 
	 * \param user_param 
	 */
	void APIENTRY message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
};