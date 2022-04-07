# Greed


## Table of contents
* [What is Greed](#what-is-greed)
* [Setup and requirements](#setup-and-requirements)
* [Graphics Pipeline](#graphics-pipeline)
* [Engine Features](#engine-features)

## What is Greed
Greed is a first person platformer written in C++ with OpenGL. It showcases what is possible with modern OpenGl and a few months time. 

### Story 
Two days of the dark caverns lay behind you, all for a treasure cave you weren't even sure exists. Now all doubt is gone - mountains of gold and valuables lay at your feet. How much of it will you be able to carry out though? And how long do you have until the “inactive” volcano you’re standing in will bathe you in molten stone?

### Gameplay 
You will have to jump through a parkour, which is more difficult, the more valuables you are carrying. Every item you collect weighs you down, makes you slower, makes your jump less high and will give you other difficulties. Additionally you only have a certain amount of time for collecting and jumping, because there is lava rising from the bottom of the level, which kills on contact. If you reach the top of the cave, you win and can see what riches you have secured for yourself.

## Setup and Requirements
### Minimum Requirements
* OS: Windows 10 32 bit or 64 bit
* CPU: 3.0 GHz
* RAM: 2 GB
* GPU: one that supports OpenGL 4.60
* DISK: 1 GB

### Setup
If you clone the Project and open it in Visual Studio, make sure to set the working directory under properties->Debugging to %(SolutionDir). Next change the
platform to x86, then be able to run Greed. If you want to change the settings, go to src/assets/settings.ini. There you can edit resolution, refresh rate, exposure,
and various other settings. If you want to load your own levels or modify the game engine, keep reading.

## Graphics Pipeline
### Main.cpp
Let's go through the important steps of the main render loop. When you start Greed a global state is initiated and loaded with your settings. The window
for the game is created with a GLFWapp object. This Objects holds the window pointer and swaps the buffers, but most importantly it makes main.cpp cleaner.


Next various callbacks for registering keyboard and mouse input are defined. The first part of the game is loaded by creating a level object, which 
loads a .fbx file containing the level geometry. Then a Renderer is created. The Renderer manages the main render loop and includes the postprocessing.
After that the physics engine is intitalized. Before the render loop starts a FPS counter is created. The counter will average the frames per second during
the render loop. 


Now the interesting part: the main render loop. We update the FPS counter and the time delta. Then we resize the window, if the size was changed. The 
phsyics simulation does its thing and the perframeData gets filled with the view-projection matrix, camera position and other common varaibles which are constant
over i single render iteration. Then we draw our scene and swap buffers. After the loop there are only a few clean up calls.

### Level.h
The main purpose of this class is to load and manage level data. To do that it needs various structs and a loader library. We choose to use Assimp. 
What happens if you call the Level constructor with a file path is that it loads the file (if it exists) and saves the fbx data in an aiScene. Why did we 
use fbx files specificly, you may ask. That's because we wanted to design our level in Autodesk Maya and export it with textures and a scene graph.


With Assimmp's scene we can poplate our level. For the meshes we go through each mesh assimp has found and extract its vertices and indices. To save 
on cycle time we decided to use one giant vertex and one giant index array. This way we only need to bind a single vao and use it for the majority of the
draw calls. To draw a single mesh from our data, we need to tell OpenGL where in our mesh starts and how many triangles it needs to draw. Thats why we have
our subMesh struct. It keeps track of the index and vertex offsets. After loading all the meshes, we are going to load all materials. For that we just iterate
through all the materials in our assimp scene and add them to the materials vector. A Material is basicly a wrapper for multiply textures. A single Material
constsits of five textures, or to be precise five texture handles. You need to manually delete those textures handles if you don't need them.

Now we are ready
to create some models. A model is a collection of indices for rendering a mesh. For our game models could be replace by meshes, but we keep them 
seperate to be able to use instancing. If we want to draw models, we need to have model matrices, thats where the scene graph is needed. It creates
a hierachy of nodes, each containing models to draw or just matrices. The scene graph is build recursiveley from the scene graph assimp provides. To save
on memory we don't save the matrix, but instead decompose it into T,R and S components. The last thing we need to do is to create buffers for our vertex
and index data and load the lightsources.

### Renderer.h
This class owns the shaders and does the important draw calls. When a rendere is created it needs to have a view into the global state. It will compile
a few shader programms and setup some settings in the perframeData struct for rendering. After that the light soruces are bound to their uniforms and a few
preperations needed for the framebuffers in the draw loop are made.

If we call the draw function in the main loop, the renderer will clear the main
framebuffer which we'll render to and update some data. Then the Rendering starts with the first pass. Here we render our skybox and then our scene 
into the framebuffer. In the second pass we convert the image to luminace using a rough estimation of the human light sensitivity. In the third pass 
we use a compute shader and three single texel textures (this texel holds the average luminance of our rendered image) to simulate the light adaption
of the human vision.

The forth pass filter bright spots from our image. Those bright spots get blurred by switching between a vertical blur shader and a
horizontal blur shader. If the bloom flag is set, the sixth pass will combine our rendered immage with the blurred highlights and apply tonemapping to 
the image. After that the luminance textures need to be switched.

### Shader
The game engine can load and compile different shaders by creating a Shader object and linking one to five shaders to a shader program. Here is a short
overview our shaders and what they do:

#### PBRShader
Implements a physical based rendering scheme and is based on the code from learnopengl.com . Besides vertex data it also has access to light sources.
For our needs we only used directional and positional lights, which need to be specified before loading the shader, as the variables xMAXLIGHTS get 
replaced when loading the shader code. Normal mapping also takes place here, by using an algorithm that is based on the paper of Blinn's pertubed normals.
For every light source the radiance is then calculated using the Cook-Torrance BRDF... todo

#### skyboxShader
Simple objects like boxes don't need actual vertices to be rendered. Instead we can hardcode the mesh in the shader and call the glDraw function with
the number of indices the box has. then we read the glVertexID and hand output the correct vertex position. The rest is simple cubemap sampling.

#### CombineHDR
This Shader composes a bloom effect ontop of an image and applies tonemapping. Again we need to render a texture to a fullscreen quad which is so
simple we don't need actual geometry. We use the VertexID to output a set of uv coordinates, which is a trick a lot of the other postprocessing shader
use. The tone mapping uses the Reinhard 2 algorithm and has variables that can be set in the settings.ini, like exposure or bloom strength.

#### lightAdapt
We wanted to create a smooth transition between light changes and simulate the human vision. To achieve that three 1 texel framebuffers are needed, the first one contains our average luminance, the second one the actual luminance of the current frame and last one is our target. We use an 
exponential smoothing filter to calculate our target luminance from the first two luminances. The first and the last framebuffers are then swapped
every frame so our newly calulate value because the average in the next cycle.

## Creating a Level - Guidlines
In theory assimp can load any fbx file, but for our game engine we decided on a few prerequisites:


* the Maya projects is located in the assets folder
* a texture is located in assets/texture/(texture_name)/(albedo/normal/metal/rough/ao).jpg
* in Maya only directional and pointlights should be added and only on the root node
* meshes should be parented in rigid and dynamic
* every mesh must have a material
* a material is based on the PBSstingray shader, and needs to have every textures except the emissive, associatet with it
* a material must have the same name as the (texture_name) folder and can only use textures in that folder
* when exporting to fbx, triangulate the mesh, and split vertex normals. Uncheck everything besides lights and keep the scale factor at 1.0. Save as binary

## Engine Features
### Gameplay
#### 3D Geometry
Geometry can be loaded from fbx files with our Level class

#### 60 FPS and Framerate Indipendence
Our render is able to draw more than 1000000 triangles at about 800 fps on our machines. For every animation and physics simulation the delta time is used

#### Win/Lose Condition
If you manage to reach the top of the cave, you win, but if the rising lava catches up to you, you get fried.

#### Intuitive Camera & Controls
For this game we didn't want to create a player model and decided it would be the best to use a quaternion based first person camera.
Movement is mapped to the standard WASD scheme: 
* WASD - move player horizontally
* Space - jump
* Mouse - rotate camera
* LMB - pickup object

#### Ilumination Model
To light our scene we used a physically based rendering approach which is implemented in assets/shaders/pbr.

#### Textures
Every loaded model has to have a set of five textures, describing it's material. In the render loop at least nine textures are used, five of the are 
the material of the model three are for image based lighting and one is for the skybox.

#### Moving Objects
In our fbx file we use seperate groups for static and dynamic objects. Dynamic objects can move through the scene and are part of the physics simulation. additionally there is the lava triangle which rises from the bottom to the top as implemented in the lavaFloor shaders

#### Adjustable Parameters
Screen size, fullscreen mode, refresh rate and exposure aswell as other settings can be set in assets/settings.ini

#### Physics
With the bullet library we have implemented collision detection and pressure plates.

#### View Frustum Culling
todo

#### HUD
todo

### Graphics & Effects
#### Shadows
todo

#### Fluid Simulation / Subdivision Surface
maybe do

#### Procedural Textures
todo

#### Normal Mapping
Every model has a normal map which is calulated in the PBRShader. For an explaination look at the shader section.

#### kd-Tree
todo

#### Post Processing
In the Renderer class we implemented a pipeline for HDR rendering, blooming and tonemapping. For details look at the Renderer section.

