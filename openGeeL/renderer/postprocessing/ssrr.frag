#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gSpecular;

uniform mat4 projection;

float border = 1.f;

float absDistance(float a, float b);
vec3 interpolate(vec3 a, vec3 b, float i);
vec4 transformToClip(vec3 vector);
vec3 getReflection(vec3 fragPos, vec3 reflectionDir, vec3 normal);

void main() {
	vec3 result   = texture(image, TexCoords).rgb; 
	float specular = texture(gSpecular, TexCoords).r; 
	vec3 fragPos  = texture(gPositionDepth, TexCoords).xyz;
	vec3 normal   = normalize(texture(gNormalMet, TexCoords).rgb);
	vec3 reflectionDir = normalize(reflect(fragPos, normal));

	float dotNF = dot(normal, normalize(-fragPos));
	vec3 reflectionColor = specular * getReflection(fragPos, reflectionDir, normal);// * (1 - dotNF);

	color = vec4(result + reflectionColor, 1.0f);
	//color = vec4(vec3(specular), 1.0f);
}

vec3 getReflection(vec3 fragPos, vec3 reflectionDir, vec3 normal) {
	int stepCount = 30;
	float stepSize = 0.12f;
	float stepGain = 1.05f;
	float maxDistance = 0.6f;

	vec3 reflectionColor = vec3(0.f);
	vec3 currPosition = fragPos;
	for(int i = 0; i < stepCount; i++) {
		
		currPosition = currPosition + reflectionDir * stepSize;
		float depth = currPosition.z;
		stepSize *= stepGain;

		vec4 currPosProj = transformToClip(currPosition);
		float currDepth = texture(gPositionDepth, currPosProj.xy).z;

		//Break when reaching border of image
		if(currPosProj.x >= border || currPosProj.x <= (1.f - border) 
			|| currPosProj.y >= border || currPosProj.y <= (1.f - border))
			break; 

		if(currDepth > depth && (currDepth - depth) < maxDistance * stepGain && currDepth < -1) {
			
			vec3 left  = currPosition - reflectionDir * stepSize;
			vec3 right = currPosition;
			for(int i = 0; i < 3; i++) {
				
				currPosition = interpolate(left, right, 0.5f);
				depth        = currPosition.z;
				vec4 currPosProjTemp  = transformToClip(currPosition);
				currDepth    = texture(gPositionDepth, currPosProj.xy).z;

				if(currDepth > depth && (currDepth - depth) < maxDistance) {
					right = currPosition;
					currPosProj = currPosProjTemp;
				}
				else {
					left = currPosition;
				}
			}

			//vec3 reflectedNormal = normalize(texture(gNormalMet, currPosProj.xy).rgb);
			//float dotNR = -dot(normal, reflectedNormal);
			reflectionColor = texture(image, currPosProj.xy).rgb;
			break;
		}
	}

	return reflectionColor;
}

//Transform to clip space
vec4 transformToClip(vec3 vector) {
	vec4 vecProj = vec4(vector, 1.f);
	vecProj = projection * vecProj;
	vecProj.xyz = vecProj.xyz / vecProj.w;
	vecProj.xyz = vecProj.xyz * 0.5 + 0.5;

	return vecProj;
}

float absDistance(float a, float b) {
	if(a > b)
		return a - b;
	else 
		return b - a;
}

vec3 interpolate(vec3 a, vec3 b, float i) {
	return a * (1.f - i) + b * i;
}