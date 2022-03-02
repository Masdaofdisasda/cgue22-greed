#include "Mesh.h"
#include <algorithm>

// box geometry constructor
Mesh::Mesh(float w, float h, float d, float r, float g, float b)
{
	if (w <= 0.0f || h <= 0.0f || d <= 0.0f)
	{
		std::cerr << "ERROR: Can't generate box mesh with negative dimensions" << std::endl;
		return;
	}

	std::cout << "generate box with:" << std::endl;
	std::cout << "width = " << w << std::endl;
	std::cout << "height = " << h << std::endl;
	std::cout << "depth = " << d << std::endl;
	std::cout << "color = " << r << "f " << g << "f " << b << "f " << std::endl;
	glm::vec3 color = glm::vec3(r, g, b);

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

		//	postion		x * width	y * height	z * depth	color	normal	uv
		// back
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	back, botleft}, // A0
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	back, topright}, // B1
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	back, botright}, // C2
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	back, topleft}, // D3

		// front
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	front, botleft}, // E4
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	front, topright}, // F5
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	front, botright}, // G6
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	front, topleft}, // H7

		// right
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	right, botleft}, // B8
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	right, topleft}, // C9
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	right, botright}, // F10
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	right, topright}, // G11

		// left
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	left, botleft}, // A12
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	left, topleft}, // D13
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	left, botright}, // E14
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	left, topright}, // H15

		// top
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	top, botright}, // C16
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	top, botleft}, // D17
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	top, topright}, // G18
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	top, topleft}, // H19

		// bottom
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	bottom, botleft}, // A20
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	bottom, botright}, // B21
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	bottom, topleft}, // E22
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	bottom, topright}, // F23

	};

	std::cout << "box geometry has " << vertices.size() << " vertices" << std::endl;
	indices = {

		3,0,1,		2,3,1,		//back
		6,5,4,		7,6,4,		//front
		9,8,10,		11,9,10,	//right
		15,14,12,	13,15,12,	//left
		19,17,16,	18,19,16,	//top
		20,22,23,	21,20,23,	//bottom

	};

	std::cout << "and consists of " << indices.size()/3 << " triangles" << std::endl;

	PrepareBuffer();

	std::cout << std::endl;
}

// box geometry constructor
Mesh::Mesh(float size)
{
	std::cout << "generate skybox with:" << std::endl;
	std::cout << "size = " << size << std::endl;
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

		//	postion		x * width	y * height	z * depth	color	normal	uv
		// back
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	back, botleft}, // A0
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	back, topright}, // B1
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	back, botright}, // C2
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	back, topleft}, // D3

		// front
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	front, botleft}, // E4
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	front, topright}, // F5
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	front, botright}, // G6
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	front, topleft}, // H7

		// right
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	right, botleft}, // B8
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	right, topleft}, // C9
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	right, botright}, // F10
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	right, topright}, // G11

		// left
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	left, botleft}, // A12
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	left, topleft}, // D13
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	left, botright}, // E14
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	left, topright}, // H15

		// top
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	-0.5f * d),	color,	top, botright}, // C16
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	-0.5f * d),	color,	top, botleft}, // D17
		Vertex{glm::vec3(0.5f * w,	0.5f * h,	0.5f * d),	color,	top, topright}, // G18
		Vertex{glm::vec3(-0.5f * w,	0.5f * h,	0.5f * d),	color,	top, topleft}, // H19

		// bottom
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	-0.5f * d),	color,	bottom, botleft}, // A20
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	-0.5f * d),	color,	bottom, botright}, // B21
		Vertex{glm::vec3(-0.5f * w,	-0.5f * h,	0.5f * d),	color,	bottom, topleft}, // E22
		Vertex{glm::vec3(0.5f * w,	-0.5f * h,	0.5f * d),	color,	bottom, topright}, // F23

	};

	std::cout << "box geometry has " << vertices.size() << " vertices" << std::endl;
	indices = {

		3,0,1,		2,3,1,		//back
		6,5,4,		7,6,4,		//front
		9,8,10,		11,9,10,	//right
		15,14,12,	13,15,12,	//left
		19,17,16,	18,19,16,	//top
		20,22,23,	21,20,23,	//bottom

	};

	std::cout << "and consists of " << indices.size() / 3 << " triangles" << std::endl;

	reverseIndices();
	PrepareBuffer();

	std::cout << std::endl;
}

// cylinder geometry constructor
Mesh::Mesh(int s, float h, float rad, float r, float g, float b)
{
	if (s < 3 || h <= 0.0f || rad <= 0.0f)
	{
		std::cerr <<"ERROR: Can't generate cylinder mesh with negative dimensions or less than 3 segments" << std::endl;
		return;
	}

	std::cout << "generating cylinder with:" << std::endl;
	std::cout << "segments = " << s << std::endl;
	std::cout << "height = " << h << std::endl;
	std::cout << "radius = " << r << std::endl;
	std::cout << "color = " << r << "f " << g << "f " << b << "f " << std::endl;
	glm::vec3 color = glm::vec3(r, g, b);
	const float PI = 3.1415926f;

	std::cout << "generating cylinder vertices..." << std::endl;

	//--------TOP CIRCLE--------//
	glm::vec3 top = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(Vertex{ glm::vec3(0.0f, h / 2, 0.0f), color , top, glm::vec2(0.5f,0.5f)}); // top circle center

	for(int i = 0; i < s; i++){ // create vertices
		float theta = (float)i/s * PI * 2.0f;
		vertices.push_back(Vertex{ glm::vec3(rad* cos(theta), h/2, rad*sin(theta)),color, top , glm::vec2(0.5f*cos(theta)+0.5f,0.5f * sin(theta) + 0.5f) });
	}
	
	for(int i = 1; i < s; i++){ // assign indices 
		//		next						current					center
		indices.push_back(i + 1),	indices.push_back(0),	indices.push_back(i);
	}
	//		first					last					center
	indices.push_back(1),	indices.push_back(0),	indices.push_back(s); // connect start & end vertices
	


	//--------BOTTOM CIRCLE--------//
	glm::vec3 bottom = glm::vec3(0.0f, -1.0f, 0.0f);
	vertices.push_back(Vertex{ glm::vec3(0.0f, -h / 2, 0.0f), color, bottom, glm::vec2(0.5f,0.5f) }); // bottom circle center
	for(int i = 0; i < s; i++){ // create vertices 
		float theta = (float)i / s * PI * 2.0f;
		vertices.push_back(Vertex{ glm::vec3(rad * cos(theta), -h / 2, rad * sin(theta)),color, bottom, glm::vec2(0.5f * cos(theta) + 0.5f,0.5f * sin(theta) + 0.5f) });
	}
	
	for (int i = s+2; i < s*2+1; i++) { // assign indices
		//		current					next						center
		indices.push_back(i),	indices.push_back(s+1),	indices.push_back(i + 1);
	}
	//		last					first					center
	indices.push_back(1+s*2), indices.push_back(s+1), indices.push_back(s + 2); // connect start & end vertices
	
	int offset = 2 * s + 2;

	
	//--------MIDDLE TUBE--------//
	for (int i = 0; i < s; i++) { // create vertices
		float theta = (float)i / s * PI * 2.0f;
		glm::vec3 position = glm::vec3(rad * cos(theta), h / 2, rad * sin(theta));
		glm::vec3 normal = glm::normalize(glm::vec3(cos(theta), 0.0f, sin(theta)));
		glm::vec2 uv = glm::vec2(theta/ (PI * 2.0f), 1);
		vertices.push_back(Vertex{ position ,color, normal, uv });
	}
	for (int i = 0; i < s; i++) { // create vertices
		float theta = (float)i / s * PI * 2.0f;
		glm::vec3 position = glm::vec3(rad * cos(theta), -h / 2, rad * sin(theta));
		glm::vec3 normal = glm::normalize(glm::vec3(cos(theta), 0.0f, sin(theta)));
		glm::vec2 uv = glm::vec2(theta / (PI * 2.0f), 0);
		vertices.push_back(Vertex{ position ,color, normal, uv });
	}
	for (int i = offset; i < offset + s-1; i++){ // 2s+2 vertices
		int bl = i,		br = bl + 1,		tl = bl + s,		tr = tl + 1;	//					tl---tr
		indices.push_back(bl), indices.push_back(tl), indices.push_back(br);	//top triangle		|  \ |
		indices.push_back(tl), indices.push_back(tr), indices.push_back(br);	//bottom triangle	bl---br
	}
	indices.push_back(offset+s-1),	indices.push_back(offset+s),  indices.push_back(offset); // connect start & end top triangle
	indices.push_back(offset+s),	indices.push_back(offset + s - 1),	indices.push_back(offset - 1); // connect start & end bottom triangle
	

	std::cout << "cylinder geometry has " << vertices.size() << " vertices" << std::endl;
	std::cout << "and consists of " << indices.size() / 3 << " triangles" << std::endl;

	// load arrays into buffer
	PrepareBuffer();


	std::cout << std::endl;
}

// sphere geometry constructor
Mesh::Mesh(int longs, int lats, float rad, float r, float g, float b)
{
	if (longs < 3 || lats < 3 || rad <= 0.0f)
	{
		std::cerr << "ERROR: Can't generate sphere mesh with negative dimensions or less than 3 segments" << std::endl;
		return;
	}

	std::cout << "generating sphere with:" << std::endl;
	std::cout << "longitude segments = " << longs << std::endl;
	std::cout << "latitude segments = " << lats << std::endl;
	std::cout << "sphere radius = " << rad << std::endl;
	std::cout << "color = " << r << "f " << g << "f " << b << "f " << std::endl;

	glm::vec3 color = glm::vec3(r, g, b);
	const float PI = 3.1415926f;
	r = rad; // prevents confusion and errors

	std::cout << "generating sphere vertices..." << std::endl;

	// 1 - generate vertices
	// move vector from center to the north pole of the sphere with distance r
	// rotate vector along an halfcircle and place lats number of vertices 
	// along the path until the vector points at the south pole
	// create longs number of half circles and place them evenly around the center
	// creating a sphere
	// north and south pole are only added at the end to avoid duplicates

	for (int i = 0; i < longs; i++)
	{
		float Theta = (float)i / longs * PI * 2.0f;
		glm::mat4 RMh = glm::rotate(glm::mat4(1.0f), Theta, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 poleN = glm::vec3(0.0f, r, 0.0f);

		for (int j = 1; j < lats; j++)
		{
			float theta = (float)j / lats * PI;
			glm::mat4 RMv = glm::rotate(glm::mat4(1.0f),theta, glm::vec3(0.0f, 0.0f, 1.0f));

			glm::vec4 finalPos = RMh * RMv * glm::vec4(poleN, 1.0f);
			glm::vec3 position = glm::vec3(finalPos.x, finalPos.y, finalPos.z);
			glm::vec3 normal = glm::normalize(glm::vec3(finalPos.x, finalPos.y, finalPos.z));
			glm::vec3 p = glm::normalize(position - glm::vec3(0.0f));
			glm::vec2 uv = glm::vec2(atan2(p.x,p.z)/ (2.0f*PI) + 0.5f, p.y * 0.5f + 0.5f);

			vertices.push_back(Vertex{ position,color, normal, uv});
		}

	}
	vertices.push_back(Vertex{ glm::vec3(0.0f, r, 0.0f),color });
	vertices.push_back(Vertex{ glm::vec3(0.0f, -r, 0.0f),color });

	int south = vertices.size()-1;
	int north = south - 1;

	// 2 - calculate indices
	// select first half circle and connect all vertices with its neighboring vertices
	// for the first vertex in every half circle use north vertex, for last one use south vertex
	// repeat one revolution and connect the last half circle with the first one in a
	// similar fashion as the ones before

	for (int i = 0; i < longs; i++)
	{
		if (i < longs-1) // connect half circle i to half circle i+1
		{

			for (int j = 0; j < lats -1; j++)
			{
				int cur, nex, nCur, nNex;
				if (j < lats - 2)
				{
					if (j == 0) // first vertex is north pole
					{
						cur = north, nex = i * (lats - 1), nCur = north, nNex = (i + 1) * (lats - 1);
						indices.push_back(nNex), indices.push_back(nex), indices.push_back(nCur);	//first triangle
					}
					// connect vertex j and j +1 with its neighbor
					cur = i * (lats - 1) + j, nex = cur + 1, nCur = (i + 1) * (lats - 1) + j, nNex = nCur + 1;
					indices.push_back(nCur), indices.push_back(nex), indices.push_back(cur);	//first triangle
					indices.push_back(nNex), indices.push_back(nex), indices.push_back(nCur);	//second triangle
				}
				else{ // last vertex is south pole

					cur = i * (lats - 1) + j, nex = south, nCur = (i + 1) * (lats - 1) + j, nNex = south;
					indices.push_back(nCur), indices.push_back(nex), indices.push_back(cur);	//last triangle
				}
			}
		}else{ // connect last half circle with first half circle
			for (int j = 0; j < lats - 1; j++)
			{
				int cur, nex, nCur, nNex;
				if (j < lats - 2)
				{
					if (j == 0) // first vertex is north pole
					{
						cur = north, nex = i * (lats - 1), nCur = north, nNex = 0;
						indices.push_back(nNex), indices.push_back(nex), indices.push_back(nCur);	//first triangle
					}
					// connect vertex j and j +1 with its neighbor
					cur = i * (lats - 1) + j, nex = cur + 1, nCur = j, nNex = nCur + 1;
					indices.push_back(nCur), indices.push_back(nex), indices.push_back(cur);	//first triangle
					indices.push_back(nNex), indices.push_back(nex), indices.push_back(nCur);	//second triangle
				}
				else { // last vertex is south pole

					cur = i * (lats - 1) + j, nex = south, nCur = j, nNex = south;
					indices.push_back(nCur), indices.push_back(nex), indices.push_back(cur);	//last triangle
				}
			}
		}
	}

	std::cout << "sphere geometry has " << vertices.size() << " vertices" << std::endl;
	std::cout << "and consists of " << indices.size() / 3 << " triangles" << std::endl;

	PrepareBuffer();

	std::cout << std::endl;
}


void Mesh::PrepareBuffer()
{

	reverseIndices();
	VAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttrib(&VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(&VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(&VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(&VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}


glm::mat4 Mesh::translate(glm::vec3 position)
{
	return  model = glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Mesh::rotate(float theta, glm::vec3 axis)
{
	return  model = glm::rotate(glm::mat4(1.0f),glm::radians(theta) , axis);
}

void Mesh::Draw(Shader shader)
{
	// load model matrix on shader
	shader.setMat4("model", model);

	// use texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	// load mesh on shader
	uploadMaterial(shader);

	// draw meshgl
	VAO.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setMaterial(const char* texFile, const char* specFile, const char* cubeFile, glm::vec4 coeff, float reflect)
{
	loadTexture(texFile, specFile, cubeFile);
	coefficients = coeff;
	reflection = reflect;
}

void Mesh::uploadMaterial(Shader shader)
{
	shader.setInt("material.tex0", 0);
	shader.setInt("material.tex1", 1);
	shader.setInt("material.tex2", 2);
	shader.setVec4("material.coefficients", coefficients);
	shader.setFloat("material.reflection", reflection);
}

void Mesh::reverseIndices()
{
	std::reverse(indices.begin(), indices.end());
}

void Mesh::loadTexture(const char* tex, const char* spec, const char* cube)
{
	// generate diffuse texture
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	DDSImage imgTex = loadDDS(tex);

	if (imgTex.size > 0)
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, imgTex.format, imgTex.width, imgTex.height, 0, imgTex.size, imgTex.data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "could not load texture" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// generate specular map
	glGenTextures(1, &specular);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	DDSImage imgSpec = loadDDS(spec);

	if (imgSpec.size > 0)
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, imgSpec.format, imgSpec.width, imgSpec.height, 0, imgSpec.size, imgSpec.data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "could not load texture" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// generate cube map
	glGenTextures(1, &cubemap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap); 

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	char c[100];
	strcpy(c, cube);

	const char* location = strcat(c, "/negx.dds");
	DDSImage imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, cube);
	location = strcat(c, "/negy.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, cube);
	location = strcat(c, "/negz.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, cube);
	location = strcat(c, "/posx.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, cube);
	location = strcat(c, "/posy.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, cube);
	location = strcat(c, "/posz.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	// debugging black cubemap
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "ERROR" << std::endl;
		std::cout << err << std::endl;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

/*
Mesh::~Mesh()
{
	std::cout << "delete Mesh..." << std::endl;
	VAO.Delete();
}*/

