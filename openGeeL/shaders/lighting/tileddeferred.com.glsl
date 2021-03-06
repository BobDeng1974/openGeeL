#version 430

#define ENABLE_DEFERRED_EMISSIVITY 0

#define POSITION_MAP	gPosition
#define NORMAL_MAP		gNormal
#define DIFFUSE_MAP		gDiffuse
#define PROPERTY_MAP	gProperties

#if (ENABLE_DEFERRED_EMISSIVITY == 1)
#define EMISSION_MAP    gEmission
#endif


#define GROUP_SIZE 8
layout (local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE, local_size_z = 1) in;

#include <shaders/helperfunctions.glsl>
#include <shaders/sampling.glsl>
#include <shaders/lighting/lights.glsl>
#include <shaders/lighting/cooktorrance.glsl>


#if (DIFFUSE_SPECULAR_SEPARATION == 0)
layout(binding = 0, rgba16f) uniform image2D target;
#else
layout(binding = 0, rgba16f) uniform image2D diffuse;
layout(binding = 1, rgba16f) uniform image2D specular;
#endif

uniform vec2 resolution;

shared uint uMinDepth;
shared uint uMaxDepth;

shared uint pointLightIndicies[MAX_POINTLIGHTS];
shared uint pointLightCounter;

uniform sampler2D POSITION_MAP;
uniform sampler2D NORMAL_MAP;
uniform sampler2D DIFFUSE_MAP;
uniform sampler2D PROPERTY_MAP;
#if (ENABLE_DEFERRED_EMISSIVITY == 1)
uniform sampler2D EMISSION_MAP;
#endif

#include <shaders/gbufferread.glsl>

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;   

#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>


void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 textureCoordinates = gl_GlobalInvocationID.xy / resolution;

	//Initialize shared memory
	if (gl_LocalInvocationID.x == 0 && gl_LocalInvocationID.y == 0) {
		uMinDepth = 0xFFFFFFF;
		uMaxDepth = 0;
		pointLightCounter = 0;
	}


	vec3 fragPosition = readPosition(textureCoordinates);

#if (VOLUMETRIC_LIGHT == 1)
	if(length(fragPosition) <= 0.001f) {
		//Reconstruct position
		vec4 screen = vec4(textureCoordinates, 1.f, 1.f);
		vec4 eye = inverseProjection * 2.f * (screen - vec4(0.5f));

		fragPosition =  eye.xyz / eye.w;
	}
#endif

	
	//Write min and max depth of workgroup for later view frustum creation
	float depth = 0.f;//-fragPosition.z;
	depth = (depth + 1.f) * 0.5f; 
	uint uDepth = uint(depth * 0xFFFFFFFF);

	atomicMin(uMinDepth, uDepth);
	atomicMax(uMaxDepth, uDepth);

	barrier();
	
	float minDepth = float(uMinDepth / float(0xFFFFFFFF));
    float maxDepth = float(uMaxDepth / float(0xFFFFFFFF));

	//Extract culling planes in clip space according to 
	//'http://www.lighthouse3d.com/tutorials/view-frustum-culling/clip-space-approach-extracting-the-planes/'
	vec4 frustumPlanes[6];

	//Create tile projection matrix according to
	//'https://gamedev.stackexchange.com/questions/67431/deferred-tiled-shading-tile-frusta-calculation-in-opengl'
	vec2 tileScale = resolution / float(2.f * GROUP_SIZE);
	vec2 tileBias  = tileScale - vec2(gl_WorkGroupID.xy);

	vec4 col1 = vec4(-projection[0][0] * tileScale.x, 0.f, tileBias.x, 0.f);
	vec4 col2 = vec4(0.f, -projection[1][1] * tileScale.y, tileBias.y, 0.f);
	vec4 col4 = vec4(0.f, 0.f,  1.f, 0.f);

	frustumPlanes[0] =  col1 + col4;
	frustumPlanes[1] = -col1 + col4;
	frustumPlanes[2] =  col2 + col4;
	frustumPlanes[3] = -col2 + col4;
	frustumPlanes[4] = vec4(0.f, 0.f, -1.f, -minDepth);
	frustumPlanes[5] = vec4(0.f, 0.f, -1.f,  maxDepth);
	
	//Normalize plane normals
	for(uint i = 0; i < 4; i++)
	    frustumPlanes[i] *= 1.f / length(frustumPlanes[i].xyz);

	//Frustum culling
	//Pointlights
	uint threadCount = GROUP_SIZE * GROUP_SIZE;
	uint lightsPerThread = (uint(plCount) + threadCount - 1) / threadCount;
	
	for(uint i = 0; i < lightsPerThread; i++) {
		uint lightIndex = (i * threadCount) + gl_LocalInvocationIndex;

		if(lightIndex > uint(plCount)) break;

		vec3 lightPosition = pointLights[lightIndex].position;
		float lightRadius  = pointLights[lightIndex].radius;

		bool inView = true;
		for (uint j = 0; j < 6 && inView; j++) {
			//Project light direction on plane normal to get distance
            float dist = dot(frustumPlanes[j].xyz, lightPosition);
            inView = (-lightRadius <= dist);
        }
		
        if (inView) {
            uint id = atomicAdd(pointLightCounter, 1);
            pointLightIndicies[id] = lightIndex;
        }

	}

	
	barrier();
	
	//Proceed as usual

	vec3 viewDirection = normalize(-fragPosition);
    vec3 normal	= readNormal(textureCoordinates);
    vec4 albedo	= readDiffuse(textureCoordinates);

	vec3 emissivity;
	float roughness, metallic;
	readProperties(textureCoordinates, roughness, metallic, emissivity);
	float translucency = readTranslucency(textureCoordinates);


	vec3 irradiance = albedo.rgb * emissivity;
	
#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	for(int i = 0; i < pointLightCounter - 1; i++) {
		uint index = pointLightIndicies[i];
		irradiance += calculatePointLight(int(index), pointLights[index], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, translucency);
	}
       
	for(int i = 0; i < dlCount; i++)
        irradiance += calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);

	for(int i = 0; i < slCount; i++)
		irradiance += calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic);


	imageStore(target, coords, vec4(irradiance, 1.f));

#else
	vec3 diff = vec3(0.f);
	vec3 spec = vec3(0.f);

	for(int i = 0; i < pointLightCounter - 1; i++) {
		uint index = pointLightIndicies[i];
		calculatePointLight(int(index), pointLights[index], normal, fragPosition, 
			viewDirection, albedo.rgb, roughness, metallic, translucency, diff, spec);
	}
       
	for(int i = 0; i < dlCount; i++)
        calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);

	for(int i = 0; i < slCount; i++)
		calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);
	
	imageStore(diffuse, coords, vec4(irradiance + diff, 1.f));
	imageStore(specular, coords, vec4(spec, 1.f));

#endif

}



