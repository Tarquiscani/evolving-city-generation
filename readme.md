Evolving City Generation Demo
=====

Evolving City Generation is a demo showing a procedural generation algorithm to create a city in a top-down 2D tile-based world. 
* The purpose of the algorithm is to simulate the birth and the growth of a settlement that follows a spontaneous architecture pattern (for instance a medieval village, or a modern slum).
* The algorithm is intended to be real-time (even though it's not optimized yet).
* The demo also contains an algorithm to generate a 3D hip roof for any kind of tile-based shape.

This was an abandoned side project developed in 2018 and 2019 with the main goal of learning 3D programming, OpenGL and shaders. I just added a tutorial and polished the code and the folder structure a bit.

## Download
The binaries are available on Itch.io.

## Dependencies
* OpenGL 4.3 implementation
* glad
* GLFW
* GLM
* Dear ImGui
* miniaudio
* stb
* Earcut
* FlatBuffers

## Supported Platforms
* Windows (MSVC)
* Linux (Clang/GCC)

## Engine Features
* Pixel perfect (for Zoom = 1)
* Support for multiple versions of the same texture, depending on the screen resolution (low-definition, HD, 4K)
* Edge-detection filter
* Dev tools:
  * LOD filter
  * Inspector for off-screen framebuffers

## Build from source

### Windows

With Powershell:

* Ensure to have the following tools installed:
  * git
  * CMake
  * Visual Studio with the following modules:
    * Desktop development with C++
        
1.      git clone --recurse-submodules https://github.com/Tarquiscani/evolving-city-generation.git
2.      cd evolving-city-generation
3.      mkdir windows_build; cd windows_build
4.      cmake ..\
5.      cmake --build .
6. Ensure that the demo is run with the dedicated GPU.

7. Run it:

        cd .\Debug; .\evolving-city-generation.exe

### Linux

Keep in mind that the following has been tested just on Ubuntu 20.04 LTS (native) and on Windows with WSL2 + Ubuntu.

1. Ensure to have the OpenGL package installed by running:

        sudo apt-get install freeglut3-dev

2. Ensure to have the xrandr package installed by running:

        sudo apt-get install xorg-dev libglu1-mesa-dev

3. (optional) Install clang 12:

        sudo apt install clang-12 --install-suggests
        sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 100
        sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 100

4.      git clone --recurse-submodules https://github.com/Tarquiscani/evolving-city-generation.git 

5.      cd evolving-city-generation

6. Create a new folder:

        mkdir linux_build & cd linux_build

7. Configure the project:

        cmake ../

8. Build:

        cmake --build .

9. Ensure that the demo is run with the dedicated GPU.

10. Run the demo:

        ./evolving_city_generation