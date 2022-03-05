#include "Material.h"


Material::Material(Texture* diff, Texture* spec, Cubemap* cube, glm::vec4 coeffs, float reflect)
{
	diffuse = diff;
	specular = spec;
	cubemap = cube;
	coefficients = coeffs;
	reflection = reflect;
}