#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba16f) uniform image2D target;

uniform vec2 resolution;


void main() {
	//if(gl_GlobalInvocationID.x >= resolution.x || gl_GlobalInvocationID.y >= resolution.y) return;
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 texCoords = vec2(coords) / resolution;

	//vec4 col = texture2D(image, texCoords);
	vec4 col = imageLoad(target, coords);
	imageStore(target, coords, col * vec4(vec3(0.5f, 0.1f, 0.9f), 1.f));
}
