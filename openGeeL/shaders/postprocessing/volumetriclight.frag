#version 430 core

#define POSITION_MAP gPosition

const float PI = 3.14159265359;

struct SpotLight {
	sampler2D shadowMap;
	mat4 lightTransform;

    vec3 position;
    vec3 direction;
	vec3 diffuse;

    float angle;
    float outerAngle;
	float bias;
};

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D POSITION_MAP;
uniform sampler2D shadowMap;
uniform sampler2D lightCookie;

#include <shaders/gbufferread.glsl>

uniform SpotLight light;
uniform float density;
uniform float minCutoff;
uniform int samples;
uniform mat4 inverseView;
uniform mat4 inverseProjection;

uniform int effectOnly;
uniform bool useCookie;
uniform bool lightActive;

float random(vec2 co);
vec3 convertToLightSpace(vec3 fragPosition);
//Convert current pixel to view space coordiantes
vec3 convertToViewSpace();
float calculateSpotLightShadows(vec3 coords);

void main() {
	if(lightActive) {
		vec3 fragPos = readPosition(TexCoords);
		float depth = length(fragPos);

		//Detect pixels that aren't present in gbuffer
		float noFrag = step(depth, 0.1f);
		//Try to reconstruct view space coordinates from texture coordinates for those pixels
		fragPos = (1.f - noFrag) * fragPos + noFrag * convertToViewSpace();

		vec3 fragLightSpace = convertToLightSpace(fragPos);
		vec3 origLightSpace = convertToLightSpace(vec3(0.f));
		vec3 direLightSpace = fragLightSpace - origLightSpace;
	
		vec3 volume = vec3(0.f);
		float f = 1.f / float(samples);
		float stepi = random(TexCoords);
		for(int i = 0; i < samples; i++) {
			vec3 frag = origLightSpace + (direLightSpace * f * stepi);
			vec3 fragView = fragPos * f * stepi;

			float shadow = 1.f - calculateSpotLightShadows(frag);
			shadow *= useCookie ? texture(lightCookie, frag.xy).r : 1.f;

			vec3 lightDirection = light.position - fragView;
			float lightDistance = length(lightDirection);
			float cutOff = step(minCutoff, lightDistance);
			lightDirection = normalize(lightDirection);

			float attenuation = 1.f / (lightDistance * lightDistance);

			//Set intensity depending on if object is inside spotlights halo (or outer halo)
			float theta = dot(lightDirection, normalize(-light.direction)); 
			float epsilon = (light.angle - light.outerAngle);
			float intensity = clamp((theta - light.outerAngle) / epsilon, 0.f, 1.f);

			vec3 vol = light.diffuse * shadow * attenuation * density * intensity * cutOff;

			volume += vol;
			stepi += 1.f;
		}

		volume *= (1.f / stepi);
		color = vec4(volume, 1.f);
	}
}


float calculateSpotLightShadows(vec3 coords) {
    if(coords.z > 1.f)
        return 0.f;
	else {
		float bias = light.bias;
		float curDepth = coords.z;
		float depth = texture(shadowMap, coords.xy).r;
	
		return step(depth, curDepth - bias);
	}
}

vec3 convertToLightSpace(vec3 fragPosition) {
	vec4 posLightSpace = light.lightTransform * inverseView * vec4(fragPosition, 1.f);
	vec3 coords = posLightSpace.xyz / posLightSpace.w;
	coords = coords * 0.5f + 0.5f;

	return coords;
}

vec3 convertToViewSpace() {
	vec4 screen = vec4(TexCoords, 1.f, 1.f);
	vec4 eye = inverseProjection * 2.f * (screen - vec4(0.5f));

	return eye.xyz / eye.w; //Transfer from homogeneous coordinates
}

float random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
