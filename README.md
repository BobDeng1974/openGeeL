# openGeeL 
OpenGeeL is a deferred rendering engine entirely written in *C++*(11+) and *OpenGL*(4.3+) with scripting support for *Lua* 
and large support for post-processing.

## Key Features

- PBR with Cook-Torrance BRDF
- *Image Based Lighting* and *Screen Space Reflections* for indirect light approximations
- Experimental realtime global illumination via *Voxel Cone Tracing*
- HDR and Tonemapping. Default: Academy Color Encoding System (**ACES**)
- Large array of post processing effects like motion blur, depth of field, lens flar, bloom, color correction & light shafts
- Deferred rendering by default with experimental Tile-based renderer
- Separated lighting passes for diffuse and specular lighting to allow effects like *Subsurface Scattering*
- Forward rendering supported. Used by default for transparent objects and for use of custom shaders
- Physics via [*Bullet SDK*](https://github.com/bulletphysics/bullet3).
- Multithreaded support with default separation of render, scripting and physics threads
- Scene descriptions and simple scene/object behaviour via the *Lua* scripting language
