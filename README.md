# Solar System OpenGL

A simple solar system (Sun, Earth, Moon) created with OpenGL.  
This is built on a linux machine, and makefile is written to work in a Linux Ubuntu environment. (Ubuntu probably does not matter).

## Contributor

Ιωάννης Μαλιάρας - 1115201500084

## Installation

Following instructions from [eclass](https://eclass.uoa.gr/modules/document/file.php/D35/OpenGL/%CE%95%CF%81%CE%B3%CE%B1%CF%83%CF%84%CE%AE%CF%81%CE%B9%CE%B1%202019/opengl33_ubuntu1804_installation.txt) (you will need an active UoA account to access the intstructions), run:

```sh
sudo apt-get install cmake libx11-dev xorg-dev libglu1-mesa-dev freeglut3-dev libglew1.5 libglew1.5-dev libglu1-mesa libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev libglfw3-dev libglfw3 libassimp-dev assimp-utils
```

to install required GL libraries + assimp library.

## Usage

```sh
# To compile and run
make

# To Run
./bin/make
# or
make run

# To compile
make compile
```

## Controls

* W : Rotates upwards around the x axis
* A : Rotates leftwards around the y axis
* S : Rotates downwards around the x axis
* D : Rotates rightwards around the y axis
* Enter : Toggles orbiting animation
* Escape : Closes the window
* Scroll : Zooms in and out

## Comments

* I used code from [learnopengl.com](learnopengl.com) as a headstart.
* model.h, mesh.h, shader.h are basically copied from that tutorial.
* camera.h is a modified and extended version of the camera class shown in the tutorial.
* circle.h is a class that I created to draw line circles in a 3d environment.
* skybox.h is a class that I created to display skyboxes given the right textures.
* ./bin/main is the main executable file
* I did not find the need to explain any more of the implementation. For any questions refer to the code.

## Extra Libraries Used

* glad (+ KHR)
* stbi
* glm

Above libraries are located in the /lib directory.
