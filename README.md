# Greed


## Table of contents
* [What is Greed](#what-is-greed)
* [Graphics Pipeline](#graphics-pipeline)
* [Setup and requirements](#setup-and-requirements)
* [Creating a Level](#creating-a-level)
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
## cloned from Git
You need Visual Studio 2019 or newer to open the project file. Make sure your debuggers target directory is set to %(TargetDir). Your build should target x86 and the main function can be found in Main.cpp.
## running .exe
To run the game, open the .exe in bin/Release. Make sure you meet the requirements and change the settings in assets/settings.ini accordingly. If you have not installed the latest Visual C++ Redistributable, install it before launching the game.

## Graphics Pipeline
### Main.cpp
Here you can find the setup process and the main render loop.

### Level.h
This class is needed for loading a level from an fbx file and converting it to OpenGL friendly data. It creates a scene graph which also is a BVH and builds a render queue before every render call using frustum culling.

### Renderer.h
Implements the render pipeline. It creates all needed Shaders and Framebuffers and does depth mapping, SSAO, bloom/HDR and also renders the HUD.

### Shader
The game engine can load and compile different shaders by creating a Shader object and linking one to five shaders to a shader program. Here is a short
selection of our shaders and what they do:

#### PBRShader
Implements a physical based rendering based on the code on learnopengl.com. It also does normal mapping using simplified version of Blinns Perturbed Normals.

#### skyboxShader
Simple objects like boxes don't need actual vertices to be rendered. Instead we can hardcode the mesh in the shader and call the glDraw function with
the number of indices the box has (36). then we read the glVertexID and output the correct vertex position. The rest is simple 3D sampling.

#### CombineHDR
This Shader composes a bloom effect ontop of an image and applies tonemapping. Again we need to render a texture to a fullscreen quad which is so
simple we don't need actual geometry. We use the VertexID to output a set of uv coordinates, which is a trick a lot of the other postprocessing shader
use. The tone mapping uses the Reinhard 2 algorithm and has variables that can be set in the settings.ini, like exposure or bloom strength.

#### lightAdapt
We wanted to create a smooth transition between light changes and simulate the human vision. To achieve that three 1 texel framebuffers are needed, the first one contains our average luminance, the second one the actual luminance of the current frame and last one is our target. We use an 
exponential smoothing filter to calculate our target luminance from the first two luminances. The first and the last framebuffers are then swapped
every frame so our newly calulate value because the average in the next cycle.

## Creating a Level
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
Complex geometry can be loaded from fbx files with our Level class

#### 60 FPS and Framerate Indipendence
Our render is able to draw more than 1000000 triangles at about 800 fps on our machines. For every animation and physics simulation the delta time is used consistently.

#### Win/Lose Condition
If you manage to reach the top of the cave, you win, but if the rising lava catches up to you, you get fried.

#### Intuitive Camera & Controls
For this game we didn't want to create a player model and decided it would be the best to use a quaternion based first person camera.
Movement is mapped to the standard WASD scheme: 
* WASD - move player horizontally
* Space - jump
* Mouse - rotate camera
* LMB - pickup object

For debugging and effects you can use:
* F1 - sets window to fullscreen (can't be reversed)
* F2 - toggle frustum culling debug mode, make AABBs and frustum visible, output culled objects to console
* F3 - toggle bloom effect (also tonemapping so image may look dark)
* F4 - toggle physics debug mode
* F5 - toggle normal mapping
* F6 - switch between player camera and free camera
* F7 - freeze frustum for culling
* F8 - toggle frustum culling
* F9 - toggle SSAO

#### Ilumination Model
To illuminate our scene we used a physically based rendering approach which is implemented in assets/shaders/PBR.

#### Textures
Every loaded model has to have a texture for albedo, normal mapping, metal, roughness and ambient occlusion, which is loaded by the Texture class and saved in a Material object.

#### Moving Objects
In our fbx file we use seperate groups for static and dynamic objects. Dynamic objects can move through the scene and are part of the dynamic physics simulation. Additionally there is the lava triangle which rises from the bottom to the top as implemented in the lavaFloor shaders

#### Adjustable Parameters
Screen size, fullscreen mode, refresh rate and exposure aswell as other settings can be set in assets/settings.ini. The window can be resized during runtime, although it will look warped if the aspect ratio is changed and if it's resized to a bigger size than set in the settings it will look pixelated.

#### Physics
With the bullet library we have implemented collision detection and pressure plates.

#### View Frustum Culling
In the Level class, before every draw call, the program iterates through a BVH and test the AABBs against the frustum planes and the frustum box. By pressing F2 you can render the the calculated bounding volumes (green) and the view frustum (yellow) and also output culling data every 2 seconds to the console. By pressing F7 the cull frustum is frozen in place and by pressing F8 the culling routine is skipped.

#### HUD
The HUD consists of a simple png image, with a crosshair in the center and a note in the bottom right corner.

### Graphics & Effects
#### Shadows
The depth map from the view of the sun is rendered to a framebuffer. This framebuffer is not used yet.

#### Fluid Simulation / Subdivision Surface
maybe do

#### Procedural Textures
todo

#### Normal Mapping
Every model has a normal map which is calulated in the PBRShader. For an explaination look at the shader section.

#### kd-Tree
todo

#### Post Processing
In the Renderer class we implemented a pipeline for HDR rendering, blooming and tonemapping. This is done by rendering the scene to a framebuffer, downscaling and converting the image to a luminance texture, using the single texel level mipmap view into the luminance to adjust the image brightness in a compute shader. Then rendering all bright spots from the framebuffer to a seperate one, applying ping pong blur to the bright spots and combining the result with the original framebuffer using Reinhard 2 tone map operator and the computed luminance value.

