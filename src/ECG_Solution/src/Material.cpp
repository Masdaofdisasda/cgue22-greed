#include "Material.h"


Material::Material(Texture* alb, Texture* norm, Texture* metal, Texture* rough, Texture* ao, Cubemap* cube)
{
	albedo = alb;
	normal = norm;
	metallic = metal;
	roughness = rough;
	ambientocclusion = ao;
	cubemap = cube;
}