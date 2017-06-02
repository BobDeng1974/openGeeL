#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vNormal[];
in vec3 vPosition[];
in vec2 vTexCoords[];

out vec3 normal;
out vec3 fragPosition;
out vec2 texCoords;

flat out int  axis;	//X-, Y- or Z-axis for projection in fragment shader
flat out vec4 AABB; //Primitve for AABB test in fragment shader

//MVP matrices X-, Y- and Z-axis
uniform mat4 transformX;
uniform mat4 transformY;
uniform mat4 transformZ;

uniform vec2 resolution;


//Mesh voxelization according to
//https://developer.nvidia.com/content/basics-gpu-voxelization and
//https://github.com/otaku690/SparseVoxelOctree
void main() {
	vec3 span1 = vPosition[1] - vPosition[0];
	vec3 span2 = vPosition[2] - vPosition[0];
	vec3 faceNormal = normalize(cross(span1, span2));

	float NdotX = abs(faceNormal.x);
	float NdotY = abs(faceNormal.y);
	float NdotZ = abs(faceNormal.z);

	//Find axis that presents the largest face area
	mat4 transform;
	float maxFace = max(NdotX, max(NdotY, NdotZ));
	if(maxFace == NdotX) {
		transform = transformX;		
		axis = 1;
	}
	else if(maxFace == NdotY) {
		transform = transformY;
		axis = 2;
	}
	else {
		transform = transformZ;
		axis = 3;
	}

	//Transform vertices to clip space according to choosen transformation matrix
	vec4 position[3];
	position[0] = transform * gl_in[0].gl_Position;
	position[1] = transform * gl_in[1].gl_Position;
	position[2] = transform * gl_in[2].gl_Position;


	//Enlarge triangle to enable conservative rasterization
	vec2  hPixel = vec2(1.f / resolution); //Dimensions of half a pixel cell
	float pl = 1.4142135637309 / resolution.x;

	//Calculate bounding box of triangle
	vec4 gAABB;
	gAABB.xy = min(position[0].xy, min(position[1].xy, position[2].xy));
	gAABB.zw = max(position[0].xy, max(position[1].xy, position[2].xy));

	//Enlarge by half pixel
	gAABB.xy -= hPixel;
	gAABB.zw += hPixel;

	AABB = gAABB;

	//Triangle edges and corresponding normals
	vec3 e0 = vec3(position[1].xy - position[0].xy, 0.f);
	vec3 e1 = vec3(position[2].xy - position[1].xy, 0.f);
	vec3 e2 = vec3(position[0].xy - position[2].xy, 0.f);
	vec3 n0 = cross(e0, vec3(0.f, 0.f, 1.f));
	vec3 n1 = cross(e1, vec3(0.f, 0.f, 1.f));
	vec3 n2 = cross(e2, vec3(0.f, 0.f, 1.f));

	//Dilate the triangle
	position[0].xy = position[0].xy + pl * ((e2.xy / dot(e2.xy, n0.xy)) + (e0.xy / dot(e0.xy, n2.xy)) );
	position[1].xy = position[1].xy + pl * ((e0.xy / dot(e0.xy, n1.xy)) + (e1.xy / dot(e1.xy, n0.xy)) );
	position[2].xy = position[2].xy + pl * ((e1.xy / dot(e1.xy, n2.xy)) + (e2.xy / dot(e2.xy, n1.xy)) );


	//Write vertices
	gl_Position = position[0];
	fragPosition = position[0].xyz;
	normal = vNormal[0];
	texCoords = vTexCoords[0];
	EmitVertex();

	gl_Position = position[1];
	fragPosition = position[1].xyz;
	normal = vNormal[1];
	texCoords = vTexCoords[1];
	EmitVertex();

	gl_Position = position[2];
	fragPosition = position[2].xyz;
	normal = vNormal[2];
	texCoords = vTexCoords[2];
	EmitVertex();

	EndPrimitive();
}