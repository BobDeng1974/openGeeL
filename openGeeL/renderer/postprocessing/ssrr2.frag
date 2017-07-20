#version 430 core

#include <renderer/shaders/helperfunctions.glsl>
#include <renderer/shaders/samplingvector.glsl>
#include <renderer/lighting/cooktorrance.glsl>

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMet;

uniform int sampleCount = 40;
uniform int stepCount = 60;
uniform	float stepSize = 0.2f;
uniform	float stepGain = 1.02f;

uniform mat4 projection;
uniform int effectOnly;

float border = 1.f;

bool computeReflectionColor(vec3 fragPos, vec3 reflectionDir, vec3 normal, float roughness, out vec3 color);
vec3 getReflectionFussy(vec3 fragPos, vec3 normal, vec3 reflectionDirection, float roughness);

vec4 transformToClip(vec3 vector);
vec2 hammersleySeq(int i, int count);
vec3 generateSampledVector(float roughness, vec2 samp);


void main() {
	vec3 result = step(effectOnly, 0.f) * texture(image, TexCoords).rgb;

	vec4 posRough = texture(gPositionRoughness, TexCoords);
	vec3 fragPos = posRough.xyz;
	float depth = -fragPos.z;
	float roughness = posRough.w; 
	
	vec3 normal = normalize(texture(gNormalMet, TexCoords).rgb);
	vec3 reflectionDirection = normalize(reflect(fragPos, normal));

	//DBranch to filter out large bunch of pixels
	if(depth > 0.1f && roughness < 0.99f) {
		color = vec4(result + getReflectionFussy(fragPos, normal, reflectionDirection, roughness), 1.f);
	}
	else 
		color = vec4(result, 1.f);
}


vec3 getReflectionFussy(vec3 fragPos, vec3 normal, vec3 reflectionDirection, float roughness) {
	vec3 viewDirection = normalize(-fragPos);
	float metallic = texture(gNormalMet, TexCoords).w;

	vec3 right = cross(reflectionDirection, normal);
	vec3 up = cross(reflectionDirection, right);
	float NoV = doto(normal, viewDirection);

	vec3 radiance = vec3(0.f);
	float samples = 0.000001f;
	for(int i = 0; i < sampleCount; i++) {
		vec3 sampleVector = generateSampledVector(roughness, hammersleySeq(i, sampleCount));
		sampleVector = sampleVector.x * right + sampleVector.y * up + sampleVector.z * reflectionDirection; //To world coordinates
		sampleVector = normalize(sampleVector);

		vec3 reflectionColor;
		bool hit = computeReflectionColor(fragPos, sampleVector, normal, roughness, reflectionColor);
		samples += float(hit); //Only increment sample count if sample actual hit a surface in the scene

		vec3 halfway = normalize(sampleVector + viewDirection);
		float cosT = doto(sampleVector, normal);
		cosT = clamp(cosT, 0.f, 1.f);
		float sinT = sqrt(1.f - cosT * cosT);

		float theta = doto(halfway, viewDirection);
		vec3  fresnel = calculateFresnelTerm(theta, reflectionColor, metallic, roughness);
		float geo = calculateGeometryFunctionSmith(normal, viewDirection, sampleVector, roughness);
		float denom =  1.f / (4.f * NoV * doto(halfway, normal) + 0.001f); 

		radiance += reflectionColor * geo * fresnel * sinT * denom;
	}

	return radiance / samples;
}

bool computeReflectionColor(vec3 fragPos, vec3 reflectionDir, vec3 normal, float roughness, out vec3 reflectionColor) {
	
	float _stepSize = stepSize * (1.f - roughness); 
	float _stepCount = stepCount * (1.f - roughness);

	reflectionColor = vec3(0.f);
	vec3 currPosition = fragPos;
	int i = 0;
	for(int i = 0; i < _stepCount; i++) {
		currPosition = currPosition + reflectionDir * _stepSize;
		float depth = currPosition.z;
		
		vec4 currPosProj = transformToClip(currPosition);
		vec3 sampledPosition = texture(gPositionRoughness, currPosProj.xy).xyz;
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
					currDepth    = texture(gPositionRoughness, currPosProj.xy).z;
					currPosProj  = transformToClip(currPosition);

					float leftDep = abs(left.z - currDepth);
					float rightDep = abs(right.z - currDepth);

					if(rightDep > leftDep)
						right = currPosition;
					else
						left = currPosition;
				}

				vec3 reflectedNormal = normalize(texture(gNormalMet, currPosProj.xy).rgb);
				float dotNR = abs(dot(normal, reflectedNormal));

				//Discard when surface normal and reflected surface normal are too similar. In this case
				//the backside of reflected surface should have been used, but isn't visible to viewer
				reflectionColor = step(dotNR, 0.33f) * texture(image, currPosProj.xy).rgb;
				return true;
			}
		}

		_stepSize *= stepGain;
	}

	return false;
}

//Transform to clip space
vec4 transformToClip(vec3 vector) {
	vec4 vecProj = vec4(vector, 1.f);
	vecProj = projection * vecProj;
	vecProj.xyz = vecProj.xyz / vecProj.w;
	vecProj.xyz = vecProj.xyz * 0.5 + 0.5;

	return vecProj;
}


