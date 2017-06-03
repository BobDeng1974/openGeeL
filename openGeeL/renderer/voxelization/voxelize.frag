#version 430

flat in int  axis;
flat in vec4 AABB;

in vec3 normal;
in vec3 fragPosition;
in vec2 texCoords;


layout(location = 0) out vec4 gl_FragColor;
layout(pixel_center_integer) in vec4 gl_FragCoord;

//Voxel data is stored in these 1D textures to be later integrated into concrete data structures
uniform layout(binding = 0, rgb10_a2ui) uimageBuffer voxelPositions;
uniform layout(binding = 1, rgba8) imageBuffer voxelColors;
uniform layout(binding = 2, rgba16f) imageBuffer voxelNormals;

//Atomic counter that is used to index above 1D textures
layout (binding = 0, offset = 0) uniform atomic_uint voxelCount;

uniform sampler2D diffuse;
uniform vec2 resolution;
uniform bool drawVoxel; 

//Mesh voxelization according to
//https://developer.nvidia.com/content/basics-gpu-voxelization and
//https://github.com/otaku690/SparseVoxelOctree
void main() {

	//Discard if fragment is outside of triangles bounding box
	discard(fragPosition.x < AABB.x || fragPosition.y < AABB.y 
		|| fragPosition.x > AABB.z || fragPosition.y > AABB.w);


	uvec4 tCoords = uvec4(gl_FragCoord.x, gl_FragCoord.y, resolution.x * gl_FragCoord.z, 0.f);
	uvec4 coords;
	if(axis == 1) {
	    coords.x = int(resolution.x) - tCoords.z;
		coords.z = tCoords.x;
		coords.y = tCoords.y;
	}
	else if(axis == 2) {
	    coords.z = tCoords.y;
		coords.y = int(resolution.x) - tCoords.z;
		coords.x = tCoords.x;
	}
	else
	    coords = tCoords;


	uint index = atomicCounterIncrement(voxelCount);
	if(!drawVoxel) return; //Return in this case since we only want to count voxels

	//Example implementation. Should be later replaced with proper shading
	vec3 color = texture(diffuse, texCoords).rgb;
	vec3 normal = normal;

	imageStore(voxelPositions, int(index), coords);
	imageStore(voxelNormals, int(index), vec4(normal, 0.f));
	imageStore(voxelColors, int(index), vec4(color, 0.f));
}