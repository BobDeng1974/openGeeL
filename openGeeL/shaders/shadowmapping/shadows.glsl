
#include <shaders/sampling.glsl>
#include <shaders/lighting/lights.glsl>


float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition) {

	//No shadow
	if(pointLights[i].type == 0)
		return 0.f;
	
	vec4 posLightSpace = vec4(fragPosition - pointLights[i].position, 1);
	vec3 direction = posLightSpace.xyz;
	float curDepth = length(direction) / pointLights[i].farPlane;

	//Dont' draw shadow when too far away from the viewer or from light
	float camDistance = length(fragPosition) / pointLights[i].farPlane;
	if(camDistance > 10.f || curDepth > 1.f)
		return 0.f;
	else {
		float minBias = pointLights[i].bias;
		vec3 dir = normalize(pointLights[i].position - fragPosition);
		float bias = max((minBias * 10.f) * (1.f - abs(dot(norm, dir))), minBias);

		//Hard shadow
		if(pointLights[i].type == 1) {
			float depth = getShadowDepthCube(pointLights[i].shadowmapIndex, direction);
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow

		float mag = (1.f / pow(pointLights[i].scale, 2)) * 50000.f;
		float dist = length(fragPosition - pointLights[i].position);
		float diskRadius = (1.f + (dist / 150.f)) / mag;

		float shadow = 0.f;
		int samples = pointLights[i].resolution * 3;

		for(int j = 0; j < pointLights[i].resolution * 3; j++) {
			float depth = getShadowDepthCube(pointLights[i].shadowmapIndex, direction + sampleDirections3D[j] * diskRadius);
			shadow += step(depth, curDepth - bias);
		}    

		return (shadow) / float(samples);
	}
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition, inout vec3 coords) {

	vec4 posLightSpace = spotLights[i].lightTransform * vec4(fragPosition, 1.f);
	coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//No shadow. Called after coordinate computation since these are needed for light cookie regardless
	if(spotLights[i].type == 0.f)
		return 0.f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float minBias = spotLights[i].bias;
		vec3 lightDir =  spotLights[i].position - fragPosition;
		float bias = max((minBias * 10.f) * (1.f - dot(norm, lightDir)), minBias);
		float curDepth = coords.z - bias;

		//Hard shadow
		if(spotLights[i].type == 1) {
			float depth = getShadowDepth2D(spotLights[i].shadowmapIndex, coords.xy);
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow

		float shadow = 0.f;
		vec2 texelSize = vec2(spotLights[i].scale) / 1000.f;
		int samples = spotLights[i].resolution;
		for(int j = 0; j < samples; j++) {
			float depth = getShadowDepth2D(spotLights[i].shadowmapIndex, coords.xy + sampleDirections2D[j] * texelSize);
			shadow += step(depth, curDepth - bias);
		}    
	
		return shadow / float(samples);
	}
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {

	if(directionalLights[i].type == 0)
		return 0.f;

#if (USE_CASCASDED_MAP == 1)

	//TODO: implement hard shadows

	int smIndex = 0;
	float xOffset = 0;
	float yOffset = 0;
	float clipDepth = length(cameraPosition - fragPosition);
	for(int k = 0; k < DIRECTIONAL_SHADOWMAP_COUNT; k++) {
		if(clipDepth < directionalLights[i].cascadeEndClip[k]) {
			xOffset = mod(float(k), 2.f);
			yOffset = floor(float(k) / 2.f);
			smIndex = k;
			break;
		}
	}

	vec4 posLightSpace = directionalLights[i].lightTransforms[smIndex] * vec4(fragPosition, 1.f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Translate texture coordinates according to current sub shadow map
	coords.x *= 0.5f;
	coords.y *= 0.5f;
	coords.x += xOffset * 0.5f;
	coords.y += yOffset * 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;
	else {
		float minBias = directionalLights[i].bias;
		float bias = max(minBias * 100.f * (1.f - dot(norm, directionalLights[i].direction)), minBias);
		float curDepth = coords.z - bias;

		float shadow = 0.f;
		vec2 texelSize = vec2(directionalLights[i].scale) / 1000.f;
		int samples = 8;
		for(int j = 0; j < samples; j++) {
			int index = int(20.f * random(floor(fragPosition.xyz * 1000.f), j)) % 20;

			float depth = getShadowDepth2D(directionalLights[i].shadowmapIndex, coords.xy + sampleDirections2D[index] * texelSize); 
			shadow += step(depth, curDepth - bias);      
		}    
	
		return shadow / float(samples);
	}

#else
	vec4 posLightSpace = directionalLights[i].lightTransform * vec4(fragPosition, 1.f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	//Don't draw shadow when outside of farPlane region.
    if(coords.z > 1.f)
        return 0.f;

	float bias = directionalLights[i].bias;
	//float bias = max(0.05f * (1.0f - dot(norm, lightDir)), 0.005f);
	float curDepth = coords.z - bias;

	//Hard shadow
	if(directionalLights[i].type == 1) {
		float depth = getShadowDepth2D(directionalLights[i].shadowmapIndex, coords.xy);
		return curDepth > depth ? 1.f : 0.f; 
	}

	//Soft shadows

	float shadow = 0.f;
	vec2 texelSize = vec2(directionalLights[i].scale) / 1000.f;

	//Interpolate shadow map in kernel around point
	int kernel = directionalLights[i].resolution;
	for(int x = -kernel; x <= kernel; x++) {
		for(int y = -kernel; y <= kernel; y++) {
			float depth = getShadowDepth2D(directionalLights[i].shadowmapIndex, coords.xy + vec2(x, y) * texelSize);
			shadow += curDepth - bias > depth ? 1.f : 0.f;        
		}    
	}
	
	float kernelSize = 2 * kernel + 1; 
	return shadow / (kernelSize * kernelSize);

#endif

}


