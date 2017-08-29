#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D image;
layout(binding = 1, rgba16f) uniform image2D source;

uniform vec2 resolution;

shared vec4 groupColor = vec4(0.f);

void main() {
	//if(gl_GlobalInvocationID.x >= resolution.x || gl_GlobalInvocationID.y >= resolution.y) return;
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	vec4 c = imageLoad(source, coords);
	groupColor = groupColor + c;

	//memoryBarrierShared();
	groupMemoryBarrier();	barrier();

	vec4 col = groupColor;
	imageStore(image, coords, col);
}