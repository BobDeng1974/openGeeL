#version 430

#define GROUP_SIZE 10
#define FLOAT_MAX 99999999.f

layout (local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE, local_size_z = 1) in;

layout (binding = 1, std430) buffer OutputBlock { 
	float minimum; 
};


uniform sampler2D gPosition;

shared float depths[10][10];


void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);;
	vec2 texCoords = vec2(coords) / vec2(GROUP_SIZE);

	float depth = -texture2D(gPosition, texCoords).z;
	depth = (depth <= 0) ? FLOAT_MAX : depth;

	depths[coords.x][coords.y] = depth;

	groupMemoryBarrier();
	barrier();


	if(gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0) {
		float m = FLOAT_MAX;

		for (int x = 4; x < GROUP_SIZE; x++) {
			for (int y = 4; y < GROUP_SIZE; y++) {
				m = min(m, depths[x - 2][y - 2]);
			}
		}

		minimum = m;
	}

}
