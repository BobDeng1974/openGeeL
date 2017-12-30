#version 430

#define MAX_LIGHTS 5
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

shared uint uMinDepth = 0xFFFFFFF;
shared uint uMaxDepth = 0;

shared uint pointLightIndicies[MAX_LIGHTS];
shared uint pointLightCounter = 0;

uniform int plCount;
uniform int dlCount;
uniform int slCount;

uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuse;
uniform sampler2D gEmissivity;

uniform bool useEmissivity;

uniform mat4 projection;
uniform mat4 inverseView;
uniform vec3 origin;

uniform PointLight pointLights[MAX_LIGHTS];
uniform DirectionalLight directionalLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];     

#include <shaders/shadowmapping/shadowsView.glsl>
#include <shaders/lighting/cooktorrancelights.glsl>


void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 textureCoordinates = gl_GlobalInvocationID.xy / resolution;

	vec4 posRough = texture2D(gPositionRoughness, textureCoordinates);
	vec3 fragPosition = posRough.rgb;

	
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

	vec4 normMet  = texture2D(gNormalMet, textureCoordinates);

    vec3 normal		= normMet.rgb;
    vec4 albedo		= texture2D(gDiffuse, textureCoordinates);
	vec3 emissivity = useEmissivity ? texture2D(gEmissivity, textureCoordinates).rgb : vec3(0.f);

	float roughness	= posRough.a;
	float metallic  = normMet.a;

	vec3  viewDirection = normalize(-fragPosition);


	vec3 irradiance = albedo.rgb * emissivity;
	
#if (DIFFUSE_SPECULAR_SEPARATION == 0)
	for(int i = 0; i < pointLightCounter - 1; i++) {
		uint index = pointLightIndicies[i];
		irradiance += calculatePointLight(int(index), pointLights[index], normal, fragPosition, viewDirection, albedo, roughness, metallic);
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
			viewDirection, albedo, roughness, metallic, diff, spec);
	}
       
	for(int i = 0; i < dlCount; i++)
        calculateDirectionaLight(i, directionalLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);

	for(int i = 0; i < slCount; i++)
		calculateSpotLight(i, spotLights[i], normal, fragPosition, viewDirection, albedo.rgb, roughness, metallic, diff, spec);
	
	imageStore(diffuse, coords, vec4(irradiance + diff, 1.f));
	imageStore(specular, coords, vec4(spec, 1.f));

#endif

}



