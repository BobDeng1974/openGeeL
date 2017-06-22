#version 430

const float PI = 3.14159265359;
const float epsilon = 0.000001f;
const float FLOAT_MAX = 100000.f;
const vec3  luminance = vec3(0.299f, 0.587f, 0.114f);

in vec2 TexCoords;

layout (location = 0) out vec4 color;

uniform mat4 inverseView;
uniform vec3 origin;

uniform int dimensions;
uniform float farClip;
uniform int minStep;

uniform sampler3D voxelTexture;
uniform sampler2D image;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormalMet;
uniform sampler2D gDiffuseSpec;

vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 camPosition, vec3 reflection, vec3 albedo);
vec3 indirectSpecular(vec3 position, vec3 direction, vec3 camPosition, float roughness);

vec4 sampleTexture(vec3 position, vec3 direction, float levelGain);
vec4 getFragmentColor(vec3 position, float lvl);


void main() {
	vec4 normMet  = texture(gNormalMet, TexCoords);
	vec4 diffSpec = texture(gDiffuseSpec, TexCoords);

	vec3 baseColor = texture(image, TexCoords).rgb;
	vec3 posView  = texture(gPositionDepth, TexCoords).rgb;
	vec3 normView = normalize(normMet.rgb);
	vec3 viewView = normalize(-posView);

	vec3 position = (inverseView * vec4(posView, 1.f)).xyz;
	vec3 normal = normalize((inverseView * vec4(origin + normView, 1.f)).xyz);
	vec3 view = normalize((inverseView * vec4(origin + viewView, 1.f)).xyz);
	vec3 refl = normalize(reflect(-view, normal));
	vec3 camPosition = (inverseView * vec4(vec3(0.f), 1.f)).xyz;

	vec3 albedo = diffSpec.rgb;
	float roughness = diffSpec.w;
	float metallic = normMet.w;
	float luma = dot(luminance, baseColor);

	//vec4 fragColor = getFragmentColor(position, 0.5);

	vec3 indirectDiffuse = indirectDiffuse(position, normal, camPosition, refl, albedo);
	vec3 indirectSpecular = indirectSpecular(position, refl, camPosition, roughness);
	//color = vec4(indirectSpecular, 1.f);
	color = vec4(baseColor + indirectDiffuse + indirectSpecular, 1.f);
}


vec3 indirectDiffuse(vec3 position, vec3 normal, vec3 camPosition, vec3 reflection, vec3 albedo) {

	vec3 right = cross(normal, reflection);
	vec3 forward = cross(normal, right);
	
	vec3 sampleVectors[6];
	sampleVectors[0] = vec3(0.f, 1.f, 0.f);
	sampleVectors[1] = vec3(0.f, 0.5f, 0.866f);
	sampleVectors[2] = vec3(0.824f, 0.5f, 0.268f);
	sampleVectors[3] = vec3(0.51f, 0.5f, -0.7f);
	sampleVectors[4] = vec3(-0.51f, 0.5f, -0.7f);
	sampleVectors[5] = vec3(-0.824f, 0.5f, 0.268f);

	for(int i = 0; i < 6; i++) 
		sampleVectors[i] = forward * sampleVectors[i].x + normal * sampleVectors[i].y + right * sampleVectors[i].z;

	float weights[2];
	weights[0] = PI / 4.f;
	weights[1] = (3.f * PI) / 20.f;

	float gain = 0.325f;
	vec3 color = vec3(0.f);
	color += weights[0] * sampleTexture(position, sampleVectors[0], gain).rgb;
	color += weights[1] * sampleTexture(position, sampleVectors[1], gain).rgb;
	color += weights[1] * sampleTexture(position, sampleVectors[2], gain).rgb;
	color += weights[1] * sampleTexture(position, sampleVectors[3], gain).rgb;
	color += weights[1] * sampleTexture(position, sampleVectors[4], gain).rgb;
	color += weights[1] * sampleTexture(position, sampleVectors[5], gain).rgb;

	return color * albedo;
}


vec3 indirectSpecular(vec3 position, vec3 direction, vec3 camPosition, float roughness) {
	float gain = 0.5f * (1.f - roughness);
	vec4 radiance = sampleTexture(position, direction, gain);

	return radiance.rgb * (1.f - roughness);
}



vec4 sampleTexture(vec3 position, vec3 direction, float levelGain) {
	
	int counter = 0;

	vec3 pos = position;
	float minDist = 3.f;

	float lvl = 0.f;

	vec4 color = vec4(0.f);
	while((color.a < 0.9f) && counter < minStep) {
		pos += minDist * direction;
		float rest = 1.f - color.a;
		color += getFragmentColor(pos, lvl) * rest;

		lvl += levelGain;
		minDist = 3.f * (1.f + lvl);

		counter++;
	}

	color /= color.a;
	color = clamp(color, 0.f, 1.f);

	return color;
}


vec4 getFragmentColor(vec3 position, float lvl) {
	vec4 col = textureLod(voxelTexture, position * 0.5f / dimensions, lvl); 
	col.rgb = pow(col.rgb, vec3(2.2f)); //Gamma correction

	return col;
}

