#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gSpecular;

uniform mat4 projection;
uniform int effectOnly;

float border = 1.f;

vec3 interpolate(vec3 a, vec3 b, float i);
vec4 transformToClip(vec3 vector);
vec3 getReflection(vec3 fragPos, vec3 reflectionDir, vec3 normal);

void main() {
	vec3 result    = step(effectOnly, 0.f) * texture(image, TexCoords).rgb;
	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	float depth = -fragPos.z;
	float specular = texture(gSpecular, TexCoords).r; 
	
	vec3 normal = normalize(texture(gNormalMet, TexCoords).rgb);
	vec3 reflectionDir = normalize(reflect(fragPos, normal));
	float dotNF = dot(normal, normalize(-fragPos));

	//DBranch to filter out large bunch of pixels
	if(depth > 0.1f && specular > 0.02f) {
		vec3 reflectionColor = specular * getReflection(fragPos, reflectionDir, normal);
		color = vec4(result + reflectionColor, 1.0f);
	}
	else 
		color = vec4(result, 1.0f);
}

vec3 getReflection(vec3 fragPos, vec3 reflectionDir, vec3 normal) {
	int stepCount = 60;
	float stepSize = 0.2f;
	float stepGain = 1.02f;

	vec3 reflectionColor = vec3(0.f);
	vec3 currPosition = fragPos;
	int i = 0;
	while(i < stepCount) {
		currPosition = currPosition + reflectionDir * stepSize;
		float depth = currPosition.z;
		
		vec4 currPosProj = transformToClip(currPosition);
		vec3 sampledPosition = texture(gPositionDepth, currPosProj.xy).xyz;
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

				vec3 left  = currPosition - reflectionDir * stepSize;
				vec3 right = currPosition;
				for(int i = 0; i < 5; i++) {
				
					currPosition = interpolate(left, right, 0.5f);
					currDepth    = texture(gPositionDepth, currPosProj.xy).z;
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

				//Discard if surface normal and reflected surface normal are too similar. In this case
				//the backside of reflected surface should be used, but isn't visible to viewer
				reflectionColor = step(dotNR, 0.33f) * texture(image, currPosProj.xy).rgb;

				break;
			}
		}

		stepSize *= stepGain;
		i++;
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

vec3 interpolate(vec3 a, vec3 b, float i) {
	return a * (1.f - i) + b * i;
}