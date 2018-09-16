#version 430

#define PROPERTY_MAP gProperties

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D target;

uniform vec2 resolution;
uniform uint mode; //0 => Roughness, 1 => Metallic, 2 => Translucency, 3 => Occlusion
uniform sampler2D PROPERTY_MAP;

#include <shaders/gbufferread.glsl>


void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 texCoords = vec2(coords) / resolution;

	vec4 properties = readProperties(texCoords);
	vec3 property = vec3(properties[mode]);

	imageStore(target, coords, vec4(property, 1.f));
}
