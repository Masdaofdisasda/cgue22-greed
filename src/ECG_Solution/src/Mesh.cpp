#include "Mesh.h"
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>


// skybox geometry constructor
Mesh::Mesh(float size)
{
	material = nullptr;
	float w = size, h = size, d = size;
	glm::vec3 color = glm::vec3(0.0f);

	// normals
	glm::vec3 back = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 left = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 top = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 bottom = glm::vec3(0.0f, -1.0f, -0.0f);

	// uv coordinates
	glm::vec2 botleft = glm::vec2(0.0f, 0.0f);
	glm::vec2 botright = glm::vec2(1.0f, 0.0f);
	glm::vec2 topleft = glm::vec2(0.0f, 1.0f);
	glm::vec2 topright = glm::vec2(1.0f, 1.0f);

	vertices = {
		// 8 original cube vertices
		//Vertex{glm::vec3(	-0.5f*w,	-0.5f*h,	-0.5f*d),	color}, // A
		//Vertex{glm::vec3(	0.5f*w,		-0.5f*h,	-0.5f*d),	color}, // B
		//Vertex{glm::vec3(	0.5f*w,		0.5f*h,		-0.5f*d),	color}, // C
		//Vertex{glm::vec3(	-0.5f*w,	0.5f*h,		-0.5f*d),	color}, // D
		//Vertex{glm::vec3(	-0.5f*w,	-0.5f*h,	0.5f*d),	color}, // E
		//Vertex{glm::vec3(	0.5f*w,		-0.5f*h,	0.5f*d),	color}, // F
		//Vertex{glm::vec3(	0.5f*w,		0.5f*h,		0.5f*d),	color}, // G
		//Vertex{glm::vec3(	-0.5f*w,	0.5f*h,		0.5f*d),	color}, // H

		//	postion		x * width	y * height	z * depth	normal	uv
		// back
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	back, botleft}, // A0
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	back, topright}, // B1
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	back, botright}, // C2
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	back, topleft}, // D3

		// front
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	front, botleft}, // E4
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	front, topright}, // F5
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	front, botright}, // G6
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	front, topleft}, // H7

		// right
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	right, botleft}, // B8
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	right, topleft}, // C9
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	right, botright}, // F10
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	right, topright}, // G11

		// left
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	left, botleft}, // A12
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	left, topleft}, // D13
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	left, botright}, // E14
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	left, topright}, // H15

		// top
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	top, botright}, // C16
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	top, botleft}, // D17
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	top, topright}, // G18
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	top, topleft}, // H19

		// bottom
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	bottom, botleft}, // A20
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	bottom, botright}, // B21
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	bottom, topleft}, // E22
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	bottom, topright}, // F23

	};

	indices = {

		3,0,1,		2,3,1,		//back
		6,5,4,		7,6,4,		//front
		9,8,10,		11,9,10,	//right
		15,14,12,	13,15,12,	//left
		19,17,16,	18,19,16,	//top
		20,22,23,	21,20,23,	//bottom

	};

	reverseIndices();
	PrepareBuffer();

	std::cout << std::endl;
}

// obj loader
Mesh::Mesh(const char* fileName, Material* mat)
{
	material = mat;

	Assimp::Importer importer;
	const aiScene* modelScene;
	const aiNode* modelNode;
	const aiMesh* modelMesh;
	const aiFace* modelFace;

	std::vector<const aiNode*> nodeBuff;
	unsigned int numNodeBuff;

	
	modelScene = importer.ReadFile(fileName, 
		aiProcess_MakeLeftHanded | 
		aiProcess_FlipWindingOrder | 
		aiProcess_FlipUVs | 
		aiProcess_PreTransformVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInvalidData |
		aiProcess_ValidateDataStructure | 0);


	if (!modelScene)
	{
		std::cerr << "ERROR: Couldn't load model " << fileName  << std::endl;
	}

	bool repeat = true;

	nodeBuff.push_back(modelScene->mRootNode);

	// I raise all nodes tree to the root level 
	while (repeat)
	{
		for (unsigned int a = 0; a < nodeBuff.size(); a++)
		{
			modelNode = nodeBuff.at(a);
			if (modelNode->mNumChildren > 0)
				for (unsigned int c = 0; c < modelNode->mNumChildren; c++)
				{
					nodeBuff.push_back(modelNode->mChildren[c]);

				}

			else repeat = false;
		}
	}

	// Get node information from the root level (all nodes)
	for (unsigned int a = 0; a < nodeBuff.size(); a++)
	{
		modelNode = nodeBuff.at(a);

		if (modelNode->mNumMeshes > 0)
			for (unsigned int b = 0; b < modelNode->mNumMeshes; b++) {
				const aiMesh* mesh = modelScene->mMeshes[b];
				aiFace* face;


				for (unsigned int v = 0; v < mesh->mNumVertices; v++)
				{
					glm::vec3 vertex(0.0f);
					glm::vec2 uv(0.0f);
					glm::vec3 normal(0.0f);


					if (mesh->HasFaces())
					{
						vertex = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
					}

					if (mesh->HasTextureCoords(0)) {
						uv = glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
					}

					if (mesh->HasNormals())
					{
						normal = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
					}
					

					vertices.push_back(Vertex{ vertex,	normal, uv });

				}

				for (unsigned int f = 0; f < mesh->mNumFaces; f++)
				{
					face = &mesh->mFaces[f];
					indices.push_back(face->mIndices[0]);
					indices.push_back(face->mIndices[1]);
					indices.push_back(face->mIndices[2]);
				}
			}
	}

	PrepareBuffer();
}


Mesh::Mesh()
{
	glGenVertexArrays(1, &vao_ID);
}

void Mesh::PrepareBuffer()
{

	reverseIndices();

	glGenVertexArrays(1, &vao_ID);
	glBindVertexArray(vao_ID);

	VBO vbo(vertices);
	EBO ebo(indices);

	vbo.Bind();
	LinkAttrib(0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	LinkAttrib(1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	LinkAttrib(2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::LinkAttrib(GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset)
{
	glVertexAttribPointer(layout, numComp, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
}


void Mesh::setMatrix(glm::vec3 translate, float degree, glm::vec3 axis, glm::vec3 scale)
{
	glm::mat4 T = glm::translate(glm::mat4(1), translate);
	glm::mat4 R = glm::rotate(glm::mat4(1), glm::radians(degree), axis);
	glm::mat4 S = glm::scale(glm::mat4(1), scale);
	model = T*R*S;
}


Material* Mesh::getMaterial()
{
	return material;
}

int Mesh::getIndicesSize()
{
	return indices.size();
}

void Mesh::BindVAO()
{
	glBindVertexArray(vao_ID);
}

void Mesh::reverseIndices()
{
	std::reverse(indices.begin(), indices.end());
}

int Mesh::getVerticeAmount() {
	return vertices.size();
}

btScalar* Mesh::getVerticeCoordinates() {
	int amountOfVertices = getVerticeAmount();
	btScalar* coordinates = new btScalar[amountOfVertices*3];
	for (int i = 0; i < amountOfVertices; i++) {
		glm::vec3 pos = vertices[i].position;
		coordinates[i * 3] = btScalar(pos.x);
		coordinates[i * 3 + 1] = btScalar(pos.y);
		coordinates[i * 3 + 2] = btScalar(pos.z);
	}

	return coordinates;
}
