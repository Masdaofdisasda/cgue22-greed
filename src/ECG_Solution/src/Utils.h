/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include "INIReader.h"
#include <iostream>
#include <Windows.h>
#include <memory>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

struct global_state
{
	int width = 800;
	int height = 800;
	int refresh_rate = 60;
	bool fullscreen = false;
	std::string window_title = "Greed";
	float fov = 60;
	float znear = 0.1f;
	float zfar = 1000.0;

	bool bloom = true;
	bool cull = true;
	bool freeze_cull = false;
	bool ssao = true;
	bool cull_debug = false;
	bool debug_draw_physics = false;
	bool using_debug_camera = false;
	bool using_animation_camera = false;
	//bloom
	float exposure = 0.9f;
	float max_white = 1.07f;
	float bloom_strength = 0.2f;
	float adaptation_speed = 1.3f;
	bool intel_mode = false;
	//ssao
	float scale = 1.0f;
	float bias = 0.2f;
	float radius = 0.2f;
	float att_scale = 1.0f;
	float dist_scale = 0.5f;
	//lightFX
	int shadow_res = 4;
	int fog_quality = 2;
	bool use_lod = false;
	//game logic
	bool won = false;
	bool lost = false;
	float time_of_death = 0;
	bool lava_triggered = false;
	bool paused = false;
	bool restart = false;
	float lava_height = 0.0f;
	bool cheat_fly_mode = false;
	//ui
	bool hud = true;
	bool display_collect_item_hint = false;
	bool display_walk_tutorial = false;
	bool display_pause_tutorial = false;
	bool display_jump_tutorial = false;
	bool display_loot_obj = false;
	bool display_escape_obj = false;
	glm::vec3 waypoint = glm::vec3(0);
	float total_cash = 0;
	int collected_items = 0;
	int item_score = 0;
	int time_bonus = 0;
	int score = 0;
};

struct keyboard_input_state {
	bool pressing_w = false;
	bool pressing_s = false;
	bool pressing_a = false;
	bool pressing_d = false;
	bool pressing_1 = false;
	bool pressing_2 = false;
	bool pressing_shift = false;
	bool pressing_space = false;
	bool pressing_e = false;
	bool pressing_q = false;
};

struct mouse_state
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressed_left = false;
	bool pressed_right = false;
};

/*files using this structure:
Frustumviewer.vert
AABBviwer.vert
bulletDebug.vert
combineHDR.frag
lavafloor.vert
lightadaption.comp
pbr.vert/.frag
skybox.vert/.frag
SSAO.frag
combineSSAO.frag
depthMap.frag/.vert
VolumetricLight.frag
*/
struct PerFrameData
{
	glm::vec4 view_pos;		// view/eye vector
	glm::mat4 view_proj;		// view projection matrix
	glm::mat4 light_view;	// light view matrix
	glm::mat4 light_view_proj;// light matrix for shadowmapping
	glm::mat4 view_inv;		// inverse view matrix
	glm::mat4 proj_inv;		// inverse projection matric
	glm::vec4 bloom;		// x = exposure, y = maxWhite, z = bloomStrength, w = adaptionSpeed
	glm::vec4 delta_time;	// x = deltaSeconds, y = summedTime, z = screen width, w = screen height
	glm::vec4 normal_map;	// x = normalMapToogle, y = lavaID, z = vl density, w = vl quality
	glm::vec4 ssao1;		// x = scale,, y = bias, z = znear, w = zfar
	glm::vec4 ssao2;		// x = radius, y = attscale, z = distscale, w = shadow debug
};

#define EXIT_WITH_ERROR(err) \
        { \
                glfwTerminate(); \
                std::cout << "ERROR: " << err << std::endl; \
                system("PAUSE"); \
                return EXIT_FAILURE; \
        }


glm::vec3 translation_from_transform(glm::mat4 transform);

glm::quat rotation_from_transform(glm::mat4 transform);

glm::vec3 scale_from_transform(glm::mat4 transform);

/// @brief an implementation of the glm::lookat() function, because this framework
/// makes it impossible to use, same code as in the Camera class
/// @param pos is the position aka eye or view of the camera
/// @param target to "look at" from the position
/// @param up is the up vector of the world
/// @return a view matrix according to the input vectors
glm::mat4 glm_look_at(const glm::vec3 pos, const glm::vec3 target, const glm::vec3 up);


/**
 * \brief implements the function of the same name from glm
 * \tparam T some type of matrix
 * \param yaw angle in radians
 * \param pitch angle in radians
 * \param roll angle in radians
 * \return view matrix from euler angles
 */
template <typename T>
glm::mat4 glm_euler_angle_xyz
(
	T const& yaw,
	T const& pitch,
	T const& roll
)
{
	T tmp_ch = glm::cos(yaw);
	T tmp_sh = glm::sin(yaw);
	T tmp_cp = glm::cos(pitch);
	T tmp_sp = glm::sin(pitch);
	T tmp_cb = glm::cos(roll);
	T tmp_sb = glm::sin(roll);

	glm::mat4 Result;
	Result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
	Result[0][1] = tmp_sb * tmp_cp;
	Result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
	Result[0][3] = static_cast<T>(0);
	Result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
	Result[1][1] = tmp_cb * tmp_cp;
	Result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
	Result[1][3] = static_cast<T>(0);
	Result[2][0] = tmp_sh * tmp_cp;
	Result[2][1] = -tmp_sp;
	Result[2][2] = tmp_ch * tmp_cp;
	Result[2][3] = static_cast<T>(0);
	Result[3][0] = static_cast<T>(0);
	Result[3][1] = static_cast<T>(0);
	Result[3][2] = static_cast<T>(0);
	Result[3][3] = static_cast<T>(1);
	return Result;
}

/**
 * \brief loads settings from settings.ini, should only be called at startup
 * \return the loaded settings
 */
global_state load_settings();

/**
 * \brief random direction vector generator for SSAO
 * \return normalized vector in some direction
 */
glm::vec3 uniform_circle();

/// texture bindings in shader:
///	model material
///	0 - albedo
///	1 - normal
///	2 - metal
///	3 - roughness
///	4 - ao
/// 5 - emissive
/// 6 - height
/// 7 -
///	global textures
///	8 - irradiance
///	9 - preFilter
///	10 - brdfLut
///	11 - skybox
/// 12 - depth map
///	13 - 3D LUT look
/// 14 - volumetric light
///	15 - lava particle
///	postprocessing
///	16 - ssao / bloom / fullscreen images
///	17 - ssao / bloom 
///	18 - bloom postprocessing
///	19 - text
///	20 - blue noise volumetric light
///	21 - perlin noise
///	22 - 
///	23 - 
