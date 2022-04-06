#include "Renderer.h"

Renderer::Renderer(GlobalState& state, PerFrameData& pfdata, LightSources& sources)
{
	// initialize Renderer
	globalState = &state; // link global variables
	perframeData = &pfdata; // link per frame data
	lights = sources; // set lights and lightcounts for shaders
	buildShaderPrograms(); // build shader programs
	setRenderSettings();	// set effect settings 
	fillLightsources(); // binds lights to binding points in shader
	perframeBuffer.fillBuffer(pfdata); // load UBO to shader;

	prepareFramebuffers(); // for hdr rendering and tonemapping
	std::cout << "load enviroment map and process it.." << std::endl;
	IBL.loadHDR("assets/textures/cubemap/cellar.pic");
	std::cout << "load skybox and process it.." << std::endl;
	skyTex.loadHDR("assets/textures/cubemap/cloudy.hdr");
	glCreateVertexArrays(1, &emptyVAO);
	PBRShader.uploadIBL(IBL.getIrradianceID(),IBL.getPreFilterID(), IBL.getBdrfLutID(), IBL.getEnvironment());
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
	state.window_title = reader.Get("window", "title", "Greed");
	state.fov = reader.GetReal("camera", "fov", 60.0f);
	state.Znear = reader.GetReal("camera", "near", 0.1f);
	state.Zfar = reader.GetReal("camera", "far", 1000.0f);

	state.exposure_ = reader.GetReal("image", "exposure", 0.9f);
	state.maxWhite_ = reader.GetReal("image", "maxWhite", 1.07f);
	state.bloomStrength_ = reader.GetReal("image", "bloomStrength", 0.2f);
	state.adaptationSpeed_ = reader.GetReal("image", "lightAdaption", 0.1f);

	return state;

}

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

void Renderer::setRenderSettings()
{
	perframeData->bloom = glm::vec4(globalState->exposure_, globalState->maxWhite_,
		globalState->bloomStrength_,globalState->adaptationSpeed_);

	perframeData->normalMap = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::buildShaderPrograms()
{
	// build shader programms
	Shader pbrVert("assets/shaders/pbr/pbr.vert");
	Shader pbrFrag("assets/shaders/pbr/pbr.frag", glm::ivec3(lights.directional.size(), lights.point.size(), 0));
	PBRShader.buildFrom(pbrVert, pbrFrag);
	PBRShader.Use();

	Shader skyboxVert("assets/shaders/skybox/skybox.vert");
	Shader skyboxFrag("assets/shaders/skybox/skybox.frag");
	skyboxShader.buildFrom(skyboxVert, skyboxFrag);
	skyboxShader.Use();

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

	Shader lightAdaptComp("assets/shaders/lightAdaption/lightAdaption.comp");
	lightAdapt.buildFrom(lightAdaptComp);

	PBRShader.Use();
}

void Renderer::prepareFramebuffers() {

	glGenTextures(1, &luminance1x1);
	glTextureView(luminance1x1, GL_TEXTURE_2D, luminance.getTextureColor().getHandle(), GL_RGBA16F, 6, 1, 0, 1);

	const glm::vec4 startingLuminance(glm::vec3(0.0f), 1.0f);
	glTextureSubImage2D(luminance0.getHandle(), 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &startingLuminance[0]);
}

void Renderer::Draw(Level* level)
{
	
	glClearNamedFramebufferfv(framebuffer.getHandle(), GL_COLOR, 0, &(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]));
	glClearNamedFramebufferfi(framebuffer.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);

	perframeBuffer.Update(*perframeData);
	PBRShader.uploadIBL(IBL.getIrradianceID(), IBL.getPreFilterID(), IBL.getBdrfLutID(), IBL.getEnvironment());


	// 1. pass - render scene to framebuffer
	glEnable(GL_DEPTH_TEST);
	framebuffer.bind();

		// draw skybox (background)    
		skyboxShader.Use();
		glDepthMask(false);
		glBindVertexArray(emptyVAO);
		skyboxShader.setMat4("model", glm::mat4(0));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);

		// draw models
		glEnable(GL_CULL_FACE);
		PBRShader.Use();
		level->DrawGraph();
		glDisable(GL_CULL_FACE);

	framebuffer.unbind(); 
	glGenerateTextureMipmap(framebuffer.getTextureColor().getHandle());
	glTextureParameteri(framebuffer.getTextureColor().getHandle(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glDisable(GL_DEPTH_TEST);

	// 2. pass - downscale for addiational blur and convert framebuffer to luminance
	luminance.bind();
		ToLuminance.Use();
		glBindTextureUnit(9, framebuffer.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	luminance.unbind();
	glGenerateTextureMipmap(luminance.getTextureColor().getHandle());

	// 3. pass - compute light adaption (OpenGL memory model requires these memory barriers: https://www.khronos.org/opengl/wiki/Memory_Model )
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	lightAdapt.Use();
	glBindImageTexture(0, luminances[0]->getHandle(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, luminance1x1, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(2, luminances[1]->getHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

	// 4. pass - filter bright spots from framebuffer
	brightPass.bind();
		BrightPass.Use();
		glBindTextureUnit(9, framebuffer.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	brightPass.unbind();
	glBlitNamedFramebuffer(brightPass.getHandle(), bloom1.getHandle(), 0, 0, 256, 256, 0, 0, 256, 256, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	// 5. pass - blur bright spots using ping pong buffers and a seperate blur in x and y direction
	for (int i = 0; i < 4; i++)
	{
		// blur x
		bloom0.bind();
			BlurX.Use();
			glBindTextureUnit(9, bloom1.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 6);
		bloom0.unbind();
		// blur y
		bloom1.bind();
			BlurY.Use();
			glBindTextureUnit(9, bloom0.getTextureColor().getHandle());
			glDrawArrays(GL_TRIANGLES, 0, 6);
		bloom1.unbind();
	}

	// 6. pass - combine framebuffer with blurred image 
	glViewport(0, 0, globalState->width, globalState->height);

	if (globalState->bloom_)
	{
		CombineHDR.Use();
		glBindTextureUnit(9, framebuffer.getTextureColor().getHandle());
		glBindTextureUnit(10, luminances[1]->getHandle());
		glBindTextureUnit(11, bloom1.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else
	{
		glBlitNamedFramebuffer(framebuffer.getHandle(), 0, 0, 0, globalState->width, globalState->height, 0, 0, globalState->width, globalState->height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

void Renderer::swapLuminance()
{
	std::swap(luminances[0], luminances[1]);
}

Renderer::~Renderer()
{
	globalState = nullptr;
	perframeData = nullptr;
	glDeleteTextures(1, &luminance1x1);
}