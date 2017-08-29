#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D image;
layout(binding = 1, rgba16f) uniform image2D source;

uniform vec2 resolution;

void main() {
	//if(gl_GlobalInvocationID.x >= resolution.x || gl_GlobalInvocationID.y >= resolution.y) return;
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	vec4 col = imageLoad(source, coords);
	imageStore(image, coords, col * vec4(vec3(0.5f, 0.1f, 0.9f), 1.f));
}
