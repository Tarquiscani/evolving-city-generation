# Build

## Linux

Keep in mind that the following has been tested just on Ubuntu 20.04 LTS (native) and on Windows with WSL2 + Ubuntu.



1. Ensure to have the OpenGL package installed by running:

        sudo apt-get install freeglut3-dev

2. Ensure to have the xrandr package installed by running:

        sudo apt-get install xorg-dev libglu1-mesa-dev

3. (Optional) Install clang 12:

        sudo apt install clang-12 --install-suggests
        sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 100
        sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 100

4. Create a new folder:

        mkdir linux_build & cd linux_build

5. Configure the project:

        cmake ../

6. Build:

        cmake --build .

7. Ensure that the game run with the dedicated GPU.
8. Run the game:

        ./evolving_city_generation