#version 430 core

const float PI = 3.14159265359;

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


vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness);
float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness);

//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);
vec3 interpolate(vec3 a, vec3 b, float i);
float interpolatei(vec3 a, vec3 b, vec3 v);
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
				
					currPosition = interpolate(left, right, 0.5f);
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

//Compute fresnel term with Fresnel-Schlick approximation
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness) {
	vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

	vec3 fres = F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(1.f - theta, 5.f);
	return clamp(fres, 0.f, 1.f);
}

//Trowbridge-Reitz GGX normal distribution function
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness) {
	
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

float calculateGeometryFunctionSchlick(float NdotV, float roughness) {
    float r = (roughness + 1.f);
    float k = (r * r) / 8.f;

    float nom   = NdotV;
    float denom = NdotV * (1.f - k) + k;
	
    return nom / denom;
}

float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness) {
    float NdotV = doto(normal, viewDirection);
    float NdotL = doto(normal, lightDirection);
    float ggx2  = calculateGeometryFunctionSchlick(NdotV, roughness);
    float ggx1  = calculateGeometryFunctionSchlick(NdotL, roughness);
	
    return ggx1 * ggx2;
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

float interpolatei(vec3 a, vec3 b, vec3 v) {
	return length((v - a) / (b - a));
}

float doto(vec3 a, vec3 b) {
	return max(dot(a, b), 0.0f);
}


//Hammersley sequence according to
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec2 hammersleySeq(int i, int count) {

	uint bits = i;
	bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float j = float(bits) * 2.3283064365386963e-10;

	return vec2(float(i) / float(count), j);
}


vec3 generateSampledVector(float roughness, vec2 samp) {
	float e1 = samp.x;
	float e2 = samp.y;

	float theta = atan((roughness * sqrt(e1)) / sqrt(1.f - e2));
	//float theta = atan((roughness * roughness * sqrt(e1)) / sqrt(1.f - e2));
	float phi   = 2.f * PI * e2;

	return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}