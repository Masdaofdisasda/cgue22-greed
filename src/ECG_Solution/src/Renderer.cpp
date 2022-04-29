#include "Renderer.h"

/// @brief sets up shaders for rendering and post processing and also enviroment maps
/// @param state is the global state of the program at runtime
/// @param pfdata is per render cycle static data
/// @param sources are the light sources of some level
Renderer::Renderer(PerFrameData& pfdata, LightSources& sources)
{
	assert(Renderer::state != nullptr);
	perframeData = &pfdata; // link per frame data
	lights = sources; // set lights and lightcounts for shaders
	buildShaderPrograms(); // build shader programs
	setRenderSettings();	// set effect settings 
	fillLightsources(); // binds lights to binding points in shader
	perframeBuffer.fillBuffer(pfdata); // load UBO to shader;

	prepareFramebuffers(); // for hdr rendering and tonemapping
	std::cout << "load Enviroment Map.." << std::endl;
	IBL.loadHDR("../../assets/textures/cubemap/env.hdr");
	std::cout << "load Skybox.." << std::endl;
	skyTex.loadHDR("../../assets/textures/cubemap/beach.hdr");
	Lut3D = Texture::load3Dlut("../../assets/textures/look32.CUBE");
	glCreateVertexArrays(1, &emptyVAO);
	PBRShader.uploadIBL(IBL.getIrradianceID(),IBL.getPreFilterID(), IBL.getBdrfLutID(), skyTex.getEnvironment());
	glBindTextureUnit(13, Lut3D);

	fontRenderer.init("../../assets/fonts/Quasimoda/Quasimoda-Regular.otf", state->width, state->height);

	lavaSim.init(glm::ivec3(lights.directional.size(), lights.point.size(), 0));
}

std::shared_ptr<GlobalState> Renderer::getState()
{
	return state;
}
/// @brief loads settings from settings.ini, called in main
/// @return a globalstate from settings.ini
GlobalState Renderer::loadSettings()
{
	if (state == nullptr)
	{

	GlobalState state;
	std::cout << "reading setting from settings.ini..." << std::endl;
	INIReader reader("../../assets/settings.ini");

	// first param: section [window], second param: property name, third param: default value
	state.width = reader.GetInteger("window", "width", 800);
	state.height = reader.GetInteger("window", "height", 800);
	state.refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	state.fullscreen_ = reader.GetBoolean("window", "fullscreen", false);
	state.window_title = "Greed";
	state.fov = reader.GetReal("camera", "fov", 60.0f);
	state.Znear = 0.1f;
	state.Zfar = 1000.0f;

	state.bloom_ = reader.GetBoolean("image", "bloom", true);
	state.exposure_ = reader.GetReal("image", "exposure", 0.9f);
	state.maxWhite_ = reader.GetReal("image", "maxWhite", 1.07f);
	state.bloomStrength_ = reader.GetReal("image", "bloomStrength", 0.2f);
	state.adaptationSpeed_ = reader.GetReal("image", "lightAdaption", 0.1f);
	state.ssao_ = reader.GetBoolean("image", "ssao", true);
	state.scale_ = reader.GetReal("image", "scale", 1.0f);
	state.bias_ = reader.GetReal("image", "bias", 0.2f);
	state.radius = reader.GetReal("image", "radius", 0.2f);
	state.attScale = reader.GetReal("image", "attScale", 1.0f);
	state.distScale = reader.GetReal("image", "distScale", 0.5f);
	state.shadowRes_ = reader.GetInteger("image", "shadowRes", 4);

	return state;
	}

	// TODO 
	return *Renderer::state;
}


std::shared_ptr<GlobalState> Renderer::state = std::make_shared<GlobalState>(Renderer::loadSettings());

// TODO
void Renderer::fillLightsources()
{
	// create Uniform Buffer Objects from light source struct vectors
	directionalLights.fillBuffer(lights.directional);
	positionalLights.fillBuffer(lights.point);

	// bind UBOs to bindings in shader
	PBRShader.bindLightBuffers(&directionalLights, &positionalLights);
	// set light source count variables
	PBRShader.setuInt("dLightCount", lights.directional.size());
	PBRShader.setuInt("pLightCount", lights.point.size());
}

/// @brief initializes settings for post processing and rendering
void Renderer::setRenderSettings()
{
	perframeData->bloom = glm::vec4(
		state->exposure_,
		state->maxWhite_,
		state->bloomStrength_,
		state->adaptationSpeed_);

	perframeData->normalMap = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	perframeData->ssao1 = glm::vec4(
		state->scale_,
		state->bias_,
		state->Znear,
		state->Zfar);
	perframeData->ssao2 = glm::vec4(
		state->radius,
		state->attScale,
		state->distScale,
		1.0f);

	perframeData->deltaTime = glm::vec4(0);
}

/// @brief compiles all needed shaders for the render loop
void Renderer::buildShaderPrograms()
{
	Shader pbrVert("../../assets/shaders/PBR/pbr.vert");
	Shader pbrFrag("../../assets/shaders/PBR/pbr.frag", glm::ivec3(lights.directional.size(), lights.point.size(), 0));
	PBRShader.buildFrom(pbrVert, pbrFrag);
	PBRShader.Use();

	Shader skyboxVert("../../assets/shaders/skybox/skybox.vert");
	Shader skyboxFrag("../../assets/shaders/skybox/skybox.frag");
	skyboxShader.buildFrom(skyboxVert, skyboxFrag);
	skyboxShader.Use();

	Shader fullScreenTriangleVert("../../assets/shaders/fullScreenTriangle.vert");

	Shader BrightPassFrag("../../assets/shaders/Bloom/BrightPass.frag");
	BrightPass.buildFrom(fullScreenTriangleVert, BrightPassFrag);

	Shader CombineHDRFrag("../../assets/shaders/Bloom/CombineHDR.frag");
	CombineHDR.buildFrom(fullScreenTriangleVert, CombineHDRFrag);

	Shader BlurXFrag("../../assets/shaders/Bloom/BlurX.frag");
	Shader BlurYFrag("../../assets/shaders/Bloom/BlurY.frag");
	BlurX.buildFrom(fullScreenTriangleVert, BlurXFrag);
	BlurY.buildFrom(fullScreenTriangleVert, BlurYFrag);

	Shader LuminanceFrag("../../assets/shaders/Bloom/toLuminance.frag");
	ToLuminance.buildFrom(fullScreenTriangleVert, LuminanceFrag);

	Shader lightAdaptComp("../../assets/shaders/Bloom/lightAdaption.comp");
	lightAdapt.buildFrom(lightAdaptComp);

	Shader SSAOFrag("../../assets/shaders/SSAO/SSAO.frag");
	Shader combineSSAOFrag("../../assets/shaders/SSAO/combineSSAO.frag");
	SSAO.buildFrom(fullScreenTriangleVert, SSAOFrag);
	CombineSSAO.buildFrom(fullScreenTriangleVert, combineSSAOFrag);

	Shader renderImgVert("../../assets/shaders/fullScreenTriangle.vert");
	Shader renderImgFrag("../../assets/shaders/HUD/fullScreenImage.frag");
	Shader renderColFrag("../../assets/shaders/HUD/fullScreenColor.frag");
	renderImage.buildFrom(renderImgVert, renderImgFrag);
	renderColor.buildFrom(renderImgVert, renderColFrag);

	Shader depthVert("../../assets/shaders/lightFX/depthMap.vert");
	Shader depthFrag("../../assets/shaders/lightFX/depthMap.frag");
	DepthMap.buildFrom(depthVert, depthFrag);

	Shader volightFrag("../../assets/shaders/lightFX/VolumetricLight.frag", glm::ivec3(lights.directional.size(), lights.point.size(), 0));
	VolumetricLight.buildFrom(fullScreenTriangleVert, volightFrag);

	Shader downVlVert("../../assets/shaders/lightFX/downVL.vert");
	Shader downVlFrag("../../assets/shaders/lightFX/downVL.frag");
	downsampleVL.buildFrom(downVlVert, downVlFrag);

	Shader upVlVert("../../assets/shaders/lightFX/upVL.vert");
	Shader upVlFrag("../../assets/shaders/lightFX/upVL.frag");
	upsampleVL.buildFrom(upVlVert, upVlFrag);

	PBRShader.Use();
}


/// @brief sets OpenGL states before first draw call, which can't be done in header file
void Renderer::prepareFramebuffers() {

	glGenTextures(1, &luminance1x1);
	glTextureView(luminance1x1, GL_TEXTURE_2D, luminance.getTextureColor().getHandle(), GL_RGBA16F, 6, 1, 0, 1);

	const glm::vec4 startingLuminance(glm::vec3(0.0f), 1.0f);
	glTextureSubImage2D(luminance0.getHandle(), 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &startingLuminance[0]);

	pattern = Texture::loadTexture("../../assets/shaders/SSAO/pattern.bmp");

	hud = Texture::loadTextureTransparent("../../assets/textures/loading/alpha HUD.png");
}

/// @brief implements the pipeline for HDR, tonemapping and shadows
/// @param level is the geometry which should be drawn
void Renderer::Draw(Level* level)
{
	
	glClearNamedFramebufferfv(framebuffer1.getHandle(), GL_COLOR, 0, &(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]));
	glClearNamedFramebufferfi(framebuffer1.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);

	glm::vec3 dir = glm::vec3(lights.directional[0].direction);
	glm::mat4 lightView = glmlookAt2(glm::vec3(0, 0, 0), dir, glm::vec3(0, 0, 1));
	glm::mat4 lightProj = level->getTightSceneFrustum();
	perframeData->lightViewProj = lightProj * lightView;

	perframeBuffer.Update(*perframeData);

	glEnable(GL_DEPTH_TEST);

	// 1 - depth mapping
#if 1 // under construction
	depthMap.bind();
		glClearNamedFramebufferfi(depthMap.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);
		DepthMap.Use();
		level->DrawSceneFromLightSource();
	depthMap.unbind();
	glBindTextureUnit(12, depthMap.getTextureDepth().getHandle());
#endif // constructions ends

	// 2 - render scene to framebuffer
	framebuffer1.bind();

		// 2.1 - draw skybox (background)    
		skyboxShader.Use();
		glDepthMask(false);
		glBindVertexArray(emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);

		// 2.2 - draw scene
		PBRShader.Use();
		level->DrawScene();

		// 2.3 - draw lava
		lavaSim.update(perframeData->deltaTime.x);
		lavaSim.simulationStep();
		lavaSim.Draw();

	framebuffer1.unbind(); 
	glGenerateTextureMipmap(framebuffer1.getTextureColor().getHandle());
	glTextureParameteri(framebuffer1.getTextureColor().getHandle(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glDisable(GL_DEPTH_TEST);

	// Volumetric Light
	// https://github.com/metzzo/ezg17-transition
	if (false)
	{
		/*
		// down sample scene using depth aware downsampling
		downsampleVL.Use();
		depthHalfRes.bind();
		glClearNamedFramebufferfi(depthHalfRes.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		depthHalfRes.unbind();
		glBindTextureUnit(12, depthHalfRes.getTextureDepth().getHandle());
		*/
		// calculate volumetric lighting
		VolumetricLight.Use();
		blur0.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		blur0.unbind();
		glBindTextureUnit(12, blur0.getTextureColor().getHandle());


		/*
		// blur volumetric lighting vertically
		// Blur X
		blur1.bind();
		BlurX.Use();
		glBindTextureUnit(12, blur0.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 3);
		blur1.unbind();

		// blur volumetric lighting horizontally
		// Blur Y
		blur0.bind();
		BlurY.Use();
		glBindTextureUnit(12, blur1.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 3);
		blur0.unbind();
		glBindTextureUnit(12, blur0.getTextureColor().getHandle());
		/*
		// get volumetric lighting to full resolution using depth aware upsampling
		upsampleVL.Use();
		glBindTextureUnit(9, framebuffer1.getTextureColor().getHandle());
		glBindTextureUnit(12, blur0.getTextureColor().getHandle());
		framebuffer3.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		framebuffer3.unbind();
		glBindTextureUnit(12, framebuffer3.getTextureColor().getHandle());
		*/
	}

	
	// 3 - Apply SSAO
	if (state->ssao_)
	{
		//3.1 - render scene with ssao pattern
		glClearNamedFramebufferfv(ssao.getHandle(), GL_COLOR, 0, &(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]));
		ssao.bind();
			SSAO.Use();
			glBindTextureUnit(9, framebuffer1.getTextureDepth().getHandle());
			glBindTextureUnit(10, pattern);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		ssao.unbind();

		// 3.2 - blur SSAO image
		// Blur X
		blur.bind();
			BlurX.Use();
			glBindTextureUnit(9, ssao.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		blur.unbind();
		// Blur Y
		ssao.bind();
			BlurY.Use();
			glBindTextureUnit(9, blur.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		ssao.unbind();

		glClearNamedFramebufferfv(framebuffer2.getHandle(), GL_COLOR, 0, &(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]));

		// 3.3 - combine SSAO with rendered scene
		glViewport(0, 0, state->width, state->height);

		framebuffer2.bind();
			CombineSSAO.Use();
			glBindTextureUnit(9, framebuffer1.getTextureColor().getHandle());
			glBindTextureUnit(10, ssao.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		framebuffer2.unbind();
	}
	else
	{
		glBlitNamedFramebuffer(framebuffer1.getHandle(), framebuffer2.getHandle(), 0, 0, state->width, state->height,
			0, 0, state->width, state->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	// 4 - Apply Bloom
	if (state->bloom_)
	{

		// 4.1 - downscale for addiational blur and convert framebuffer to luminance
		luminance.bind();
			ToLuminance.Use();
			glBindTextureUnit(9, framebuffer2.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		luminance.unbind();
		glGenerateTextureMipmap(luminance.getTextureColor().getHandle());

		// 4.2 - compute light adaption (OpenGL memory model requires these memory barriers: https://www.khronos.org/opengl/wiki/Memory_Model )
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		lightAdapt.Use();
		glBindImageTexture(0, luminances[0]->getHandle(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
		glBindImageTexture(1, luminance1x1, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
		glBindImageTexture(2, luminances[1]->getHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		// 4.3 - filter bright spots from framebuffer
		brightPass.bind();
			BrightPass.Use();
			glBindTextureUnit(9, framebuffer2.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		brightPass.unbind();
		glBlitNamedFramebuffer(brightPass.getHandle(), bloom1.getHandle(), 0, 0, 256, 256, 0, 0, 256, 256, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// 4.4 - blur bright spots using ping pong buffers and a seperate blur in x and y direction
		for (int i = 0; i < 4; i++)
		{
			// blur x
			bloom0.bind();
				BlurX.Use();
				glBindTextureUnit(9, bloom1.getTextureColor().getHandle());
				glDrawArrays(GL_TRIANGLES, 0, 3);
			bloom0.unbind();
			// blur y
			bloom1.bind();
				BlurY.Use();
				glBindTextureUnit(9, bloom0.getTextureColor().getHandle());
				glDrawArrays(GL_TRIANGLES, 0, 3);
			bloom1.unbind();
		}

		// 4.5 - combine framebuffer with blurred image 
		glViewport(0, 0, state->width, state->height);

		CombineHDR.Use();
		glBindTextureUnit(9, framebuffer2.getTextureColor().getHandle());
		glBindTextureUnit(10, luminances[1]->getHandle());
		glBindTextureUnit(11, bloom1.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else
	{
		glBlitNamedFramebuffer(framebuffer2.getHandle(), 0, 0, 0, state->width, state->height, 0, 0, 
			state->width, state->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	
	// 5 - render HUD
	renderImage.Use();
	glEnable(GL_BLEND);
	glBindTextureUnit(9, hud);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	fontRenderer.print("CLOSED BETA FOOTAGE", state->width * 0.78f, state->height * 0.08f, .5f, glm::vec3(.5f, .5f, .5f));
	fontRenderer.print("all content is subject to change", state->width * 0.78f, state->height * 0.05f, .5f, glm::vec3(.5f, .5f, .5f));

	renderColor.Use();
	renderColor.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
	glViewport(state->width * 0.04, state->height * 0.06, state->width * 0.1, state->height * 0.08);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glViewport(0, 0, state->width, state->height);

	int items = state->collectedItems;
	std::string weightText = "Items: " + std::to_string(items);
	fontRenderer.print(weightText, state->width * 0.05f, state->height * 0.105f, .5f, glm::vec3(.95f, .86f, .6f));
	int money = (int)state->totalCash;
	std::string loot = "Loot: " + std::to_string(money) + "$";
	fontRenderer.print(loot, state->width * 0.05f, state->height * 0.07f, .5f, glm::vec3(.95f, .86f, .6f));

	if (state->won_)
	{
		renderColor.Use();
		renderColor.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		fontRenderer.print("You made it!", state->width * 0.36f, state->height * 0.48f, 2.0f, glm::vec3(.85f, .68f, .19f));
	}
	else if (state->lost_)
	{
		renderColor.Use();
		renderColor.setVec4("color", glm::vec4(0.710, 0.200, 0.180, 1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		fontRenderer.print(deathMsgs[0], state->width * 0.2f, state->height * 0.48f, 2.0f, glm::vec3(.0f, .0f, .0f));
	}
	if (state->displayCollectItemHint_) {
		fontRenderer.print("Click to collect", state->width * 0.42f, state->height * 0.60f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	glDisable(GL_BLEND);
}

/// @brief swaps framebuffers for light adaption computation
void Renderer::swapLuminance()
{
	std::swap(luminances[0], luminances[1]);
}


/// @brief an implementation of the glm::lookat() function, because this framework
/// makes it impossible to use, same code as in the Camera class
/// @param pos is the position aka eye or view of the camera
/// @param target to "look at" from the position
/// @param up is the up vetor of the world
/// @return a view matrix according to the input vectors
glm::mat4 Renderer::glmlookAt2(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - target);
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 translation;
	translation[3][0] = -pos.x;
	translation[3][1] = -pos.y;
	translation[3][2] = -pos.z;
	glm::mat4 rotation;
	rotation[0][0] = xaxis.x;
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x;
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x;
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;

	return rotation * translation;
}

/// @brief frees resources
Renderer::~Renderer()
{
	perframeData = nullptr;
	glDeleteTextures(1, &luminance1x1);
	glDeleteTextures(1, &pattern);
	glDeleteTextures(1, &hud);
}