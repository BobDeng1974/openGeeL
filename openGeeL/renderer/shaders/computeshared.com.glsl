#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D image;
layout(binding = 1, rgba16f) uniform image2D source;

uniform vec2 resolution;

shared vec4 groupColor;
shared vec4 groupColors[8][8];

void main() {
	//if(gl_GlobalInvocationID.x >= resolution.x || gl_GlobalInvocationID.y >= resolution.y) return;
	
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	groupColors[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = imageLoad(source, coords);

	groupMemoryBarrier();	barrier();

	if(gl_LocalInvocationID.x == 0 && gl_LocalInvocationID.y == 0) {
		groupColor = vec4(0.f);		
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				groupColor = groupColor + groupColors[x][y];
			}
		}

		groupColor = groupColor / 64.f;
	}

	groupMemoryBarrier();	barrier();

	imageStore(image, coords, groupColor);
}