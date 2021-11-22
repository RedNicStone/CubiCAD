# CubiCAD
A fast and modern voxel based raytracing engine

Currently in heavy development and unusable at its current state.
This reposity will hold the latest changes.

## Building
The project can be built using CMake and gcc.
All project dependecies except for cmake, std libraries and gcc should be included in this repository.

First, start by cloning the repository:
```shell
git clone --recurse-submodules -j8 https://github.com/RedNicStone/CubiCAD.git
cd CubiCAD
```

Create a build directory and run CMake:
```shell
mkdir build
cd build
cmake ..
```

Finally build the project and run the binary:
```shell
make
chmod +x CubiCAD
./CubiCAD
```
