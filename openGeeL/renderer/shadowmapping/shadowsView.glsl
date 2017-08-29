
#include <renderer/shaders/sampling.glsl>
#include <renderer/lights/lights.glsl>


float calculatePointLightShadows(int i, vec3 norm, vec3 fragPosition, inout float travelDistance) {

	//No shadow
	if(pointLights[i].type == 0)
		return 0.f;
	
	vec4 posLightSpace = inverseView * vec4(origin + fragPosition - pointLights[i].position, 1);
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
			float depth = texture(pointLights[i].shadowMap, direction).r;

			travelDistance = abs(curDepth - depth) + bias;
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow

		vec2 size = textureSize(pointLights[i].shadowMap, 0);
		float mag = (size.x + size.y) * (1.f / pow(pointLights[i].scale, 2));
		float dist = length(fragPosition - pointLights[i].position);
		float diskRadius = (1.f + (dist / 150.f)) / mag;

		float testSamples = 4;
		float testShadow = 0;
		for(int j = 0; j < testSamples; j++) {
			float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[j] * diskRadius).r;

			travelDistance += abs(curDepth - depth);
			testShadow += curDepth - bias > depth ? 1 : 0;        
		}  

		travelDistance = travelDistance / float(testSamples) + bias;
		testShadow = testShadow / float(testSamples);

		if(testShadow == 0.f || testShadow == 1.f)
			return testShadow;
		else {
			float shadow = 0.f;
			int samples = pointLights[i].resolution;
			for(int j = 0; j < samples; j++) {
				int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

				float depth = texture(pointLights[i].shadowMap, direction + sampleDirections3D[index] * diskRadius).r;
				
				shadow += step(depth, curDepth - bias);
			}    

			return (shadow) / float(samples);
		}
	}
}


float chebyshevInequality(int i, vec3 coords) {

	float dist = coords.z;
	vec2 moments = texture2D(spotLights[i].shadowMap, coords.xy).rg;
	
	// Surface is fully lit. as the current fragment is before the light occluder
	if (dist <= moments.x) return 1.f;


	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, spotLights[i].bias);

	float d = dist - moments.x;
	float m = variance / (variance + d * d);

	return m;
}

float calculateSpotLightShadows(int i, vec3 norm, vec3 fragPosition, inout vec3 coords) {

	vec4 posLightSpace = spotLights[i].lightTransform * inverseView * vec4(fragPosition, 1.0f);
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
		float bias = max((minBias * 10.0f) * (1.0f - dot(norm, lightDir)), minBias);
		float curDepth = coords.z - bias;

		//Hard shadow
		if(spotLights[i].type == 1) {
			float depth = texture(spotLights[i].shadowMap, coords.xy).r;
			return curDepth - bias > depth ? 1.f : 0.f; 
		}

		//Soft shadow
		return 1.f - chebyshevInequality(i, coords);

		/*
		float shadow = 0.f;
		vec2 texelSize = spotLights[i].scale / textureSize(spotLights[i].shadowMap, 0);
		int samples = spotLights[i].resolution;
		for(int j = 0; j < samples; j++) {
			int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

			float depth = texture(spotLights[i].shadowMap, coords.xy + sampleDirections2D[index] * texelSize).r;
			shadow += step(depth, curDepth - bias);
		}    
	
		return shadow / float(samples);
		*/
	}
}

float calculateDirectionalLightShadows(int i, vec3 norm, vec3 fragPosition) {

	if(directionalLights[i].type == 0)
		return 0.f;

	//TODO: implement hard shadows

	int smIndex = 0;
	float xOffset = 0;
	float yOffset = 0;
	float clipDepth = (projection * vec4(fragPosition, 1.f)).z;
	for(int k = 0; k < DIRECTIONAL_SHADOWMAP_COUNT; k++) {
		if(clipDepth < directionalLights[i].cascadeEndClip[k]) {
			xOffset = mod(float(k), 2.f);
			yOffset = floor(float(k) / 2.f);
			smIndex = k;
			break;
		}
	}

	vec4 posLightSpace = directionalLights[i].lightTransforms[smIndex] * inverseView * vec4(fragPosition, 1.f);
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
		vec2 texelSize = 0.8f / textureSize(directionalLights[i].shadowMap, 0);
		int samples = 8;
		for(int j = 0; j < samples; j++) {
			int index = int(20.0f * random(floor(fragPosition.xyz * 1000.0f), j)) % 20;

			float depth = texture(directionalLights[i].shadowMap, coords.xy + sampleDirections2D[index] * texelSize).r;
			shadow += step(depth, curDepth - bias);      
		}    
	
		return shadow / float(samples);
	}
}
