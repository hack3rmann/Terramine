# Terramine

Faster Minecraft clone written in C++23.

| Sunset Screenshot | House Built in the Game |
| ------ | ----- |
| ![image](https://github.com/user-attachments/assets/edc08fb1-d551-43d4-b8a8-da752406897c) | ![Screenshot_30-Nov_02-07-25_3687](https://github.com/user-attachments/assets/01e5b2db-04cf-440d-8605-2f7a4106f8c1) |

Voxel game engine built on OpenGL and GLFW featuring:

1. Realtime world editing accelerated with multithreading.
2. Transparency handler using fast multithreaded geometry reordering algorithm.
3. Physics Engine with FPS-independent solver.
4. Ordinary Differential Equations (ODEs) solver built for incredibly smooth and dynamic animations.
5. Simple font rendering.

## Build

### 1. Clone repo

```shell
git clone https://github.com/slava30727/Terramine.git --recurse-submodules --depth=1
cd Terramine
```

### 2. Build the project and its dependencies with CMake.

```shell
cmake -B build -DOPTIMIZE=ON
cmake --build build -j20
```

## Controls

- use *WASD* to move around
- use *T* to toggle mouse camera control
- use *F* to toggle fly mode
- use *Space* key to jump or fly upwards
- use *Shift* key to fly downwards
- hold *Control* key to move faster
- use *Escape* to open pause menu
- use *1..9* and *0* (for 10) keys to select voxel
- use *Left Mouse Button* to break blocks
- use *Right Mouse Button* to place blocks
