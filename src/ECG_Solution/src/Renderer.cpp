#include "Renderer.h"

Renderer::Renderer(GlobalState& state, PerFrameData& pfdata, LightSources lights)
{
	globalState = &state; // link global variables
	perframeData = &pfdata; // link per frame data
	this->lights = lights; // set lights and lightcounts for shaders
	buildShaderPrograms(); // build shader program
	setRenderSettings();
	fillLightsources(); // binds lights to biding points in shader
	perframeBuffer.fillBuffer(pfdata); // load buffer to shader;
	perframesetBuffer.fillBuffer(perframsets);
	prepareFramebuffers();
}

GlobalState Renderer::loadSettings(GlobalState state)
{
	// init reader for ini files
	std::cout << "reading setting from settings.ini..." << std::endl;
	INIReader reader("assets/settings.ini");
	// load values from ini file
	// first param: section [window], second param: property name, third param: default value
	state.width = reader.GetInteger("window", "width", 800);
	state.height = reader.GetInteger("window", "height", 800);
	state.refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	state.fullscreen_ = reader.GetBoolean("window", "fullscreen", false);
	state.window_title = reader.Get("window", "title", "ECG 2021");
	state.fov = reader.GetReal("camera", "fov", 60.0f);
	state.Znear = reader.GetReal("camera", "near", 0.1f);
	state.Zfar = reader.GetReal("camera", "far", 100.0f);

	return state;

}

void Renderer::fillLightsources()
{
	// create Uniform Buffer Objects from light source struct vectors
	directionalLights.fillBuffer(lights.directional);
	positionalLights.fillBuffer(lights.point);
	spotLights.fillBuffer(lights.spot);

	// bind UBOs to bindings in shader
	PBRShader.bindLightBuffers(&directionalLights, &positionalLights, &spotLights);
	// set light source count variables
	PBRShader.setuInt("dLightCount", lights.directional.size());
	PBRShader.setuInt("pLightCount", lights.point.size());
	PBRShader.setuInt("sLightCount", lights.spot.size());
}

void Renderer::setRenderSettings()
{
	perframsets.bloom = glm::vec4(globalState->exposure_, globalState->maxWhite_, globalState->bloomStrength_, 1.0f);
}

void Renderer::buildShaderPrograms()
{
	// build shader programms
	Shader pbrVert("assets/shaders/pbr/pbr.vert");
	Shader pbrFrag("assets/shaders/pbr/pbr.frag", glm::ivec3(lights.directional.size(), lights.point.size(), lights.spot.size()));
	PBRShader.buildFrom(pbrVert, pbrFrag);
	PBRShader.Use();
	PBRShader.setTextures();

	Shader skyboxVert("assets/shaders/skybox/skybox.vert");
	Shader skyboxFrag("assets/shaders/skybox/skybox.frag");
	skyboxShader.buildFrom(skyboxVert, skyboxFrag);
	skyboxShader.Use();
	skyboxShader.setSkyboxTextures();

	Shader BrightPassVert("assets/shaders/BrightPass/BrightPass.vert");
	Shader BrightPassFrag("assets/shaders/BrightPass/BrightPass.frag");
	BrightPass.buildFrom(BrightPassVert, BrightPassFrag);

	Shader CombineHDRVert("assets/shaders/CombineHDR/CombineHDR.vert");
	Shader CombineHDRFrag("assets/shaders/CombineHDR/CombineHDR.frag");
	CombineHDR.buildFrom(CombineHDRVert, CombineHDRFrag);

	Shader BlurVert("assets/shaders/Blur/Blur.vert");
	Shader BlurXFrag("assets/shaders/Blur/BlurX.frag");
	Shader BlurYFrag("assets/shaders/Blur/BlurY.frag");
	BlurX.buildFrom(BlurVert, BlurXFrag);
	BlurY.buildFrom(BlurVert, BlurYFrag);

	Shader LuminanceVert("assets/shaders/toLuminance/toLuminance.vert");
	Shader LuminanceFrag("assets/shaders/toLuminance/toLuminance.frag");
	ToLuminance.buildFrom(LuminanceVert, LuminanceFrag);

	PBRShader.Use();
}

void Renderer::prepareFramebuffers() {

	glGenTextures(1, &luminance1x1);
	glTextureView(luminance1x1, GL_TEXTURE_2D, luminance.getTextureColor().getHandle(), GL_R16F, 6, 1, 0, 1);
	const GLint Mask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
	glTextureParameteriv(luminance1x1, GL_TEXTURE_SWIZZLE_RGBA, Mask);

}

void Renderer::Draw(std::vector <Mesh*> models, Mesh& skybox)
{
	
	glClearNamedFramebufferfv(framebuffer.getHandle(), GL_COLOR, 0, &(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]));
	glClearNamedFramebufferfi(framebuffer.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);

	perframeBuffer.Update(*perframeData);
	perframesetBuffer.Update(perframsets);


	// first pass
	glEnable(GL_DEPTH_TEST);
	framebuffer.bind();

	// draw skybox    
	skyboxShader.Use();
	glDepthFunc(GL_LEQUAL);
	skyboxShader.DrawSkybox(skybox);
	glDepthFunc(GL_LESS);

	// draw models
	PBRShader.Use();
	for (auto& model : models)
	{
		PBRShader.Draw(*model);
	}

	framebuffer.unbind();

	glDisable(GL_DEPTH_TEST);
	// 2.1 Extract bright areas
	brightPass.bind();
	BrightPass.Use();
	glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	brightPass.unbind();

	// 2.2 Downscale and convert to luminance
	luminance.bind();
	ToLuminance.Use();
	glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	luminance.unbind();
	glGenerateTextureMipmap(luminance.getTextureColor().getHandle());

	glBlitNamedFramebuffer(brightPass.getHandle(), bloom2.getHandle(), 0, 0, 256, 256, 0, 0, 256, 256, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
	for (int i = 0; i != 4; i++)
	{
		// 2.3 Blur X
		bloom1.bind();
		BlurX.Use();
		glBindTextureUnit(0, bloom2.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		bloom1.unbind();
		// 2.4 Blur Y
		bloom2.bind();
		BlurY.Use();
		glBindTextureUnit(0, bloom1.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		bloom2.unbind();
	}

	// 3. Apply tone mapping
	glViewport(0, 0, globalState->width, globalState->height);

	if (globalState->bloom_)
	{
		//glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, sizeof(g_HDRParams), &g_HDRParams);

		CombineHDR.setFloat("exposure", globalState->exposure_);
		CombineHDR.setFloat("maxWhite", globalState->maxWhite_);
		CombineHDR.setFloat("bloomStrength", globalState->bloomStrength_);
		CombineHDR.Use();
		glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
		glBindTextureUnit(1, luminance1x1);
		glBindTextureUnit(2, bloom2.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else
	{
		glBlitNamedFramebuffer(framebuffer.getHandle(), 0, 0, 0, globalState->width, globalState->height, 0, 0, globalState->width, globalState->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}


Renderer::~Renderer()
{
	globalState = nullptr;
	perframeData = nullptr;
}