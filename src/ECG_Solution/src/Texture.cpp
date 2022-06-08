#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <thread>

GLuint Texture::defaults_[7] = { 0,0,0,0,0,0,0 };
uint64_t Texture::defaults64_[7] = { 0,0,0,0,0,0,0 };

Texture::Texture(const GLenum type, const int width, const int height, const GLenum internal_format)
	: type_(type)
{
	glCreateTextures(type, 1, &tex_id_);
	glTextureParameteri(tex_id_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(tex_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex_id_, get_num_mip_map_levels_2d(width, height), internal_format, width, height);
}

GLuint Texture::load_texture(const char* tex_path)
{
	GLuint handle = 0;
	// generate texture
	glCreateTextures(GL_TEXTURE_2D, 1, &handle);

	glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	gli::texture gli_tex = gli::load_ktx(tex_path);

	if (!gli_tex.empty())
	{
		gli_tex = flip(gli_tex);
		const gli::gl GL(gli::gl::PROFILE_KTX);
		gli::gl::format const format = GL.translate(gli_tex.format(), gli_tex.swizzles());
		const glm::tvec3<GLsizei> extent(gli_tex.extent(0));
		const int w = extent.x;
		const int h = extent.y;
		//const int numMipmaps = get_num_mip_map_levels_2d(w, h);
		glTextureStorage2D(handle, 1, format.Internal, w, h);
		glTextureSubImage2D(handle, 0, 0, 0, w, h, format.External, format.Type, gli_tex.data(0, 0, 0));
	}
	else
	{
		std::cout << "could not load texture" << tex_path << std::endl;
	}

	return handle;
}

void Texture::load_texture_mt(const char* tex_path, GLuint handles[], uint64_t bindless[])
{
	
	gli::texture img_data[7];

	img_data[0] = gli::load_ktx(append(tex_path, "/albedo.ktx"));
	img_data[1] = gli::load_ktx(append(tex_path, "/normal.ktx"));
	img_data[2] = gli::load_ktx(append(tex_path, "/metal.ktx"));
	img_data[3] = gli::load_ktx(append(tex_path, "/rough.ktx"));
	img_data[4] = gli::load_ktx(append(tex_path, "/ao.ktx"));
	img_data[5] = gli::load_ktx(append(tex_path, "/emissive.ktx"));
	img_data[6] = gli::load_ktx(append(tex_path, "/height.ktx"));

	for (size_t i = 0; i < 7; i++)
	{
		if (!img_data[i].empty())
		{
			//img_data[i] = flip(img_data[i]);
			const gli::gl GL(gli::gl::PROFILE_KTX);
			gli::gl::format const format = GL.translate(img_data[i].format(), img_data[i].swizzles());
			const glm::tvec3<GLsizei> extent(img_data[i].extent(0));
			const int w = extent.x;
			const int h = extent.y;
			const int mipMapLevel = get_num_mip_map_levels_2d(w, h);
			glCreateTextures(GL_TEXTURE_2D, 1, &handles[i]);

			glTextureParameteri(handles[i], GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(handles[i], GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(handles[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(handles[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTextureStorage2D(handles[i], mipMapLevel, format.Internal, w, h);
			glTextureSubImage2D(handles[i], 0, 0, 0, w, h, format.External, format.Type, img_data[i].data(0, 0, 0));
			glGenerateTextureMipmap(handles[i]);
			glTextureParameteri(handles[i], GL_TEXTURE_MAX_LEVEL, mipMapLevel - 1);
			glTextureParameteri(handles[i], GL_TEXTURE_MAX_ANISOTROPY, 16);
			bindless[i] = glGetTextureHandleARB(handles[i]);
			glMakeTextureHandleResidentARB(bindless[i]);
		}
		else
		{
#ifdef _DEBUG
			std::cout << "could not load texture nr " <<i << " from " << tex_path << "\n"<<"using fallback...\n";
#endif
			if (defaults_[0] == 0)
			{
				load_texture_mt("../assets/textures/default", defaults_, defaults64_);
			}
			handles[i] = defaults_[i];
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::load_3dlut(const char* tex_path)
{
	// Load .CUBE file 
	printf("Loading LUT file \n");
	FILE* file = fopen(tex_path, "r");

	if (file == nullptr) {
		printf("Could not open file \n");
		return false;
	}

	float* lut_data = nullptr;
	int size = 0;

	// Iterate through lines
	while (true) {
		char line[128];
		fscanf(file, "%128[^\n]\n", line);



		if (strcmp(line, "#LUT size") == 0) {
			// Read LUT size
			fscanf(file, "%s %i\n", &line, &size);
			lut_data = new float[size * size * size * 3];
		}
		else if (strcmp(line, "#LUT data points") == 0) {

			// Read colors
			int row = 0;
			do {
				float r, g, b;
				fscanf(file, "%f %f %f\n", &r, &g, &b);
				lut_data[row * 3 + 0] = r;
				lut_data[row * 3 + 1] = g;
				lut_data[row * 3 + 2] = b;
				row++;
			} while (row < size * size * size);
			break;
		}
	}
	fclose(file);

	// Create texture
	GLuint texture;
	glCreateTextures(GL_TEXTURE_3D, 1, &texture);

	// Load data to texture
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,size, size, size,0,GL_RGB,GL_FLOAT,lut_data);

	// Set sampling parameters
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_R, GL_CLAMP);

	return texture;

}

int Texture::get_num_mip_map_levels_2d(const int w, const int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}

GLuint Texture::get_ssao_kernel()
{
	int size = 4;
	std::vector<GLfloat> randDirections(3 * size * size);
	for (int i = 0; i < size * size; i++) {
		glm::vec3 v = uniform_circle();
		randDirections[i * 3 + 0] = v.x;
		randDirections[i * 3 + 1] = v.y;
		randDirections[i * 3 + 2] = v.z;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, size, size);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGB, GL_FLOAT, randDirections.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	return tex;
}

GLuint Texture::get_3D_noise(int size, float base_freq) // todo
{
	int width = size;
	int height = size;
	int depth = size;
	float persistence = 0.5f;

	printf("Generating noise texture...\n");

	GLfloat* data = static_cast<GLfloat*>(malloc(width * height * depth * 8));

	float xFactor = 1.0f / (width - 1);
	float yFactor = 1.0f / (height - 1);
	float zFactor = 1.0f / (depth - 1);

	for (int slice = 0; slice < depth; slice++) {
		for (int row = 0; row < height; row++) {
			for (int col = 0; col < width; col++) {
				float x = xFactor * col;
				float y = yFactor * row;
				float z = zFactor * slice;
				float sum = 0.0f;
				float freq = base_freq;
				float persist = base_freq;
				for (int oct = 0; oct < 4; oct++) {
					glm::vec3 p(x * freq, y * freq, z * freq);

					float val = 0.0f;
					val = glm::perlin(p, glm::vec3(freq)) * persist;

					sum += val;

					float result = (sum + 1.0f) / 2.0f;

					// Clamp strictly between 0 and 1
					result = result > 1.0f ? 1.0f : result;
					result = result < 0.0f ? 0.0f : result;

					// Store in texture
					data[(slice * size * size + row * size + col) + oct] = result;
					freq *= 2.0f;
					persist *= persistence;
				}
			}
		}
	}

	GLuint tex_id_;
	glCreateTextures(GL_TEXTURE_3D, 1, &tex_id_);
	glTextureStorage3D(tex_id_, 1, GL_R32F, width, height, depth);
	glTextureSubImage3D(tex_id_, 0, 0, 0, 0, width, height, depth, GL_RED, GL_FLOAT, data);
	glTextureParameteri(tex_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(tex_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(tex_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(tex_id_, GL_TEXTURE_WRAP_R, GL_REPEAT);

	free(data);

	return tex_id_;
}

void Texture::destory_texture_mt(GLuint handles[], uint64_t bindless[])
{
	for (size_t i = 0; i < 7; i++)
	{
		if (defaults_[i] != handles[i] && glIsTexture(handles[i]))
		{
			glMakeTextureHandleNonResidentARB(bindless[i]);
			glDeleteTextures(1, &handles[i]);
		}
	}
}

void Texture::destroy_defaults()
{
	for (size_t i = 0; i < 7; i++)
	{
		glMakeTextureHandleNonResidentARB(defaults64_[i]);
		glDeleteTextures(1, &defaults_[i]);
	}
}

std::string Texture::append(const char* tex_path, const char* tex_type)
{
	char c[100];
	strcpy(c, tex_path);
	std::string result = strcat(c, tex_type);
	return result;
}