#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;

uniform mat4 projection;

vec4 transformToClip(vec3 vector);
vec3 getReflection(vec3 fragPos, vec3 reflectionDir);
//vec3 getReflectionOptimized(vec3 fragPos, vec3 reflectionDir);

void main() {
	vec3 result  = texture(image, TexCoords).rgb; 
	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	vec3 normal  = texture(gNormalMet, TexCoords).rgb;
	vec3 reflectionDir = normalize(reflect(fragPos, normal));

	vec3 reflectionColor = getReflection(fragPos, reflectionDir);
	//vec3 reflectionColor = getReflectionOptimized(fragPos, reflectionDir);

	color = vec4(result + reflectionColor, 1.0f);
}

vec3 getReflection(vec3 fragPos, vec3 reflectionDir) {
	int stepCount = 25;
	float stepSize = 0.3f;

	vec3 reflectionColor = vec3(0.f);
	for(int i = 0; i < stepCount; i++) {
		
		vec3 currPosition = fragPos + reflectionDir * stepSize * i;
		float depth = currPosition.z;

		vec4 currPosProj = transformToClip(currPosition);
		float currDepth = texture(gPositionDepth, currPosProj.xy).z;

		if(currDepth > depth && (currDepth - depth) < 0.4 && currDepth < -4) {
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