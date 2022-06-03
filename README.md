# Greed
by David Köppl (12022493) and Nicolas Eder (12020626)


## Table of contents
* [Setup and requirements](#setup-and-requirements)
* [Camera & Controls](#camera-&-controls)

## Setup and Requirements
### Minimum Requirements
* OS: Windows 10 32 bit or 64 bit
* CPU: 3.0 GHz
* RAM: 2 GB
* GPU: Nvidia 600 series, latest driver 
* DISK: 1 GB

The game was tested on an Nvdia GTX 1080 at 1920x1080

### Setup
#### Cloning from Git
You need Visual Studio 2019 or newer to open the project file. Make sure your debuggers target directory is set to %(TargetDir). Your build should target x86 and the main function can be found in Main.cpp.
#### Running the executable
To run the game, open the .exe in bin/Release. Make sure you meet the requirements and change the settings in assets/settings.ini accordingly. If you have not installed the latest Visual C++ Redistributable, install it before launching the game.
#### settings
* width/height - sets the window resolution
* refresh - rate should match your monitor refresh rate
* fullscreen - sets the window to fullscreen, change your resolution before activtating this
* fov - sets the field of view of the camera

* bloom - sets hdr rendering and the bloom effect
* exposure - changes the brightness of the game, bigger value means brighter image
* maxWhite - everything brighter than this value gets mapped to 1
* bloomStrength - how visible the bloom is
* lightAdaption - how fast the light adapts to the scene
* ssao - sets ssao effect
* scale - sets the scale of ssao tiling
* bias - sets strength of the ssao
* attScale - used for ssao
* distScale -  used for ssao
* shadowRes - how big the shadow map texture is, gets multiplied by 1024
* fogQuality - how many raymarch steps the volumetric light shader does, gets multiplied by 16
* useLOD - use discrete LOD meshes, breaks vertex animation

## Camera & Controls

### Player Controls:
* WASD - move player horizontally
* Space - jump
* Mouse - rotate camera
* LMB - pickup object
* R - restart game
* ESC - pause game (press again to quit)

### For debugging and effects you can use:
* F1 - sets window to fullscreen
* F2 - toggle frustum culling debug mode, make AABBs and frustum visible, output culled objects to console
* F3 - toggle bloom effect (and tonemapping, can make image dark)
* F4 - toggle physics debug mode (onyl in debug build, very slow)
* F5 - toggle normal mapping
* F6 - switch between player camera and free camera
* F7 - freeze frustum for culling
* F8 - toggle frustum culling
* F9 - toggle SSAO
* F10 - start camera animation
* F11 - toggle shadow debugging, renders shadow value instead of color
* ESC - exit game

### Free Camera Controls (F6):
* WASD - move forward/backword and left/right on the cameras xz plane
* 1,2 - move up/down along the cameras y axis
* LSHIT - faster camera movement
* Mouse - click with LMB and drag to rotate

