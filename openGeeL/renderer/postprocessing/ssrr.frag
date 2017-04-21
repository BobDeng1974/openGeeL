#version 430 core

const float PI = 3.14159265359;

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gSpecular;

uniform mat4 projection;
uniform int effectOnly;

float border = 1.f;

vec3 getReflection(vec3 fragPos, vec3 reflectionDir, vec3 normal);
vec3 calculateFresnelTerm(float theta, vec3 albedo, float metallic, float roughness);
float calculateNormalDistrubution(vec3 normal, vec3 halfway, float roughness);
float calculateGeometryFunctionSmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness);

//Return dot(a,b) >= 0
float doto(vec3 a, vec3 b);
vec3 interpolate(vec3 a, vec3 b, float i);
float interpolatei(vec3 a, vec3 b, vec3 v);
vec4 transformToClip(vec3 vector);


void main() {
	vec3 result = step(effectOnly, 0.f) * texture(image, TexCoords).rgb;
	vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
	float depth = -fragPos.z;
	float roughness = texture(gSpecular, TexCoords).w; 
	
	vec3 normal = normalize(texture(gNormalMet, TexCoords).rgb);
	vec3 reflectionDirection = normalize(reflect(fragPos, normal));

	//DBranch to filter out large bunch of pixels
	if(depth > 0.1f && roughness < 0.99f) {
		vec3 viewDirection = normalize(-fragPos);
		float metallic = texture(gNormalMet, TexCoords).w;
		vec3 reflectionColor = getReflection(fragPos, reflectionDirection, normal);

		vec3 halfwayDirection = normalize(reflectionDirection + viewDirection);
		float NdotL = dot(normal, viewDirection);

		//BRDF
		float ndf = calculateNormalDistrubution(normal, halfwayDirection, roughness);
		float geo = calculateGeometryFunctionSmith(normal, viewDirection, reflectionDirection, roughness);
		vec3 fres = calculateFresnelTerm(doto(halfwayDirection, viewDirection), reflectionColor, metallic, roughness);

		//Lighting equation
		vec3  nom   = geo * fres * ndf;
		float denom = 4.f * doto(viewDirection, normal) * NdotL + 0.001f; 
		vec3  brdf  = nom / denom;

		color = vec4(result + reflectionColor * brdf * NdotL , 1.f);
	}
	else 
		color = vec4(result, 1.f);
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