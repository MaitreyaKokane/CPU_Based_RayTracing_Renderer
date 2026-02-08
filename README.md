RAYTRACER (VULKAN + WALNUT)

LumaRT is a real-time, interactive ray tracer built in C++ on top of the Walnut application template.
Walnut provides the Vulkan renderer + windowing + ImGui-based tooling so the project can focus
on ray tracing logic (per-pixel rendering, camera, materials, etc.).  

--------------------------------------------------------------------------------
PROJECT OVERVIEW

This project is created using the Walnut app template (Walnut kept as an external submodule),
and implements a ray tracer as a desktop application. 

Typical workflow:
- Update camera / UI
- Ray trace into an image buffer
- Present the image in a Walnut viewport (Vulkan-backed)

--------------------------------------------------------------------------------
KEY FEATURES (CURRENT / INTENDED)

- Interactive viewport (Walnut)
- CPU ray tracing loop (per-pixel shading) rendered into a framebuffer
- UI controls for parameters (via Walnut/ImGui)
- Easy iteration using Premake + Visual Studio solution generation

Note:
Exact rendering features (materials, bounces, BVH, etc.) depend on the current implementation.
This README is structured so you can expand the "FEATURES" section as you add more.

--------------------------------------------------------------------------------
REPOSITORY LAYOUT

/ RayTracer/        -> Main application (your ray tracer code)
/ Walnut/           -> Walnut framework as a git submodule 
/ scripts/          -> Setup scripts (Premake generation, dependencies, etc.) 
  premake5.lua      -> Premake workspace configuration 
--------------------------------------------------------------------------------
PREREQUISITES (RECOMMENDED)

- Windows 10/11
- Visual Studio 2022
- Git (with submodules)
- Vulkan-capable GPU + drivers (Walnut uses Vulkan)

--------------------------------------------------------------------------------
BUILD & RUN (WINDOWS)

1) Clone with submodules:
   git clone --recursive https://github.com/MaitreyaKokane/LumaRT.git

   If you already cloned without --recursive:
   git submodule update --init --recursive

2) Generate Visual Studio project files:
   scripts\Setup.bat 

3) Open the generated Visual Studio 2022 solution and build.
4) Run the RayTracer application from Visual Studio.



--------------------------------------------------------------------------------
CREDITS
- Credits:
- This ray tracer was implemented by following The Cherno’s Ray Tracing tutorial series (YouTube).
- Application framework: Walnut / Walnut App Template by StudioCherno (The Cherno).
- Walnut framework provides the Vulkan + ImGui application layer.



--------------------------------------------------------------------------------
LINKS
- Template source: StudioCherno/WalnutAppTemplate .
