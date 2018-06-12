#version 430 core

#define POSITION_MAP	gPosition
#define NORMAL_MAP		gNormal
#define PROPERTY_MAP	gProperties

#include <shaders/helperfunctions.glsl>
#include <shaders/lighting/cooktorrance.glsl>

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D POSITION_MAP;
uniform sampler2D NORMAL_MAP;
uniform sampler2D PROPERTY_MAP;

#include <shaders/gbufferread.glsl>

uniform int stepCount = 60;
uniform	float stepSize = 0.2f;
uniform	float stepGain = 1.02f;

uniform mat4 projection;
uniform int effectOnly;

float border = 1.f;

vec3 getReflectionColor(vec3 fragPos, vec3 reflectionDir, vec3 normal, float roughness);
vec3 getReflection(vec3 fragPos, vec3 normal, vec3 reflectionDirection, float roughness, float metallic);

vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);
float calculateNormalDistrubution2(vec3 normal, vec3 halfway, float roughness);
float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness);

float interpolatei(vec3 a, vec3 b, vec3 v);
vec4 transformToClip(vec3 vector);


void main() {
	vec3 result = step(effectOnly, 0.f) * texture(image, TexCoords).rgb;

	vec3 fragPos = readPosition(TexCoords);
	float depth = -fragPos.z;

	float roughness, metallic;
	readProperties(TexCoords, roughness, metallic);
	
	vec3 normal = normalize(readNormal(TexCoords));
	vec3 reflectionDirection = normalize(reflect(fragPos, normal));

	//DBranch to filter out large bunch of pixels
	if(depth > 0.1f && roughness < 0.99f) {
		color = vec4(result + getReflection(fragPos, normal, reflectionDirection, roughness, metallic), 1.f);
	}
	else 
		color = vec4(result, 1.f);
}


vec3 getReflection(vec3 fragPos, vec3 normal, vec3 reflectionDirection, float roughness, float metallic) {
	vec3 viewDirection = normalize(-fragPos);

	vec3 reflectionColor = getReflectionColor(fragPos, reflectionDirection, normal, roughness);
	vec3 halfwayDirection = normalize(reflectionDirection + viewDirection);
	float NdotL = doto(normal, reflectionDirection);

	//BRDF
	float ndf = calculateNormalDistrubution2(normal, halfwayDirection, roughness);
	float geo = calculateGeometryFunctionSmith(normal, viewDirection, reflectionDirection, roughness);
	vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), reflectionColor, metallic, roughness);

	//Lighting equation
	vec3  nom   = geo * fres * ndf;
	float denom = 4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
	vec3  brdf  = nom / denom;

	return reflectionColor * brdf * NdotL;
}


vec3 getReflectionColor(vec3 fragPos, vec3 reflectionDir, vec3 normal, float roughness) {
	
	float _stepSize = stepSize * (1.f - roughness); 
	float _stepCount = stepCount * (1.f - roughness);

	vec3 reflectionColor = vec3(0.f);
	vec3 currPosition = fragPos;
	int i = 0;
	while(i < _stepCount) {
		currPosition = currPosition + reflectionDir * _stepSize;
		float depth = currPosition.z;
		
		vec4 currPosProj = transformToClip(currPosition);
		vec3 sampledPosition = readPosition(currPosProj.xy);
		float currDepth = sampledPosition.z;

		//Break when reaching border of image
		if(currPosProj.x >= border || currPosProj.x <= (1.f - border) || currPosProj.y >= border || currPosProj.y <= (1.f - border))
			break;
		if(currDepth > depth) {
			
			//Check angle between reflection direction and vector to the 'hit' point. 
			//Both should be identical and therefore the angle ~0. If this is not the case, 
			//the reflection vector probably points behind the hit point and is thus
			//not usable
			float angle = abs(dot(normalize(sampledPosition - fragPos), normalize(reflectionDir)));
			if(angle > 0.9995f || abs(currDepth - depth) < 0.3f) {

				vec3 left  = currPosition - reflectionDir * _stepSize;
				vec3 right = currPosition;
				for(int j = 0; j < 5; j++) {
				
					currPosition = mix(left, right, 0.5f);
					currDepth    = -readDepth(currPosProj.xy);
					currPosProj  = transformToClip(currPosition);

					float leftDep = abs(left.z - currDepth);
					float rightDep = abs(right.z - currDepth);

					if(rightDep > leftDep)
						right = currPosition;
					else
						left = currPosition;
				}

				vec3 reflectedNormal = normalize(texture(NORMAL_MAP, currPosProj.xy).rgb);
				float dotNR = abs(dot(normal, reflectedNormal));

				//Discard when surface normal and reflected surface normal are too similar. In this case
				//the backside of reflected surface should have been used, but isn't visible to viewer
				reflectionColor = step(dotNR, 0.33f) * texture(image, currPosProj.xy).rgb;

				break;
			}
		}

		_stepSize *= stepGain;
		i++;
	}

	return reflectionColor;
}

//Trowbridge-Reitz GGX normal distribution function
float calculateNormalDistrubution2(vec3 normal, vec3 halfway, float roughness) {
	
	//TODO: Fix problems in ndf and remove this cop out
	float offset = 3.f;
	float copOut = step(roughness, 0.2f);

    float a = roughness * roughness;
    float NdotH  = doto(normal, halfway);
    float NdotH2 = NdotH * NdotH;
	
    float denom  = (NdotH2 * (a - 1.0f) + 1.0f);
    denom = PI * denom * denom + 0.0001f;

    float ndf = (a / denom) * (1.f - copOut) + (copOut * offset);
	ndf = clamp(ndf, 0.f, offset);
	ndf /= offset;

	return ndf;
}

//Transform to clip space
vec4 transformToClip(vec3 vector) {
	vec4 vecProj = vec4(vector, 1.f);
	vecProj = projection * vecProj;
	vecProj.xyz = vecProj.xyz / vecProj.w;
	vecProj.xyz = vecProj.xyz * 0.5 + 0.5;

	return vecProj;
}


float interpolatei(vec3 a, vec3 b, vec3 v) {
	return length((v - a) / (b - a));
}

